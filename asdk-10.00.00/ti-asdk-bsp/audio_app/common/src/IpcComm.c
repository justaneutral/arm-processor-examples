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

#include "FreeRTOS.h"
#include "task.h"

#include "platform.h"
#include "IpcComm.h"

extern IpcComm_obj ipcObj;

SemaphoreP_Object semGatewayReady;

/* main core that starts the message exchange */
uint32_t gMainCoreId = CSL_CORE_ID_MCU_R5FSS0_0;

void awe_sendCoreAliveSignal(uint16_t remoteCoreId, uint32_t msg)
{
#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    IpcNotify_sendMsg(remoteCoreId, REMOTE_ALIVE_NOTIFY, msg, 1);
#endif
}

void awe_sendLayoutProcessingTime(uint32_t cpuCycleCnt)
{
    IpcNotify_sendMsg(gMainCoreId, LAYOUT_PROCESSING_TIME, cpuCycleCnt, 1);
}

void awe_coreAliveHandler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
    gMainCoreId = remoteCoreId;

    // this is for the case, C7x was started earlier
    awe_sendCoreAliveSignal(gMainCoreId, 0xFF);

    SemaphoreP_post(&semGatewayReady);
}

void awe_remoteDataReadyHandler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
    // Notify audio processing task to pump the data
    SemaphoreP_post(&semAudioDataReady);
}

void awe_remotePacketReadyHandler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
	// seconary remote received the tuning packet
    ipcObj.packetReceived = TRUE;
}

void awe_tuningPacketReadyHandler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
	// primary core received the tuning packet
	ipcObj.packetReceived = TRUE;
	ipcObj.recvMsgCnt++;
}

#ifdef ENABLE_AVB_AUDIO
void ti_avb_data_handler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{

}

void ti_avb_control_handler(uint16_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{

}
#endif

int32_t awe_signalRemoteCore(uint32_t instanceId, int32_t signalId)
{
    uint32_t msg = 0;
    uint32_t remoteCoreId = CSL_CORE_ID_A53SS0_0;
    int32_t  status = SystemP_SUCCESS;

    if(instanceId == TI_AWE_SECONDARY_CORE0)
    {
        remoteCoreId = CSL_CORE_ID_A53SS0_0;
    }

    //DebugP_log("awe_signalRemoteCore: to core %d\r\n", remoteCoreId);

    IpcNotify_sendMsg(remoteCoreId, signalId, msg, 1);

    return status;
}

int32_t awe_ipc_sendreply(IpcComm_obj* ipcObj)
{
	int32_t  status = SystemP_SUCCESS;
	uint32_t msg = 0;
	IpcNotify_sendMsg(gMainCoreId, AWE_TUNING_PCKT_READY, msg, 1);
    ipcObj->sendMsgCnt++;
    
    //DebugP_log("awe_ipcSend: to core %d, size %d\r\n",
    //    ipcObj->dstCoreId, msgSize);
	
    return status;
}


int32_t awe_ipc_setup(IpcComm_obj* ipcObj)
{
    int32_t status = SystemP_SUCCESS;

#if (INSTANCE_ID == TI_AWE_PRIMARY_CORE)
    ipcObj->recvMsgCnt = 0;
    ipcObj->sendMsgCnt = 0;

    // create semephore object for tuning gateway
    SemaphoreP_constructBinary(&semGatewayReady, 0);
    
    DebugP_log("Registering for ipc_notify ..\r\n");

    // register for initial handshake notification, core alive
    status = IpcNotify_registerClient(REMOTE_ALIVE_NOTIFY, awe_coreAliveHandler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);
    
    // register for tuning packet from gateway core
    status = IpcNotify_registerClient(AWE_TUNING_PCKT_READY, awe_tuningPacketReadyHandler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

#ifdef ENABLE_AVB_AUDIO
    // register for AVB audio data ready
    status = IpcNotify_registerClient(TI_AVB_DATA_READY, ti_avb_data_handler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

    // register for AVB control ready
    status = IpcNotify_registerClient(TI_AVB_CONTROL_CMD, ti_avb_control_handler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);
#endif

    //IpcNotify_syncAll(SystemP_WAIT_FOREVER);

    awe_sendCoreAliveSignal(gMainCoreId, 0xFE);

    DebugP_log("Waiting for MCU/R5F...\r\n");
    SemaphoreP_pend(&semGatewayReady, SystemP_WAIT_FOREVER);
    
    DebugP_log("Primary core tuning server ready...\r\nVersion: %02d.%02d.%02d.%02d\r\n",
        USER_VER/1000000,
        (USER_VER/10000)%100,
        (USER_VER/100)%10000,
        USER_VER%1000000);
    
#else
    // secondary core, register for remote data ready
    status = IpcNotify_registerClient(REMOTE_AWE_DATA_NOTIFY, awe_remoteDataReadyHandler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);
    
    // secondary core, register remote tuning packet
    status = IpcNotify_registerClient(REMOTE_AWE_PACKET_READY, awe_remotePacketReadyHandler, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

    DebugP_log("Secondary core ready...\r\n");
#endif
    return status;
}

