/*
 *  Copyright (c) Texas Instruments Incorporated 2024
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
 * \file ti_enet_open_close.c
 *
 * \brief This file contains enet driver memory allocation related functionality.
 */


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include <enet.h>
#include "enet_appmemutils.h"
#include "enet_appmemutils_cfg.h"
#include "enet_apputils.h"
#include <enet_cfg.h>
#include <include/core/enet_per.h>
#include <include/core/enet_utils.h>
#include <include/core/enet_dma.h>
#include <include/common/enet_utils_dflt.h>
#include <include/per/cpsw.h>
#include <priv/per/cpsw_priv.h>
#include <drivers/udma/udma_priv.h>
#include <include/core/enet_soc.h>
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/EventP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/QueueP.h>

#include "ti_enet_config.h"
#include "ti_drivers_config.h"
#include "ti_enet_open_close.h"
#include <utils/include/enet_appsoc.h>

#define ENETAPP_PHY_STATEHANDLER_TASK_PRIORITY        (7U)
#define ENETAPP_PHY_STATEHANDLER_TASK_STACK     (3 * 1024)
#define AppEventId_CPSW_PERIODIC_POLL    (1 << 3)
#define ENETAPP_NUM_IET_VERIFY_ATTEMPTS               (20U)



#include <drivers/udma/udma_priv.h>
#include <drivers/udma.h>

typedef struct EnetAppTxDmaSysCfg_Obj_s
{
    /* TX channel handle */
    EnetDma_TxChHandle hTxCh;
    /* TX channel number */
    uint32_t txChNum;
} EnetAppTxDmaSysCfg_Obj;

typedef struct EnetAppRxDmaSysCfg_Obj_s
{
    /* RX channel handle */
    EnetDma_RxChHandle hRxCh;
    /* RX flow start index */
    uint32_t rxFlowIdx;
    /* RX flow start index */
    uint32_t rxFlowStartIdx;
    /* num mac Address valid in macAddr[][] list below*/
    uint32_t numValidMacAddress;
    /* MAC address. It's port/ports MAC address in Dual-MAC or
     * host port MAC address in Switch */
    uint8_t macAddr[ENET_MAX_NUM_MAC_PER_PHER][ENET_MAC_ADDR_LEN];
} EnetAppRxDmaSysCfg_Obj;

typedef struct EnetAppRxDmaCfg_Info_s
{
    /* mac Address valid */
    uint32_t     numValidMacAddress;
    
    /*! Whether to use the shared global event or not. If set to false, a dedicated event
     *  will be used for this channel. */
    bool  useGlobalEvt;
    /*! Whether to use the shared global event or not. If set to false, a dedicated event
     *  will be used for this channel. */
    bool  useDefaultFlow;
    /*! UDMA driver handle*/
    Udma_DrvHandle hUdmaDrv;
    /*! [IN] UDMAP receive flow packet size based free buffer queue enable configuration
     * to be programmed into the rx_size_thresh_en field of the RFLOW_RFC register.
     * See the UDMAP section of the TRM for more information on this setting.
     * Configuration of the optional size thresholds when this configuration is
     * enabled is done by sending the @ref tisci_msg_rm_udmap_flow_size_thresh_cfg_req
     * message to System Firmware for the receive flow allocated by this request.
     * This parameter can be no greater than
     * @ref TISCI_MSG_VALUE_RM_UDMAP_RX_FLOW_SIZE_THRESH_MAX */
    uint8_t                 sizeThreshEn;
#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    bool  useRingMon;
#endif
} EnetAppRxDmaCfg_Info;

typedef struct EnetAppTxDmaCfg_Info_s
{
    /*! Whether to use the shared global event or not. If set to false, a dedicated event
     *  will be used for this channel. */
    bool  useGlobalEvt;
    /*! UDMA driver handle*/
    Udma_DrvHandle hUdmaDrv;
} EnetAppTxDmaCfg_Info;

static int32_t EnetAppUtils_allocRxFlowForChIdx(Enet_Handle hEnet,
                                                uint32_t coreKey,
                                                uint32_t coreId,
                                                uint32_t chIdx,
                                                uint32_t *rxFlowStartIdx,
                                                uint32_t *flowIdx);
static int32_t EnetAppUtils_allocTxCh(Enet_Handle hEnet,
                                      uint32_t coreKey,
                                      uint32_t coreId,
                                      uint32_t *txPSILThreadId);
static int32_t EnetAppUtils_freeRxFlowForChIdx(Enet_Handle hEnet,
                                               uint32_t coreKey,
                                               uint32_t coreId,
                                               uint32_t chIdx,
                                               uint32_t rxFlowIdx);
static int32_t EnetAppUtils_freeTxCh(Enet_Handle hEnet,
                                     uint32_t coreKey,
                                     uint32_t coreId,
                                     uint32_t txChNum);
static uint32_t EnetAppUtils_getStartFlowIdx(Enet_Handle hEnet,
                                             uint32_t coreId);
static void EnetAppUtils_absFlowIdx2FlowIdxOffset(Enet_Handle hEnet,
                                                  uint32_t coreId,
                                                  uint32_t absRxFlowId,
                                                  uint32_t *pStartFlowIdx,
                                                  uint32_t *pFlowIdxOffset);
static void EnetAppUtils_openTxCh(Enet_Handle hEnet,
                                  uint32_t coreKey,
                                  uint32_t coreId,
                                  uint32_t *pTxChNum,
                                  EnetDma_TxChHandle *pTxChHandle,
                                  EnetUdma_OpenTxChPrms *pCpswTxChCfg);

static void EnetAppUtils_closeTxCh(Enet_Handle hEnet,
                                   uint32_t coreKey,
                                   uint32_t coreId,
                                   EnetDma_PktQ *pFqPktInfoQ,
                                   EnetDma_PktQ *pCqPktInfoQ,
                                   EnetDma_TxChHandle hTxChHandle,
                                   uint32_t txChNum);

static void EnetAppUtils_openRxFlowForChIdx(Enet_Type enetType,
                                            Enet_Handle hEnet,
                                            uint32_t coreKey,
                                            uint32_t coreId,
                                            bool useDfltFlow,
                                            uint32_t numAllocMacAddr,
                                            uint32_t chIdx,
                                            uint32_t *pRxFlowStartIdx,
                                            uint32_t *pRxFlowIdx,
                                            uint8_t macAddr[ENET_MAX_NUM_MAC_PER_PHER][ENET_MAC_ADDR_LEN],
                                            EnetDma_RxChHandle *pRxFlowHandle,
                                            EnetUdma_OpenRxFlowPrms *pRxFlowPrms);

static void EnetAppUtils_closeRxFlowForChIdx(Enet_Type enetType,
                                            Enet_Handle hEnet,
                                            uint32_t coreKey,
                                            uint32_t coreId,
                                            bool useDfltFlow,
                                            EnetDma_PktQ *pFqPktInfoQ,
                                            EnetDma_PktQ *pCqPktInfoQ,
                                            uint32_t chIdx,
                                            uint32_t rxFlowStartIdx,
                                            uint32_t rxFlowIdx,
                                            uint32_t numValidMacAddress,
                                            uint8_t macAddr[ENET_MAX_NUM_MAC_PER_PHER][ENET_MAC_ADDR_LEN],
                                            EnetDma_RxChHandle hRxFlow);

static void EnetApp_openRxDma(EnetAppRxDmaSysCfg_Obj *rx,
                              uint32_t numRxPkts,
                              Enet_Handle hEnet, 
                              uint32_t coreKey,
                              uint32_t coreId,
                              uint32_t allocMacAddrCnt,
                              EnetAppRxDmaCfg_Info *rxCfg);

static void EnetApp_openTxDma(EnetAppTxDmaSysCfg_Obj *tx,
                              uint32_t numTxPkts,
                              Enet_Handle hEnet, 
                              uint32_t coreKey,
                              uint32_t coreId,
                              EnetAppTxDmaCfg_Info *txCfg);


static void EnetAppUtils_setCommonRxFlowPrms(EnetUdma_OpenRxFlowPrms *pRxChPrms);

static void EnetAppUtils_setCommonTxChPrms(EnetUdma_OpenTxChPrms *pTxChPrms);

Udma_DrvHandle EnetApp_getUdmaInstanceHandle(void);


typedef struct EnetAppDmaSysCfg_Obj_s
{
    EnetAppTxDmaSysCfg_Obj tx[ENET_SYSCFG_TX_CHANNELS_NUM];
    EnetAppRxDmaSysCfg_Obj rx[ENET_SYSCFG_RX_FLOWS_NUM];
} EnetAppDmaSysCfg_Obj;


typedef struct EnetAppSysCfg_Obj_s
{

    Enet_Handle hEnet;
    EnetAppDmaSysCfg_Obj dma;
    ClockP_Object timerObj;

    TaskP_Object task_phyStateHandlerObj;

    SemaphoreP_Object timerSemObj;

    volatile bool timerTaskShutDownFlag;

    volatile bool timerTaskShutDownDoneFlag;

    uint8_t appPhyStateHandlerTaskStack[ENETAPP_PHY_STATEHANDLER_TASK_STACK] __attribute__ ((aligned(32)));
}EnetAppSysCfg_Obj;

