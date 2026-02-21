/*
 *  Copyright (c) Texas Instruments Incorporated 2022-23
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

/*!
 * \file  enet_setup.c
 *
 * \brief This file contains Enet intialization
 *
 */
/* ========================================================================== */
/*                              Include Files                                 */
/* ========================================================================== */

#include <stdint.h>
#include <enet.h>
#include <tsn_combase/combase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_gptp/gptpman.h>
#include <tsn_gptp/tilld/lld_gptp_private.h>
#include <tsn_gptp/gptpconf/gptpgcfg.h>
#include <tsn_gptp/gptpconf/xl4-extmod-xl4gptp.h>
#include <tsn_uniconf/yangs/ieee1588-ptp-tt_access.h>

#include <tsn_uniconf/uc_dbal.h>
#include <enet_apputils.h>
#include <enet_appmemutils.h>
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"

#include "ti_board_config.h"
#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"
#include "ti_enet_open_close.h"
#include "ti_enet_config.h"
#include "ti_enet_lwipif.h"
#include "enet_tcpserver.h"
#include "enet_setup.h"


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

extern int EnetApp_avtpInit(EnetApp_ModuleCtx_t *modCtxTbl);
extern void EnetApp_avtpDeinit(void);

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


EnetApp_Cfg gEnetAppCfg =
{
    .name = ENETAPP_DEFAULT_CFG_NAME,
};

/* these vars are shared with gptp task to configure gptp, put it in the global mem */
static char g_netdevices[MAX_NUM_MAC_PORTS][CB_MAX_NETDEVNAME] = {0};

static const uint8_t BROADCAST_MAC_ADDRESS[ENET_MAC_ADDR_LEN] = 
{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

/* dhcp struct for the ethernet netif */
static struct dhcp g_netifDhcp[ENET_SYSCFG_NETIF_COUNT];
static struct netif *g_pNetif[ENET_SYSCFG_NETIF_COUNT];

/* Handle to the Application interface for the LwIPIf Layer
 */
LwipifEnetApp_Handle hlwipIfApp = NULL;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Logger_onConsoleOut sDrvConsoleOut;

int EnetApp_lldCfgUpdateCb(cb_socket_lldcfg_update_t *update_cfg)
{
    if (update_cfg->proto == ETH_P_1588)
    {
        update_cfg->numRxChannels = 1;
        update_cfg->dmaTxChId = ENET_DMA_TX_CH_PTP;
        update_cfg->dmaRxChId[0] = ENET_DMA_RX_CH_PTP;
        update_cfg->nTxPkts = ENET_DMA_TX_CH_PTP_NUM_PKTS;
        update_cfg->nRxPkts[0] = ENET_DMA_RX_CH_PTP_NUM_PKTS;
        update_cfg->pktSize = ENET_MEM_LARGE_POOL_PKT_SIZE;
    }
    else if (update_cfg->proto == ETH_P_TSN)
    {
        update_cfg->numRxChannels = 1;
        update_cfg->dmaTxChId = ENET_DMA_TX_CH_AVTP;
        update_cfg->dmaRxChId[0] = ENET_DMA_RX_CH_AVTP;
        update_cfg->nTxPkts = ENET_DMA_TX_CH_AVTP_NUM_PKTS;
        update_cfg->nRxPkts[0] = ENET_DMA_RX_CH_AVTP_NUM_PKTS;
        update_cfg->pktSize = ENET_MEM_LARGE_POOL_PKT_SIZE;
    }
    return 0;
}

static void EnetApp_updateCfg(EnetApp_Cfg *enet_cfg)
{
    EnetApp_getEnetInstInfo(CONFIG_ENET_CPSW0, &enet_cfg->enetType, &enet_cfg->instId);
    EnetApp_getEnetInstMacInfo(enet_cfg->enetType, enet_cfg->instId,
                               enet_cfg->macPorts, &enet_cfg->numMacPorts);
}

#define LOG_BUFFER_SIZE (1024)
void ConsolePrint(const char *pcString, ...)
{
    /* Use DebugP_log() because EnetAppUtils_print() has limit bufsize */
    va_list args;
    char buffer[LOG_BUFFER_SIZE];

    va_start(args, pcString);
    vsnprintf(buffer, sizeof(buffer), pcString, args);
    va_end(args);

    DebugP_log("%s", buffer);
}

static bool IsMacAddrSet(uint8_t *mac)
{
    return ((mac[0]|mac[1]|mac[2]|mac[3]|mac[4]|mac[5]) != 0);
}

static int AddVlan(Enet_Handle hEnet, uint32_t coreId, uint32_t vlanId)
{
    CpswAle_VlanEntryInfo inArgs;
    uint32_t outArgs;
    Enet_IoctlPrms prms;
    int32_t status = ENET_SOK;

    inArgs.vlanIdInfo.vlanId        = vlanId;
    inArgs.vlanIdInfo.tagType       = ENET_VLAN_TAG_TYPE_INNER;
    inArgs.vlanMemberList           = CPSW_ALE_ALL_PORTS_MASK;
    inArgs.unregMcastFloodMask      = CPSW_ALE_ALL_PORTS_MASK;
    inArgs.regMcastFloodMask        = CPSW_ALE_ALL_PORTS_MASK;
    inArgs.forceUntaggedEgressMask  = 0U;
    inArgs.noLearnMask              = 0U;
    inArgs.vidIngressCheck          = BFALSE;
    inArgs.limitIPNxtHdr            = BFALSE;
    inArgs.disallowIPFrag           = BFALSE;

    ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &outArgs);
    ENET_IOCTL(hEnet, coreId, CPSW_ALE_IOCTL_ADD_VLAN, &prms, status);
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("%s():CPSW_ALE_IOCTL_ADD_VLAN failed: %d\r\n",
                           __func__, status);
    }
    else
    {
        EnetAppUtils_print("CPSW_ALE_IOCTL_ADD_VLAN: %d\r\n", vlanId);
    }

    return status;
}

