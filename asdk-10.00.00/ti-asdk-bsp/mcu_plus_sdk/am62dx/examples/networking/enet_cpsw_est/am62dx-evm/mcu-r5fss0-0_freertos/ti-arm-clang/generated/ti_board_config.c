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
#include "ti_board_config.h"

/*
 * Auto generated file
 */


#include <stdint.h>
#include <enet.h>
#include <networking/enet/core/include/phy/enetphy.h>
#include <networking/enet/core/include/phy/dp83867.h>
#include <networking/enet/core/include/phy/dp83869.h>
#include <networking/enet/utils/include/enet_apputils.h>
#include <kernel/dpl/SystemP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include <networking/enet/core/src/phy/enetphy_priv.h>
#include <drivers/gpio.h>
#include <board/eeprom.h>
#include "ti_board_open_close.h"
#include "ti_drivers_config.h"
#include <board/ioexp/ioexp_tca6424.h>

#define CONFIG_ENET_CPSW0_PHY0_ADDR (15U)
#define CONFIG_ENET_CPSW0_PHY1_ADDR (3U)
#define ENET_GET_NUM_MAC_ADDR(num) ((num>>3)+1)
#define ENET_MAC_ADDR_VALIDATE_MASK (0x01U)

#define MSS_CPSW_CONTROL_PORT_MODE_RMII                                   (0x1U)
#define MSS_CPSW_CONTROL_PORT_MODE_RGMII                                  (0x2U)

#include <drivers/i2c.h>

#define IO_EXPANDER_PORT0_OUTPUT_REG (0x04U)
#define IO_EXPANDER_PORT0_DIR_REG (0x0CU)
#define IO_EXPANDER_I2C_ADDR (0x22U)
#define ENET_BOARD_NUM_MACADDR_MAX (4U)
#define I2C_EEPROM_MAC_CTRL_OFFSET (0x40)
#define I2C_EEPROM_MAC_DATA_OFFSET (0x42)

static void EnetBoard_setMacPortIOExpanderCfg(void);

/* PHY drivers */
extern EnetPhy_Drv gEnetPhyDrvGeneric;
extern EnetPhy_Drv gEnetPhyDrvDp83867;

/*! \brief All the registered PHY specific drivers. */
static const EnetPhyDrv_Handle gEnetPhyDrvs[] =
{
    &gEnetPhyDrvDp83867,   /* DP83867 */
    &gEnetPhyDrvGeneric,   /* Generic PHY - must be last */
};

const EnetPhy_DrvInfoTbl gEnetPhyDrvTbl =
{
    .numHandles = ENET_ARRAYSIZE(gEnetPhyDrvs),
    .hPhyDrvList = gEnetPhyDrvs,
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static const EnetBoard_PortCfg *EnetBoard_getPortCfg(const EnetBoard_EthPort *ethPort);

static const EnetBoard_PortCfg *EnetBoard_findPortCfg(const EnetBoard_EthPort *ethPort,
                                                      const EnetBoard_PortCfg *ethPortCfgs,
                                                      uint32_t numEthPorts);

static void EnetBoard_setEnetControl(Enet_Type enetType,
                                     Enet_MacPort macPort,
                                     EnetMacPort_Interface *mii);

static void EnetBoard_enableExternalMux();

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*!
 * \brief Common Processor Board (CPB) board's DP83867 PHY configuration.
 */
 
static const Dp83867_Cfg gEnetCpbBoard_dp83867PhyCfg =
{
/* The delay values are set based on trial and error and not tuned per port of the evm */
    .txClkShiftEn         = true,
    .rxClkShiftEn         = true,
    .txDelayInPs          = 250U,   /* 0.25 ns */
    .rxDelayInPs          = 2000U,  /* 2.00 ns */
    .txFifoDepth          = 4U,
    .impedanceInMilliOhms = 35000,  /* 35 ohms */
    .idleCntThresh        = 4U,     /* Improves short cable performance */
    .gpio0Mode            = DP83867_GPIO0_LED3,
    .gpio1Mode            = DP83867_GPIO1_COL, /* Unused */
    .ledMode              =
    {
        DP83867_LED_LINKED,         /* Unused */
        DP83867_LED_LINKED_100BTX,
        DP83867_LED_RXTXACT,
        DP83867_LED_LINKED_1000BT,
    },
};

/*
 * AM62Dx EVM board configuration.
 *
 * 2 x RGMII PHY connected to am62dx-evm
 */
static const EnetBoard_PortCfg gEnetCpbBoard_am62dx_evm_EthPort[] =
{
    {    /* "CPSW3G" */
        .enetType = ENET_CPSW_3G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 15,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetCpbBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetCpbBoard_dp83867PhyCfg),
        },
        .flags    = 0U,
    },
    {    /* "CPSW3G" */
        .enetType = ENET_CPSW_3G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr         = 3,
            .isStrapped      = false,
            .skipExtendedCfg = false,
            .extendedCfg     = &gEnetCpbBoard_dp83867PhyCfg,
            .extendedCfgSize = sizeof(gEnetCpbBoard_dp83867PhyCfg),
        },
        .flags    = 0U,
    },
};


