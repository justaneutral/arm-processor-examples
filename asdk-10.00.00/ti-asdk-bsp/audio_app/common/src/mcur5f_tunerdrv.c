/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     TuningDriver.c
********************************************************************************
*
*     Description:  AudioWeaver Tuning Driver for STM Discovery board
*
*     Copyright:    (c) 2018 DSP Concepts, Inc. All rights reserved.
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/ClockP.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include "FreeRTOS.h"
#include "task.h"
#include <platform.h>

#ifdef ETH_TUNING

#ifdef ENABLE_AVB_AUDIO
	#include "enet_setup.h"
#else
	#include "audio_enet.h"
#endif

#endif

extern uint32_t read_byte_cnt;

/* Buffer used for Tuning Packet Processing*/
uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN] \
   __attribute__((section(".awe_tuning_pkt")));
      
extern void am62ax_uart_setup();
extern void awe_uart_sendreply(uint32_t bufSize);
extern int32_t awe_uart_receive_byte();

extern int32_t awe_r5f_ipcSend(char* pMsg, uint16_t msgSize);
extern int32_t awe_r5f_ipcRecv(char* recvBuffer, uint16_t *pRecvBufSize, uint32_t timeout);
extern void am62ax_ipc_close();
extern int32_t awe_r5f_ipcSetup();
extern uint8_t gUartRx_data;

SemaphoreP_Object semDataReady;
SemaphoreP_Object semWriteDone;
SemaphoreP_Object semRemoteReady;
SemaphoreP_Object semEthernetReady;

#define MAX_IPC_BUFSIZE (MAX_COMMAND_BUFFER_LEN*sizeof(uint32_t))
uint8_t  ipcRecvBuf[MAX_IPC_BUFSIZE];
uint16_t recvBufLen = MAX_IPC_BUFSIZE;

extern uint16_t dstCoreId;

extern int32_t configure_codec(void);
extern int32_t Board_clockgenConfig(I2C_Handle handle, uint8_t devAddr);
extern void asdk_sendCoreAlive();

void configure_EVM_Codec()
{
    int32_t     status = SystemP_SUCCESS;
    I2C_Handle  i2cHandle;

    i2cHandle = gI2cHandle[CONFIG_I2C0];

    /* Configure clock generator for getting the external HCLK */
    status = Board_clockgenConfig(i2cHandle, 0x68);
    DebugP_assert(status == SystemP_SUCCESS);
    
    /* Configure codec */
    status = configure_codec(); //Board_codecConfig();
    DebugP_assert(status == SystemP_SUCCESS);
}

extern void EnetApp_printCpuLoad();
uint8_t CPU_RunInfo2[1024];


void tuning_processing(void* args)
{
	int32_t status = SystemP_SUCCESS;

	Drivers_open();
    Board_driversOpen();

#ifndef ETH_TUNING
	int32_t nPacketState;
	uint16_t msgSize;
#endif

	status = SemaphoreP_constructBinary(&semDataReady, 0);
	DebugP_assert(SystemP_SUCCESS == status);
	
	status = SemaphoreP_constructBinary(&semWriteDone, 0);
	DebugP_assert(SystemP_SUCCESS == status);

	status = SemaphoreP_constructBinary(&semRemoteReady, 0);
	DebugP_assert(SystemP_SUCCESS == status);

	DebugP_log("mcu/r5f tuning server : Configuring Codec...\r\n");

	configure_EVM_Codec();
	DebugP_log("mcu/r5f tuning server : Configuring Codec...Done\r\n");

#ifdef ETH_TUNING

#ifdef ENABLE_AVB_AUDIO
	enetApp_setup();
#else /* ENABLE_AVB_AUDIO */

	status = SemaphoreP_constructBinary(&semEthernetReady, 0);
	DebugP_assert(SystemP_SUCCESS == status);
	
	awe_enet_setup();
	SemaphoreP_pend(&semEthernetReady, SystemP_WAIT_FOREVER);

	// start tark for tcp server
	awe_createEnetTask();

#endif /* ENABLE_AVB_AUDIO */

#endif /*  ETH_TUNING */

    // Setup ipc interface
	DebugP_log("mcu/r5f tuning server : Configuring IPC...\r\n");
    awe_r5f_ipcSetup();
	asdk_sendCoreAlive();
	DebugP_log("mcu/r5f tuning server : Configuring IPC...\r\n");

	DebugP_log("mcu/r5f tuning server : waiting for remote core\r\n");
	SemaphoreP_pend(&semRemoteReady, SystemP_WAIT_FOREVER);
	DebugP_log("Remote Core : %d ready\r\n", dstCoreId);

	
#ifndef ETH_TUNING

	// Setup uart now
	am62ax_uart_setup();

    while(TRUE)
    {
		SemaphoreP_pend(&semDataReady, SystemP_WAIT_FOREVER);
		nPacketState = tuningDecodeByteUART(AWE_Packet_Buffer, gUartRx_data);
		if (nPacketState == COMPLETE_NEW_PACKET)
		{
			//DebugP_log("Uart Recvd : %d bytes\r\n", read_byte_cnt);
		
			// send the tunning buffer to primary core
			msgSize = (uint16_t) (AWE_Packet_Buffer[0]>>16) * sizeof(uint32_t);
			awe_r5f_ipcSend((char *)AWE_Packet_Buffer, msgSize);
        
			recvBufLen = MAX_IPC_BUFSIZE;

			//wait for response from c7x
			awe_r5f_ipcRecv((char *)AWE_Packet_Buffer, &recvBufLen, SystemP_WAIT_FOREVER);
        
			//DebugP_log("IPC Recvd : %d bytes\r\n", recvBufLen);

			awe_uart_sendreply(recvBufLen);
			SemaphoreP_pend(&semWriteDone, SystemP_WAIT_FOREVER);
		} 
		awe_uart_receive_byte();

    }

#else

    while(1)
	{ 
		memset(CPU_RunInfo2,0,640);
        vTaskGetRunTimeStats((char *)&CPU_RunInfo2);
		DebugP_log("\r\nName\t\t\t\tRunCnt\t\tLoadRate\r\n");
        DebugP_log("%s\r\n\r\n\r\n", CPU_RunInfo2);

		ClockP_usleep(10000000);
        //EnetApp_printCpuLoad();
		taskYIELD();
	}
#endif

    vTaskSuspend(NULL);
    vTaskDelete(NULL);
}