int32_t EnetApp_applyClassifier(Enet_Handle hEnet, uint32_t coreId, uint8_t *dstMacAddr,
                                uint32_t vlanId, uint32_t ethType, uint32_t rxFlowIdx)
{
    Enet_IoctlPrms prms;
    CpswAle_SetPolicerEntryOutArgs setPolicerEntryOutArgs;
    CpswAle_SetPolicerEntryInArgs setPolicerEntryInArgs;
    int32_t status;

    if (IsMacAddrSet(dstMacAddr) == BTRUE)
    {
        status = EnetAppUtils_addHostPortMcastMembership(hEnet, dstMacAddr);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("%s:EnetAppUtils_addAllPortMcastMembership failed: %d\r\n",
                               gEnetAppCfg.name, status);
        }
    }
    memset(&setPolicerEntryInArgs, 0, sizeof (setPolicerEntryInArgs));

    if (ethType > 0)
    {
        setPolicerEntryInArgs.policerMatch.policerMatchEnMask |=
            CPSW_ALE_POLICER_MATCH_ETHERTYPE;
        setPolicerEntryInArgs.policerMatch.etherType = ethType;
    }
    setPolicerEntryInArgs.policerMatch.portIsTrunk = BFALSE;
    setPolicerEntryInArgs.threadIdEn = BTRUE;
    setPolicerEntryInArgs.threadId = rxFlowIdx;

    ENET_IOCTL_SET_INOUT_ARGS(&prms, &setPolicerEntryInArgs, &setPolicerEntryOutArgs);
    ENET_IOCTL(hEnet, coreId,
            CPSW_ALE_IOCTL_SET_POLICER, &prms, status);

    if (status != ENET_SOK)
    {
        EnetAppUtils_print("%s():CPSW_ALE_IOCTL_ADD_VLAN failed: %d\r\n",
                           __func__, status);
    }
    else
    {
        if (vlanId > 0)
        {
            status = AddVlan(hEnet, coreId, vlanId);
        }
    }
    return status;
}

void EnetApp_printCpuLoad(void)
{
    static uint32_t startTime_ms = 0;
    const  uint32_t currTime_ms  = ClockP_getTimeUsec()/1000;
    const  uint32_t printInterval_ms = 5000;

    if (startTime_ms == 0)
    {
        startTime_ms = currTime_ms;
    }
    else if ((currTime_ms - startTime_ms) > printInterval_ms)
    {
        const uint32_t cpuLoad = TaskP_loadGetTotalCpuLoad();

        DebugP_log(" %6d.%3ds : CPU load = %3d.%02d %%\r\n",
                  currTime_ms/1000, currTime_ms%1000,
                  cpuLoad/100, cpuLoad%100 );

        startTime_ms = currTime_ms;
        TaskP_loadResetAll();
    }
    return;
}