static EnetAppSysCfg_Obj gEnetAppSysCfgObj;

static int32_t EnetApp_enablePorts(Enet_Handle hEnet,
                                   Enet_Type enetType,
                                   uint32_t instId,
                                   uint32_t coreId,
                                   Enet_MacPort macPortList[ENET_MAC_PORT_NUM],
                                   uint8_t numMacPorts);

static void EnetApp_getCpswInitCfg(Enet_Type enetType,
                                   uint32_t instId,
                                   Cpsw_Cfg *cpswCfg);

static void EnetApp_getMacPortInitConfig(CpswMacPort_Cfg *pMacPortCfg, const Enet_MacPort portIdx);

static void EnetApp_openDmaChannels(EnetAppDmaSysCfg_Obj *dma,
                                    Enet_Type enetType, 
                                    uint32_t instId,
                                    Enet_Handle hEnet, 
                                    uint32_t coreKey,
                                    uint32_t coreId);

static void EnetApp_openAllRxDmaChannels(EnetAppDmaSysCfg_Obj *dma,
                                         Enet_Handle hEnet, 
                                         uint32_t coreKey,
                                         uint32_t coreId);

static void EnetApp_openAllTxDmaChannels(EnetAppDmaSysCfg_Obj *dma,
                                         Enet_Handle hEnet, 
                                         uint32_t coreKey,
                                         uint32_t coreId);

static void EnetApp_ConfigureDscpMapping(Enet_Type enetType,
                                         uint32_t instId,
                                         uint32_t coreId,
                                         Enet_MacPort macPortList[ENET_MAC_PORT_NUM],
                                         uint8_t numMacPorts);


void EnetApp_phyStateHandler(void * appHandle);

static Enet_Handle EnetApp_doCpswOpen(Enet_Type enetType, uint32_t instId, const Cpsw_Cfg *cpswCfg)
{
    int32_t status = ENET_SOK;
    void *perCfg = NULL_PTR;
    uint32_t cfgSize;
    Enet_Handle hEnet;

    EnetAppUtils_assert(true == Enet_isCpswFamily(enetType));

    perCfg = (void *)cpswCfg;
    cfgSize = sizeof(*cpswCfg);

    hEnet = Enet_open(enetType, instId, perCfg, cfgSize);
    if(hEnet == NULL_PTR)
    {
        EnetAppUtils_print("Enet_open failed\r\n");
        EnetAppUtils_assert(hEnet != NULL_PTR);
    }

    status = EnetMem_init();
    EnetAppUtils_assert(ENET_SOK == status);

    return hEnet;
}


static void EnetApp_timerCb(ClockP_Object *clkInst, void * arg)
{
    SemaphoreP_Object *pTimerSem = (SemaphoreP_Object *)arg;

    /* Tick! */
    SemaphoreP_post(pTimerSem);
}
void EnetApp_phyStateHandler(void * appHandle)
{
    SemaphoreP_Object *timerSem;
    EnetAppSysCfg_Obj *hEnetAppObj       = (EnetAppSysCfg_Obj *)appHandle;

    timerSem = &hEnetAppObj->timerSemObj;
    hEnetAppObj->timerTaskShutDownDoneFlag = false;
    while (hEnetAppObj->timerTaskShutDownFlag != true)
    {
        SemaphoreP_pend(timerSem, SystemP_WAIT_FOREVER);
        /* Enet_periodicTick should be called from only task context */
        Enet_periodicTick(hEnetAppObj->hEnet);
    }
    hEnetAppObj->timerTaskShutDownDoneFlag = true;
    TaskP_destruct(&hEnetAppObj->task_phyStateHandlerObj);
    TaskP_exit();
}

static int32_t EnetApp_createPhyStateHandlerTask(EnetAppSysCfg_Obj *hEnetAppObj) // FREERTOS
{
    TaskP_Params tskParams;
    int32_t status;

    status = SemaphoreP_constructCounting(&hEnetAppObj->timerSemObj, 0, 128);
    EnetAppUtils_assert(status == SystemP_SUCCESS);
    {
        ClockP_Params clkParams;
        const uint32_t timPeriodTicks = ClockP_usecToTicks((ENETPHY_FSM_TICK_PERIOD_MS)*1000U);  // Set timer expiry time in OS ticks

        ClockP_Params_init(&clkParams);
        clkParams.start     = TRUE;
        clkParams.timeout   = timPeriodTicks;
        clkParams.period    = timPeriodTicks;
        clkParams.args      = &hEnetAppObj->timerSemObj;
        clkParams.callback  = &EnetApp_timerCb;

        /* Creating timer and setting timer callback function*/
        status = ClockP_construct(&hEnetAppObj->timerObj ,
                                  &clkParams);
        if (status == SystemP_SUCCESS)
        {
            hEnetAppObj->timerTaskShutDownFlag = false;
        }
        else
        {
            EnetAppUtils_print("EnetApp_createClock() failed to create clock\r\n");
        }
    }
    /* Initialize the taskperiodicTick params. Set the task priority higher than the
     * default priority (1) */
    TaskP_Params_init(&tskParams);
    tskParams.priority       = ENETAPP_PHY_STATEHANDLER_TASK_PRIORITY;
    tskParams.stack          = &hEnetAppObj->appPhyStateHandlerTaskStack[0];
    tskParams.stackSize      = sizeof(hEnetAppObj->appPhyStateHandlerTaskStack);
    tskParams.args           = hEnetAppObj;
    tskParams.name           = "EnetApp_PhyStateHandlerTask";
    tskParams.taskMain       =  &EnetApp_phyStateHandler;

    status = TaskP_construct(&hEnetAppObj->task_phyStateHandlerObj, &tskParams);
    EnetAppUtils_assert(status == SystemP_SUCCESS);

    return status;

}

void EnetApp_driverInit()
{
/* keep this implementation that is generic across enetType and instId.
 * Initialization should be done only once.
 */
    EnetOsal_Cfg osalPrms;
    EnetUtils_Cfg utilsPrms;

    /* Initialize Enet driver with default OSAL, utils */
    Enet_initOsalCfg(&osalPrms);
    Enet_initUtilsCfg(&utilsPrms);
    Enet_init(&osalPrms, &utilsPrms);
}

int32_t EnetApp_driverOpen(Enet_Type enetType, uint32_t instId)
{
    int32_t status = ENET_SOK;
    Cpsw_Cfg cpswCfg;
    Enet_MacPort macPortList[ENET_MAC_PORT_NUM];
    uint8_t numMacPorts;
    uint32_t selfCoreId;
    EnetRm_ResCfg *resCfg = &cpswCfg.resCfg;
    EnetApp_HandleInfo handleInfo;
    EnetPer_AttachCoreOutArgs attachInfo;

    EnetUdma_Cfg dmaCfg;

    EnetAppUtils_assert(Enet_isCpswFamily(enetType) == true);
    EnetApp_getEnetInstMacInfo(enetType, instId, macPortList, &numMacPorts);

    /* Set configuration parameters */
    /* Open UDMA */
    dmaCfg.rxChInitPrms.dmaPriority = UDMA_DEFAULT_RX_CH_DMA_PRIORITY;
    dmaCfg.hUdmaDrv = EnetApp_getUdmaInstanceHandle();
    selfCoreId   = EnetSoc_getCoreId();
    cpswCfg.dmaCfg = (void *)&dmaCfg;
    Enet_initCfg(enetType, instId, &cpswCfg, sizeof(cpswCfg));
    cpswCfg.dmaCfg = (void *)&dmaCfg;

    EnetApp_getCpswInitCfg(enetType, instId, &cpswCfg);

    resCfg = &cpswCfg.resCfg;
    EnetAppUtils_initResourceConfig(enetType, instId, selfCoreId, resCfg);


    EnetApp_updateCpswInitCfg(enetType, instId, &cpswCfg);


    gEnetAppSysCfgObj.hEnet = EnetApp_doCpswOpen(enetType, instId, &cpswCfg);
    EnetAppUtils_assert(NULL != gEnetAppSysCfgObj.hEnet);
    
    EnetApp_enablePorts(gEnetAppSysCfgObj.hEnet, enetType, instId, selfCoreId, macPortList, numMacPorts);

    EnetApp_ConfigureDscpMapping(enetType, instId, selfCoreId, macPortList, numMacPorts);
    status = EnetApp_createPhyStateHandlerTask(&gEnetAppSysCfgObj);
    EnetAppUtils_assert(status == SystemP_SUCCESS);
    /* Open all DMA channels */
    EnetApp_acquireHandleInfo(enetType, 
                              instId,
                              &handleInfo);

    (void)handleInfo; /* Handle info not used. Kill warning */
    EnetApp_coreAttach(enetType, 
                       instId,
                       selfCoreId,
                       &attachInfo);
    EnetApp_openDmaChannels(&gEnetAppSysCfgObj.dma,
                            enetType, 
                            instId,
                            gEnetAppSysCfgObj.hEnet, 
                            attachInfo.coreKey,
                            selfCoreId);
    return status;
}

