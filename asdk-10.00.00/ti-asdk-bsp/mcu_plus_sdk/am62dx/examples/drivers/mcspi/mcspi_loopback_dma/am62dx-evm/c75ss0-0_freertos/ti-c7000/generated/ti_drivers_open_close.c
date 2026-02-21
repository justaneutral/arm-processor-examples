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
    Drivers_mcspiOpen();
    Drivers_udmaOpen();
}

void Drivers_close(void)
{
    Drivers_mcspiClose();
    Drivers_udmaClose();
    Drivers_uartClose();
}

/*
 * MCSPI
 */
/* MCSPI Driver handles */
MCSPI_Handle gMcspiHandle[CONFIG_MCSPI_NUM_INSTANCES];
/* MCSPI Driver Open Parameters */
MCSPI_OpenParams gMcspiOpenParams[CONFIG_MCSPI_NUM_INSTANCES] =
{
    {
        .transferMode           = MCSPI_TRANSFER_MODE_BLOCKING,
        .transferTimeout        = SystemP_WAIT_FOREVER,
        .transferCallbackFxn    = NULL,
        .msMode                 = MCSPI_MS_MODE_MASTER,
        .mcspiDmaIndex = 0,
    },
};
/* MCSPI Driver Channel Configurations */
MCSPI_ChConfig gConfigMcspi0ChCfg[CONFIG_MCSPI0_NUM_CH] =
{
    {
        .chNum              = MCSPI_CHANNEL_0,
        .frameFormat        = MCSPI_FF_POL0_PHA0,
        .bitRate            = 1000000,
        .csPolarity         = MCSPI_CS_POL_LOW,
        .trMode             = MCSPI_TR_MODE_TX_RX,
        .inputSelect        = MCSPI_IS_D0,
        .dpe0               = MCSPI_DPE_ENABLE,
        .dpe1               = MCSPI_DPE_DISABLE,
        .slvCsSelect        = MCSPI_SLV_CS_SELECT_0,
        .startBitEnable     = FALSE,
        .startBitPolarity   = MCSPI_SB_POL_LOW,
        .csIdleTime         = MCSPI_TCS0_0_CLK,
        .defaultTxData      = 0x0U,
        .txFifoTrigLvl      = 16U,
        .rxFifoTrigLvl      = 16U,
    },
};

#include <drivers/mcspi/v0/dma/mcspi_dma.h>
#include <drivers/mcspi/v0/dma/udma/mcspi_dma_udma.h>
#include <drivers/udma.h>

/*
 * Ring parameters
 */
/** \brief Number of ring entries - we can prime this much ADC operations */
#define MCSPI_UDMA_TEST_RING_ENTRIES          (1U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define MCSPI_UDMA_TEST_RING_ENTRY_SIZE       (sizeof(uint64_t))
/** \brief Total ring memory */
#define MCSPI_UDMA_TEST_RING_MEM_SIZE         (MCSPI_UDMA_TEST_RING_ENTRIES * MCSPI_UDMA_TEST_RING_ENTRY_SIZE)
/** \brief UDMA host mode buffer descriptor memory size. */
#define MCSPI_UDMA_TEST_DESC_SIZE             (sizeof(CSL_UdmapCppi5HMPD))



/* MCSPI Driver DMA Channel Configurations */
/* MCSPI UDMA TX Channel Objects */
static Udma_ChObject gMcspi0UdmaTxChObj0;

/* MCSPI UDMA RX Channel Objects */
static Udma_ChObject gMcspi0UdmaRxChObj0;

/**< UDMA TX completion queue object */
static Udma_EventObject        gMcspi0UdmaCqTxEventObjCh0;
/**< UDMA RX completion queue object */
static Udma_EventObject        gMcspi0UdmaCqRxEventObjCh0;