int EnetApp_initTsn(void)
{
    lld_ethdev_t ethdevs[MAX_NUMBER_ENET_DEVS] = {0};
    int i;
    int res = 0;
    AppTsnCfg_t appCfg =
    {
        .consoleOutCb = ConsolePrint,
    };

    for (i = 0; i < gEnetAppCfg.numMacPorts; i++)
    {
        snprintf(&g_netdevices[i][0], CB_MAX_NETDEVNAME, "tilld%d", i);
        appCfg.netdevs[i] = &g_netdevices[i][0];
        ethdevs[i].netdev = g_netdevices[i];
        ethdevs[i].macport = gEnetAppCfg.macPorts[i];
        if (i == 0)
        {
            /* tilld0 reuses the allocated source mac, other interfaces will allocate
             * the mac by themself */
            memcpy(ethdevs[i].srcmac, gEnetAppCfg.macAddr, ENET_MAC_ADDR_LEN);
        }
    }
    appCfg.netdevs[i] = NULL;
    if (EnetApp_initTsnByCfg(&appCfg) < 0)
    {
        EnetAppAbort("Failed to int tsn!\r\n");
    }
    if (cb_lld_init_devs_table(ethdevs, i, gEnetAppCfg.enetType,
                               gEnetAppCfg.instId) < 0)
    {
        EnetAppAbort("Failed to int devs table!\r\n");
    }
    cb_socket_set_lldcfg_update_cb(EnetApp_lldCfgUpdateCb);

    if (EnetApp_startTsn() < 0)
    {
        EnetAppAbort("Failed to start TSN App!\r\n");
    }
    EnetAppUtils_print("%s:TSN app start done!\r\n", __func__);

    return res;
}

int32_t EnetApp_filterPriorityPacketsCfg(Enet_Handle hEnet, uint32_t coreId)
{
#ifdef SOC_AM273X
    EnetMacPort_SetPriorityRegenMapInArgs params;
    Enet_IoctlPrms prms;
    int32_t retVal = ENET_SOK;

    params.macPort = ENET_MAC_PORT_1;

    params.priorityRegenMap.priorityMap[0] =0U;
    for (int i = 1; i < 8U; i++)
    {
        params.priorityRegenMap.priorityMap[i] =1U;  // Map all priorities from (1 to 7) to priority 1, these packets will be received on DMA channel 1.
    }

    ENET_IOCTL_SET_IN_ARGS(&prms, &params);

    ENET_IOCTL(hEnet, coreId, ENET_MACPORT_IOCTL_SET_PRI_REGEN_MAP, &prms, retVal);

    return retVal;
#else
    return ENET_SOK;
#endif
}

static void EnetApp_enableTsSync()
{
    Enet_IoctlPrms prms;
    CpswCpts_OutputBitSel bitSelect;
    int32_t status;

    bitSelect = CPSW_CPTS_TS_OUTPUT_BIT_17;
    ENET_IOCTL_SET_IN_ARGS(&prms, &bitSelect);
    ENET_IOCTL(gEnetAppCfg.hEnet, gEnetAppCfg.coreId, CPSW_CPTS_IOCTL_SELECT_TS_OUTPUT_BIT, &prms, status);
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("%s: Failed to set TS SYNC OUT BIT : %d\r\n", gEnetAppCfg.name, status);
    }
    return;
}