static void EnetApp_ConfigureDscpMapping(Enet_Type enetType,
                                         uint32_t instId,
                                         uint32_t coreId,
                                         Enet_MacPort macPortList[ENET_MAC_PORT_NUM],
                                         uint8_t numMacPorts)
{
    Enet_IoctlPrms prms;
    int32_t status;
    EnetMacPort_SetIngressDscpPriorityMapInArgs setMacDscpInArgs;
    EnetPort_DscpPriorityMap setHostDscpInArgs;
    uint32_t pri;
    uint8_t i;

    Enet_Handle hEnet = Enet_getHandle(enetType, instId);
    memset(&setMacDscpInArgs, 0, sizeof(setMacDscpInArgs));
    /* Each Port can have different dscp priority mapping values */
    for (i = 0U; i < numMacPorts; i++)
    {
        setMacDscpInArgs.macPort = macPortList[i];
        setMacDscpInArgs.dscpPriorityMap.dscpIPv4En = true;
        /* Example Mapping: 0 to 7  dscp values are mapped to 0 priority
         *                  8 to 15 dscp values are mapped to 1 priority
         *                  and so on
         *                  56 to 63 dscp values are mapped to 7 priority
         */
        for (pri = 0U; pri < 64U; pri++)
        {
            setMacDscpInArgs.dscpPriorityMap.tosMap[pri] = pri/8;
        }
        ENET_IOCTL_SET_IN_ARGS(&prms, &setMacDscpInArgs);
        ENET_IOCTL(hEnet, coreId, ENET_MACPORT_IOCTL_SET_INGRESS_DSCP_PRI_MAP, &prms, status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to set dscp Priority map for Port %u - %d \r\n", macPortList[i], status);
        }
    }

    /* Fill the dscp priority mapping reg of host port */
    memset(&setHostDscpInArgs, 0, sizeof(setHostDscpInArgs));
    setHostDscpInArgs.dscpIPv4En = true;
    for (pri = 0U; pri < 64U; pri++)
    {
        setHostDscpInArgs.tosMap[pri] = pri/8;
    }
    ENET_IOCTL_SET_IN_ARGS(&prms, &setHostDscpInArgs);
    ENET_IOCTL(hEnet, coreId, ENET_HOSTPORT_IOCTL_SET_INGRESS_DSCP_PRI_MAP, &prms, status);
    if (status != ENET_SOK)
    {
        EnetAppUtils_print("Failed to set dscp Priority map for Host Port - %d\r\n", status);
    }
    /* Enable the p0_rx_remap_dscp_ipv4 in CPPI_P0_Control for host port, This is done through syscfg */

}

static int32_t EnetApp_enablePorts(Enet_Handle hEnet,
                                   Enet_Type enetType,
                                   uint32_t instId,
                                   uint32_t coreId,
                                   Enet_MacPort macPortList[ENET_MAC_PORT_NUM],
                                   uint8_t numMacPorts)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;
    uint8_t i;

    for (i = 0U; i < numMacPorts; i++)
    {
        EnetPer_PortLinkCfg linkArgs;
        CpswMacPort_Cfg cpswMacCfg;

        linkArgs.macCfg = &cpswMacCfg;
        CpswMacPort_initCfg(&cpswMacCfg);
        EnetPhy_initCfg(&linkArgs.phyCfg);

        linkArgs.macPort = macPortList[i];
        EnetApp_getMacPortInitConfig(linkArgs.macCfg, macPortList[i]);
        EnetApp_initLinkArgs(enetType, instId, &linkArgs, macPortList[i]);

        ENET_IOCTL_SET_IN_ARGS(&prms, &linkArgs);
        ENET_IOCTL(hEnet,
                   coreId,
                   ENET_PER_IOCTL_OPEN_PORT_LINK,
                   &prms,
                   status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("EnetApp_enablePorts() failed to open MAC port: %d\r\n", status);
        }

    }

    if ((status == ENET_SOK) && (Enet_isCpswFamily(enetType)))
    {
        CpswAle_SetPortStateInArgs setPortStateInArgs;

        setPortStateInArgs.portNum   = CPSW_ALE_HOST_PORT_NUM;
        setPortStateInArgs.portState = CPSW_ALE_PORTSTATE_FORWARD;
        ENET_IOCTL_SET_IN_ARGS(&prms, &setPortStateInArgs);
        prms.outArgs = NULL_PTR;
        ENET_IOCTL(hEnet,
                   coreId,
                   CPSW_ALE_IOCTL_SET_PORT_STATE,
                   &prms,
                   status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("EnetApp_enablePorts() failed CPSW_ALE_IOCTL_SET_PORT_STATE: %d\r\n", status);
        }

        if (status == ENET_SOK)
        {
            ENET_IOCTL_SET_NO_ARGS(&prms);
            ENET_IOCTL(hEnet,
                       coreId,
                       ENET_HOSTPORT_IOCTL_ENABLE,
                       &prms,
                       status);
            if (status != ENET_SOK)
            {
                EnetAppUtils_print("EnetApp_enablePorts() Failed to enable host port: %d\r\n", status);
            }
        }
    }

    /* Show alive PHYs */
    if (status == ENET_SOK)
    {
        Enet_IoctlPrms prms;
        bool alive;
        uint32_t i;

        for (i = 0U; i < ENET_MDIO_PHY_CNT_MAX; i++)
        {
            ENET_IOCTL_SET_INOUT_ARGS(&prms, &i, &alive);
            ENET_IOCTL(hEnet,
                       coreId,
                       ENET_MDIO_IOCTL_IS_ALIVE,
                       &prms,
                       status);
            if (status == ENET_SOK)
            {
                if (alive == true)
                {
                    EnetAppUtils_print("PHY %d is alive\r\n", i);
                }
            }
            else
            {
                EnetAppUtils_print("Failed to get PHY %d alive status: %d\r\n", i, status);
            }
        }
    }

    return status;
}

static void EnetApp_deleteClock(EnetAppSysCfg_Obj *hEnetAppObj)
{

    hEnetAppObj->timerTaskShutDownFlag = true;

    ClockP_stop(&hEnetAppObj->timerObj);

    /* Post Timer Sem once to get the Periodic Tick task terminated */
    SemaphoreP_post(&hEnetAppObj->timerSemObj);

    do
    {
        ClockP_usleep(ClockP_ticksToUsec(1));
    } while (hEnetAppObj->timerTaskShutDownDoneFlag != true);

    ClockP_destruct(&hEnetAppObj->timerObj);
    SemaphoreP_destruct(&hEnetAppObj->timerSemObj);

}
void EnetApp_driverClose(Enet_Type enetType, uint32_t instId)
{
    Enet_IoctlPrms prms;
    int32_t status;
    uint32_t selfCoreId;
    uint32_t i;
    Enet_MacPort macPortList[ENET_MAC_PORT_NUM];
    uint8_t numMacPorts;
    Enet_Handle hEnet = Enet_getHandle(enetType, instId);

    EnetAppUtils_assert(Enet_isCpswFamily(enetType) == true);
    selfCoreId   = EnetSoc_getCoreId();
    EnetApp_getEnetInstMacInfo(enetType, instId, macPortList, &numMacPorts);
    EnetApp_deleteClock(&gEnetAppSysCfgObj);
    /* Disable host port */
	ENET_IOCTL_SET_NO_ARGS(&prms);
	ENET_IOCTL(hEnet,
               selfCoreId,
               ENET_HOSTPORT_IOCTL_DISABLE,
               &prms,
               status);
	if (status != ENET_SOK)
	{
	    EnetAppUtils_print("Failed to disable host port: %d\r\n", status);
	}

    for (i = 0U; i < numMacPorts; i++)
    {
        Enet_MacPort macPort = macPortList[i];

        ENET_IOCTL_SET_IN_ARGS(&prms, &macPort);
        ENET_IOCTL(hEnet,
                   selfCoreId,
                   ENET_PER_IOCTL_CLOSE_PORT_LINK,
                   &prms,
                   status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("close() failed to close MAC port: %d\r\n", status);
        }
    }

    Enet_close(hEnet);

    EnetMem_deInit();

    Enet_deinit();
}




static uint32_t EnetApp_retrieveFreeTxPkts(EnetDma_TxChHandle hTxCh, EnetDma_PktQ *txPktInfoQ)
{
    EnetDma_PktQ txFreeQ;
    EnetDma_Pkt *pktInfo;
    uint32_t txFreeQCnt = 0U;
    int32_t status;

    EnetQueue_initQ(&txFreeQ);

    /* Retrieve any packets that may be free now */
    status = EnetDma_retrieveTxPktQ(hTxCh, &txFreeQ);
    if (status == ENET_SOK)
    {
        txFreeQCnt = EnetQueue_getQCount(&txFreeQ);

        pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&txFreeQ);
        while (NULL != pktInfo)
        {
            EnetDma_checkPktState(&pktInfo->pktState,
                                    ENET_PKTSTATE_MODULE_APP,
                                    ENET_PKTSTATE_APP_WITH_DRIVER,
                                    ENET_PKTSTATE_APP_WITH_FREEQ);

            EnetQueue_enq(txPktInfoQ, &pktInfo->node);
            pktInfo = (EnetDma_Pkt *)EnetQueue_deq(&txFreeQ);
        }
    }
    else
    {
        EnetAppUtils_print("retrieveFreeTxPkts() failed to retrieve pkts: %d\r\n", status);
    }

    return txFreeQCnt;
}



