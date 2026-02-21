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

/* This example demonstrates the MibSPI RX and TX operation configured
 * in blocking, interrupt mode of operation.
 *
 * This example sends a known data in the TX mode of length
 * APP_MIBSPI_MSGSIZE and then receives the same in RX mode.
 * Digital loopback mode is enabled to receive data.
 *
 * When transfer is completed, TX and RX buffer data are compared.
 * If data is matched, test result is passed otherwise failed.
 */
#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <kernel/dpl/ClockP.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <drivers/mcasp.h>
#include <board/ioexp/ioexp_tca6424.h>
#include "ti_drivers_config.h"
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"

#include <platform.h>

#include "shm_cirbuf.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
#define NUM_BUFS                (4)

uint32_t gMcaspTxCnt = 0;
uint32_t gMcaspRxCnt = 0;
uint32_t gMcaspErrCnt = 0;

uint8_t rxbuf[NUM_BUFS][DMA_RX_BUF_SIZE] \
    __attribute__((section("awe_mcasp_buffer"), aligned(BUFALIGN)));
uint8_t txbuf[NUM_BUFS][DMA_TX_BUF_SIZE] \
    __attribute__((section("awe_mcasp_buffer"), aligned(BUFALIGN)));

extern uint32_t AWE_Packet_Buffer[];

volatile int rxFrameIndex = (NUM_BUFS - 1);
volatile int txFrameIndex = (NUM_BUFS - 1);

/* MCASP Data Process Task */
#define MCASP_PROCESS_TASK_PRI          5 //(configMAX_PRIORITIES-1)
#define MCASP_PROCESS_TASK_STACKSIZE    (65536U / sizeof(configSTACK_DEPTH_TYPE))
StackType_t gMcaspProcessTaskStack[MCASP_PROCESS_TASK_STACKSIZE];
StaticTask_t gMcaspProcessTaskObj;
TaskHandle_t gMcaspProcessTask;

SemaphoreP_Object semTx;
SemaphoreP_Object semRx;
SemaphoreP_Object semAudioDataReady;

MCASP_Transaction rxFrame[NUM_BUFS];
MCASP_Transaction txFrame[NUM_BUFS];

MCASP_Handle mcaspHandle;
#ifdef ENABLE_AVB_AUDIO
shm_handle shm_rx = NULL;
shm_handle shm_tx = NULL;
#endif

extern uint8_t gTxLoopjobBuf0[];
extern uint8_t gRxLoopjobBuf0[];
extern uint32_t g_audioProcessing;

int gtxFrameIndexCount = 0;
int grxFrameIndexCount = 0;
int total_frames_sent = 0;
Awe_Data_Buf g_aweBuf;

#ifdef ENABLE_AVB_AUDIO

uint8_t g_avb_rxbuf[SHM_AVB_DATA_RX_SIZE] \
	__attribute__((section(".asdk_avb_rx_buffer")));

uint8_t g_avb_txbuf[SHM_AVB_DATA_TX_SIZE] \
	__attribute__((section(".asdk_avb_tx_buffer")));

//Common denominator for 8-ch 64 samples
#define  AVB_BUF_CMN_SIZE   3072

uint32_t avbRxFrmCnt = 0;
uint32_t avbTxFrmCnt = 0;
uint32_t avbRxErrCnt = 0;
uint32_t avbTxErrCnt = 0;

void asdk_createShmbuffer()
{
    uint32_t rxSize         = AVB_BUF_CMN_SIZE; //AVB_RX_FRAME_SIZE;
    uint32_t txSize         = AVB_BUF_CMN_SIZE; //AVB_TX_FRAME_SIZE;
    uint32_t shmOvrHd       = shm_metadata_overhead();
    uint32_t rxShmBufSize   = ((SHM_AVB_DATA_RX_SIZE - shmOvrHd)/rxSize) * rxSize + shmOvrHd;
    uint32_t txShmBufSize   = ((SHM_AVB_DATA_TX_SIZE - shmOvrHd)/txSize) * txSize + shmOvrHd;

    DebugP_assert(rxShmBufSize < SHM_AVB_DATA_RX_SIZE);
    DebugP_assert(txShmBufSize < SHM_AVB_DATA_TX_SIZE);

    shm_rx = shm_create((uint32_t)g_avb_rxbuf, rxShmBufSize);
    shm_tx = shm_create((uint32_t)g_avb_txbuf, txShmBufSize);
}