void EnetApp_initAppCfg(EnetPer_AttachCoreOutArgs *attachArgs, EnetApp_HandleInfo *handleInfo)
{
    /* To support gptp switch mode, we must configure from syscfg file:
     * enet_cpsw1.DisableMacPort2 = BFALSE; */
    EnetApp_updateCfg(&gEnetAppCfg);

    gEnetAppCfg.coreId = EnetSoc_getCoreId();
    EnetQueue_initQ(&gEnetAppCfg.txFreePktInfoQ);
    EnetAppUtils_enableClocks(gEnetAppCfg.enetType, gEnetAppCfg.instId);
    DebugP_log("start to open driver.\r\n");
    EnetApp_driverInit();
    EnetApp_driverOpen(gEnetAppCfg.enetType, gEnetAppCfg.instId);
    EnetApp_acquireHandleInfo(gEnetAppCfg.enetType, gEnetAppCfg.instId, handleInfo);
    gEnetAppCfg.hEnet = handleInfo->hEnet;
    EnetApp_coreAttach(gEnetAppCfg.enetType, gEnetAppCfg.instId, gEnetAppCfg.coreId, attachArgs);
    gEnetAppCfg.coreKey = attachArgs->coreKey;

    EnetApp_enableTsSync();
}

static void EnetApp_macMode2MacMii(emac_mode macMode, EnetMacPort_Interface *mii)
{
    switch (macMode)
    {
    case RMII:
        mii->layerType = ENET_MAC_LAYER_MII;
        mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
        mii->variantType = ENET_MAC_VARIANT_NONE;
        break;

    case RGMII:
        mii->layerType = ENET_MAC_LAYER_GMII;
        mii->sublayerType = ENET_MAC_SUBLAYER_REDUCED;
        mii->variantType = ENET_MAC_VARIANT_FORCED;
        break;
    default:
        EnetAppUtils_print("Invalid MAC mode: %u\r\n", macMode);
        EnetAppUtils_assert(BFALSE);
        break;
    }
}

void EnetApp_initLinkArgs(Enet_Type enetType, uint32_t instId,
                          EnetPer_PortLinkCfg *linkArgs, Enet_MacPort macPort)
{
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;
    EnetMacPort_LinkCfg *linkCfg = &linkArgs->linkCfg;
    EnetMacPort_Interface *mii = &linkArgs->mii;
    EnetPhy_Cfg *phyCfg = &linkArgs->phyCfg;
    int32_t status = ENET_SOK;

    EnetAppUtils_print("%s: Open port %u\r\n",
                       gEnetAppCfg.name, ENET_MACPORT_ID(macPort));

    /* Setup board for requested Ethernet port */
    ethPort.enetType = gEnetAppCfg.enetType;
    ethPort.instId = gEnetAppCfg.instId;
    ethPort.macPort = macPort;
    ethPort.boardId = EnetBoard_getId();
    EnetApp_macMode2MacMii(RGMII, &ethPort.mii);

    status = EnetBoard_setupPorts(&ethPort, 1U);
    if (status != ENET_SOK) {
        EnetAppUtils_print("%s: Failed to setup MAC port %u\r\n",
                           gEnetAppCfg.name, ENET_MACPORT_ID(macPort));
        EnetAppUtils_assert(BFALSE);
    }

    /* Set port link params */
    linkArgs->macPort = macPort;

    mii->layerType = ethPort.mii.layerType;
    mii->sublayerType = ethPort.mii.sublayerType;
    mii->variantType = ENET_MAC_VARIANT_FORCED;

    linkCfg->speed = ENET_SPEED_AUTO;
    linkCfg->duplexity = ENET_DUPLEX_AUTO;

    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    if (boardPhyCfg != NULL) {
        EnetPhy_initCfg(phyCfg);
        phyCfg->phyAddr = boardPhyCfg->phyAddr;
        phyCfg->isStrapped = boardPhyCfg->isStrapped;
        phyCfg->loopbackEn = BFALSE;
        phyCfg->skipExtendedCfg = boardPhyCfg->skipExtendedCfg;
        phyCfg->extendedCfgSize = boardPhyCfg->extendedCfgSize;
        /* Setting Tx and Rx skew delay values */
        memcpy(phyCfg->extendedCfg, boardPhyCfg->extendedCfg, phyCfg->extendedCfgSize);

    } else {
        EnetAppUtils_print("%s: No PHY configuration found\r\n", gEnetAppCfg.name);
        EnetAppUtils_assert(BFALSE);
    }
}

