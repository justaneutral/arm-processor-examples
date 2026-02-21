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

/*
 * Auto generated file 
 */

#include "ti_drivers_open_close.h"
#include <kernel/dpl/DebugP.h>

void Drivers_open(void)
{

    Drivers_uartOpen();
    Drivers_i2cOpen();
    Drivers_mcaspOpen();
    Drivers_udmaOpen();
}

void Drivers_close(void)
{
    Drivers_i2cClose();
    Drivers_mcaspClose();
    Drivers_udmaClose();
    Drivers_uartClose();
}

/*
 * I2C
 */

/* I2C Driver handles */
I2C_Handle gI2cHandle[CONFIG_I2C_NUM_INSTANCES];

/* I2C Driver Parameters */
I2C_Params gI2cParams[CONFIG_I2C_NUM_INSTANCES] =
{
    {
        .transferMode        = I2C_MODE_BLOCKING,
        .transferCallbackFxn = NULL,
        .bitRate             = I2C_400KHZ,
    },
};

void Drivers_i2cOpen(void)
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_I2C_NUM_INSTANCES; instCnt++)
    {
        gI2cHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_I2C_NUM_INSTANCES; instCnt++)
    {
        gI2cHandle[instCnt] = I2C_open(instCnt, &gI2cParams[instCnt]);
        if(NULL == gI2cHandle[instCnt])
        {
            DebugP_logError("I2C open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Drivers_i2cClose();   /* Exit gracefully */
    }

    return;
}

void Drivers_i2cClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_I2C_NUM_INSTANCES; instCnt++)
    {
        if(gI2cHandle[instCnt] != NULL)
        {
            I2C_close(gI2cHandle[instCnt]);
            gI2cHandle[instCnt] = NULL;
        }
    }

    return;
}


/*
 * MCASP
 */

/* MCASP transmit callback */
void mcasp_txcb(MCASP_Handle handle, MCASP_Transaction *transaction);
/* MCASP receive Callback */
void mcasp_rxcb(MCASP_Handle handle, MCASP_Transaction *transaction);
/* MCASP transmit loopjob buffer */
uint8_t gTxLoopjobBuf0[256] __attribute__((aligned(256))) = {0};
/* MCASP receive loopjob buffer */
uint8_t gRxLoopjobBuf0[256] __attribute__((aligned(256))) = {0};

/* Arrays containing indices of MCASP Tx/Rx serializers used */
uint8_t gMcasp0TxSersUsed[1] = {0};
uint8_t gMcasp0RxSersUsed[1] = {14};

Udma_EventObject gBcdmaTxCqEventObj;
Udma_EventObject gBcdmaRxCqEventObj;

