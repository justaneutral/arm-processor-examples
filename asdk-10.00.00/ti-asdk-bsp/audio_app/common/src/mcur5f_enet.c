/*
 *  Copyright (C) 2018-2022 Texas Instruments Incorporated
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
#include "enet_apputils.h"
#include <platform.h>
#include "audio_enet.h"

static const uint8_t BROADCAST_MAC_ADDRESS[ENET_MAC_ADDR_LEN] = 
{ 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};

/* dhcp struct for the ethernet netif */
static struct netif *g_pNetif[ENET_SYSCFG_NETIF_COUNT];
static AWE_EnetInfo gEnetParams[ENET_SYSCFG_MAX_ENET_INSTANCES];

#define ENET_TASK_PRI  (configMAX_PRIORITIES-1)
#define ENET_TASK_SIZE (16384U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gEnetTaskStack[ENET_TASK_SIZE] __attribute__((aligned(32)));
StaticTask_t gEnetTaskObj;
TaskHandle_t gEnetTask;

void avbTimerIsr(void)
{
    //SemaphoreP_post(&gTimerSem);
}

extern SemaphoreP_Object semEthernetReady;

/* Handle to the Application interface for the LwIPIf Layer
 */
LwipifEnetApp_Handle hlwipIfApp = NULL;

static void awe_mdioLinkStatusChange(Cpsw_MdioLinkStateChangeInfo *info,
                                         void *appArg);

static void awe_portLinkStatusChangeCb(Enet_MacPort macPort,
                                           bool isLinkUp,
                                           void *appArg);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EnetApp_initLinkArgs(Enet_Type enetType,
                          uint32_t instId,
                          EnetPer_PortLinkCfg *linkArgs,
                          Enet_MacPort macPort)
{
    EnetPhy_Cfg *phyCfg = &linkArgs->phyCfg;
    EnetMacPort_LinkCfg *linkCfg = &linkArgs->linkCfg;
    EnetMacPort_Interface *mii = &linkArgs->mii;
    EnetBoard_EthPort ethPort;
    const EnetBoard_PhyCfg *boardPhyCfg;
    int32_t status;

    /* Setup board for requested Ethernet port */
    ethPort.enetType = enetType;
    ethPort.instId   = instId;
    ethPort.macPort  = macPort;
    ethPort.boardId  = EnetBoard_getId();

    /* Get the Mii config for Ethernet port */
    EnetBoard_getMiiConfig(&ethPort.mii);

    status = EnetBoard_setupPorts(&ethPort, 1U);
    EnetAppUtils_assert(status == ENET_SOK);

    if (Enet_isCpswFamily(ethPort.enetType))
    {
        CpswMacPort_Cfg *macCfg = (CpswMacPort_Cfg *)linkArgs->macCfg;
        CpswMacPort_initCfg(macCfg);
        if (EnetMacPort_isSgmii(mii) || EnetMacPort_isQsgmii(mii))
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_SGMII_WITH_PHY;
        }
        else
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_INVALID;
        }
    }

    boardPhyCfg = EnetBoard_getPhyCfg(&ethPort);
    if (boardPhyCfg != NULL)
    {
        EnetPhy_initCfg(phyCfg);
        phyCfg->phyAddr     = boardPhyCfg->phyAddr;
        phyCfg->isStrapped  = boardPhyCfg->isStrapped;
        phyCfg->loopbackEn  = false;
        phyCfg->skipExtendedCfg = boardPhyCfg->skipExtendedCfg;
        phyCfg->extendedCfgSize = boardPhyCfg->extendedCfgSize;
        memcpy(phyCfg->extendedCfg, boardPhyCfg->extendedCfg, phyCfg->extendedCfgSize);

    }
    else
    {
        DebugP_log("No PHY configuration found for MAC port %u\r\n",
                           ENET_MACPORT_ID(ethPort.macPort));
        EnetAppUtils_assert(false);
    }

    mii->layerType     = ethPort.mii.layerType;
    mii->sublayerType  = ethPort.mii.sublayerType;
    mii->variantType   = ENET_MAC_VARIANT_FORCED;
    linkCfg->speed     = ENET_SPEED_AUTO;
    linkCfg->duplexity = ENET_DUPLEX_AUTO;

    if (Enet_isCpswFamily(ENET_CPSW_3G))
    {
        CpswMacPort_Cfg *macCfg = (CpswMacPort_Cfg *)linkArgs->macCfg;

        if (EnetMacPort_isSgmii(mii) || EnetMacPort_isQsgmii(mii))
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_SGMII_WITH_PHY;
        }
        else
        {
            macCfg->sgmiiMode = ENET_MAC_SGMIIMODE_INVALID;
        }
    }
}