void EnetApp_addMCastEntry(Enet_Type enetType,
                           uint32_t instId,
                           uint32_t coreId,
                           const uint8_t *testMCastAddr,
                           uint32_t portMask)
{
    Enet_IoctlPrms prms;
    int32_t status;
    CpswAle_SetMcastEntryInArgs setMcastInArgs;
    uint32_t setMcastOutArgs;

    if (Enet_isCpswFamily(enetType))
    {
        Enet_Handle hEnet = Enet_getHandle(enetType, instId);

        EnetAppUtils_assert(hEnet != NULL);
        memset(&setMcastInArgs, 0, sizeof(setMcastInArgs));
        memcpy(&setMcastInArgs.addr.addr[0U], testMCastAddr,
               sizeof(setMcastInArgs.addr.addr));
        setMcastInArgs.addr.vlanId  = 0;
        setMcastInArgs.info.super = BFALSE;
        setMcastInArgs.info.numIgnBits = 0;
        setMcastInArgs.info.fwdState = CPSW_ALE_FWDSTLVL_FWD;
        setMcastInArgs.info.portMask = portMask;
        ENET_IOCTL_SET_INOUT_ARGS(&prms, &setMcastInArgs, &setMcastOutArgs);
        ENET_IOCTL(hEnet, coreId, CPSW_ALE_IOCTL_ADD_MCAST, &prms, status);
        if (status != ENET_SOK)
        {
           EnetAppUtils_print("EnetTestBcastMcastLimit_AddAleEntry() failed CPSW_ALE_IOCTL_ADD_MCAST: %d\n",
                               status);
        }
    }
}

void EnetApp_addBroadcastEntry(void)
{
    EnetApp_addMCastEntry(gEnetAppCfg.enetType,
                          gEnetAppCfg.instId,
                          EnetSoc_getCoreId(),
                          BROADCAST_MAC_ADDRESS,
                          CPSW_ALE_ALL_PORTS_MASK);
}

void EnetApp_setMacAddr(uint8_t hwaddr[])
{
    memcpy(gEnetAppCfg.macAddr, hwaddr, ENET_MAC_ADDR_LEN);
    EnetAppUtils_print("Host MAC address Set: ");
    EnetAppUtils_printMacAddr(hwaddr);
}

static void EnetApp_portLinkStatusChangeCb(Enet_MacPort macPort,
                                           bool isLinkUp, void *appArg)
{
    EnetAppUtils_print("MAC Port %u: link %s\r\n",
                       ENET_MACPORT_ID(macPort), isLinkUp ? "up" : "down");
}

static void EnetApp_mdioLinkStatusChange(Cpsw_MdioLinkStateChangeInfo *info,
                                             void *appArg)
{
}

static void EnetApp_initAleConfig(CpswAle_Cfg *aleCfg)
{
    aleCfg->modeFlags = CPSW_ALE_CFG_MODULE_EN;
    aleCfg->agingCfg.autoAgingEn = BTRUE;
    aleCfg->agingCfg.agingPeriodInMs = 1000;

    aleCfg->nwSecCfg.vid0ModeEn = BTRUE;
    aleCfg->vlanCfg.unknownUnregMcastFloodMask = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownRegMcastFloodMask = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->vlanCfg.unknownVlanMemberListMask = CPSW_ALE_ALL_PORTS_MASK;
    aleCfg->policerGlobalCfg.policingEn = BTRUE;
    aleCfg->policerGlobalCfg.yellowDropEn = BFALSE;
    /* Enables the ALE to drop the red colored packets. */
    aleCfg->policerGlobalCfg.redDropEn = BTRUE;
    /* Policing match mode */
    aleCfg->policerGlobalCfg.policerNoMatchMode = CPSW_ALE_POLICER_NOMATCH_MODE_GREEN;
}

static void EnetApp_initEnetLinkCbPrms(Cpsw_Cfg *cpswCfg)
{
#if (ENET_SYSCFG_ENABLE_MDIO_MANUALMODE == 1U)
    cpswCfg->mdioLinkStateChangeCb = NULL;
    cpswCfg->mdioLinkStateChangeCbArg = NULL;
#else
    cpswCfg->mdioLinkStateChangeCb = EnetApp_mdioLinkStatusChange;
    cpswCfg->mdioLinkStateChangeCbArg = NULL;
#endif

    cpswCfg->portLinkStatusChangeCb    = &EnetApp_portLinkStatusChangeCb;
    cpswCfg->portLinkStatusChangeCbArg = NULL;
}

