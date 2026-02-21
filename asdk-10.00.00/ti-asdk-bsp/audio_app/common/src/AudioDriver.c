/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*    AudioDriver.c
********************************************************************************
*
*     Description:  AudioWeaver Driver
*
*     Copyright:    (c) 2016-2021 DSP Concepts Inc., All rights reserved
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/
#include <kernel/dpl/SemaphoreP.h>
#include "platform.h"
#include "targetInfo.h"
#include "IpcComm.h"

#define MAX_PUMP_COUNT 100

extern SemaphoreP_Object LayoutSem[NUM_AUDIO_THREADS];
extern uint32_t g_sendLayoutTime;
uint32_t layoutCycleCnt;

//-----------------------------------------------------------------------------
// METHOD:  awe_process_audioData
// PURPOSE: Pass new samples to Audio Weaver and return processed samples
//-----------------------------------------------------------------------------
void awe_transferAudioBufs(AWEInstance *pAWE, Awe_Data_Buf* pAweBuf)
{
   if (pAWE->instanceId == TI_AWE_PRIMARY_CORE)
   {
      uint32_t i;
      // Insert the received HW input samples into the AudioWeaver buffer
      for(i = 0; i < ADC_CHANNEL_COUNT; i++)
      {
         awe_audioImportSamples(pAWE, &(pAweBuf->pDmaRxBuf[i]), ADC_CHANNEL_STRIDE, i, Sample32bit);
      }

      for(i = 0; i < DAC_CHANNEL_COUNT; i++)
      {
         awe_audioExportSamples(pAWE, &(pAweBuf->pDmaTxBuf[i]), DAC_CHANNEL_STRIDE, i, Sample32bit);
      }

#ifdef ENABLE_AVB_AUDIO
      for(i = 0; i < AVB_RX_CHANNEL_COUNT; i++)
      {
         awe_audioImportSamples(pAWE, &(pAweBuf->pIpcRxBuf[i]), AVB_RX_CHANNEL_STRIDE, ADC_CHANNEL_COUNT+i, Sample16bit);
      }
      
      for(i = 0; i < AVB_TX_CHANNEL_COUNT; i++)
      {
         awe_audioExportSamples(pAWE, &(pAweBuf->pIpcTxBuf[i]), AVB_TX_CHANNEL_STRIDE, DAC_CHANNEL_COUNT+i, Sample16bit);
      }
#endif   
	}

#ifdef AWE_MULTI_INSTANCE
   if (pAWE->numProcessingInstances > 1 && pAWE->instanceId == TI_AWE_PRIMARY_CORE)
   {
      if (awe_audioIsReadyToPumpMulti(pAWE, TI_AWE_SECONDARY_CORE0))
      {
         awe_signalRemoteCore(TI_AWE_SECONDARY_CORE0, REMOTE_AWE_DATA_NOTIFY);
      }
   }
#endif
}


void awe_executeAudioPump(AWEInstance *pAWE)
{
   int32_t layoutMask = 0;
   int32_t i;

   layoutMask = awe_audioGetPumpMask(pAWE);

   for (i = 0; i < NUM_AUDIO_THREADS; i++)
   {
      if (layoutMask & (1 << i))
      {
         SemaphoreP_post(&LayoutSem[i]);
      }
   }
}

//-----------------------------------------------------------------------------
// METHOD:  AudioWeaver Pump Interrupt Handler
// PURPOSE: Perform AudioWeaver Processing
//-----------------------------------------------------------------------------
void AWEPump(void* Arg1)
{
   LayoutTaskArgs *Args = (LayoutTaskArgs*)Arg1;

   while (1)
   {
      SemaphoreP_pend( Args->LayoutSem, SystemP_WAIT_FOREVER);

      if ( awe_audioIsStarted(Args->pAWE) )
      {
         // If we are not servicing the idle task, skip this frame to free up processing.
         if (g_nPumpCount++ < MAX_PUMP_COUNT)
         {
            g_bDeferredProcessingRequired |= awe_audioPump( Args->pAWE, Args->LayoutNum );
         }
      }

      //debugGpio_low(awecore_debug_pin);
      awe_getAverageLayoutCycles(Args->pAWE, Args->LayoutNum, &(Args->layoutCycleCount));

#if 0
      /* Send the processing time for main layout (layout0)
       * This will be sent to mcu/r5f everytime new layout is
       * loaded by designer.
       */
      if(g_sendLayoutTime && Args->LayoutNum == 0)
      {
         g_sendLayoutTime = 0;
         layoutCycleCnt = ((Args->layoutCycleCount >> 8) * 1000000U) / SAMPLE_SPEED;
         awe_sendLayoutProcessingTime(layoutCycleCnt);
         DebugP_log("Layout processing time : %d us\r\n", layoutCycleCnt);
      }
#endif

   }
}

