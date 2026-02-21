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
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <kernel/dpl/DebugP.h>

#include <enet_apputils.h>

#include "FreeRTOS.h"
#include "task.h"

#include "platform.h"
#include "IpcComm.h"

#ifdef ENABLE_AVB_AUDIO

uint8_t g_avb_rx_buf[IPC_RX_BUFF_CNT][IPC_RX_FRAME_SIZE] \
   __attribute__((section (".ti_avb_rx_data"), aligned(128)));

uint8_t g_avb_tx_buf[IPC_TX_BUFF_CNT][IPC_TX_FRAME_SIZE] \
   __attribute__((section (".ti_avb_tx_data"), aligned(128)));

uint8_t g_avbcntrl_cmd_buf[SHM_AVB_CNTL_RX_SIZE] \
   __attribute__((section (".ti_avb_ctrl_cmd"), aligned(128)));

uint8_t g_avbcntrl_res_buf[SHM_AVB_CNTL_TX_SIZE] \
   __attribute__((section (".ti_avb_ctrl_res"), aligned(128)));

extern uint32_t gMainCoreId;

uint32_t g_avbRxIndex = 0;
uint32_t g_avbTxIndex = 0;

EnetPer_AttachCoreOutArgs attachCoreOutArgs;
EnetApp_HandleInfo handleInfo;

//EnetApp_Cfg gEnetAppCfg =
//{
//    .name = "ti_avb_am62x",
//};

void ti_avb_control_handler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{

}

void ti_avb_data_handler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{

}

void ti_avb_send_response(uint32_t cmd, uint32_t streamId, uint32_t strmStatus)
{
    uint32_t msg = cmd; 
    IpcNotify_sendMsg(gMainCoreId, TI_AVB_CONTROL_RESP, msg, 1);
}

int16_t* ti_avb_getAvbRxDataPtr()
{
    int16_t* ptr = (int16_t *)&(g_avb_rx_buf[g_avbRxIndex][0]);
    g_avbRxIndex = (g_avbRxIndex+1) % IPC_RX_BUFF_CNT;
    return ptr;
}

int16_t* ti_avb_getAvbTxDataPtr()
{
    int16_t* ptr = (int16_t *)&(g_avb_tx_buf[g_avbTxIndex][0]);
    g_avbRxIndex = (g_avbTxIndex+1) % IPC_TX_BUFF_CNT;
    return ptr;
}

#if 0
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
        DebugP_log("%s: Failed to set TS SYNC OUT BIT : %d\r\n", gEnetAppCfg.name, status);
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
    
    EnetApp_driverInit();
    EnetApp_driverOpen(gEnetAppCfg.enetType, gEnetAppCfg.instId);
    EnetApp_acquireHandleInfo(gEnetAppCfg.enetType, gEnetAppCfg.instId, handleInfo);
    gEnetAppCfg.hEnet = handleInfo->hEnet;
    EnetApp_coreAttach(gEnetAppCfg.enetType, gEnetAppCfg.instId, gEnetAppCfg.coreId, attachArgs);
    gEnetAppCfg.coreKey = attachArgs->coreKey;

    EnetApp_enableTsSync();
}

void ti_avb_init(void* arg)
{
   EnetApp_initAppCfg(&attachCoreOutArgs, &handleInfo);

   EnetApp_createRxTask();

}

#endif

#endif /* ENABLE_AVB_AUDIO */