void EnetApp_updateCpswInitCfg(Enet_Type enetType, uint32_t instId, Cpsw_Cfg *cpswCfg)
{
    CpswHostPort_Cfg *hostPortCfg = &cpswCfg->hostPortCfg;
    /* Prepare init configuration for all peripherals */
    EnetAppUtils_print("\nInit all configs\r\n");
    EnetAppUtils_print("----------------------------------------------\r\n");
    EnetAppUtils_print("%s: init config\r\n", gEnetAppCfg.name);

    cpswCfg->hostPortCfg.removeCrc = BTRUE;
    cpswCfg->hostPortCfg.padShortPacket = BTRUE;
    cpswCfg->hostPortCfg.passCrcErrors = BTRUE;
    EnetApp_initEnetLinkCbPrms(cpswCfg);
    EnetApp_initAleConfig(&cpswCfg->aleCfg);

    /* Hardware switch priority is taken from packet's PCP or DSCP */
    hostPortCfg->rxVlanRemapEn     = BTRUE;
    hostPortCfg->rxDscpIPv4RemapEn = BTRUE;
    hostPortCfg->rxDscpIPv6RemapEn = BTRUE;

#ifdef SOC_AM263X
    EnetCpdma_Cfg *dmaCfg;
    /* Set the enChOverrideFlag to enable the channel override feature of CPDMA */
    dmaCfg=(EnetCpdma_Cfg *)cpswCfg->dmaCfg;
    dmaCfg->enChOverrideFlag = BTRUE;
#endif
}

static void EnetApp_closePort()
{
    Enet_IoctlPrms prms;
    Enet_MacPort macPort;
    uint32_t i;
    int32_t status;

    for (i = 0U; i < gEnetAppCfg.numMacPorts; i++)
    {
        macPort = gEnetAppCfg.macPorts[i];

        EnetAppUtils_print("%s: Close port %u\r\n", gEnetAppCfg.name, ENET_MACPORT_ID(macPort));

        /* Close port link */
        ENET_IOCTL_SET_IN_ARGS(&prms, &macPort);

        EnetAppUtils_print("%s: Close port %u link\r\n", gEnetAppCfg.name, ENET_MACPORT_ID(macPort));
        ENET_IOCTL(gEnetAppCfg.hEnet, gEnetAppCfg.coreId, ENET_PER_IOCTL_CLOSE_PORT_LINK, &prms, status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("%s: Failed to close port link: %d\r\n", gEnetAppCfg.name, status);
        }
    }
}