/*
 * am62dx-evm virtual board used for MAC loopback setup.
 */
static const EnetBoard_PortCfg gEnetVirtBoard_am62dx_evm_EthPort[] =
{
    {    /* RGMII MAC loopback */
        .enetType = ENET_CPSW_3G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* RGMII MAC loopback */
        .enetType = ENET_CPSW_3G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_GMII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* RMII MAC loopback */
        .enetType = ENET_CPSW_3G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_1,
        .mii      = { ENET_MAC_LAYER_MII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
    {    /* RMII MAC loopback */
        .enetType = ENET_CPSW_3G,
        .instId   = 0U,
        .macPort  = ENET_MAC_PORT_2,
        .mii      = { ENET_MAC_LAYER_MII, ENET_MAC_SUBLAYER_REDUCED },
        .phyCfg   =
        {
            .phyAddr = ENETPHY_INVALID_PHYADDR,
        },
        .flags    = 0U,
    },
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

const EnetBoard_PhyCfg *EnetBoard_getPhyCfg(const EnetBoard_EthPort *ethPort)
{
    const EnetBoard_PortCfg *portCfg;

    portCfg = EnetBoard_getPortCfg(ethPort);

    return (portCfg != NULL) ? &portCfg->phyCfg : NULL;
}

static void EnetBoard_enableExternalMux()
{
    /* Enable external MUXes, if any, as per the board design */
}

static const EnetBoard_PortCfg *EnetBoard_getPortCfg(const EnetBoard_EthPort *ethPort)
{
    const EnetBoard_PortCfg *portCfg = NULL;

    if (ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_CPB_ID))
    {
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetCpbBoard_am62dx_evm_EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetCpbBoard_am62dx_evm_EthPort));
    }
    if ((portCfg == NULL) &&
        ENET_NOT_ZERO(ethPort->boardId & ENETBOARD_LOOPBACK_ID))
    {
        portCfg = EnetBoard_findPortCfg(ethPort,
                                        gEnetVirtBoard_am62dx_evm_EthPort,
                                        ENETPHY_ARRAYSIZE(gEnetVirtBoard_am62dx_evm_EthPort));
    }
    return portCfg;
}

static const EnetBoard_PortCfg *EnetBoard_findPortCfg(const EnetBoard_EthPort *ethPort,
                                                      const EnetBoard_PortCfg *ethPortCfgs,
                                                      uint32_t numEthPorts)
{
    const EnetBoard_PortCfg *ethPortCfg = NULL;
    bool found = false;
    uint32_t i;

    for (i = 0U; i < numEthPorts; i++)
    {
        ethPortCfg = &ethPortCfgs[i];

        if ((ethPortCfg->enetType == ethPort->enetType) &&
            (ethPortCfg->instId == ethPort->instId) &&
            (ethPortCfg->macPort == ethPort->macPort) &&
            (ethPortCfg->mii.layerType == ethPort->mii.layerType) &&
            (ethPortCfg->mii.sublayerType == ethPort->mii.sublayerType))
        {
            found = true;
            break;
        }
    }

    return found ? ethPortCfg : NULL;
}

void EnetBoard_getMiiConfig(EnetMacPort_Interface *mii)
{
    mii->layerType      = ENET_MAC_LAYER_GMII;
    mii->variantType    = ENET_MAC_VARIANT_FORCED;
    mii->sublayerType   = ENET_MAC_SUBLAYER_REDUCED;
}