void get_avb_rx_buffer(uint8_t* buf, uint16_t* dataSize)
{
	shm_read(shm_rx, buf, dataSize);
}

void get_avb_rx_bufptr(uint8_t** buf, uint16_t* dataSize)
{
	shm_readBufPtr(shm_rx, buf, dataSize);
}

void get_avb_tx_buffer(int16_t** buf, uint16_t dataSize)
{
	shm_writeBufPtr(shm_tx, buf, dataSize);	
}

#endif

int16_t avb_tx_buf[AVB_RX_CHANNEL_COUNT*AUDIO_BLOCK_SIZE];
int16_t avb_rx_buf[AVB_RX_CHANNEL_COUNT*AUDIO_BLOCK_SIZE];

void asdk_copy_audiosamples_avb2dac(int16_t *pAvbBuf, int32_t *pDacBuf)
{
    uint32_t i, j;
    int32_t *tx = pDacBuf;
    int16_t *rx = pAvbBuf;

    for (i = 0, j = 0; i < AUDIO_BLOCK_SIZE; i++)
    {
        tx[8 * i + 0] = (int32_t)rx[j++] << 16;
        tx[8 * i + 4] = (int32_t)rx[j++] << 16;

        tx[8 * i + 1] = (int32_t)rx[j++] << 16;
        tx[8 * i + 5] = (int32_t)rx[j++] << 16;

        tx[8 * i + 2] = (int32_t)rx[j++] << 16;
        tx[8 * i + 6] = (int32_t)rx[j++] << 16;

        tx[8 * i + 3] = (int32_t)rx[j++] << 16;
        tx[8 * i + 7] = (int32_t)rx[j++] << 16;
    }
}

void asdk_copy_audiosamples_adc2avb(int16_t *pAvbBuf, int32_t *pAdcBuf)
{
    uint32_t i, j;
    int32_t *rx = pAdcBuf;
    int16_t *tx = pAvbBuf;

    for (i = 0, j = 0; i < AUDIO_BLOCK_SIZE; i++)
    {
        tx[j++] = (int16_t)(rx[8 * i + 0] >> 16);
        tx[j++] = (int16_t)(rx[8 * i + 4] >> 16);

        tx[j++] = (int16_t)(rx[8 * i + 1] >> 16);
        tx[j++] = (int16_t)(rx[8 * i + 5] >> 16);

        tx[j++] = (int16_t)(rx[8 * i + 2] >> 16);
        tx[j++] = (int16_t)(rx[8 * i + 6] >> 16);

        tx[j++] = (int16_t)(rx[8 * i + 3] >> 16);
        tx[j++] = (int16_t)(rx[8 * i + 8] >> 16);
    }
}

