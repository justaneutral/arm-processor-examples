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

#include "ti_drivers_config.h"
#include <drivers/sciclient.h>

/*
 * UDMA
 */
/* UDMA driver instance object */
Udma_DrvObject          gUdmaDrvObj[CONFIG_UDMA_NUM_INSTANCES];
/* UDMA driver instance init params */
static Udma_InitPrms    gUdmaInitPrms[CONFIG_UDMA_NUM_INSTANCES] =
{
    {
        .instId             = UDMA_INST_ID_PKTDMA_0,
        .skipGlobalEventReg = FALSE,
        .virtToPhyFxn       = Udma_defaultVirtToPhyFxn,
        .phyToVirtFxn       = Udma_defaultPhyToVirtFxn,
    },
};

/*
 * UART
 */

/* UART atrributes */
static UART_Attrs gUartAttrs[CONFIG_UART_NUM_INSTANCES] =
{
    {
        .baseAddr           = CSL_UART1_BASE,
        .inputClkFreq       = 48000000U,
    },
};
/* UART objects - initialized by the driver */
static UART_Object gUartObjects[CONFIG_UART_NUM_INSTANCES];
/* UART driver configuration */
UART_Config gUartConfig[CONFIG_UART_NUM_INSTANCES] =
{
    {
        .attrs = &gUartAttrs[CONFIG_UART_CONSOLE],
        .object = &gUartObjects[CONFIG_UART_CONSOLE],
        .traceInstance = TRUE,
    },
};

uint32_t gUartConfigNum = CONFIG_UART_NUM_INSTANCES;

#include <drivers/uart/v0/dma/uart_dma.h>
#include <drivers/uart/v0/dma/udma/uart_dma_udma.h>
#include <drivers/udma.h>

/*
 * Ring parameters
 */
/** \brief Number of ring entries - we can prime this much UART operations */
#define UART_UDMA_TEST_RING_ENTRIES          (1U)
/** \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit) */
#define UART_UDMA_TEST_RING_ENTRY_SIZE       (sizeof(uint64_t))
/** \brief Total ring memory */
#define UART_UDMA_TEST_RING_MEM_SIZE         (UART_UDMA_TEST_RING_ENTRIES * UART_UDMA_TEST_RING_ENTRY_SIZE)
/** \brief UDMA host mode buffer descriptor memory size. */
#define UART_UDMA_TEST_DESC_SIZE             (sizeof(CSL_UdmapCppi5HMPD))


/* UART UDMA TX Channel Objects */
static Udma_ChObject gUart0UdmaTxObj;

/* UART UDMA RX Channel Objects */
static Udma_ChObject gUart0UdmaRxObj;

/**< UDMA TX completion queue object */
static Udma_EventObject        gUart0UdmaCqTxEventObj;
/**< UDMA RX completion queue object */
static Udma_EventObject        gUart0UdmaCqRxEventObj;

/* UART UDMA Channel Ring Mem */
static uint8_t gUart0UdmaRxRingMem[UART_UDMA_TEST_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gUart0UdmaTxRingMem[UART_UDMA_TEST_RING_MEM_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

/* UART UDMA Channel HPD Mem */
static uint8_t gUart0UdmaTxHpdMem[UART_UDMA_TEST_DESC_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gUart0UdmaRxHpdMem[UART_UDMA_TEST_DESC_SIZE] __attribute__((aligned(UDMA_CACHELINE_ALIGNMENT)));

UartDma_UdmaArgs gUartUdma0Args =
{
    .drvHandle        = &gUdmaDrvObj[0U],
    .txChHandle       = &gUart0UdmaTxObj,
    .rxChHandle       = &gUart0UdmaRxObj,
    .cqTxEvtHandle    = &gUart0UdmaCqTxEventObj,
    .cqRxEvtHandle    = &gUart0UdmaCqRxEventObj,
    .txHpdMem         = &gUart0UdmaTxHpdMem,
    .rxHpdMem         = &gUart0UdmaRxHpdMem,
    .hpdMemSize       = UART_UDMA_TEST_DESC_SIZE,
    .txRingMem        = &gUart0UdmaTxRingMem,
    .rxRingMem        = &gUart0UdmaRxRingMem,
    .ringMemSize      = UART_UDMA_TEST_RING_MEM_SIZE,
    .ringElemCnt      = UART_UDMA_TEST_RING_ENTRIES,
    .isOpen           = FALSE,
};

UART_DmaConfig gUartDmaConfig[CONFIG_UART_NUM_DMA_INSTANCES] =
{
    {
        .fxns        = &gUartDmaUdmaFxns,
        .uartDmaArgs = (void *)&gUartUdma0Args,
    },
};

uint32_t gUartDmaConfigNum = CONFIG_UART_NUM_DMA_INSTANCES;


void Drivers_uartInit(void)
{
    UART_init();
}


void Pinmux_init();
void PowerClock_init(void);
void PowerClock_deinit(void);

/*
 * Common Functions
 */
void System_init(void)
{
    /* DPL init sets up address transalation unit, on some CPUs this is needed
     * to access SCICLIENT services, hence this needs to happen first
     */
    Dpl_init();
    /* We should do sciclient init before we enable power and clock to the peripherals */
    /* SCICLIENT init */
    {

        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_direct_init();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);

    }

   
    /* initialize PMU */
    CycleCounterP_init(SOC_getSelfCpuClk());


    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
    Drivers_uartInit();
    /* UDMA */
    {
        uint32_t        instId;
        int32_t         retVal = UDMA_SOK;

        for(instId = 0U; instId < CONFIG_UDMA_NUM_INSTANCES; instId++)
        {
            retVal += Udma_init(&gUdmaDrvObj[instId], &gUdmaInitPrms[instId]);
            DebugP_assert(UDMA_SOK == retVal);
        }
    }
}

void System_deinit(void)
{
    /* UDMA */
    {
        uint32_t        instId;
        int32_t         retVal = UDMA_SOK;

        for(instId = 0U; instId < CONFIG_UDMA_NUM_INSTANCES; instId++)
        {
            retVal += Udma_deinit(&gUdmaDrvObj[instId]);
            DebugP_assert(UDMA_SOK == retVal);
        }
    }
    UART_deinit();
    PowerClock_deinit();
    /* SCICLIENT deinit */
    {
        int32_t         retVal = SystemP_SUCCESS;

        retVal = Sciclient_deinit();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);
    }
    Dpl_deinit();
}