void awe_addMCastEntry(Enet_Type enetType,
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
        setMcastInArgs.info.super = false;
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

void EnetApp_updateCpswInitCfg(Enet_Type enetType, uint32_t instId, Cpsw_Cfg *cpswCfg)
{
#if defined (ENET_SOC_HOSTPORT_DMA_TYPE_CPDMA)
    EnetCpdma_Cfg * dmaCfg = (EnetCpdma_Cfg *)cpswCfg->dmaCfg;

    EnetAppUtils_assert(dmaCfg != NULL);
    EnetAppUtils_assert(EnetAppUtils_isDescCached() == false);
    dmaCfg->rxInterruptPerMSec = 8;
    dmaCfg->txInterruptPerMSec = 2;
#endif
    cpswCfg->mdioLinkStateChangeCb    = &awe_mdioLinkStatusChange;
    cpswCfg->mdioLinkStateChangeCbArg = NULL;

    cpswCfg->portLinkStatusChangeCb = &awe_portLinkStatusChangeCb;
    cpswCfg->portLinkStatusChangeCbArg = NULL;
}

static void awe_mdioLinkStatusChange(Cpsw_MdioLinkStateChangeInfo *info,
                                         void *appArg)
{
    if (info->linkChanged)
    {
        EnetAppUtils_print("Link Status Changed. PHY: 0x%x, state: %s\r\n",
                info->phyAddr,
                info->isLinked? "up" : "down");
    }
}

static void awe_portLinkStatusChangeCb(Enet_MacPort macPort,
                                           bool isLinkUp,
                                           void *appArg)
{
    EnetAppUtils_print("MAC Port %u: link %s\r\n",
                       ENET_MACPORT_ID(macPort), isLinkUp ? "up" : "down");
}

void awe_setupNetif()
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
        netif_set_status_callback(g_pNetif[netifIdx], awe_netifStatusChangeCb);
        netif_set_link_callback(g_pNetif[netifIdx], awe_netifLinkChangeCb);
        netif_set_up(g_pNetif[NETIF_INST_ID0 + netifIdx]);
    }
    LwipifEnetApp_startSchedule(hlwipIfApp, g_pNetif[ENET_SYSCFG_DEFAULT_NETIF_IDX]);
}

