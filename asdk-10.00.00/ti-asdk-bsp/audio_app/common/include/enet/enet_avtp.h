/*
 *  Copyright (C) 2018-2024 Texas Instruments Incorporated
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

#ifndef __ENET_SET_H__
#define __ENET_SET_H__

#include "sharedmem_audio.h"
#include <tsn_combase/tilld/lldtype.h>
#include <enet_apputils.h>

#include <tsn_combase/cb_rate_reporter.h>

#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/DebugP.h>
#include <core/enet_types.h>
#include <core/enet_mod_tas.h>
#include <core/enet_types.h>
#include <enet_apputils.h>
#include <enet_ethutils.h>
#include <tsn_combase/tilld/lldtype.h>

#if defined(SAFERTOS)
#define TSN_TSK_STACK_SIZE                         (16U * 1024U)
#define TSN_TSK_STACK_ALIGN                        TSN_TSK_STACK_SIZE
#else
#if _DEBUG_ == 1
/* 16k is enough for debug mode */
#define TSN_TSK_STACK_SIZE                         (16U * 1024U)
#else
#define TSN_TSK_STACK_SIZE                         (8U * 1024U)
#endif
#define TSN_TSK_STACK_ALIGN                        (32U)
#endif

#define MAX_NUM_MAC_PORTS           (3U)
#define ENETAPP_DEFAULT_CFG_NAME    "sitara-cpsw"
#define EnetAppAbort(message)       \
    EnetAppUtils_print(message);    \
    EnetAppUtils_assert(false);

#ifndef DISABLE_FAT_FS

#include <ff_stdio.h>

#define INTERFACE_CONFFILE_PATH "/sd0/conffiles/interface.conf"
#define UNICONF_DBFILE_PATH     "/sd0/uniconfdb/example.bin"

typedef FF_Stat_t       EnetApp_fsInfo_t;
#define FSTAT(fn, st)   ff_stat((fn), (st))
#define FSSTAT_OK       (0)
#define NETCONF_YANG_SCHEMA_DIR "/sd0/schemas/xmlsafe"

#endif //DISABLE_FAT_FS

#define EnetApp_sleep           ClockP_usleep
#define EnetApp_yield           TaskP_yield
#define ENDLINE "\r\n"
#define USE_CRLF
#ifndef BTRUE
#define BTRUE    ((bool) 1)
#endif

#ifndef BFALSE
#define BFALSE    ((bool) 0)
#endif

static inline char EnetTsnApp_getChar(void)
{
    char ch;

    DebugP_scanf("%c", &ch);

    return ch;
}

static inline int32_t EnetTsnApp_getNum(void)
{
    int32_t num;

    DebugP_scanf("%d", &num);

    return num;
}

#define MAX_KEY_SIZE            (256)
#define TSNAPP_UNUSED_ARG(x)    (void)x

#ifdef DISABLE_FAT_FS
#define UNICONF_CONF_FILE_NUM   (0)
#define INTERFACE_CONFFILE_PATH (NULL)
#define UNICONF_DBFILE_PATH     (NULL)
#else
#define UNICONF_CONF_FILE_NUM   (1)
#endif //DISABLE_FAT_FS

#ifndef AVTP_TALKER_NUM
#define AVTP_TALKER_NUM         (2)
#endif

#ifndef AVTP_LISTENER_NUM
#define AVTP_LISTENER_NUM       (1)
#endif

typedef enum {
    ENETAPP_UNICONF_TASK_IDX,
    ENETAPP_NETCONF_TASK_IDX,
    ENETAPP_GPTP_TASK_IDX,
    ENETAPP_LLDP_TASK_IDX,
    ENETAPP_AVTPD_TASK_IDX,
    ENETAPP_CRF_TALKER_TASK_IDX,
    ENETAPP_CRF_LISTENER_TASK_IDX,
    ENETAPP_ACF_TASK_IDX,
    ENETAPP_TALKER_TASK_IDX,
    ENETAPP_LISTENER_TASK_IDX = ENETAPP_TALKER_TASK_IDX + AVTP_TALKER_NUM,
    ENETAPP_EST_TASK_IDX = ENETAPP_LISTENER_TASK_IDX + AVTP_LISTENER_NUM,
    ENETAPP_CBS_TASK_IDX,
    ENETAPP_XMRPD_TASK_IDX,
    ENETAPP_XMRPC_TASK_IDX,
    ENETAPP_AAF_DOLBY_EC3_TASK_IDX,
    ENETAPP_AAF_PCM_TASK_IDX,
    ENETAPP_MAX_TASK_IDX
} EnetApp_TsnTask_Idx_t;