static void EnetApp_openDmaChannels(EnetAppDmaSysCfg_Obj *dma,
                                    Enet_Type enetType,
                                    uint32_t instId,
                                    Enet_Handle hEnet, 
                                    uint32_t coreKey,
                                    uint32_t coreId)
{
    EnetApp_openAllTxDmaChannels(dma,
                                 hEnet, 
                                 coreKey,
                                 coreId);
    EnetApp_openAllRxDmaChannels(dma,
                                 hEnet, 
                                 coreKey,
                                 coreId);
}


/*
 * Function to Poll the Verify status and restart verification.
 * Application needs to call this after link-up, Verify timeout
 * is set based on the link-speed from ENET handle.
 */
void EnetApp_doIetVerification(Enet_Handle hEnet,
                               uint32_t coreId,
                               Enet_MacPort macPort)
{
    uint32_t try = ENETAPP_NUM_IET_VERIFY_ATTEMPTS;
    int32_t status = ENET_SOK;
    EnetMacPort_GenericInArgs fpe;
    Enet_IoctlPrms prms;
    EnetMacPort_PreemptVerifyStatus verifyStatus;

    fpe.macPort = macPort;
    do{
        ENET_IOCTL_SET_IN_ARGS(&prms, &fpe);
        ENET_IOCTL(hEnet, coreId, ENET_MACPORT_IOCTL_ENABLE_PREEMPT_VERIFICATION , &prms, status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to start IET verification: %d\r\n", status);
            break;
        }
        /*
         * Takes 10 msec to complete this in h/w assuming other
         * side is already ready. However since both side might
         * take variable setup/config time, need to Wait for
         * additional time. Chose 50 msec through trials
         */
        ClockP_usleep(50000U);
        ENET_IOCTL_SET_INOUT_ARGS(&prms, &fpe, &verifyStatus);
        ENET_IOCTL(hEnet, coreId, ENET_MACPORT_IOCTL_GET_PREEMPT_VERIFY_STATUS , &prms, status);
        if (status != ENET_SOK)
        {
            EnetAppUtils_print("Failed to read IET verify status : %d\r\n", status);
            break;
        }
        if(verifyStatus == ENET_MAC_VERIFYSTATUS_SUCCEEDED)
        {
            EnetAppUtils_print("IET verify Success \r\n");
            break;
        }
        else if(verifyStatus == ENET_MAC_VERIFYSTATUS_FAILED )
        {
            EnetAppUtils_print("IET verify failed, trying again \r\n");
        }
        else
        {
            EnetAppUtils_print("Unexpected IET Failure : %d \r\n", verifyStatus);
            break;
        }
        try--;
    } while(try > 0);

    if(try == 0){
        EnetAppUtils_print("IET verify timeout \r\n");
    }
}

static void EnetApp_setPortTsEventPrms(CpswMacPort_TsEventCfg *tsPortEventCfg)
{
    memset(tsPortEventCfg, 0, sizeof(CpswMacPort_TsEventCfg));

    tsPortEventCfg->txAnnexFEn = true;
    tsPortEventCfg->rxAnnexFEn = true;
    tsPortEventCfg->txHostTsEn = true;
    /* Enable ts for SYNC, PDELAY_REQUEST, PDELAY_RESPONSE */
    tsPortEventCfg->messageType = 13;
    tsPortEventCfg->seqIdOffset = 30;
    tsPortEventCfg->domainOffset = 4;
}

int32_t EnetApp_enablePortTsEvent(Enet_Handle hEnet, uint32_t coreId, uint32_t macPort[], uint32_t numPorts)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;
    CpswMacPort_EnableTsEventInArgs enableTsEventInArgs;
    uint8_t i = 0U;
    
    EnetApp_setPortTsEventPrms(&(enableTsEventInArgs.tsEventCfg));
    for (i = 0U; i < numPorts; i++) 
    {
        enableTsEventInArgs.macPort = (Enet_MacPort)macPort[i];
        ENET_IOCTL_SET_IN_ARGS(&prms, &enableTsEventInArgs);
        ENET_IOCTL(hEnet, coreId, CPSW_MACPORT_IOCTL_ENABLE_CPTS_EVENT, &prms, status);
        if (status != ENET_SOK) 
        {
            EnetAppUtils_print("Enet_ioctl ENABLE_CPTS_EVENT failed %d port %d\n", status, macPort[i]);
        }
    }

    return status;
}

int32_t EnetApp_getRxTimeStamp(Enet_Handle hEnet, uint32_t coreId, EnetTimeSync_GetEthTimestampInArgs* inArgs, uint64_t *ts)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;

    ENET_IOCTL_SET_INOUT_ARGS(&prms, inArgs, ts);
    ENET_IOCTL(hEnet, coreId, ENET_TIMESYNC_IOCTL_GET_ETH_RX_TIMESTAMP, &prms, status);

    return status;
}

int32_t EnetApp_setTimeStampComplete(Enet_Handle hEnet, uint32_t coreId)
{
    int32_t status = ENET_SOK;
    /* Not supported for CPSW, the completion ioctl is only vaild for ICSSG */
    /* Need this funtion definition to align CPSW and ICSSG for TSN libs */
    return status;
}






static int32_t EnetAppUtils_allocRxFlowForChIdx(Enet_Handle hEnet,
                                                uint32_t coreKey,
                                                uint32_t coreId,
                                                uint32_t chIdx,
                                                uint32_t *rxFlowStartIdx,
                                                uint32_t *flowIdx)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;
    EnetRm_AllocRxFlowInArgs inArgs;
    EnetRm_AllocRxFlow rxFlowPrms;

    inArgs.coreKey = coreKey;
    inArgs.chIdx   = chIdx;

    ENET_IOCTL_SET_INOUT_ARGS(&prms, &inArgs, &rxFlowPrms);
    ENET_IOCTL(hEnet,
               coreId,
               ENET_RM_IOCTL_ALLOC_RX_FLOW,
               &prms,
               status);

    if (status == ENET_SOK)
    {
        *rxFlowStartIdx = rxFlowPrms.startIdx;
        *flowIdx        = rxFlowPrms.flowIdx;
    }
    else
    {
        EnetAppUtils_print("EnetAppUtils_allocRxFlowForChIdx() failed : %d\n", status);
    }

    return status;
}

static int32_t EnetAppUtils_allocTxCh(Enet_Handle hEnet,
                                      uint32_t coreKey,
                                      uint32_t coreId,
                                      uint32_t *txPSILThreadId)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;

    /* Allocate Tx Ch */
    ENET_IOCTL_SET_INOUT_ARGS(&prms, &coreKey, txPSILThreadId);
    ENET_IOCTL(hEnet,
               coreId,
               ENET_RM_IOCTL_ALLOC_TX_CH_PEERID,
               &prms,
               status);
    if (status != ENET_SOK)
    {
        *txPSILThreadId = ENET_RM_TXCHNUM_INVALID;
        EnetAppUtils_print("EnetAppUtils_allocTxCh() failed: %d\n", status);
    }

    return status;
}

static int32_t EnetAppUtils_freeRxFlow(Enet_Handle hEnet,
                                       uint32_t coreKey,
                                       uint32_t coreId,
                                       uint32_t rxFlowIdx)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;
    EnetRm_FreeRxFlowInArgs freeRxFlowInArgs;

    /*Free Rx Flow*/
    freeRxFlowInArgs.coreKey = coreKey;
    freeRxFlowInArgs.flowIdx = rxFlowIdx;
    freeRxFlowInArgs.chIdx   = 0U;

    ENET_IOCTL_SET_IN_ARGS(&prms, &freeRxFlowInArgs);
    ENET_IOCTL(hEnet,
               coreId,
               ENET_RM_IOCTL_FREE_RX_FLOW,
               &prms,
               status);

    return status;
}

static int32_t EnetAppUtils_freeRxFlowForChIdx(Enet_Handle hEnet,
                                               uint32_t coreKey,
                                               uint32_t coreId,
                                               uint32_t chIdx,
                                               uint32_t rxFlowIdx)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;
    EnetRm_FreeRxFlowInArgs freeRxFlowInArgs;

    /*Free Rx Flow*/
    freeRxFlowInArgs.coreKey = coreKey;
    freeRxFlowInArgs.flowIdx = rxFlowIdx;
    freeRxFlowInArgs.chIdx   = chIdx;

    ENET_IOCTL_SET_IN_ARGS(&prms, &freeRxFlowInArgs);
    ENET_IOCTL(hEnet,
               coreId,
               ENET_RM_IOCTL_FREE_RX_FLOW,
               &prms,
               status);

    return status;
}

static int32_t EnetAppUtils_freeTxCh(Enet_Handle hEnet,
                                     uint32_t coreKey,
                                     uint32_t coreId,
                                     uint32_t txChNum)
{
    int32_t status = ENET_SOK;
    Enet_IoctlPrms prms;
    EnetRm_FreeTxChInArgs freeTxChInArgs;

    /* Release Tx Ch */
    freeTxChInArgs.coreKey = coreKey;
    freeTxChInArgs.txChNum = txChNum;

    ENET_IOCTL_SET_IN_ARGS(&prms, &freeTxChInArgs);
    ENET_IOCTL(hEnet,
               coreId,
               ENET_RM_IOCTL_FREE_TX_CH_PEERID,
               &prms,
               status);

    return status;
}


