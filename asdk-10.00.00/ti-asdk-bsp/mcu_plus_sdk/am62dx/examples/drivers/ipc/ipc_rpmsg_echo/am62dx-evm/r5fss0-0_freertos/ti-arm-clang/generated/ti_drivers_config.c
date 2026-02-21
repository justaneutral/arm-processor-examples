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
 * OSPI
 */


/* Regions restricted for DMA. We should use CPU memcpy in these cases */
static OSPI_AddrRegion gOspiDmaRestrictRegions[] =
{
    {
        .regionStartAddr = CSL_WKUP_R5FSS0_ATCM_BASE,
        .regionSize      = CSL_WKUP_R5FSS0_ATCM_SIZE,
    },
    {
        .regionStartAddr = CSL_WKUP_R5FSS0_BTCM_BASE,
        .regionSize      = CSL_WKUP_R5FSS0_BTCM_SIZE,
    },
    {
        .regionStartAddr = CSL_MCU_R5FSS0_ATCM_BASE,
        .regionSize      = CSL_MCU_R5FSS0_ATCM_SIZE,
    },
    {
        .regionStartAddr = CSL_MCU_R5FSS0_BTCM_BASE,
        .regionSize      = CSL_MCU_R5FSS0_BTCM_SIZE,
    },
    {
        .regionStartAddr = 0xFFFFFFFFU,
        .regionSize      = 0U,
    }
};

/* OSPI attributes */
static OSPI_Attrs gOspiAttrs[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        .baseAddr             = CSL_FSS0_OSPI0_CTRL_BASE,
        .dataBaseAddr         = CSL_FSS0_DAT_REG1_BASE,
        .inputClkFreq         = 166666666U,
        .intrNum              = 171U,
        .intrEnable           = FALSE,
        .intrPriority         = 4U,
        .dmaEnable            = FALSE,
        .phyEnable            = FALSE,
        .phySkipTuning        = FALSE,
        .dacEnable            = FALSE,
        .chipSelect           = OSPI_CS0,
        .frmFmt               = OSPI_FF_POL0_PHA0,
        .decChipSelect        = OSPI_DECODER_SELECT4,
        .baudRateDiv          = 4,
        .phaseDelayElement    = 3,
        .dmaRestrictedRegions = gOspiDmaRestrictRegions,
    },
};
/* OSPI objects - initialized by the driver */
static OSPI_Object gOspiObjects[CONFIG_OSPI_NUM_INSTANCES];
/* OSPI driver configuration */
OSPI_Config gOspiConfig[CONFIG_OSPI_NUM_INSTANCES] =
{
    {
        &gOspiAttrs[CONFIG_OSPI_SBL],
        &gOspiObjects[CONFIG_OSPI_SBL],
    },
};

uint32_t gOspiConfigNum = CONFIG_OSPI_NUM_INSTANCES;

#include <drivers/ospi/v0/dma/ospi_dma.h>
OSPI_DmaConfig gOspiDmaConfig[CONFIG_OSPI_NUM_DMA_INSTANCES] =
{
};

uint32_t gOspiDmaConfigNum = CONFIG_OSPI_NUM_DMA_INSTANCES;


/*
 * BOOTLOADER
 */
/* Include ti_board_config.h for flash config macros */
#include "ti_board_config.h"
#include "drivers/bootloader/bootloader_mmcsd_raw.h"