/* Number of ring entries */
#define UDMA_RING_ENTRIES_TX             (MCASP_TX_DMA_RING_ELEM_CNT)
#define UDMA_RING_ENTRIES_RX             (MCASP_RX_DMA_RING_ELEM_CNT)
/* Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define MCASP_UDMA_RING_ENTRY_SIZE       (sizeof(uint64_t))

#define MCASP_RING_MEM_SIZE_TX           (MCASP_UDMA_RING_ENTRY_SIZE*UDMA_RING_ENTRIES_TX)
#define MCASP_RING_MEM_SIZE_RX           (MCASP_UDMA_RING_ENTRY_SIZE*UDMA_RING_ENTRIES_RX)

#define MCASP_UDMA_TR3_TRPD_SIZE         (UDMA_GET_TRPD_TR3_SIZE(1))
#define MCASP_UDMA_HPD_SIZE              UDMA_ALIGN_SIZE((sizeof(CSL_UdmapCppi5HMPD)))

Udma_ChObject       gMcasp0UdmaTxChObj;
Udma_EventObject    gMcasp0_UdmaCqEventObjTx;

Udma_ChObject       gMcasp0UdmaRxChObj;
Udma_EventObject    gMcasp0_UdmaCqEventObjRx;

static uint8_t gMcasp0UdmaTxTrpdMem[MCASP_UDMA_TR3_TRPD_SIZE*MCASP_TX_DMA_RING_ELEM_CNT] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gMcasp0UdmaRxTrpdMem[MCASP_UDMA_HPD_SIZE*MCASP_RX_DMA_RING_ELEM_CNT] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

static uint8_t gMcasp0TxFqRingMem[UDMA_ALIGN_SIZE(MCASP_RING_MEM_SIZE_TX)] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gMcasp0RxFqRingMem[UDMA_ALIGN_SIZE(MCASP_RING_MEM_SIZE_RX)] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

static MCASP_Transaction *gMcasp0TxCbParam[MCASP_TX_DMA_RING_ELEM_CNT];
static MCASP_Transaction *gMcasp0RxCbParam[MCASP_RX_DMA_RING_ELEM_CNT];

MCASP_DmaChConfig gMcasp0_DmaChCfg[] =
{
    {
        .txChHandle         = &gMcasp0UdmaTxChObj,
        .rxChHandle         = &gMcasp0UdmaRxChObj,
        .cqTxEvtHandle      = &gMcasp0_UdmaCqEventObjTx,
        .cqRxEvtHandle      = &gMcasp0_UdmaCqEventObjRx,
        .txTrpdMem          = gMcasp0UdmaTxTrpdMem,
        .rxTrpdMem          = gMcasp0UdmaRxTrpdMem,
        .txRingMem          = gMcasp0TxFqRingMem,
        .rxRingMem          = gMcasp0RxFqRingMem,
        .txCbParams         = gMcasp0TxCbParam,
        .rxCbParams         = gMcasp0RxCbParam,
        .rxEvtNum           = UDMA_PDMA_CH_MAIN0_MCASP2_RX,
        .txEvtNum           = UDMA_PDMA_CH_MAIN0_MCASP2_TX,
    }
};

/* MCASP Driver handles */
MCASP_Handle gMcaspHandle[CONFIG_MCASP_NUM_INSTANCES];
/* MCASP Driver Open Parameters */
MCASP_OpenParams gMcaspOpenParams[CONFIG_MCASP_NUM_INSTANCES] =
{
    {
        .transferMode = MCASP_TRANSFER_MODE_DMA,
        .txBufferFormat = MCASP_AUDBUFF_FORMAT_1SER_MULTISLOT_INTERLEAVED,
        .rxBufferFormat = MCASP_AUDBUFF_FORMAT_1SER_MULTISLOT_INTERLEAVED,
        .txSerUsedCount = 1,
        .rxSerUsedCount = 1,
        .txSerUsedArray = (uint8_t *) gMcasp0TxSersUsed,
        .rxSerUsedArray = (uint8_t *) gMcasp0RxSersUsed,
        .txSlotCount = 2,
        .rxSlotCount = 8,
        .txCallbackFxn = mcasp_txcb,
        .rxCallbackFxn = mcasp_rxcb,
        .txLoopjobEnable = true,
        .txLoopjobBuf = (uint8_t *) gTxLoopjobBuf0,
        .txLoopjobBufLength = 64,
        .rxLoopjobEnable = true,
        .rxLoopjobBuf = (uint8_t *) gRxLoopjobBuf0,
        .rxLoopjobBufLength = 64,
        .dmaChCfg = &gMcasp0_DmaChCfg[0],
        .mcaspDmaDrvObj = &gUdmaDrvObj[CONFIG_UDMA0],
        .mcaspPktDmaDrvObj = &gUdmaDrvObj[CONFIG_UDMA1],
        .skipDriverOpen = 1,
    },
};


void Drivers_mcaspOpen(void)
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_MCASP_NUM_INSTANCES; instCnt++)
    {
        gMcaspHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_MCASP_NUM_INSTANCES; instCnt++)
    {
        if(!gMcaspOpenParams[instCnt].skipDriverOpen)
        {
            gMcaspHandle[instCnt] = MCASP_open(instCnt, &gMcaspOpenParams[instCnt]);
            if(NULL == gMcaspHandle[instCnt])
            {
                DebugP_logError("MCASP open failed for instance %d !!!\r\n", instCnt);
                status = SystemP_FAILURE;
                break;
            }
        }
    }

    if(SystemP_FAILURE == status)
    {
        Drivers_mcaspClose();   /* Exit gracefully */
    }

    return;
}

void Drivers_mcaspClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_MCASP_NUM_INSTANCES; instCnt++)
    {
        if(gMcaspHandle[instCnt] != NULL)
        {
            MCASP_close(gMcaspHandle[instCnt]);
        }
    }

    for(instCnt = 0U; instCnt < CONFIG_MCASP_NUM_INSTANCES; instCnt++)
    {
        if(gMcaspHandle[instCnt] != NULL)
        {
            gMcaspHandle[instCnt] = NULL;
        }
    }

    return;
}

/*
 * UDMA
 */
/*
 * UDMA BCDMA_0 Blockcopy Parameters
 */
/* UDMA BCDMA_0 Blockcopy Channel Objects */
static Udma_ChObject gConfigUdma0BlkCopyChObj[CONFIG_UDMA0_NUM_BLKCOPY_CH];
/* UDMA CONFIG_UDMA0 Blockcopy Channel Handle */
Udma_ChHandle gConfigUdma0BlkCopyChHandle[CONFIG_UDMA0_NUM_BLKCOPY_CH];