int32_t EnetBoard_setupPorts(EnetBoard_EthPort *ethPorts,
                             uint32_t numEthPorts)
{
    CSL_main_ctrl_mmr_cfg0Regs *regs = (CSL_main_ctrl_mmr_cfg0Regs *)(uintptr_t)CSL_CTRL_MMR0_CFG0_BASE;
    EnetAppUtils_MmrLockState prevLockState;

    DebugP_assert(numEthPorts == 1);
    DebugP_assert(ethPorts->mii.sublayerType == ENET_MAC_SUBLAYER_REDUCED);
    
    EnetBoard_enableExternalMux();
    /* Override the ENET control set by board lib */
    EnetBoard_setEnetControl(ethPorts->enetType, ethPorts->macPort, &ethPorts->mii);
    
    EnetBoard_setMacPortIOExpanderCfg();
    
    prevLockState = EnetAppUtils_mainMmrCtrl(ENETAPPUTILS_MMR_LOCK1, ENETAPPUTILS_UNLOCK_MMR);

    switch(ethPorts->macPort)
    {
        case ENET_MAC_PORT_1:
            CSL_FINS (regs->ENET1_CTRL, MAIN_CTRL_MMR_CFG0_ENET1_CTRL_PORT_MODE_SEL, MSS_CPSW_CONTROL_PORT_MODE_RGMII);
            break;
        case ENET_MAC_PORT_2:
            CSL_FINS (regs->ENET2_CTRL, MAIN_CTRL_MMR_CFG0_ENET2_CTRL_PORT_MODE_SEL, MSS_CPSW_CONTROL_PORT_MODE_RGMII);
            break;
        default:
            DebugP_assert(false);
    }

    if (prevLockState == ENETAPPUTILS_LOCK_MMR)
    {
        EnetAppUtils_mainMmrCtrl(ENETAPPUTILS_MMR_LOCK1, ENETAPPUTILS_LOCK_MMR);
    }

    /* Nothing else to do */
    return ENET_SOK;
}

static void EnetBoard_setEnetControl(Enet_Type enetType,
                                     Enet_MacPort macPort,
                                     EnetMacPort_Interface *mii)
{

}

void EnetBoard_getMacAddrList(uint8_t macAddr[][ENET_MAC_ADDR_LEN],
                              uint32_t maxMacEntries,
                              uint32_t *pAvailMacEntries)
{
/* AM62Dx EEPROM is not flashed with MAC Addressess yet. Update this after EEPROM update */
#if 0
    int32_t status = ENET_SOK;
    uint32_t macAddrCnt;
    uint32_t i;
    uint8_t macAddrBuf[ENET_BOARD_NUM_MACADDR_MAX * ENET_MAC_ADDR_LEN];
    uint8_t numMacMax;
    uint8_t validNumMac = 0;

    status = EEPROM_read(gEepromHandle[CONFIG_EEPROM0],  I2C_EEPROM_MAC_CTRL_OFFSET, &numMacMax, sizeof(uint8_t));
    EnetAppUtils_assert(status == ENET_SOK);
    EnetAppUtils_assert(ENET_GET_NUM_MAC_ADDR(numMacMax) <= ENET_BOARD_NUM_MACADDR_MAX);
    macAddrCnt = EnetUtils_min(ENET_GET_NUM_MAC_ADDR(numMacMax), maxMacEntries);

    EnetAppUtils_assert(pAvailMacEntries != NULL);

    status = EEPROM_read(gEepromHandle[CONFIG_EEPROM0], I2C_EEPROM_MAC_DATA_OFFSET, macAddrBuf, (macAddrCnt * ENET_MAC_ADDR_LEN));
    EnetAppUtils_assert(status == ENET_SOK);

    /* Save only those required to meet the max number of MAC entries */
    /* Validating that the MAC addresses from the EEPROM are not MULTICAST addresses */
    for (i = 0U; i < macAddrCnt; i++)
    {
        if(!(macAddrBuf[i * ENET_MAC_ADDR_LEN] & ENET_MAC_ADDR_VALIDATE_MASK)){
            memcpy(macAddr[validNumMac], &macAddrBuf[i * ENET_MAC_ADDR_LEN], ENET_MAC_ADDR_LEN);
            validNumMac++;
        }
    }

    *pAvailMacEntries = validNumMac;

    if (macAddrCnt == 0U)
    {
        EnetAppUtils_print("EnetBoard_getMacAddrList Failed - IDK not present\n");
        EnetAppUtils_assert(false);
    }
#endif
    *pAvailMacEntries = 0;
}

#define IO_EXP_ADDR    0x22U

static void EnetBoard_setMacPortIOExpanderCfg(void)
{

}
/*
 * Get ethernet board id
 */
uint32_t EnetBoard_getId(void)
{
    return ENETBOARD_AM62AX_EVM;
}



void Board_init(void)
{
}

void Board_deinit(void)
{
}