void process_audio_samples()
{
	uint8_t    *pTxBuf;
	uint8_t    *pRxBuf;

	pTxBuf = &txbuf[txFrameIndex][0];
	pRxBuf = &rxbuf[rxFrameIndex][0];

	CacheP_inv((void *)pRxBuf, DMA_RX_BUF_SIZE, CacheP_TYPE_ALL);

	if (g_audioProcessing == 1)
	{
        //debugGpio_high(awecore_debug_pin);

        g_aweBuf.pDmaRxBuf = (int32_t *)pRxBuf;
        g_aweBuf.pDmaTxBuf = (int32_t *)pTxBuf;

        #ifdef ENABLE_AVB_AUDIO
        //int16_t*   avbRx = NULL;
	    int16_t*   avbTx = avb_tx_buf;
        uint16_t   recvd_data_size = AVB_RX_FRAME_SIZE;

        get_avb_rx_buffer((uint8_t*)avb_rx_buf, &recvd_data_size);
        if(recvd_data_size == AVB_RX_FRAME_SIZE)
        {
            avbRxFrmCnt++;
            g_aweBuf.pIpcRxBuf = avb_rx_buf;
        } else {
            avbRxErrCnt++;
        }
        get_avb_tx_buffer(&avbTx, AVB_TX_FRAME_SIZE);
        g_aweBuf.pIpcTxBuf = avbTx;
        #endif /* ENABLE_AVB_AUDIO */

		awe_transferAudioBufs(&g_AWEInstance, &g_aweBuf);
        
        // Get the layout mask and wakeup correct layout-thread
        awe_executeAudioPump(&g_AWEInstance);

	} else {
        #ifdef ENABLE_AVB_AUDIO
        uint16_t   recvd_data_size = AVB_RX_FRAME_SIZE;
        int16_t*   avbTx = avb_tx_buf;

        // AVB listener  audio samples to DAC
        get_avb_rx_buffer((uint8_t*)avb_rx_buf, &recvd_data_size);
        if(recvd_data_size == AVB_RX_FRAME_SIZE)
        {
            avbRxFrmCnt++;
            asdk_copy_audiosamples_avb2dac((int16_t*)avb_rx_buf, (int32_t*)pTxBuf);
        } else {
            avbRxErrCnt++;
        }
        
        // ADC audio samples to AVB talker
        get_avb_tx_buffer(&avbTx, AVB_TX_FRAME_SIZE);
        if(avbTx != NULL)
        {
            avbTxFrmCnt++;
            asdk_copy_audiosamples_adc2avb((int16_t*)avbTx, (int32_t*)pRxBuf);
        } else {
            avbTxErrCnt++;
        }

        #else /* ENABLE_AVB_AUDIO */
        /* Write zeroes on output if audio not started */
		memset((void*)pTxBuf, 0, DMA_TX_BUF_SIZE);

        /* loopback */
        //memcpy((void*)pTxBuf, (void*)pRxBuf, DMA_TX_BUF_SIZE);

        #endif /* ENABLE_AVB_AUDIO */
	}

	//CacheP_wb((void *)p_txBuf, MCASP_BUF_SIZE, CacheP_TYPE_ALL);
	CacheP_wbInv((void *)pTxBuf, DMA_TX_BUF_SIZE, CacheP_TYPE_ALL);

	MCASP_submitTx(mcaspHandle, &txFrame[txFrameIndex]);
    txFrameIndex = ((txFrameIndex + 1) % NUM_BUFS);

    rxFrameIndex = ((rxFrameIndex + 1) % NUM_BUFS);
	MCASP_submitRx(mcaspHandle, &rxFrame[rxFrameIndex]);

	total_frames_sent++;
}

void mcasp_txCallback(MCASP_Handle handle,
                      MCASP_Transaction *transaction)
{
	//debugGpio_toggle(mcasp_debug_pin);
	gMcaspTxCnt++;
    SemaphoreP_post(&semTx);
}

void mcasp_rxCallback(MCASP_Handle handle,
                      MCASP_Transaction *transaction)
{
    //debugGpio_toggle(mcasp_debug_pin);
    gMcaspRxCnt++;
    SemaphoreP_post(&semRx);
}

void audio_stop()
{
    int status;

    /* Trigger McASP receive operation */
    status = MCASP_stopTransferRx(mcaspHandle);
    if (status != SystemP_SUCCESS)
    {
        DebugP_log("MCASP_startTransferRx failed, error 0x%X\r\n", status);
    }
    else
    {
        MCASP_withdrawRx(mcaspHandle);
    }

    /* Trigger McASP transmit operation */
    status = MCASP_stopTransferTx(mcaspHandle);
    if (status != SystemP_SUCCESS)
    {
        DebugP_log("MCASP_startTransferTx failed, error 0x%X\r\n", status);
    }
    else
    {
        MCASP_withdrawTx(mcaspHandle);
    }
}

