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

#ifndef __ENET_TSN_H__
#define __ENET_TSN_H__

#include <tsn_combase/tilld/lldtype.h>

#define TSN_TSK_STACK_SIZE                         (8U * 1024U)
#define TSN_TSK_STACK_ALIGN                        (32U)

#ifndef TSNAPP_LOGLEVEL
#define TSNAPP_LOGLEVEL "4,ubase:45,cbase:45,uconf:45,gptp:45,lldp:45,avtp:45,nconf:45,xmrpd:45"
#endif

typedef void (*Logger_onConsoleOut)(const char *str, ...);
extern Logger_onConsoleOut sDrvConsoleOut;

/* Reason: Printing to the console directly will create a lot of timing issue in gptp.
 * Solution: Writing to a buffer, a log task will print it out a bit later. */
typedef struct AppTsnCfg 
{
    Logger_onConsoleOut consoleOutCb; //<! A callback function for log output on console.
    char *netdevs[LLDENET_MAX_PORTS+1]; //!< A list of network interfaces each is a string, terminated by NULL;
} AppTsnCfg_t;

/* start and stop a single TSN module, @ref EnetApp_TsnTask_Idx_t for the moduleIdx */
int EnetApp_startTsnModule(int moduleIdx);

void EnetApp_stopTsnModule(int moduleIdx);

int EnetApp_initTsnByCfg(AppTsnCfg_t *cfg);

/* start and stop all the TSN modules */
int EnetApp_startTsn(void);
void EnetApp_stopTsn(void);
void EnetApp_deInitTsn(void);

#endif // __ENET_TSN_H__
