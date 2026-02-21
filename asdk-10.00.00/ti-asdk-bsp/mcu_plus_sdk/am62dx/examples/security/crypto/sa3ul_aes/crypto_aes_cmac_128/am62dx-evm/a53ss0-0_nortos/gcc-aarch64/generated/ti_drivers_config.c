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
 * SA3UL
 */
/* SA3UL Instance Macros */
#define CONFIG_SA3UL_CONSOLE            (0U)
#define CONFIG_SA3UL_NUM_INSTANCES      (1U)

#define SA3UL_HOST_DESCR_SIZE           (128U)
#define SA3UL_RING_ELSIZE_BYTES         (sizeof(uint64_t))
#define SA3UL_RING_CNT                  (2U)

#define SA3UL_RING_MEM_SIZE             (SA3UL_RING_ELSIZE_BYTES * SA3UL_RING_N_ELEMS)
#define SA3UL_RING_BACKING_MEM_SIZE     ((SA3UL_NUMBER_OF_RINGS + 1U) * SA3UL_RING_MEM_SIZE)
#define SA3UL_UDMA_DESCRITPOR_MEM_SIZE  (SA3UL_HOST_DESCR_SIZE * SA3UL_RING_N_ELEMS * 2U)

#define SA3UL_ENGINE_ENC                (1U)
#define SA3UL_ENGINE_AUTH               (1U)
#define SA3UL_ENGINE_TRNG               (0U)
#define SA3UL_ENGINE_PKA                (0U)
#define SA3UL_ENGINE_ENABLE             (SA3UL_ENGINE_ENC | (SA3UL_ENGINE_AUTH << 1) | \
                                            (SA3UL_ENGINE_TRNG << 3) | (SA3UL_ENGINE_PKA << 4))

static uint8_t gMemTxRing[SA3UL_RING_ELSIZE_BYTES * SA3UL_RING_CNT * 2] __attribute__ ((aligned (UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gMemRxRing[SA3UL_RING_ELSIZE_BYTES * SA3UL_RING_CNT * 2] __attribute__ ((aligned (UDMA_CACHELINE_ALIGNMENT)));
static uint8_t gMemDescr[SA3UL_UDMA_DESCRITPOR_MEM_SIZE] __attribute__ ((aligned (UDMA_CACHELINE_ALIGNMENT)));

/* SA3UL atrributes */
SA3UL_Attrs gSA3ULAttrs[CONFIG_SA3UL_NUM_INSTANCES] =
{
    {
        .saBaseAddr         = CSL_SA3_SS0_REGS_BASE,
        .txPsilThreadId     = TX_PSIL_THREAD_ID,
        .rxPsil0ThreadId    = RX_PSIL0_THREAD_ID,
        .rxPsil1ThreadId    = RX_PSIL1_THREAD_ID,
        .numDesc            = 0,
        .descSize           = SA3UL_HOST_DESCR_SIZE,
        .descMemSize        = SA3UL_UDMA_DESCRITPOR_MEM_SIZE,
        .udmaSaTxGroupNum   = UDMA_MAPPED_TX_GROUP_SAUL,
        .udmaSaRxGroupNum   = UDMA_MAPPED_RX_GROUP_SAUL,
        .ringCnt            = SA3UL_RING_CNT,
        .contextIdStart     = 0x80,
        .contextIdMaxNum    = 16,
        .txRingNumInt       = 0,
        .rxRingNumInt       = 1,
        .swRingNumInt       = 2,
        .privId             = 0xc3,
        .priv               = 0,
        .secure             = 0,
    },
};

PKA_Attrs gPKA_Attrs[CONFIG_SA3UL_NUM_INSTANCES] =
{
    {
        .caBaseAddr         = CSL_SA3_SS0_REGS_BASE,
        .pkaBaseAddr        = CSL_SA3_SS0_EIP_29T2_BASE,
        .isOpen             = FALSE,
    },
};

/* SA3UL objects - initialized by the driver */
static SA3UL_Object gSA3ULObjects[CONFIG_SA3UL_NUM_INSTANCES];

/* SA3UL driver configuration */
SA3UL_Config gSa3ulConfig[CONFIG_SA3UL_NUM_INSTANCES] =
{
    {
        &gSA3ULAttrs[CONFIG_SA3UL_CONSOLE],
        &gSA3ULObjects[CONFIG_SA3UL_CONSOLE],
    },
};

PKA_Config gPkaConfig[CONFIG_SA3UL_NUM_INSTANCES] =
{
    {
        &gPKA_Attrs[CONFIG_SA3UL_CONSOLE],
    },
};

uint32_t gSa3ulConfigNum = CONFIG_SA3UL_NUM_INSTANCES;
uint32_t gPkaConfigNum   = CONFIG_SA3UL_NUM_INSTANCES;
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
        .baseAddr           = CSL_UART0_BASE,
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
        .traceInstance = FALSE,
    },
};

uint32_t gUartConfigNum = CONFIG_UART_NUM_INSTANCES;

#include <drivers/uart/v0/dma/uart_dma.h>


UART_DmaConfig gUartDmaConfig[CONFIG_UART_NUM_DMA_INSTANCES] =
{
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
    if(0 == Armv8_getCoreId())
    {
    /* We should do sciclient init before we enable power and clock to the peripherals */
    /* SCICLIENT init */
    {

        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_init(CSL_CORE_ID_A53SS0_0);
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);

    }

   
    /* initialize PMU */
    CycleCounterP_init(SOC_getSelfCpuClk());


    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
    Drivers_uartInit();
    Crypto_init();
    for (int i=0; i <CONFIG_SA3UL_NUM_INSTANCES; i++)
    {
        gSA3ULAttrs[i].rxRingMemAddr     = (uint32_t)&gMemRxRing[0];
        gSA3ULAttrs[i].txRingMemAddr     = (uint32_t)&gMemTxRing[0];
        gSA3ULAttrs[i].descMemAddr       = (uint32_t)&gMemDescr[0];
        gSA3ULAttrs[i].udmaHandle        = (uint32_t)&gUdmaDrvObj[0];

    }

    SA3UL_init();
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

}

void System_deinit(void)
{
    if(0 == Armv8_getCoreId())
    {
    Crypto_deinit();
    SA3UL_deinit();
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
    }

    Dpl_deinit();
}