void mcasp_setup()
{
#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    /* Open MCASP driver after enabling the HCLK */
    gMcaspHandle[0] = MCASP_open(0, &gMcaspOpenParams[0]);
    if(NULL == gMcaspHandle[0])
    {
        DebugP_logError("MCASP open failed for instance 0 !!!\r\n");
        DebugP_assert(false);
    }
    //DebugP_log("MCASP Setup completed...\r\n");    

#ifdef ENABLE_AVB_AUDIO
    asdk_createShmbuffer();
#endif

#endif

}

void mcasp_prime()
{
#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    int32_t count = 0;
    int status;

    mcaspHandle = MCASP_getHandle(CONFIG_MCASP0);

    for (count = 0; count < NUM_BUFS; count++)
    {
        /* Issue the first & second empty buffers to the input stream */
        memset((uint8_t *)&rxbuf[count][0], 0xFF, DMA_RX_BUF_SIZE);

        /* RX frame processing */
        rxFrame[count].buf = (void *)&rxbuf[count][0];
        rxFrame[count].count = DMA_RX_BUF_SIZE / 4;
        rxFrame[count].timeout = 0xFFFFFF;

        /* Submit McASP packet for Rx */
        MCASP_submitRx(mcaspHandle, &rxFrame[count]);
    }

    for (count = 0; count < (NUM_BUFS); count++)
    {
        memset((uint8_t *)&txbuf[count][0], 0xF0, DMA_TX_BUF_SIZE);

        /* TX frame processing */
        txFrame[count].buf = (void *)&txbuf[count][0];
        txFrame[count].count = DMA_TX_BUF_SIZE / 4;
        txFrame[count].timeout = 0xFFFFFF;

        /* Submit McASP packet for Tx */
        MCASP_submitTx(mcaspHandle, &txFrame[count]);
    }

    /* Trigger McASP receive operation */
    status = MCASP_startTransferRx(mcaspHandle);
    if (status != SystemP_SUCCESS)
    {
        DebugP_log("MCASP_startTransferRx failed, error 0x%X\r\n", status);
    }

    /* Trigger McASP transmit operation */
    status = MCASP_startTransferTx(mcaspHandle);
    if (status != SystemP_SUCCESS)
    {
        DebugP_log("MCASP_startTransferTx failed, error 0x%X\r\n", status);
    }
#endif

}

void Mcasp_Process(void *arg)
{
    int32_t     done = 0;

#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)

#ifdef MULTI_CHANNEL_AUDIO
    mcasp_setup();
#endif

    mcasp_prime();
#endif

    // create semephore object for MCASP RX callback
    SemaphoreP_constructBinary(&semTx, 0);
    SemaphoreP_constructBinary(&semRx, 0);

    SemaphoreP_constructBinary(&semAudioDataReady, 0);

    while (!done)
    {
        if(g_AWEInstance.instanceId == TI_AWE_PRIMARY_CORE) 
        {
            // Wait for MCASP RX callback
            SemaphoreP_pend(&semTx, SystemP_WAIT_FOREVER);
            SemaphoreP_pend(&semRx, SystemP_WAIT_FOREVER);

            process_audio_samples();
        }
        else 
        {
            SemaphoreP_pend(&semAudioDataReady, SystemP_WAIT_FOREVER);

            //DebugP_log("Received data ready from primary\r\n");
            awe_executeAudioPump(&g_AWEInstance);
        }
                   
    }  /* While done loop*/
}

void Audio_io_setup()
{
    gMcaspProcessTask = xTaskCreateStatic(Mcasp_Process,
                                          "Mcasp_Process", MCASP_PROCESS_TASK_STACKSIZE,
                                          NULL, MCASP_PROCESS_TASK_PRI, gMcaspProcessTaskStack,
                                          &gMcaspProcessTaskObj);
    configASSERT(gMcaspProcessTask != NULL);
#ifdef SMP_FREERTOS
    // if a53 in smp mode, then only core_0 should handle it
    vTaskCoreAffinitySet(gMcaspProcessTask, 1);
#endif
}