static void EnetApp_allocateIPAddress()
{
    sys_lock_tcpip_core();
    for (uint32_t  netifIdx = 0U; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
    {
        dhcp_set_struct(g_pNetif[NETIF_INST_ID0 + netifIdx], &g_netifDhcp[NETIF_INST_ID0 + netifIdx]);

        const err_t err = dhcp_start(g_pNetif[NETIF_INST_ID0 + netifIdx]);
        EnetAppUtils_assert(err == ERR_OK);
    }
    sys_unlock_tcpip_core();
    return;
}

static void EnetApp_netifStatusChangeCb(struct netif *pNetif)
{
    if (netif_is_up(pNetif))
    {
        DebugP_log("[%d]Enet IF UP Event. Local interface IP:%s\r\n",
                    pNetif->num, ip4addr_ntoa(netif_ip4_addr(pNetif)));
    }
    else
    {
        DebugP_log("[%d]Enet IF DOWN Event\r\n", pNetif->num);
    }
    return;
}

static void EnetApp_netifLinkChangeCb(struct netif *pNetif)
{
    if (netif_is_link_up(pNetif))
    {
        DebugP_log("[%d]Network Link UP Event\r\n", pNetif->num);
    }
    else
    {
        DebugP_log("[%d]Network Link DOWN Event\r\n", pNetif->num);
    }
    return;
}

static void EnetApp_setupNetif()
{
    ip4_addr_t ipaddr, netmask, gw;

    ip4_addr_set_zero(&gw);
    ip4_addr_set_zero(&ipaddr);
    ip4_addr_set_zero(&netmask);

    DebugP_log("Starting lwIP, local interface IP is dhcp-enabled\r\n");
    hlwipIfApp = LwipifEnetApp_getHandle();
    for (uint32_t netifIdx = 0U; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
    {
        /* Open the netif and get it populated*/
        g_pNetif[netifIdx] = LwipifEnetApp_netifOpen(hlwipIfApp, NETIF_INST_ID0 + netifIdx, &ipaddr, &netmask, &gw);
        netif_set_status_callback(g_pNetif[netifIdx], EnetApp_netifStatusChangeCb);
        netif_set_link_callback(g_pNetif[netifIdx], EnetApp_netifLinkChangeCb);
        netif_set_up(g_pNetif[NETIF_INST_ID0 + netifIdx]);
    }
    LwipifEnetApp_startSchedule(hlwipIfApp, g_pNetif[ENET_SYSCFG_DEFAULT_NETIF_IDX]);
}

static void EnetApp_tcpipInitCompleteCb(void *pArg)
{
    sys_sem_t *pSem = (sys_sem_t*)pArg;
    EnetAppUtils_assert(pArg != NULL);

    /* init randomizer again (seed per thread) */
    srand((unsigned int)sys_now()/1000);

    EnetApp_setupNetif();

    EnetApp_allocateIPAddress();

    sys_sem_signal(pSem);
}

static void EnetApp_setupNetworkStack()
{
    sys_sem_t pInitSem;
    const err_t err = sys_sem_new(&pInitSem, 0);
    EnetAppUtils_assert(err == ERR_OK);

    tcpip_init(EnetApp_tcpipInitCompleteCb, &pInitSem);

    /* wait for TCP/IP initialization to complete */
    sys_sem_wait(&pInitSem);
    sys_sem_free(&pInitSem);

    return;
}

static void EnetApp_shutdownNetworkStack()
{
    for (uint32_t netifIdx = 0U; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
    {
        LwipifEnetApp_netifClose(hlwipIfApp, NETIF_INST_ID0 + netifIdx);
    }
    return;
}

static int32_t EnetApp_isNetworkUp(struct netif* netif_)
{
    return (netif_is_up(netif_) && netif_is_link_up(netif_) && !ip4_addr_isany_val(*netif_ip4_addr(netif_)));
}

void enetApp_setup()
{
    EnetPer_AttachCoreOutArgs   attachCoreOutArgs;
    EnetApp_HandleInfo          handleInfo;
    EnetApp_GetMacAddrOutArgs   outArgs;

    EnetApp_initAppCfg(&attachCoreOutArgs, &handleInfo);
    EnetApp_getMacAddress(ENET_DMA_RX_CH0, &outArgs);
    EnetAppUtils_assert(outArgs.macAddressCnt == 1);

    EnetUtils_copyMacAddr(gEnetAppCfg.macAddr, outArgs.macAddr[outArgs.macAddressCnt - 1]);

    EnetApp_addBroadcastEntry();
    if (EnetApp_initTsn())
    {
        DebugP_log("EnetApp_initTsn failed\r\n");
    }
    else
    {
        asdk_createShmbuffer();

        asdk_createListenerTask();
        asdk_createTalkerTask();

        EnetApp_setupNetworkStack();

        uint32_t netupMask = 0;
        /* wait for atleast one Network Interface to get IP */
        while (netupMask == 0)
        {
            for(uint32_t netifIdx = 0; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
            {
                if (EnetApp_isNetworkUp(g_pNetif[netifIdx]))
                {
                    netupMask |= (1 << netifIdx);
                }
                /*else
                {
                    DebugP_log("[%d]Waiting for network UP ...\r\n",g_pNetif[netifIdx]->num);
                }*/
                ClockP_sleep(2);
            }
        }

        DebugP_log("Network is UP ...\r\n");
        ClockP_sleep(2);

        asdk_startServer();
#if 0
        while (BTRUE)
        {
            // Print CPU load
            ClockP_usleep(1000);
            EnetApp_printCpuLoad();
            TaskP_yield();
        }
        
        EnetApp_stopTsn();
        EnetApp_deInitTsn();
        EnetApp_shutdownNetworkStack();
#endif 

    }

}
