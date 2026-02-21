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
 * I2C
 */
/* I2C atrributes */
static I2C_HwAttrs gI2cHwAttrs[CONFIG_I2C_NUM_INSTANCES] =
{
    {
        .baseAddr       = CSL_I2C0_CFG_BASE,
        .intNum         = 193,
        .eventId        = 0,
        .funcClk        = 96000000U,
        .enableIntr     = 1,
        .ownTargetAddr   =
        {
            0x1C,
            0x1C,
            0x1C,
            0x1C,
        },
    },
};
/* I2C objects - initialized by the driver */
static I2C_Object gI2cObjects[CONFIG_I2C_NUM_INSTANCES];
/* I2C driver configuration */
I2C_Config gI2cConfig[CONFIG_I2C_NUM_INSTANCES] =
{
    {
        .object = &gI2cObjects[CONFIG_I2C0],
        .hwAttrs = &gI2cHwAttrs[CONFIG_I2C0]
    },
};

uint32_t gI2cConfigNum = CONFIG_I2C_NUM_INSTANCES;

/*
 * MCASP
 */
/* MCASP atrributes */
static MCASP_Attrs gMcaspAttrs[CONFIG_MCASP_NUM_INSTANCES] =
{



    {
        .instNum               = MCASP2,
        .baseAddr              = (uintptr_t) CSL_MCASP2_CFG_BASE,
        .dataBaseAddr          = (uintptr_t) CSL_MCASP2_DMA_BASE,
        .isSynchronous          = 0U,
        /* MCASP Data port Address  */
        .numOfSerializers = (uint32_t)16,
        /* Serializers available in MCASP */
        .intCfgTx =
        {
            .intrNum = (uint32_t)272,
            .evntNum = (uint32_t)0,
            .intrPriority = 4,
        },
        /* Tx Int params for McASP */
        .intCfgRx =
        {
            .intrNum = (uint32_t)271,
            .evntNum = (uint32_t)0,
            .intrPriority = 3,
        },
        .txSlotSize = (uint32_t)32,
        .rxSlotSize = (uint32_t)32,
        .hwCfg =
        {
            .gbl =
            {
                (uint32_t)0x0,  /* MCASP_PFUNC */
                (uint32_t)0xB400001B,  /* MCASP_PDIR */
                (uint32_t)0x0,  /* MCASP_GBLCTL */
                (uint32_t)0x0,  /* MCASP_TXDITCTL */
                (uint32_t)0x0,  /* MCASP_LBCTL */
                (uint32_t)0x0,  /* MCASP_TXDITCTL */
                {   /* serializer setup (MCASP_XRSRCTLn) */
                    (uint32_t)0x1, /* [0] - Tx */
                    (uint32_t)0x1, /* [1] - Tx */
                    (uint32_t)0x0, /* [2] - Inactive */
                    (uint32_t)0x1, /* [3] - Tx */
                    (uint32_t)0x1, /* [4] - Tx */
                    (uint32_t)0x0, /* [5] - Inactive */
                    (uint32_t)0x0, /* [6] - Inactive */
                    (uint32_t)0x0, /* [7] - Inactive */
                    (uint32_t)0x0, /* [8] - Inactive */
                    (uint32_t)0x0, /* [9] - Inactive */
                    (uint32_t)0x0, /* [10] - Inactive */
                    (uint32_t)0x0, /* [11] - Inactive */
                    (uint32_t)0x0, /* [12] - Inactive */
                    (uint32_t)0x0, /* [13] - Inactive */
                    (uint32_t)0x2, /* [14] - Rx */
                    (uint32_t)0x0, /* [15] - Inactive */
                },
            },
            .rx =
            {
                (uint32_t)0xFFFFFFFFU, /* MCASP_RXMASK */
                (uint32_t)0x180F0U, /* MCASP_RXFMT */
                (uint32_t)0x402U, /* MCASP_RXFMCTL */
                (uint32_t)0xFFU, /* MCASP_RXTDM */
                (uint32_t)0x0U, /* MCASP_EVTCTLR */
                (uint32_t)0xffffU, /* MCASP_RXSTAT */
                (uint32_t)0x1U, /* MCASP_REVTCTL */
                {
                    (uint32_t)0x21U, /* MCASP_ACLKRCTL */
                    (uint32_t)0x0U, /* MCASP_AHCLKRCTL */
                    (uint32_t)0x0, /* MCASP_RXCLKCHK */
                    (uint32_t)0x1,  /* HCLK is external */
                    (uint32_t)3,  /* External HCLK source */
                },
                {
                    (uint32_t)0x12001U, /* RFIFOCTL */
                    (uint32_t)0x0, /* RFIFOSTS */
                },
            },
            .tx =
            {
                (uint32_t)0xFFFFFFFFU, /* MCASP_TXMASK */
                (uint32_t)0x180F0U, /* MCASP_TXFMT */
                (uint32_t)0x113U, /* MCASP_TXFMCTL */
                (uint32_t)0x3U, /* MCASP_TXTDM */
                (uint32_t)0x0U, /* MCASP_EVTCTLX */
                (uint32_t)0xffffU, /* MCASP_TXSTAT */
                (uint32_t)0x1U, /* MCASP_XEVTCTL */
                {
                    (uint32_t)0xE7U, /* MCASP_ACLKXCTL */
                    (uint32_t)0x0U, /* MCASP_AHCLKXCTL */
                    (uint32_t)0x0, /* MCASP_TXCLKCHK */
                    (uint32_t)0x1,  /* HCLK is external */
                    (uint32_t)3, /* External HCLK source */
                },
                {
                    (uint32_t)0x12004U, /* WFIFOCTL */
                    (uint32_t)0x0, /* WFIFOSTS */
                },
            },
        },
    },
};

/* MCASP objects - initialized by the driver */
static MCASP_Object gMcaspObjects[CONFIG_MCASP_NUM_INSTANCES];
/* MCASP driver configuration */
MCASP_Config gMcaspConfig[CONFIG_MCASP_NUM_INSTANCES] =
{
    {
        &gMcaspAttrs[CONFIG_MCASP0],
        &gMcaspObjects[CONFIG_MCASP0],
    },
};

uint32_t gMcaspConfigNum = CONFIG_MCASP_NUM_INSTANCES;

/*
 * UDMA
 */
/* UDMA driver instance object */
Udma_DrvObject          gUdmaDrvObj[CONFIG_UDMA_NUM_INSTANCES];
/* UDMA driver instance init params */
static Udma_InitPrms    gUdmaInitPrms[CONFIG_UDMA_NUM_INSTANCES] =
{
    {
        .instId             = UDMA_INST_ID_BCDMA_0,
        .skipGlobalEventReg = FALSE,
        .virtToPhyFxn       = Udma_defaultVirtToPhyFxn,
        .phyToVirtFxn       = Udma_defaultPhyToVirtFxn,
    },
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
    I2C_init();
    MCASP_init();
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
    I2C_deinit();
    MCASP_deinit();
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
