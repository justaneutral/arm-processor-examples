/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== Server.c ========
 *
 */

/* this define must precede inclusion of any xdc header file */
//#define Registry_CURDESC Test__Desc

#include <string.h>
#include <stdio.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/CacheP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <drivers/ipc_notify.h>

#include "TuningDriver.h"

/* Global Object for tuning service */
IpcComm_obj ipcObj;

int32_t TuningServerCreate ()
{
   awe_ipc_setup(&ipcObj);
   return SystemP_SUCCESS;
}
int32_t TuningServerDelete ()
{
   return SystemP_SUCCESS;
}

void awe_process_tuner_command(AWEInstance *pAWE)
{
   //debugGpio_low(awecore_debug_pin);
   if (TRUE == ipcObj.packetReceived)
   {
#ifdef AWE_MULTI_INSTANCE
      if(pAWE->numProcessingInstances > 1 && 
         pAWE->instanceId == TI_AWE_PRIMARY_CORE)
      {
         BOOL done = FALSE;
         while (!done)
         {
            if (E_MULTI_PACKET_WAITING != awe_packetProcessMulti(pAWE, TRUE))
            {
               done = TRUE;
               awe_ipc_sendreply(&ipcObj);
            }
            else
            {
               //Send notifi to remote core
               awe_signalRemoteCore(TI_AWE_SECONDARY_CORE0, REMOTE_AWE_PACKET_READY);
            }
         }
      }
      else
      {
         awe_packetProcessMulti(pAWE, FALSE);
      }
#else
      awe_packetProcess(pAWE);
      awe_ipc_sendreply(&ipcObj);
#endif
      
      ipcObj.packetReceived = FALSE;
   }
}