static uint32_t EnetAppUtils_getStartFlowIdx(Enet_Handle hEnet,
                                             uint32_t coreId)
{
    Enet_IoctlPrms prms;
    uint32_t p0FlowIdOffset;
    int32_t status;

    ENET_IOCTL_SET_OUT_ARGS(&prms, &p0FlowIdOffset);
    ENET_IOCTL(hEnet,
               coreId,
               CPSW_HOSTPORT_GET_FLOW_ID_OFFSET,
               &prms,
               status);

    EnetAppUtils_assert(status == ENET_SOK);
    return p0FlowIdOffset;
}

static void EnetAppUtils_absFlowIdx2FlowIdxOffset(Enet_Handle hEnet,
                                                  uint32_t coreId,
                                                  uint32_t absRxFlowId,
                                                  uint32_t *pStartFlowIdx,
                                                  uint32_t *pFlowIdxOffset)
{
    uint32_t p0FlowIdOffset;

    p0FlowIdOffset = EnetAppUtils_getStartFlowIdx(hEnet, coreId);
    EnetAppUtils_assert(absRxFlowId >= p0FlowIdOffset);
    *pStartFlowIdx  = p0FlowIdOffset;
    *pFlowIdxOffset = (absRxFlowId - p0FlowIdOffset);
}

static void EnetAppUtils_openTxCh(Enet_Handle hEnet,
                                  uint32_t coreKey,
                                  uint32_t coreId,
                                  uint32_t *pTxChNum,
                                  EnetDma_TxChHandle *pTxChHandle,
                                  EnetUdma_OpenTxChPrms *pCpswTxChCfg)
{
    EnetDma_Handle hDma = Enet_getDmaHandle(hEnet);
    int32_t status;

    EnetAppUtils_assert(hDma != NULL);

    status = EnetAppUtils_allocTxCh(hEnet,
                                    coreKey,
                                    coreId,
                                    pTxChNum);
    EnetAppUtils_assert(ENET_SOK == status);

    pCpswTxChCfg->chNum = *pTxChNum;

    *pTxChHandle = EnetDma_openTxCh(hDma, pCpswTxChCfg);
    EnetAppUtils_assert(NULL != *pTxChHandle);
}

static void EnetAppUtils_closeTxCh(Enet_Handle hEnet,
                                   uint32_t coreKey,
                                   uint32_t coreId,
                                   EnetDma_PktQ *pFqPktInfoQ,
                                   EnetDma_PktQ *pCqPktInfoQ,
                                   EnetDma_TxChHandle hTxChHandle,
                                   uint32_t txChNum)
{
    int32_t status;

    EnetQueue_initQ(pFqPktInfoQ);
    EnetQueue_initQ(pCqPktInfoQ);

    EnetDma_disableTxEvent(hTxChHandle);
    status = EnetDma_closeTxCh(hTxChHandle, pFqPktInfoQ, pCqPktInfoQ);
    EnetAppUtils_assert(ENET_SOK == status);

    status = EnetAppUtils_freeTxCh(hEnet,
                                   coreKey,
                                   coreId,
                                   txChNum);
    EnetAppUtils_assert(ENET_SOK == status);
}


static void EnetAppUtils_openRxFlowForChIdx(Enet_Type enetType,
                                            Enet_Handle hEnet,
                                            uint32_t coreKey,
                                            uint32_t coreId,
                                            bool useDefaultFlow,
                                            uint32_t allocMacAddrCnt,
                                            uint32_t chIdx,
                                            uint32_t *pRxFlowStartIdx,
                                            uint32_t *pRxFlowIdx,
                                            uint8_t macAddr[ENET_MAX_NUM_MAC_PER_PHER][ENET_MAC_ADDR_LEN],
                                            EnetDma_RxChHandle *pRxFlowHandle,
                                            EnetUdma_OpenRxFlowPrms *pRxFlowPrms)
{
    EnetDma_Handle hDma = Enet_getDmaHandle(hEnet);
    int32_t status = ENET_SOK;

    EnetAppUtils_assert(hDma != NULL);

    status = EnetAppUtils_allocRxFlowForChIdx(hEnet,
                                              coreKey,
                                              coreId,
                                              chIdx,
                                              pRxFlowStartIdx,
                                              pRxFlowIdx);
    EnetAppUtils_assert(status == ENET_SOK);

    pRxFlowPrms->startIdx = *pRxFlowStartIdx;
    pRxFlowPrms->flowIdx  = *pRxFlowIdx;
    pRxFlowPrms->chIdx    = chIdx;

    *pRxFlowHandle = EnetDma_openRxCh(hDma, pRxFlowPrms);
    EnetAppUtils_assert(*pRxFlowHandle != NULL);

    if (useDefaultFlow)
    {
        if (chIdx == 0U)
        {
           for (uint32_t i = 0; i < allocMacAddrCnt; i++)
           {
                status = EnetAppUtils_allocMac(hEnet, coreKey, coreId, macAddr[i]);
                EnetAppUtils_assert(status == ENET_SOK);

                if (Enet_isCpswFamily(enetType))
                {
                    EnetAppUtils_addHostPortEntry(hEnet, coreId, macAddr[i]);
                }
                else
                {
                   // Should we add this entry to ICSSG FDB?
                }
            }
        }

        status = EnetAppUtils_regDfltRxFlowForChIdx(hEnet,
                                                    coreKey,
                                                    coreId,
                                                    chIdx,
                                                    *pRxFlowStartIdx,
                                                    *pRxFlowIdx);
    }
    else
    {
        for (uint32_t i = 0; i < allocMacAddrCnt; i++)
        {
            if (Enet_isCpswFamily(enetType))
            {
                EnetAppUtils_assert(chIdx == 0U);

                status = EnetAppUtils_allocMac(hEnet, coreKey, coreId, macAddr[i]);
                EnetAppUtils_assert(status == ENET_SOK);
                if (i == 0)
                {
                    status = EnetAppUtils_regDstMacRxFlow(hEnet,
                                                          coreKey,
                                                          coreId,
                                                          *pRxFlowStartIdx,
                                                          *pRxFlowIdx,
                                                          macAddr[i]);
                }
            }
        }
    }

    EnetAppUtils_assert(status == ENET_SOK);
}

static void EnetAppUtils_closeRxFlowForChIdx(Enet_Type enetType,
                                            Enet_Handle hEnet,
                                            uint32_t coreKey,
                                            uint32_t coreId,
                                            bool useDefaultFlow,
                                            EnetDma_PktQ *pFqPktInfoQ,
                                            EnetDma_PktQ *pCqPktInfoQ,
                                            uint32_t chIdx,
                                            uint32_t rxFlowStartIdx,
                                            uint32_t rxFlowIdx,
                                            uint32_t allocMacAddrCnt,
                                            uint8_t macAddr[ENET_MAX_NUM_MAC_PER_PHER][ENET_MAC_ADDR_LEN],
                                            EnetDma_RxChHandle hRxFlow)
{
    int32_t status = ENET_SOK;

    EnetQueue_initQ(pFqPktInfoQ);
    EnetQueue_initQ(pCqPktInfoQ);

    EnetDma_disableRxEvent(hRxFlow);

    if (useDefaultFlow)
    {
        status = EnetAppUtils_unregDfltRxFlowForChIdx(hEnet,
                                                      coreKey,
                                                      coreId,
                                                      chIdx,
                                                      rxFlowStartIdx,
                                                      rxFlowIdx);
        EnetAppUtils_assert(status == ENET_SOK);

        for (uint32_t i = 0; i < allocMacAddrCnt; i++)
        {
            status = EnetAppUtils_freeMac(hEnet,
                                          coreKey,
                                          coreId,
                                          macAddr[i]);
            EnetAppUtils_assert(status == ENET_SOK);

            if (Enet_isCpswFamily(enetType))
            {
                EnetAppUtils_delAddrEntry(hEnet, coreId, macAddr[i]);
            }
        }
    }
    else
    {
        if (Enet_isCpswFamily(enetType))
        {
            EnetAppUtils_assert(allocMacAddrCnt > 0);

            for (uint32_t i = 0; i < allocMacAddrCnt; i++)
            {
                if (i == 0)
                {
                    status = EnetAppUtils_unregDstMacRxFlow(hEnet,
                                                    coreKey,
                                                    coreId,
                                                    rxFlowStartIdx,
                                                    rxFlowIdx,
                                                    macAddr[i]);
                    EnetAppUtils_assert(status == ENET_SOK);
                }
                EnetAppUtils_delAddrEntry(hEnet, coreId, macAddr[i]);

                status = EnetAppUtils_freeMac(hEnet,
                                          coreKey,
                                          coreId,
                                          macAddr[i]);
                EnetAppUtils_assert(status == ENET_SOK);
            }
        }
    }

    status = EnetDma_closeRxCh(hRxFlow, pFqPktInfoQ, pCqPktInfoQ);
    EnetAppUtils_assert(status == ENET_SOK);


    status = EnetAppUtils_freeRxFlowForChIdx(hEnet,
                                             coreKey,
                                             coreId,
                                             chIdx,
                                             rxFlowIdx);
    EnetAppUtils_assert(status == ENET_SOK);
}