/* MCSPI UDMA Channel Ring Mem */
static uint8_t gMcspi0UdmaRxRingMemCh0[MCSPI_UDMA_TEST_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gMcspi0UdmaTxRingMemCh0[MCSPI_UDMA_TEST_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/* MCSPI UDMA Channel HPD Mem */
static uint8_t gMcspi0UdmaTxHpdMemCh0[MCSPI_UDMA_TEST_DESC_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gMcspi0UdmaRxHpdMemCh0[MCSPI_UDMA_TEST_DESC_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));


MCSPI_DmaChConfig gConfigMcspi0DmaChCfg[CONFIG_MCSPI0_NUM_CH] =
{
{
    .txChHandle       = &gMcspi0UdmaTxChObj0,
    .rxChHandle       = &gMcspi0UdmaRxChObj0,
    .cqTxEvtHandle    = &gMcspi0UdmaCqTxEventObjCh0,
    .cqRxEvtHandle    = &gMcspi0UdmaCqRxEventObjCh0,
    .txHpdMem         = &gMcspi0UdmaTxHpdMemCh0,
    .rxHpdMem         = &gMcspi0UdmaRxHpdMemCh0,
    .hpdMemSize       = MCSPI_UDMA_TEST_DESC_SIZE,
    .txRingMem        = &gMcspi0UdmaTxRingMemCh0,
    .rxRingMem        = &gMcspi0UdmaRxRingMemCh0,
    .ringMemSize      = MCSPI_UDMA_TEST_RING_MEM_SIZE,
    .ringElemCnt      = MCSPI_UDMA_TEST_RING_ENTRIES,
    .rxEvtNum         = UDMA_PDMA_CH_MAIN0_MCSPI1_CH0_RX,
    .txEvtNum         = UDMA_PDMA_CH_MAIN0_MCSPI1_CH0_TX,
    .isOpen           = FALSE,
},

};


void Drivers_mcspiOpen(void)
{
    uint32_t instCnt, chCnt;
    int32_t  status = SystemP_SUCCESS;

    for(instCnt = 0U; instCnt < CONFIG_MCSPI_NUM_INSTANCES; instCnt++)
    {
        gMcspiHandle[instCnt] = NULL;   /* Init to NULL so that we can exit gracefully */
    }

    /* Open all instances */
    for(instCnt = 0U; instCnt < CONFIG_MCSPI_NUM_INSTANCES; instCnt++)
    {
        gMcspiHandle[instCnt] = MCSPI_open(instCnt, &gMcspiOpenParams[instCnt]);
        if(NULL == gMcspiHandle[instCnt])
        {
            DebugP_logError("MCSPI open failed for instance %d !!!\r\n", instCnt);
            status = SystemP_FAILURE;
            break;
        }
    }

    /* Channel configuration */
    for(chCnt = 0U; chCnt < CONFIG_MCSPI0_NUM_CH; chCnt++)
    {
        status = MCSPI_chConfig(
                     gMcspiHandle[CONFIG_MCSPI0],
                     &gConfigMcspi0ChCfg[chCnt]);
        if(status != SystemP_SUCCESS)
        {
            DebugP_logError("CONFIG_MCSPI0 channel %d config failed !!!\r\n", chCnt);
            break;
        }
        status = MCSPI_dmaChConfig(
                     gMcspiHandle[CONFIG_MCSPI0],
                     &gConfigMcspi0ChCfg[chCnt],
                     &gConfigMcspi0DmaChCfg[chCnt]);
        if(status != SystemP_SUCCESS)
        {
            DebugP_logError("CONFIG_MCSPI0 DMA channel %d config failed !!!\r\n", chCnt);
            break;
        }
    }

    if(SystemP_FAILURE == status)
    {
        Drivers_mcspiClose();   /* Exit gracefully */
    }

    return;
}

void Drivers_mcspiClose(void)
{
    uint32_t instCnt;
    int32_t status, chCnt;
    for(chCnt = 0U; chCnt < CONFIG_MCSPI0_NUM_CH; chCnt++)
    {
        status = MCSPI_dmaClose(gMcspiHandle[CONFIG_MCSPI0],
                                &gConfigMcspi0ChCfg[chCnt]);
        if(status != SystemP_SUCCESS)
        {
            DebugP_logError("CONFIG_MCSPI0 DMA close %d failed !!!\r\n", chCnt);
            break;
        }
    }
    /* Close all instances that are open */
    for(instCnt = 0U; instCnt < CONFIG_MCSPI_NUM_INSTANCES; instCnt++)
    {
        if(gMcspiHandle[instCnt] != NULL)
        {
            MCSPI_close(gMcspiHandle[instCnt]);
            gMcspiHandle[instCnt] = NULL;
        }
    }

    return;
}

/*
 * UDMA
 */

void Drivers_udmaOpen(void)
{
}

void Drivers_udmaClose(void)
{
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