/* Bootloader boot media specific arguments */
Bootloader_FlashArgs gBootloader0Args =
{
    .flashIndex     = CONFIG_FLASH_SBL,
    .curOffset      = 0,
    .appImageOffset = 0x240000,
    .flashType      = CONFIG_FLASH_TYPE_SERIAL_NOR,
};
Bootloader_FlashArgs gBootloader1Args =
{
    .flashIndex     = CONFIG_FLASH_SBL,
    .curOffset      = 0,
    .appImageOffset = 0x1200000,
    .flashType      = CONFIG_FLASH_TYPE_SERIAL_NOR,
};
Bootloader_FlashArgs gBootloader2Args =
{
    .flashIndex     = CONFIG_FLASH_SBL,
    .curOffset      = 0,
    .appImageOffset = 0xA00000,
    .flashType      = CONFIG_FLASH_TYPE_SERIAL_NOR,
};
Bootloader_FlashArgs gBootloader3Args =
{
    .flashIndex     = CONFIG_FLASH_SBL,
    .curOffset      = 0,
    .appImageOffset = 0x800000,
    .flashType      = CONFIG_FLASH_TYPE_SERIAL_NOR,
};
Bootloader_MmcsdArgs gBootloader4Args =
{
    .MMCSDIndex      = CONFIG_MMCSD_SBL,
    .curOffset       = 0,
    .appImageOffset  = 0x240000,
};
Bootloader_MmcsdArgs gBootloader5Args =
{
    .MMCSDIndex      = CONFIG_MMCSD_SBL,
    .curOffset       = 0,
    .appImageOffset  = 0x1200000,
};
Bootloader_MmcsdArgs gBootloader6Args =
{
    .MMCSDIndex      = CONFIG_MMCSD_SBL,
    .curOffset       = 0,
    .appImageOffset  = 0xA00000,
};
Bootloader_MmcsdArgs gBootloader7Args =
{
    .MMCSDIndex      = CONFIG_MMCSD_SBL,
    .curOffset       = 0,
    .appImageOffset  = 0x800000,
};

/* Configuration option for lockstep or standalone */
void* operatingMode = NULL;


/* Bootloader DMA attributes */
#include <drivers/udma.h>
#include <drivers/bootloader/bootloader_dma.h>

BootloaderDma_UdmaArgs gBootloaderUdmaArgs =
{
    .drvHandle     = NULL,
    .chHandle      = NULL,
    .trpdMem       = NULL,
    .trpdMemSize   = 0,
    .ringMem       = NULL,
    .ringMemSize   = 0,
    .ringElemCount = 0,
    .restrictedRegions = NULL,
};

/* Arguments to be used for driver implementation callbacks when boot media is SOC memory */
Bootloader_MemArgs gMemBootloaderArgs =
{
    .curOffset = 0,
    .appImageBaseAddr = 0,
    .enableDmaTransfer = false,
    .bootloaderDma_UdmaArgs = &gBootloaderUdmaArgs,
};