static void EnetApp_txPktNotifyCb(void *cbArg)
{


}

static void EnetApp_rxPktNotifyCb(void *cbArg)
{


}

static void EnetApp_openTxDma(EnetAppTxDmaSysCfg_Obj *tx,
                              uint32_t numTxPkts,
                              Enet_Handle hEnet,
                              uint32_t coreKey,
                              uint32_t coreId,
                              EnetAppTxDmaCfg_Info *txCfg)
{
    EnetUdma_OpenTxChPrms enetTxChCfg;

    EnetDma_initTxChParams(&enetTxChCfg);
    EnetAppUtils_setCommonTxChPrms(&enetTxChCfg);
    enetTxChCfg.hUdmaDrv  = txCfg->hUdmaDrv;
    /* This param is dont care for UDMA LCDMA variant */
    enetTxChCfg.useProxy  = false;
    enetTxChCfg.numTxPkts = numTxPkts;
    enetTxChCfg.cbArg     = tx;
    enetTxChCfg.notifyCb  = EnetApp_txPktNotifyCb;
    enetTxChCfg.useGlobalEvt = txCfg->useGlobalEvt;


    EnetAppUtils_openTxCh(hEnet,
                          coreKey,
                          coreId,
                          &tx->txChNum,
                          &tx->hTxCh,
                          &enetTxChCfg);
}

#define ENET_CPSW_RX_CH_ID  (0U)

static void EnetApp_openRxDma(EnetAppRxDmaSysCfg_Obj *rx,
                              uint32_t numRxPkts,
                              Enet_Handle hEnet,
                              uint32_t coreKey,
                              uint32_t coreId,
                              uint32_t allocMacAddrCnt,
                              EnetAppRxDmaCfg_Info *rxCfg)
{
    EnetUdma_OpenRxFlowPrms enetRxFlowCfg;
    int32_t status;
    Enet_Type enetType;
    uint32_t instId;
    const  uint32_t chIdx = ENET_CPSW_RX_CH_ID;

    status = Enet_getHandleInfo(hEnet,
                                &enetType,
                                &instId);
    EnetAppUtils_assert(status == ENET_SOK);
    (void)instId; /* Instd id not used */

    EnetDma_initRxChParams(&enetRxFlowCfg);
    EnetAppUtils_setCommonRxFlowPrms(&enetRxFlowCfg);
    enetRxFlowCfg.notifyCb  = EnetApp_rxPktNotifyCb;
    enetRxFlowCfg.numRxPkts = numRxPkts;
    enetRxFlowCfg.cbArg     = rx;
    enetRxFlowCfg.hUdmaDrv  = rxCfg->hUdmaDrv;
    /* This param is dont care for UDMA LCDMA variant */
    enetRxFlowCfg.useProxy  = false;
    enetRxFlowCfg.useGlobalEvt = rxCfg->useGlobalEvt;
    enetRxFlowCfg.flowPrms.sizeThreshEn = rxCfg->sizeThreshEn;

#if (UDMA_SOC_CFG_UDMAP_PRESENT == 1)
    {
        /* Use ring monitor for the CQ ring of RX flow */
        EnetUdma_UdmaRingPrms *pFqRingPrms = &enetRxFlowCfg.udmaChPrms.fqRingPrms;
        pFqRingPrms->useRingMon = rxCfg->useRingMon;
        pFqRingPrms->ringMonCfg.mode = TISCI_MSG_VALUE_RM_MON_MODE_THRESHOLD;
        /* Ring mon low threshold */

#if defined _DEBUG_
        /* In debug mode as CPU is processing lesser packets per event, keep threshold more */
        pFqRingPrms->ringMonCfg.data0 = (numRxPkts - 10U);
#else
        pFqRingPrms->ringMonCfg.data0 = (numRxPkts - 20U);
#endif
        /* Ring mon high threshold - to get only low  threshold event, setting high threshold as more than ring depth*/
        pFqRingPrms->ringMonCfg.data1 = numRxPkts;
    }
#endif


    EnetAppUtils_openRxFlowForChIdx(enetType,
                            hEnet,
                            coreKey,
                            coreId,
                            rxCfg->useDefaultFlow,
                            allocMacAddrCnt,
                            chIdx,
                            &rx->rxFlowStartIdx,
                            &rx->rxFlowIdx,
                            rx->macAddr,
                            &rx->hRxCh,
                            &enetRxFlowCfg);
    rx->numValidMacAddress = allocMacAddrCnt;

}

void EnetApp_closeTxDma(uint32_t enetTxDmaChId,
                        Enet_Handle hEnet,
                        uint32_t coreKey,
                        uint32_t coreId,
                        EnetDma_PktQ *fqPktInfoQ,
                        EnetDma_PktQ *cqPktInfoQ)
{
    EnetAppTxDmaSysCfg_Obj *tx;

    EnetAppUtils_assert(enetTxDmaChId < ENET_ARRAYSIZE(gEnetAppSysCfgObj.dma.tx));
    tx = &gEnetAppSysCfgObj.dma.tx[enetTxDmaChId];
    EnetQueue_initQ(fqPktInfoQ);
    EnetQueue_initQ(cqPktInfoQ);
    EnetApp_retrieveFreeTxPkts(tx->hTxCh, cqPktInfoQ);
    EnetAppUtils_closeTxCh(hEnet,
                           coreKey,
                           coreId,
                           fqPktInfoQ,
                           cqPktInfoQ,
                           tx->hTxCh,
                           tx->txChNum);
    memset(tx, 0, sizeof(*tx));
}

void EnetApp_closeRxDma(uint32_t enetRxDmaChId,
                        Enet_Handle hEnet,
                        uint32_t coreKey,
                        uint32_t coreId,
                        EnetDma_PktQ *fqPktInfoQ,
                        EnetDma_PktQ *cqPktInfoQ)
{
    EnetAppRxDmaSysCfg_Obj *rx;
    Enet_Type enetType;
    uint32_t instId;
    const  uint32_t chIdx = ENET_CPSW_RX_CH_ID;
    int32_t status;
    const EnetApp_GetRxDmaHandleOutArgs rxDmaInfo[ENET_SYSCFG_RX_FLOWS_NUM] =
    {
        
        [0] = 
        {
                .maxNumRxPkts    = 32,
                .numValidMacAddress = 1,
                .useGlobalEvt    = false,
                .useDefaultFlow  = true,
                .sizeThreshEn    = 7,
        }

    };

    status = Enet_getHandleInfo(hEnet,
                                &enetType,
                                &instId);
    EnetAppUtils_assert(status == ENET_SOK);
    (void)instId; /* Instd id not used */

    EnetAppUtils_assert(enetRxDmaChId < ENET_ARRAYSIZE(gEnetAppSysCfgObj.dma.rx));
    rx = &gEnetAppSysCfgObj.dma.rx[enetRxDmaChId];
    /* Close RX channel */
    EnetQueue_initQ(fqPktInfoQ);
    EnetQueue_initQ(cqPktInfoQ);

    EnetAppUtils_assert(enetRxDmaChId < ENET_ARRAYSIZE(rxDmaInfo));
    EnetAppUtils_closeRxFlowForChIdx(enetType,
                                     hEnet,
                                     coreKey,
                                     coreId,
                                     rxDmaInfo[enetRxDmaChId].useDefaultFlow,
                                     fqPktInfoQ,
                                     cqPktInfoQ,
                                     chIdx,
                                     rx->rxFlowStartIdx,
                                     rx->rxFlowIdx,
                                     rx->numValidMacAddress,
                                     rx->macAddr,
                                     rx->hRxCh);

    EnetAppSoc_releaseMacAddrList(rx->macAddr, rx->numValidMacAddress);
    memset(rx, 0, sizeof(*rx));
}

void EnetApp_getTxDmaHandle(uint32_t enetTxDmaChId,
                            const EnetApp_GetDmaHandleInArgs *inArgs,
                            EnetApp_GetTxDmaHandleOutArgs *outArgs)
{
    int32_t status;
    EnetAppTxDmaSysCfg_Obj *tx;
    const uint32_t txNumPkts[ENET_SYSCFG_TX_CHANNELS_NUM] =
                           {
                            16
                           };
    const uint32_t useGlobalEvt[ENET_SYSCFG_TX_CHANNELS_NUM] =
                           {
                             false
                           };

    EnetAppUtils_assert(enetTxDmaChId < ENET_ARRAYSIZE(gEnetAppSysCfgObj.dma.tx));
    tx = &gEnetAppSysCfgObj.dma.tx[enetTxDmaChId];

    EnetAppUtils_assert(tx->hTxCh != NULL);
    status = EnetDma_registerTxEventCb(tx->hTxCh, inArgs->notifyCb, inArgs->cbArg);
    EnetAppUtils_assert(status == ENET_SOK);

    outArgs->hTxCh = tx->hTxCh;
    outArgs->txChNum = tx->txChNum;
    EnetAppUtils_assert(enetTxDmaChId < ENET_ARRAYSIZE(txNumPkts));
    outArgs->maxNumTxPkts = txNumPkts[enetTxDmaChId];
    outArgs->useGlobalEvt = useGlobalEvt[enetTxDmaChId];
    return;
}