typedef struct {
    uc_dbald *dbald;
    yang_db_runtime_dataq_t *ydrd;
    uc_notice_data_t *ucntd;
} EnetApp_dbArgs;

typedef struct EnetApp_ModuleCtx EnetApp_ModuleCtx_t;
typedef int (*EnetApp_OnModuleDBInit)(EnetApp_ModuleCtx_t* mdctx,
                                      EnetApp_dbArgs *dbargs);
typedef void* (*EnetApp_OnModuleStart)(void *arg);

typedef void (*Logger_onConsoleOut)(const char *str, ...);

extern Logger_onConsoleOut sDrvConsoleOut;

#define DPRINT(str,...) sDrvConsoleOut(str ENDLINE, ##__VA_ARGS__)

int Logger_logToBuffer(bool flush, const char *str);
int Logger_directLog(bool flush, const char *str);
#if TSN_USE_LOG_BUFFER == 1
#define LOG_OUTPUT Logger_logToBuffer
#else
#define LOG_OUTPUT Logger_directLog
#endif

int Logger_init(Logger_onConsoleOut consoleOutCb);
void Logger_deInit(void);

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
typedef struct
{
    char *dbName; // Specify same DB for all modules.
    ucman_data_t ucCtx;
    char netdev[MAX_NUMBER_ENET_DEVS][CB_MAX_NETDEVNAME];
    uint8_t netdevSize;
    UC_NOTICE_SIG_T ucReadySem;
    bool dbInitFlag;
} EnetApp_Ctx_t;

struct EnetApp_ModuleCtx
{
    bool stopFlag;
    int taskPriority;
    CB_THREAD_T hTaskHandle;
    const char *taskName;
    uint8_t *stackBuffer;
    uint32_t stackSize;
    EnetApp_OnModuleDBInit onModuleDBInit;
    EnetApp_OnModuleStart onModuleRunner;
    EnetApp_Ctx_t *appCtx;
    bool enable;
};

typedef struct
{
    char *name;
    char *val;
} EnetApp_DbNameVal_t;

typedef struct
{
    char *name;
    int item;
    int val;
} EnetApp_DbIntVal_t;


/* Test parameters for each port in the multi-channel test */
typedef struct EnetApp_Cfg_s
{
    /* Peripheral type */
    Enet_Type enetType;

    /* Peripheral instance */
    uint32_t instId;

    char *name;

    /* This core's id */
    uint32_t coreId;

    /* Core key returned by Enet RM after attaching this core */
    uint32_t coreKey;

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
    
    /* AVB listener task */
    TaskP_Object listenerTaskObj;

    /* AVB talker task */
    TaskP_Object talkerTaskObj;

    /*! Shared mem handle */
    shm_handle hShmListenerQ;

    /*! Shared mem handle */
    shm_handle hShmTalkerQ;

    /* Semaphore posted from IPC Notify to start the listener */
    SemaphoreP_Object startListnerSemObj;

    /* Semaphore posted from IPC Notify to start the talker */
    SemaphoreP_Object startTalkerSemObj;

    /* Semaphore posted from RX callback when Regular packets have arrived */
    SemaphoreP_Object rxSemObj;

    /* Semaphore used to synchronize all Regular RX tasks exits */
    SemaphoreP_Object rxDoneSemObj;

    /* Peripheral's MAC ports to use */
    Enet_MacPort macPorts[MAX_NUM_MAC_PORTS];
} EnetApp_Cfg;


void enetApp_setup();
void EnetApp_addBroadcastEntry(void);
void EnetApp_setMacAddr(uint8_t hwaddr[]);

#endif /* __ENET_SET_H__ */

