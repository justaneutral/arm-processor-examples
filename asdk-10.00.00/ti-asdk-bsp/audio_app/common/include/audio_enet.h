/*
 * Copyright (c) 2001,2002 Florian Schulze.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the authors nor the names of the contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * app_cpswconfighandler.h - This file is part of lwIP test
 *
 */
#ifndef __AUDIO_ENET_H_
#define __AUDIO_ENET_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#include <kernel/dpl/TaskP.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/QueueP.h>

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"

#include <networking/enet/utils/include/enet_apputils.h>
#include <networking/enet/utils/include/enet_board.h>
#include <enet_apputils.h>
#include <enet_board.h>

#include "ti_board_config.h"
#include "ti_board_open_close.h"
#include "ti_drivers_open_close.h"
#include "ti_enet_config.h"
#include "ti_enet_open_close.h"
#include "ti_enet_lwipif.h"

#define DEFAULT_TUNING_PORT             (15002)

typedef struct AWE_EnetInfo
{
    Enet_Type           enetType;       // Peripheral type
    uint32_t            instId;         // Peripheral instance
    char*               name;           // instance name for debugging
    uint32_t            coreId;         // Peripheral instance
    uint32_t            coreKey;        // Core ket, returned by RM
    uint8_t             numMacPort;     // MAC ports useds
    Enet_MacPort        macPortList[ENET_SYSCFG_MAX_MAC_PORTS]; //Num MAC ports to be use

#if 0

    /* Enet driver handle for this peripheral type/instance */
    Enet_Handle hEnet;

    /* MAC address. It's port's MAC address in Dual-MAC or
     * host port's MAC addres in Switch */
    uint8_t macAddr[ENET_MAC_ADDR_LEN];

    /* TX channel number */
    uint32_t txChNum;

    /* TX channel handle */
    EnetDma_TxChHandle hTxCh;

    /* Queue of free TX packets */
    EnetDma_PktQ txFreePktInfoQ;

    /* Start flow index */
    uint32_t rxStartFlowIdx;

    /* Regular traffic RX flow index */
    uint32_t rxFlowIdx;

    /* RX channel handle for regular traffic */
    EnetDma_RxChHandle hRxCh;

    /* Number of Ethernet ports to be enabled */
    uint8_t numMacPorts;

    /* RX task handle - receives Regular packets, changes source/dest MAC addresses
     * and transmits the packets back */
    TaskP_Object rxTaskObj;

    /* AVB talker task */
    TaskP_Object talkerTaskObj;

    /* AVB listener task */
    TaskP_Object listenerTaskObj;

    /*! Shared mem object memory */
    ShdMemCircularBufferP_Rsv shmQObjMem;

    /*! Shared mem handle */
    ShdMemCircularBufferP_Handle hShmListenerQ;

    /* Semaphore posted from IPC Notify to start the listener */
    SemaphoreP_Object startListnerSemObj;

    /* Semaphore posted from RX callback when Regular packets have arrived */
    SemaphoreP_Object rxSemObj;

    /* Semaphore used to synchronize all Regular RX tasks exits */
    SemaphoreP_Object rxDoneSemObj;

    /* Peripheral's MAC ports to use */
    Enet_MacPort macPorts[MAX_NUM_MAC_PORTS];

#endif


    
} AWE_EnetInfo;

void awe_addMCastEntry(Enet_Type enetType,
                          uint32_t instId,
                          uint32_t coreId,
                          const uint8_t *testMCastAddr,
                          uint32_t portMask);

void awe_setupNetworkStack();
int32_t awe_isNetworkUp(struct netif* netif_);
void awe_netifLinkChangeCb(struct netif *pNetif);
void awe_netifStatusChangeCb(struct netif *pNetif);
void awe_tcpipInitCompleteCb(void *pArg);

void awe_enet_setup();

void awe_serverTask(void *arg);

void awe_createEnetTask();


#endif /* __AUDIO_ENET_H_ */