void EnetApp_getMacAddress(uint32_t enetRxDmaChId,
                            EnetApp_GetMacAddrOutArgs *outArgs)
{

    EnetAppUtils_assert(enetRxDmaChId < ENET_ARRAYSIZE(gEnetAppSysCfgObj.dma.rx));
    EnetAppRxDmaSysCfg_Obj* rx = &gEnetAppSysCfgObj.dma.rx[enetRxDmaChId];

    outArgs->macAddressCnt = rx->numValidMacAddress;
    EnetAppUtils_assert(outArgs->macAddressCnt <= ENET_MAX_NUM_MAC_PER_PHER);
    for (uint32_t i = 0; i < outArgs->macAddressCnt; i++)
    {
        EnetUtils_copyMacAddr(outArgs->macAddr[i], rx->macAddr[i]);
    }

}

void EnetApp_getRxDmaHandle(uint32_t enetRxDmaChId,
                            const EnetApp_GetDmaHandleInArgs *inArgs,
                            EnetApp_GetRxDmaHandleOutArgs *outArgs)
{
    int32_t status;
    EnetAppRxDmaSysCfg_Obj *rx;
    const EnetApp_GetRxDmaHandleOutArgs rxDmaInfo[ENET_SYSCFG_RX_FLOWS_NUM] =
    {
        
        [0] = 
        {
                .maxNumRxPkts    = 32,
                .numValidMacAddress = 1,
                .useGlobalEvt    = false,
                .useDefaultFlow  = true,
                .sizeThreshEn    = 7,
        }

    };

    EnetAppUtils_assert(enetRxDmaChId < ENET_ARRAYSIZE(gEnetAppSysCfgObj.dma.rx));
    rx = &gEnetAppSysCfgObj.dma.rx[enetRxDmaChId];

    EnetAppUtils_assert(rx->hRxCh != NULL);
    status = EnetDma_registerRxEventCb(rx->hRxCh, inArgs->notifyCb, inArgs->cbArg);
    EnetAppUtils_assert(status == ENET_SOK);

    outArgs->hRxCh = rx->hRxCh;
    outArgs->rxFlowIdx = rx->rxFlowIdx;
    outArgs->rxFlowStartIdx = rx->rxFlowStartIdx;
    EnetAppUtils_assert(enetRxDmaChId < ENET_ARRAYSIZE(rxDmaInfo));
    outArgs->numValidMacAddress = rxDmaInfo[enetRxDmaChId].numValidMacAddress;
    for (uint32_t i = 0; i < rxDmaInfo[enetRxDmaChId].numValidMacAddress; i++)
    {
        EnetUtils_copyMacAddr(outArgs->macAddr[i], rx->macAddr[i]);
    }
    outArgs->maxNumRxPkts = rxDmaInfo[enetRxDmaChId].maxNumRxPkts;
    outArgs->sizeThreshEn = rxDmaInfo[enetRxDmaChId].sizeThreshEn;
    outArgs->useDefaultFlow = rxDmaInfo[enetRxDmaChId].useDefaultFlow;
    outArgs->useGlobalEvt   = rxDmaInfo[enetRxDmaChId].useGlobalEvt;
    return;
}

static void EnetApp_openAllTxDmaChannels(EnetAppDmaSysCfg_Obj *dma,
                                         Enet_Handle hEnet,
                                         uint32_t coreKey,
                                         uint32_t coreId)
{
    const uint32_t txNumPkts[ENET_SYSCFG_TX_CHANNELS_NUM] =
                           {
                            16
                           };
    uint32_t i;
    const uint32_t useGlobalEvt[ENET_SYSCFG_TX_CHANNELS_NUM] =
                           {
                             false

                           };

    for (i = 0; i < ENET_SYSCFG_TX_CHANNELS_NUM;i++)
    {
        EnetAppTxDmaCfg_Info txDmaCfg;

        txDmaCfg.useGlobalEvt = useGlobalEvt[i];
        txDmaCfg.hUdmaDrv     = EnetApp_getUdmaInstanceHandle();
        EnetApp_openTxDma(&dma->tx[i], txNumPkts[i], hEnet, coreKey, coreId, &txDmaCfg);
    }
}

static void EnetApp_openAllRxDmaChannels(EnetAppDmaSysCfg_Obj *dma,
                                         Enet_Handle hEnet,
                                         uint32_t coreKey,
                                         uint32_t coreId)
{
    const uint32_t rxNumPkts[ENET_SYSCFG_RX_FLOWS_NUM] =
                           {
                            32
                           };
    const uint32_t allocMacAddrCnt[ENET_SYSCFG_RX_FLOWS_NUM] =
                           {
                            1
                           };
    const uint32_t useGlobalEvt[ENET_SYSCFG_RX_FLOWS_NUM] =
                           {
                             false

                           };
    const uint8_t  sizeThreshEn[ENET_SYSCFG_RX_FLOWS_NUM] =
                           {
                             7

                           };
    const bool     isDefaultFlow[ENET_SYSCFG_RX_FLOWS_NUM] =
                           {
                            true
                           };

    uint32_t i;

    for (i = 0; i < ENET_SYSCFG_RX_FLOWS_NUM;i++)
    {
        EnetAppRxDmaCfg_Info rxDmaCfg;

        rxDmaCfg.useGlobalEvt = useGlobalEvt[i];
        rxDmaCfg.hUdmaDrv     = EnetApp_getUdmaInstanceHandle();
        rxDmaCfg.sizeThreshEn = sizeThreshEn[i];
        rxDmaCfg.useDefaultFlow = isDefaultFlow[i];

        EnetApp_openRxDma(&dma->rx[i], rxNumPkts[i], hEnet, coreKey, coreId, allocMacAddrCnt[i], &rxDmaCfg);
    }
}

#define ENET_SYSCFG_DEFAULT_NUM_TX_PKT                                     (16U)
#define ENET_SYSCFG_DEFAULT_NUM_RX_PKT                                     (32U)

void EnetAppUtils_setCommonRxFlowPrms(EnetUdma_OpenRxFlowPrms *pRxFlowPrms)
{
    pRxFlowPrms->numRxPkts           = ENET_SYSCFG_DEFAULT_NUM_RX_PKT;
    pRxFlowPrms->disableCacheOpsFlag = false;
    pRxFlowPrms->rxFlowMtu           = ENET_MEM_LARGE_POOL_PKT_SIZE;

    pRxFlowPrms->ringMemAllocFxn = &EnetMem_allocRingMem;
    pRxFlowPrms->ringMemFreeFxn  = &EnetMem_freeRingMem;

    pRxFlowPrms->dmaDescAllocFxn = &EnetMem_allocDmaDesc;
    pRxFlowPrms->dmaDescFreeFxn  = &EnetMem_freeDmaDesc;
}

void EnetAppUtils_setCommonTxChPrms(EnetUdma_OpenTxChPrms *pTxChPrms)
{
    pTxChPrms->numTxPkts           = ENET_SYSCFG_DEFAULT_NUM_TX_PKT;
    pTxChPrms->disableCacheOpsFlag = false;

    pTxChPrms->ringMemAllocFxn = &EnetMem_allocRingMem;
    pTxChPrms->ringMemFreeFxn  = &EnetMem_freeRingMem;

    pTxChPrms->dmaDescAllocFxn = &EnetMem_allocDmaDesc;
    pTxChPrms->dmaDescFreeFxn  = &EnetMem_freeDmaDesc;
}


Udma_DrvHandle EnetApp_getUdmaInstanceHandle(void)
{
    Udma_DrvHandle hUdmaDrv;

    hUdmaDrv = &gUdmaDrvObj[CONFIG_UDMA_PKTDMA_0];
    return hUdmaDrv;
}





#include <string.h>

#include <enet.h>
#include <include/core/enet_utils.h>

#include <include/core/enet_dma.h>
#include <include/per/cpsw.h>

#include "enet_appmemutils.h"
#include "enet_appmemutils_cfg.h"
#include "enet_apputils.h"




static void EnetApp_initEnetLinkCbPrms(Cpsw_Cfg *cpswCfg)
{
    cpswCfg->mdioLinkStateChangeCb     = NULL;
    cpswCfg->mdioLinkStateChangeCbArg  = NULL;

    cpswCfg->portLinkStatusChangeCb    = NULL;
    cpswCfg->portLinkStatusChangeCbArg = NULL;
}