#ifdef AWE_ENET_ENABLE_DHCP
static struct dhcp g_netifDhcp[ENET_SYSCFG_NETIF_COUNT];
void awe_allocateIPAddress()
{
    uint32_t  netifIdx;
    sys_lock_tcpip_core();
    for (netifIdx = 0U; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
    {
        dhcp_set_struct(g_pNetif[NETIF_INST_ID0 + netifIdx], &g_netifDhcp[NETIF_INST_ID0 + netifIdx]);

        const err_t err = dhcp_start(g_pNetif[NETIF_INST_ID0 + netifIdx]);
        EnetAppUtils_assert(err == ERR_OK);
    }
    sys_unlock_tcpip_core();
    return;
}
#else //#ifdef AWE_ENET_ENABLE_DHCP
#define IP_ADDR_POOL_COUNT  (2U)
const ip_addr_t gStaticIP[IP_ADDR_POOL_COUNT]   =  
{ 
    IPADDR4_INIT_BYTES(192, 168, 1, 200) /* For NetifIdx = 0 */,  
    IPADDR4_INIT_BYTES(  10,  64,  1, 200) /* For NetifIdx = 1 */
};
const ip_addr_t gStaticIPGateway[IP_ADDR_POOL_COUNT] =  
{ 
    IPADDR4_INIT_BYTES(192, 168, 1, 1) /* For NetifIdx = 0 */, 
    IPADDR4_INIT_BYTES(  10,   64, 1, 1) /* For NetifIdx = 1 */
};
const ip_addr_t gStaticIPNetmask[IP_ADDR_POOL_COUNT] =  
{ 
    IPADDR4_INIT_BYTES(255,255,255,0)  /* For NetifIdx = 0 */, 
    IPADDR4_INIT_BYTES(255,255,252,0) /* For NetifIdx = 1 */
};
 
static void awe_allocateIPAddress()
{
    sys_lock_tcpip_core();
 
    for (uint32_t  netifIdx = 0U; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
    {
        netif_set_addr(g_pNetif[NETIF_INST_ID0 + netifIdx],
                                &gStaticIP[NETIF_INST_ID0 + netifIdx],
                                &gStaticIPNetmask[NETIF_INST_ID0 + netifIdx],
                                &gStaticIPGateway[NETIF_INST_ID0 + netifIdx]);
    }
    sys_unlock_tcpip_core();
    return;
}
#endif //#ifdef AWE_ENET_ENABLE_DHCP

void awe_tcpipInitCompleteCb(void *pArg)
{
    SemaphoreP_Object *pSem = (SemaphoreP_Object*)pArg;
    DebugP_assert(pArg != NULL);

    /* init randomizer again (seed per thread) */
    srand((unsigned int)sys_now()/1000);

    awe_setupNetif();

    awe_allocateIPAddress();

    SemaphoreP_post(pSem);
}

void awe_setupNetworkStack()
{
    int32_t status = SystemP_SUCCESS;
    SemaphoreP_Object semInit;
    status = SemaphoreP_constructBinary(&semInit, 0);
	DebugP_assert(SystemP_SUCCESS == status);

    tcpip_init(awe_tcpipInitCompleteCb, &semInit);

    /* wait for TCP/IP initialization to complete */
    SemaphoreP_pend(&semInit, SystemP_WAIT_FOREVER);
    SemaphoreP_destruct(&semInit);

    return;
}

void awe_netifStatusChangeCb(struct netif *pNetif)
{
    if (netif_is_up(pNetif))
    {
        SemaphoreP_post(&semEthernetReady);
        DebugP_log("[%d]Enet IF UP Event. Local interface IP:%s\r\n",
                    pNetif->num, ip4addr_ntoa(netif_ip4_addr(pNetif)));
    }
    else
    {
        DebugP_log("[%d]Enet IF DOWN Event\r\n", pNetif->num);
    }
    return;
}

void awe_netifLinkChangeCb(struct netif *pNetif)
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

int32_t awe_isNetworkUp(struct netif* netif_)
{
    return (netif_is_up(netif_) && netif_is_link_up(netif_) && !ip4_addr_isany_val(*netif_ip4_addr(netif_)));
}

void awe_enet_setup()
{
    int32_t status = SystemP_SUCCESS;
    uint32_t enetInstIdx;

    /* Read MAC Port details and enable clock for each ENET instance */
    for (enetInstIdx = 0; enetInstIdx < ENET_SYSCFG_MAX_ENET_INSTANCES; enetInstIdx++)
    {
        AWE_EnetInfo* pEnetInstInfo = &gEnetParams[enetInstIdx];
        EnetApp_getEnetInstInfo(CONFIG_ENET_CPSW0 + enetInstIdx, &pEnetInstInfo->enetType, &pEnetInstInfo->instId);
        EnetApp_getEnetInstMacInfo(pEnetInstInfo->enetType,
                            pEnetInstInfo->instId,
                            &pEnetInstInfo->macPortList[0],
                            &pEnetInstInfo->numMacPort);
        EnetAppUtils_enableClocks(pEnetInstInfo->enetType, pEnetInstInfo->instId);
    }

    /* Open ENET driver for each ENET instance */

    EnetApp_driverInit();
    for(enetInstIdx = 0; enetInstIdx < ENET_SYSCFG_MAX_ENET_INSTANCES; enetInstIdx++)
    {
        status = EnetApp_driverOpen(gEnetParams[enetInstIdx].enetType, gEnetParams[enetInstIdx].instId);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to open ENET[%d]: %d\r\n", enetInstIdx, status);
            EnetAppUtils_assert(status == ENET_SOK);
        }
        
        awe_addMCastEntry(gEnetParams[enetInstIdx].enetType,
                            gEnetParams[enetInstIdx].instId,
                            EnetSoc_getCoreId(),
                            BROADCAST_MAC_ADDRESS,
                            CPSW_ALE_ALL_PORTS_MASK);
    }

    awe_setupNetworkStack();

    uint32_t netupMask = 0;
    /* wait for atleast one Network Interface to get IP */
    while (netupMask == 0)
    {
        for(uint32_t netifIdx = 0; netifIdx < ENET_SYSCFG_NETIF_COUNT; netifIdx++)
        {
            if (awe_isNetworkUp(g_pNetif[netifIdx]))
            {
                netupMask |= (1 << netifIdx);
            }
            else
            {
                DebugP_log("[%d]Waiting for network UP ...\r\n",g_pNetif[netifIdx]->num);
            }
            ClockP_sleep(1);
        }
    }

    //awe_serverTask(NULL);
}

void awe_createEnetTask()
{
    // create task to handle enet connection
    gEnetTask = xTaskCreateStatic( awe_serverTask,
                                  "awe_serverTask",
                                  ENET_TASK_SIZE,
                                  NULL,
                                  ENET_TASK_PRI,
                                  gEnetTaskStack,
                                  &gEnetTaskObj );
    configASSERT(gEnetTask != NULL);
}

extern uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN];
extern uint32_t read_byte_cnt;
extern int32_t awe_r5f_ipcSend();
extern int32_t awe_r5f_ipcRecv();