/* UDMA BCDMA_0 Blockcopy Channel Ring Mem Size */
#define UDMA_CONFIG_UDMA0_BLK_COPY_CH_0_RING_MEM_SIZE     (((1U * 8U) + UDMA_CACHELINE_ALIGNMENT) & ~(UDMA_CACHELINE_ALIGNMENT - 1U))

/* UDMA BCDMA_0 Blockcopy Channel Ring Mem */
static uint8_t gConfigUdma0BlkCopyCh0RingMem[UDMA_CONFIG_UDMA0_BLK_COPY_CH_0_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/* UDMA BCDMA_0 Blockcopy Channel Ring Memory Pointers - for all channels */
static uint8_t *gConfigUdma0BlkCopyChRingMem[CONFIG_UDMA0_NUM_BLKCOPY_CH] = {
    &gConfigUdma0BlkCopyCh0RingMem[0U],
};
/* UDMA BCDMA_0 Blockcopy Channel Ring Elem Count */
static uint32_t gConfigUdma0BlkCopyChRingElemCnt[CONFIG_UDMA0_NUM_BLKCOPY_CH] = {
    1U,
};
/* UDMA BCDMA_0 Blockcopy Channel Ring Memory Size */
static uint32_t gConfigUdma0BlkCopyChRingMemSize[CONFIG_UDMA0_NUM_BLKCOPY_CH] = {
    UDMA_CONFIG_UDMA0_BLK_COPY_CH_0_RING_MEM_SIZE,
};
/* UDMA BCDMA_0 Blockcopy Channel Event Object */
static Udma_EventObject gConfigUdma0BlkCopyCqEventObj[CONFIG_UDMA0_NUM_BLKCOPY_CH];
/* UDMA BCDMA_0 Blockcopy Channel Event Callback */
static Udma_EventCallback gConfigUdma0BlkCopyCqEventCb[CONFIG_UDMA0_NUM_BLKCOPY_CH] = {
    NULL,
};
/* UDMA CONFIG_UDMA0 Blockcopy Event Handle */
Udma_EventHandle gConfigUdma0BlkCopyCqEventHandle[CONFIG_UDMA0_NUM_BLKCOPY_CH];


static void Drivers_udmaConfigUdma0BlkCopyOpen(void)
{
    int32_t             retVal = UDMA_SOK;
    uint32_t            chType, chCnt;
    Udma_ChHandle       chHandle;
    Udma_ChPrms         chPrms;
    Udma_ChTxPrms       txPrms;
    Udma_ChRxPrms       rxPrms;
    Udma_DrvHandle      drvHandle = &gUdmaDrvObj[CONFIG_UDMA0];
    Udma_EventPrms      cqEventPrms;
    Udma_EventHandle    cqEventHandle;

    for(chCnt = 0U; chCnt < CONFIG_UDMA0_NUM_BLKCOPY_CH; chCnt++)
    {
        chHandle = &gConfigUdma0BlkCopyChObj[chCnt];
        gConfigUdma0BlkCopyChHandle[chCnt] = chHandle;

        /* Init channel parameters */
        chType = UDMA_CH_TYPE_TR_BLK_COPY;
        UdmaChPrms_init(&chPrms, chType);
        chPrms.fqRingPrms.ringMem       = gConfigUdma0BlkCopyChRingMem[chCnt];
        chPrms.fqRingPrms.ringMemSize   = gConfigUdma0BlkCopyChRingMemSize[chCnt];
        chPrms.fqRingPrms.elemCnt       = gConfigUdma0BlkCopyChRingElemCnt[chCnt];

        /* Open channel for block copy */
        retVal = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        DebugP_assert(UDMA_SOK == retVal);

        /* Config TX channel */
        UdmaChTxPrms_init(&txPrms, chType);
        retVal = Udma_chConfigTx(chHandle, &txPrms);
        DebugP_assert(UDMA_SOK == retVal);

        /* Config RX channel - which is implicitly paired to TX channel in
         * block copy mode */
        UdmaChRxPrms_init(&rxPrms, chType);
        retVal = Udma_chConfigRx(chHandle, &rxPrms);
        DebugP_assert(UDMA_SOK == retVal);

        /* Register completion event */
        if(NULL != gConfigUdma0BlkCopyCqEventCb[chCnt])
        {
            cqEventHandle = &gConfigUdma0BlkCopyCqEventObj[chCnt];
            gConfigUdma0BlkCopyCqEventHandle[chCnt] = cqEventHandle;
            UdmaEventPrms_init(&cqEventPrms);
            cqEventPrms.eventType         = UDMA_EVENT_TYPE_DMA_COMPLETION;
            cqEventPrms.eventMode         = UDMA_EVENT_MODE_SHARED;
            cqEventPrms.chHandle          = chHandle;
            cqEventPrms.masterEventHandle = Udma_eventGetGlobalHandle(drvHandle);
            cqEventPrms.eventCb           = gConfigUdma0BlkCopyCqEventCb[chCnt];
            retVal = Udma_eventRegister(drvHandle, cqEventHandle, &cqEventPrms);
            DebugP_assert(UDMA_SOK == retVal);
        }
    }

    return;
}

static void Drivers_udmaConfigUdma0BlkCopyClose(void)
{
    int32_t         retVal, tempRetVal;
    uint32_t        chCnt;
    Udma_ChHandle   chHandle;
    uint64_t        pDesc;

    for(chCnt = 0U; chCnt < CONFIG_UDMA0_NUM_BLKCOPY_CH; chCnt++)
    {
        chHandle = &gConfigUdma0BlkCopyChObj[chCnt];

        /* Flush any pending request from the free queue */
        while(1)
        {
            tempRetVal = Udma_ringFlushRaw(
                             Udma_chGetFqRingHandle(chHandle), &pDesc);
            if(UDMA_ETIMEOUT == tempRetVal)
            {
                break;
            }
        }

        /* Unregister completion event */
        if(NULL != gConfigUdma0BlkCopyCqEventHandle[chCnt])
        {
            retVal = Udma_eventUnRegister(gConfigUdma0BlkCopyCqEventHandle[chCnt]);
            DebugP_assert(UDMA_SOK == retVal);
            gConfigUdma0BlkCopyCqEventHandle[chCnt] = NULL;
        }

        /* Close channel */
        retVal = Udma_chClose(chHandle);
        DebugP_assert(UDMA_SOK == retVal);
    }

    return;
}

void Drivers_udmaOpen(void)
{
    Drivers_udmaConfigUdma0BlkCopyOpen();
}

void Drivers_udmaClose(void)
{
    Drivers_udmaConfigUdma0BlkCopyClose();
}

/*
 * UART
 */

/* UART Driver handles */
UART_Handle gUartHandle[CONFIG_UART_NUM_INSTANCES];

/* UART Driver Parameters */
UART_Params gUartParams[CONFIG_UART_NUM_INSTANCES] =
{
    {
        .baudRate           = 115200,
        .dataLength         = UART_LEN_8,
        .stopBits           = UART_STOPBITS_1,
        .parityType         = UART_PARITY_NONE,
        .readMode           = UART_TRANSFER_MODE_BLOCKING,
        .readReturnMode     = UART_READ_RETURN_MODE_FULL,
        .writeMode          = UART_TRANSFER_MODE_BLOCKING,
        .readCallbackFxn    = NULL,
        .writeCallbackFxn   = NULL,
        .hwFlowControl      = FALSE,
        .hwFlowControlThr   = UART_RXTRIGLVL_16,
        .transferMode       = UART_CONFIG_MODE_INTERRUPT,
        .skipIntrReg         = FALSE,
        .uartDmaIndex = -1,
        .intrNum            = 23U,
        .eventId            = 441,
        .intrPriority       = 4U,
        .operMode           = UART_OPER_MODE_16X,
        .rxTrigLvl          = UART_RXTRIGLVL_8,
        .txTrigLvl          = UART_TXTRIGLVL_32,
        .rxEvtNum           = 0U,
        .txEvtNum           = 0U,
    },
};


void Drivers_uartOpen(void)
{
    uint32_t instCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_UART_NUM_INSTANCES; instCnt++)
    {
        gUartHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_UART_NUM_INSTANCES; instCnt++)
    {
        gUartHandle[instCnt] = UART_open(instCnt, &gUartParams[instCnt]);
        if(NULL == gUartHandle[instCnt])
        {
            DebugP_logError("UART open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Drivers_uartClose();   /* Exit gracefully */
    }

    return;
}

void Drivers_uartClose(void)
{
    uint32_t instCnt;

    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_UART_NUM_INSTANCES; instCnt++)
    {
            if(gUartHandle[instCnt] != NULL)
            {
                UART_Config        *config;
                config = (UART_Config *)gUartHandle[instCnt];

                if(config->traceInstance != TRUE)
                {
                    UART_close(gUartHandle[instCnt]);
                    gUartHandle[instCnt] = NULL;
                }

            }
    }

    return;
}

