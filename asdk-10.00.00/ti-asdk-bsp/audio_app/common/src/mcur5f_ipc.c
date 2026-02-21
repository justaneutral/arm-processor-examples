/*
 *  Copyright (C) 2021 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/DebugP.h>
#include <drivers/ipc_notify.h>
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include "FreeRTOS.h"
#include "task.h"

#include <platform.h>
#include <ti_bsp_defs.h>

/* main core that starts the message exchange */
uint32_t gMainCoreId = CSL_CORE_ID_MCU_R5FSS0_0;
uint32_t gRemoteCoreId[] = 
{
    CSL_CORE_ID_A53SS0_0,
    CSL_CORE_ID_C75SS0_0,
    CSL_CORE_ID_MAX 
};

#define DEBUG_IPC_PRINT 			0

extern uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN];
extern SemaphoreP_Object semRemoteReady;
SemaphoreP_Object semTuningRespReady;

uint16_t dstCoreId = CSL_CORE_ID_A53SS0_0;
uint32_t recvMsgCnt = 0;
uint32_t sendMsgCnt = 0;
uint32_t layoutProcessingTime = 0;

void asdk_sendCoreAlive();

void ti_awe_layoutPrcHndlr(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
    layoutProcessingTime = msgValue;
    DebugP_log("Layout processing time : %d\r\n", layoutProcessingTime);
}

void awe_coreAliveHandler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
    dstCoreId = remoteCoreId;
    SemaphoreP_post(&semRemoteReady);

    if(msgValue == 0xFE)
    {
        asdk_sendCoreAlive();
    }
}

void awe_tuningPacketRespHandler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
	recvMsgCnt++;
	
	if(1 == DEBUG_IPC_PRINT)
	{
		DebugP_log("awe_handleActiveConn: response received\r\n");
	}
	
	// tuning packet response ready
	SemaphoreP_post(&semTuningRespReady);
}

int32_t awe_r5f_ipcSend()
{
    int32_t  status = SystemP_SUCCESS;
    uint32_t msg = 0;
    status = IpcNotify_sendMsg(dstCoreId, AWE_TUNING_PCKT_READY, msg, 1);
    sendMsgCnt++;
    
	if(1 == DEBUG_IPC_PRINT)
	{
		uint16_t        msgSize;
		msgSize = (uint16_t) (AWE_Packet_Buffer[0]>>16) * sizeof(uint32_t);
		DebugP_log("awe_handleActiveConn: rmsgSize %d\r\n", msgSize);
	}
    
    return status;
}

void awe_r5f_ipcRecv()
{
	SemaphoreP_pend(&semTuningRespReady, SystemP_WAIT_FOREVER);
}

void asdk_sendCoreAlive()
{
    int32_t status = SystemP_SUCCESS;
    int32_t i;

    /* wait for remote core to be ready */
    for(i=0; gRemoteCoreId[i]!=CSL_CORE_ID_MAX; i++)
    {
        DebugP_log("mcu/r5f tuning server : sending mcu/r5f ipc_notify to code %d \r\n", gRemoteCoreId[i]);

        status = IpcNotify_sendMsg(gRemoteCoreId[i], REMOTE_ALIVE_NOTIFY, 0xFF, 1);
        DebugP_assert(status==SystemP_SUCCESS);
    }
}

int32_t awe_r5f_ipcSetup()
{
    int32_t status = SystemP_SUCCESS;
 
    status = SemaphoreP_constructBinary(&semTuningRespReady, 0);
	DebugP_assert(SystemP_SUCCESS == status);

    DebugP_log("mcu/r5f tuning server : registering IPC notify clients...\r\n");

    status = IpcNotify_registerClient(REMOTE_ALIVE_NOTIFY, awe_coreAliveHandler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);
    
    status = IpcNotify_registerClient(AWE_TUNING_PCKT_READY, awe_tuningPacketRespHandler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

    status = IpcNotify_registerClient(LAYOUT_PROCESSING_TIME, ti_awe_layoutPrcHndlr, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

    return status;
}



void am62ax_ipc_close()
{
    IpcNotify_unregisterClient(REMOTE_ALIVE_NOTIFY);
    IpcNotify_unregisterClient(AWE_TUNING_PCKT_READY);
}