void awe_handleActiveConn(struct netconn *pClientConn)
{
    struct netbuf   *buf;
    void            *data;
    uint16_t        len;
    err_t           err;
    uint16_t 		recvBufLen;

    read_byte_cnt = 0;
    while ((err = netconn_recv(pClientConn, &buf)) == ERR_OK)
    {
        read_byte_cnt = 0;
        do
        {
            netbuf_data(buf, &data, &len);
            memcpy(&AWE_Packet_Buffer[read_byte_cnt], data, len);
            read_byte_cnt += len;
        } while (netbuf_next(buf) >= 0);
		
		// send the tunning buffer to primary
		awe_r5f_ipcSend();

		//wait for response from primary core
		awe_r5f_ipcRecv();
		
		recvBufLen = (uint16_t) (AWE_Packet_Buffer[0]>>16) * sizeof(uint32_t);
		
        err = netconn_write(pClientConn, AWE_Packet_Buffer, recvBufLen, NETCONN_COPY);
        if (err != ERR_OK)
        {
            DebugP_log("awe_handleActiveConn: netconn_write: error \"%s\"\r\n", lwip_strerr(err));
            break;
        }

        netbuf_delete(buf);
    }
}

void awe_serverTask(void *arg)
{
    struct netconn *pConn = NULL, *pClientConn = NULL;
    err_t err;

    //awe_enet_setup();

    pConn = netconn_new(NETCONN_TCP);
    netconn_bind(pConn, IP_ADDR_ANY, DEFAULT_TUNING_PORT);
    if(pConn == NULL)
    {
        DebugP_log("awe_serverTask: invalid conn\r\n");
        return;
    }

    /* Tell connection to go into listening mode. */
    netconn_listen(pConn);

    while (1)
    {
        /* Grab new connection. */
        err = netconn_accept(pConn, &pClientConn);
        DebugP_log("Audio Weaver Connected ...\r\n");

        /* Process the new connection. */
        if (err < ERR_OK)
        {
            DebugP_log("Unable to accept connection: errno %d\r\n", err);
            break;
        }

        awe_handleActiveConn(pClientConn);

        /* Close connection and discard connection identifier. */
        netconn_close(pClientConn);
        netconn_delete(pClientConn);
    }
}
