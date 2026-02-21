/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <string.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include <kernel/dpl/ClockP.h>
#include "enet_apputils.h"

#include "enet_setup.h"
#include <platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define APP_SERVER_PORT    (15002)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

extern uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN];
extern uint32_t read_byte_cnt;
extern int32_t awe_r5f_ipcSend();
extern int32_t awe_r5f_ipcRecv();

#define ENET_TASK_PRI  (configMAX_PRIORITIES-1)
#define ENET_TASK_SIZE (16384U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gEnetTaskStack[ENET_TASK_SIZE] __attribute__((aligned(32)));
StaticTask_t gEnetTaskObj;
TaskHandle_t gEnetTask;

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

static void asdk_ServerTask(void *arg)
{
    struct netconn *pConn = NULL, *pClientConn = NULL;
    err_t err;

    pConn = netconn_new(NETCONN_TCP);
    netconn_bind(pConn, IP_ADDR_ANY, APP_SERVER_PORT);
    if(pConn == NULL)
    {
        DebugP_log("asdk_ServerTask: invalid conn\r\n");
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

void asdk_startServer()
{
    // create task to handle enet connection
    gEnetTask = xTaskCreateStatic( asdk_ServerTask,
                                  "asdk_ServerTask",
                                  ENET_TASK_SIZE,
                                  NULL,
                                  ENET_TASK_PRI,
                                  gEnetTaskStack,
                                  &gEnetTaskObj );
    configASSERT(gEnetTask != NULL);
}
/*-----------------------------------------------------------------------------------*/