/* Bootloader driver configuration */
Bootloader_Config gBootloaderConfig[CONFIG_BOOTLOADER_NUM_INSTANCES] =
{
    {
        &gBootloaderFlashFxns,
        &gBootloader0Args,
        BOOTLOADER_MEDIA_FLASH,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderFlashFxns,
        &gBootloader1Args,
        BOOTLOADER_MEDIA_FLASH,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderFlashFxns,
        &gBootloader2Args,
        BOOTLOADER_MEDIA_FLASH,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderFlashFxns,
        &gBootloader3Args,
        BOOTLOADER_MEDIA_FLASH,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderMmcsdFxns,
        &gBootloader4Args,
        BOOTLOADER_MEDIA_EMMC,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderMmcsdFxns,
        &gBootloader5Args,
        BOOTLOADER_MEDIA_EMMC,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderMmcsdFxns,
        &gBootloader6Args,
        BOOTLOADER_MEDIA_EMMC,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
    {
        &gBootloaderMmcsdFxns,
        &gBootloader7Args,
        BOOTLOADER_MEDIA_EMMC,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
    },
};

/* Bootloader driver configuration specifically for SOC memory */
Bootloader_Config gMemBootloaderConfig =
{
        &gBootloaderMemFxns,
        &gMemBootloaderArgs,
        BOOTLOADER_MEDIA_MEM,
        0,
        0,
        NULL,
        .socCoreOpMode= (void *)&operatingMode,
        FALSE,
};

uint32_t gBootloaderConfigNum = CONFIG_BOOTLOADER_NUM_INSTANCES;

/*
 * IPC Notify
 */
#include <drivers/ipc_notify.h>
#include <drivers/ipc_notify/v0/ipc_notify_v0.h>

/* this function is called within IpcNotify_init, this function returns core specific IPC config */
void IpcNotify_getConfig(IpcNotify_InterruptConfig **interruptConfig, uint32_t *interruptConfigNum)
{
    /* extern globals that are specific to this core */
    extern IpcNotify_InterruptConfig gIpcNotifyInterruptConfig_r5fss0_0[];
    extern uint32_t gIpcNotifyInterruptConfigNum_r5fss0_0;

    *interruptConfig = &gIpcNotifyInterruptConfig_r5fss0_0[0];
    *interruptConfigNum = gIpcNotifyInterruptConfigNum_r5fss0_0;
}

/*
 * IPC RP Message
 */
#include <drivers/ipc_rpmsg.h>

/* Number of CPUs that are enabled for IPC RPMessage */
#define IPC_RPMESSAGE_NUM_CORES           (4U)
/* Number of VRINGs for the numner of CPUs that are enabled for IPC */
#define IPC_RPMESSAGE_NUM_VRINGS          (IPC_RPMESSAGE_NUM_CORES*(IPC_RPMESSAGE_NUM_CORES-1))
/* Number of a buffers in a VRING, i.e depth of VRING queue */
#define IPC_RPMESSAGE_NUM_VRING_BUF       (8U)
/* Max size of a buffer in a VRING */
#define IPC_RPMESSAGE_MAX_VRING_BUF_SIZE  (128U)

/* Size of each VRING is
 *     2 x number of buffers x size of each buffer
 */
#define IPC_RPMESSAGE_VRING_SIZE          RPMESSAGE_VRING_SIZE_PDK(IPC_RPMESSAGE_NUM_VRING_BUF, IPC_RPMESSAGE_MAX_VRING_BUF_SIZE)


/* VRING base address, all VRINGs are put one after other in the below region.
 *
 * IMPORTANT: Make sure of below,
 * - The section defined below should be placed at the exact same location in memory for all the CPUs
 * - The memory should be marked as non-cached for all the CPUs
 * - The section should be marked as NOLOAD in all the CPUs linker command file
 */
/* In this case gRPMessageVringMem size is 24576 bytes */
uint8_t gRPMessageVringMem[IPC_RPMESSAGE_NUM_VRINGS][IPC_RPMESSAGE_VRING_SIZE] __attribute__((aligned(128), section(".bss.ipc_vring_mem")));



/*
 * MMCSD
 */

/* MMCSD attributes */
MMCSD_Attrs gMmcsdAttrs[CONFIG_MMCSD_NUM_INSTANCES] =
{
    {
        .ctrlBaseAddr         = CSL_MMCSD0_CTL_CFG_BASE,
        .ssBaseAddr           = CSL_MMCSD0_SS_CFG_BASE,
        .inputClkFreq         = 200000000U,
        .intrNum              = 161U,
        .intrEnable           = FALSE,
        .enableDma            = TRUE,
        .phyType              = MMCSD_PHY_TYPE_SW_PHY,
        .cardType             = MMCSD_CARD_TYPE_EMMC,
        .busWidth             = MMCSD_BUS_WIDTH_8BIT,
        .supportedModes       = MMCSD_SUPPORT_MMC_DS | MMCSD_SUPPORT_MMC_HS200,
        .tuningType           = MMCSD_PHY_TUNING_TYPE_AUTO,
    },
};
/* MMCSD objects - initialized by the driver */
static MMCSD_Object gMmcsdObjects[CONFIG_MMCSD_NUM_INSTANCES];
/* MMCSD driver configuration */
MMCSD_Config gMmcsdConfig[CONFIG_MMCSD_NUM_INSTANCES] =
{
    {
        &gMmcsdAttrs[CONFIG_MMCSD_SBL],
        &gMmcsdObjects[CONFIG_MMCSD_SBL],
    },
};

uint32_t gMmcsdConfigNum = CONFIG_MMCSD_NUM_INSTANCES;

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
    {
        .baseAddr           = CSL_WKUP_UART0_BASE,
        .inputClkFreq       = 48000000U,
    },
};
/* UART objects - initialized by the driver */
static UART_Object gUartObjects[CONFIG_UART_NUM_INSTANCES];
/* UART driver configuration */
UART_Config gUartConfig[CONFIG_UART_NUM_INSTANCES] =
{
    {
        .attrs = &gUartAttrs[CONFIG_UART_SBL],
        .object = &gUartObjects[CONFIG_UART_SBL],
        .traceInstance = FALSE,
    },
    {
        .attrs = &gUartAttrs[CONFIG_UART_APP],
        .object = &gUartObjects[CONFIG_UART_APP],
        .traceInstance = TRUE,
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
    /* We should do sciclient init before we enable power and clock to the peripherals */
    /* SCICLIENT init */
    {

        int32_t retVal = SystemP_SUCCESS;

        retVal = Sciclient_direct_init();
        DebugP_assertNoLog(SystemP_SUCCESS == retVal);

    }

   


    PowerClock_init();
    /* Now we can do pinmux */
    Pinmux_init();
    /* finally we initialize all peripheral drivers */
    Drivers_uartInit();
    OSPI_init();
    /* IPC Notify */
    {
        IpcNotify_Params notifyParams;
        int32_t status;

        /* initialize parameters to default */
        IpcNotify_Params_init(&notifyParams);

        /* specify the core on which this API is called */
        notifyParams.selfCoreId = CSL_CORE_ID_R5FSS0_0;

        /* list the cores that will do IPC Notify with this core
        * Make sure to NOT list 'self' core in the list below
        */
        notifyParams.numCores = 3;
        notifyParams.coreIdList[0] = CSL_CORE_ID_A53SS0_0;
        notifyParams.coreIdList[1] = CSL_CORE_ID_C75SS0_0;
        notifyParams.coreIdList[2] = CSL_CORE_ID_MCU_R5FSS0_0;

        /* initialize the IPC Notify module */
        status = IpcNotify_init(&notifyParams);
        DebugP_assert(status==SystemP_SUCCESS);

    }
    /* IPC RPMessage */
    {
        RPMessage_Params rpmsgParams;
        int32_t status;

        /* initialize parameters to default */
        RPMessage_Params_init(&rpmsgParams);

        /* VRING mapping from source core to destination core, '-1' means NO VRING,
            a53ss0_0 => {"a53ss0_0":-1,"r5fss0_0":0,"c75ss0_0":1,"mcu_r5fss0_0":2}
            r5fss0_0 => {"a53ss0_0":3,"r5fss0_0":-1,"c75ss0_0":4,"mcu_r5fss0_0":5}
            c75ss0_0 => {"a53ss0_0":6,"r5fss0_0":7,"c75ss0_0":-1,"mcu_r5fss0_0":8}
            mcu_r5fss0_0 => {"a53ss0_0":9,"r5fss0_0":10,"c75ss0_0":11,"mcu_r5fss0_0":-1}
         */
        /* TX VRINGs */
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_A53SS0_0] = (uintptr_t)gRPMessageVringMem[3];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_C75SS0_0] = (uintptr_t)gRPMessageVringMem[4];
        rpmsgParams.vringTxBaseAddr[CSL_CORE_ID_MCU_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[5];
        /* RX VRINGs */
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_A53SS0_0] = (uintptr_t)gRPMessageVringMem[0];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_C75SS0_0] = (uintptr_t)gRPMessageVringMem[7];
        rpmsgParams.vringRxBaseAddr[CSL_CORE_ID_MCU_R5FSS0_0] = (uintptr_t)gRPMessageVringMem[10];
        /* Other VRING properties */
        rpmsgParams.vringSize = IPC_RPMESSAGE_VRING_SIZE;
        rpmsgParams.vringNumBuf = IPC_RPMESSAGE_NUM_VRING_BUF;
        rpmsgParams.vringMsgSize = IPC_RPMESSAGE_MAX_VRING_BUF_SIZE;


        /* initialize the IPC RP Message module */
        status = RPMessage_init(&rpmsgParams);
        DebugP_assert(status==SystemP_SUCCESS);
    }

    MMCSD_init();
}

void System_deinit(void)
{
    OSPI_deinit();
    RPMessage_deInit();
    IpcNotify_deInit();

    MMCSD_deinit();
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