static const CpswAle_Cfg enetAppCpswAleCfg =
{
    .modeFlags = (CPSW_ALE_CFG_MODULE_EN),
    .policerGlobalCfg =
    {
        .policingEn         = false,
        .yellowDropEn       = false,
        .redDropEn          = false,
        .yellowThresh       = CPSW_ALE_POLICER_YELLOWTHRESH_DROP_PERCENT_100,
        .policerNoMatchMode = CPSW_ALE_POLICER_NOMATCH_MODE_GREEN,
        .noMatchPolicer     = {
                                  .peakRateInBitsPerSec   = 0,
                                  .commitRateInBitsPerSec = 0,
                              }
    },
    .agingCfg =
    {
        .autoAgingEn        = true,
        .agingPeriodInMs    = 1000,
    },
    .vlanCfg =
    {
        .aleVlanAwareMode   = true,
        .cpswVlanAwareMode  = false,
        .autoLearnWithVlan  = false,
        .unknownVlanNoLearn = false,
        .unknownForceUntaggedEgressMask = (0),
        .unknownRegMcastFloodMask       = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
        .unknownUnregMcastFloodMask     = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
        .unknownVlanMemberListMask      = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
    },
    .nwSecCfg =
    {
        .hostOuiNoMatchDeny  = false,
        .vid0ModeEn          = true,
        .malformedPktCfg     = {
                                   .srcMcastDropDis = false,
                                   .badLenPktDropEn = false,
                               },
        .ipPktCfg            = {
                                   .dfltNoFragEn          = false,
                                   .dfltNxtHdrWhitelistEn = false,
                                   .ipNxtHdrWhitelistCnt  =  0,
                                   .ipNxtHdrWhitelist     = {
                                                              
                                                            },
                               },


        .macAuthCfg          = {
                                   .authModeEn           = false,
                                   .macAuthDisMask       = (0 | CPSW_ALE_HOST_PORT_MASK),
                               },
    },
    .portCfg =
    {
        [CPSW_ALE_HOST_PORT_NUM] =
        {
            .learningCfg =
            {
                .noLearn         = false,
                .noSaUpdateEn    = false,
            },
            .vlanCfg =
            {
                .vidIngressCheck = false,
                .dropUntagged    = false,
                .dropDualVlan    = false,
                .dropDoubleVlan  = false,
            },
            .macModeCfg =
            {
                .macOnlyCafEn    = false,
                .macOnlyEn       = false,
            },
            .pvidCfg =
            {
                .vlanIdInfo      =
                {
                    .vlanId   = 0,
                    .tagType  = ENET_VLAN_TAG_TYPE_INNER,
                },
                .vlanMemberList          = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .unregMcastFloodMask     = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .regMcastFloodMask       = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .forceUntaggedEgressMask = (0),
                .noLearnMask             = (0),
                .vidIngressCheck         = false,
                .limitIPNxtHdr           = false,
                .disallowIPFrag          = false,
            },
        },
        [CPSW_ALE_MACPORT_TO_ALEPORT(ENET_MAC_PORT_1)] =
        {
            .learningCfg =
            {
                .noLearn         = false,
                .noSaUpdateEn    = false,
            },
            .vlanCfg =
            {
                .vidIngressCheck = false,
                .dropUntagged    = false,
                .dropDualVlan    = false,
                .dropDoubleVlan  = false,
            },
            .macModeCfg =
            {
                .macOnlyCafEn    = false,
                .macOnlyEn       = false,
            },
            .pvidCfg =
            {
                .vlanIdInfo      =
                {
                    .vlanId   = 0,
                    .tagType  = ENET_VLAN_TAG_TYPE_INNER,
                },
                .vlanMemberList          = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .unregMcastFloodMask     = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .regMcastFloodMask       = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .forceUntaggedEgressMask = (0),
                .noLearnMask             = (0),
                .vidIngressCheck         = false,
                .limitIPNxtHdr           = false,
                .disallowIPFrag          = false,
            },
        },
        [CPSW_ALE_MACPORT_TO_ALEPORT(ENET_MAC_PORT_2)] =
        {
            .learningCfg =
            {
                .noLearn         = false,
                .noSaUpdateEn    = false,
            },
            .vlanCfg =
            {
                .vidIngressCheck = false,
                .dropUntagged    = false,
                .dropDualVlan    = false,
                .dropDoubleVlan  = false,
            },
            .macModeCfg =
            {
                .macOnlyCafEn    = false,
                .macOnlyEn       = false,
            },
            .pvidCfg =
            {
                .vlanIdInfo      =
                {
                    .vlanId   = 0,
                    .tagType  = ENET_VLAN_TAG_TYPE_INNER,
                },
                .vlanMemberList          = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .unregMcastFloodMask     = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .regMcastFloodMask       = (0 | CPSW_ALE_HOST_PORT_MASK | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_1) | CPSW_ALE_MACPORT_TO_PORTMASK(ENET_MAC_PORT_2)),
                .forceUntaggedEgressMask = (0),
                .noLearnMask             = (0),
                .vidIngressCheck         = false,
                .limitIPNxtHdr           = false,
                .disallowIPFrag          = false,
            },
        },
    }

};

static const Mdio_Cfg enetAppCpswMdioCfg =
{
    .mode               = MDIO_MODE_STATE_CHANGE_MON,
    .mdioBusFreqHz      = 2200000,
    .phyStatePollFreqHz = 22000,
    .pollEnMask         = -1,
    .c45EnMask          = 0,
    .isMaster           = true,
    .disableStateMachineOnInit = false,
};


static const CpswCpts_Cfg enetAppCpswCptsCfg =
{
    .hostRxTsEn     = true,
    .tsCompPolarity = true,
    .tsRxEventsDis  = false,
    .tsGenfClrEn    = true,
    .cptsRftClkFreq = CPSW_CPTS_RFTCLK_FREQ_200MHZ,
};

static const CpswHostPort_Cfg enetAppCpswHostPortCfg =
{
    .crcType           = ENET_CRC_ETHERNET,
    .removeCrc         = false,
    .padShortPacket    = true,
    .passCrcErrors     = false,
    .rxMtu             = 1518,
    .passPriorityTaggedUnchanged = false,
    .rxCsumOffloadEn   = true,
    .txCsumOffloadEn   = true,
    .rxVlanRemapEn     = false,
    .rxDscpIPv4RemapEn = true,
    .rxDscpIPv6RemapEn = false,
    .vlanCfg           =
    {
        .portPri = 0,
        .portCfi = false,
        .portVID = 0,
    },
    .rxPriorityType    = ENET_INGRESS_PRI_TYPE_FIXED,
    .txPriorityType    = ENET_EGRESS_PRI_TYPE_FIXED,
};

static const CpswMacPort_Cfg enetAppCpswMacPortCfg[] =
{
    [ENET_MAC_PORT_1] =
    {
        .loopbackEn = false,
        .crcType    = ENET_CRC_ETHERNET,
        .rxMtu      = 1518,
        .passPriorityTaggedUnchanged = false,
        .vlanCfg =
        {
            .portPri = 0,
            .portCfi = false,
            .portVID = 0,
        },
        .txPriorityType = ENET_EGRESS_PRI_TYPE_FIXED,
        .sgmiiMode      = ENET_MAC_SGMIIMODE_INVALID, // INVALID as SGMII is not supported in MCU+ devices

    },
    [ENET_MAC_PORT_2] =
    {
        .loopbackEn = false,
        .crcType    = ENET_CRC_ETHERNET,
        .rxMtu      = 1518,
        .passPriorityTaggedUnchanged = false,
        .vlanCfg =
        {
            .portPri = 0,
            .portCfi = false,
            .portVID = 0,
        },
        .txPriorityType = ENET_EGRESS_PRI_TYPE_FIXED,
        .sgmiiMode      = ENET_MAC_SGMIIMODE_INVALID, // INVALID as SGMII is not supported in MCU+ devices

    },
};

static void EnetApp_initAleConfig(CpswAle_Cfg *pAleCfg)
{
    *pAleCfg = enetAppCpswAleCfg;
}

static void EnetApp_initMdioConfig(Mdio_Cfg *pMdioCfg)
{
    *pMdioCfg = enetAppCpswMdioCfg;
}

static void EnetApp_initCptsConfig(CpswCpts_Cfg *pCptsCfg)
{
    *pCptsCfg = enetAppCpswCptsCfg;
}

static void EnetApp_initHostPortConfig(CpswHostPort_Cfg *pHostPortCfg)
{
    *pHostPortCfg = enetAppCpswHostPortCfg;
}

static void EnetApp_getCpswInitCfg(Enet_Type enetType,
                                   uint32_t instId,
                                   Cpsw_Cfg *cpswCfg)
{
    cpswCfg->vlanCfg.vlanAware          = false;
    cpswCfg->hostPortCfg.removeCrc      = true;
    cpswCfg->hostPortCfg.padShortPacket = true;
    cpswCfg->hostPortCfg.passCrcErrors  = true;
    EnetApp_initEnetLinkCbPrms(cpswCfg);
    EnetApp_initAleConfig(&cpswCfg->aleCfg);
    EnetApp_initMdioConfig(&cpswCfg->mdioCfg);
    EnetApp_initCptsConfig(&cpswCfg->cptsCfg);
    EnetApp_initHostPortConfig(&cpswCfg->hostPortCfg);
}

static void EnetApp_getMacPortInitConfig(CpswMacPort_Cfg *pMacPortCfg, const Enet_MacPort portIdx)
{
    EnetAppUtils_assert(portIdx < ENET_ARRAYSIZE(enetAppCpswMacPortCfg));
    *pMacPortCfg = enetAppCpswMacPortCfg[portIdx];
}



