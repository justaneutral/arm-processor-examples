/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     Platform.c
********************************************************************************
*
*     Description:  AWE Platform Interface
*
*     Copyright:    (c) 2018 DSP Concepts, Inc. All Rights Reserved.
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <kernel/dpl/CacheP.h>
#include <kernel/dpl/CycleCounterP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/HwiP.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "platform.h"
#include "TuningDriver.h"


/* ----------------------------------------------------------------------
** Global AWE Instance
** ------------------------------------------------------------------- */
AWEInstance g_AWEInstance;

/** The only input pin for this core. */
static IOPinDescriptor s_InputPin = {0};

/** The only output pin for this core. */
static IOPinDescriptor s_OutputPin = {0};

volatile BOOL g_bDeferredProcessingRequired = FALSE;

extern UINT32 AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN];
extern uint32_t UpdatePingPongState();

/* ----------------------------------------------------------------------
** Task handling
** ------------------------------------------------------------------- */
SemaphoreP_Object LayoutSem[NUM_AUDIO_THREADS];
LayoutTaskArgs    LayoutArgs[NUM_AUDIO_THREADS];

/* ----------------------------------------------------------------------
** Memory heaps
** ------------------------------------------------------------------- */
AWE_FW_SLOW_ANY_CONST UINT32 g_master_heap_size = MASTER_HEAP_SIZE;
AWE_FW_SLOW_ANY_CONST UINT32 g_slow_heap_size = SLOW_HEAP_SIZE;
AWE_FW_SLOW_ANY_CONST UINT32 g_fastb_heap_size = FASTB_HEAP_SIZE;

#define AWE_HEAP_ALIGN   64

UINT32 g_master_heap[MASTER_HEAP_SIZE] \
   __attribute__((section (".awe_heap_fast"), aligned(AWE_HEAP_ALIGN)));

UINT32 g_slow_heap[SLOW_HEAP_SIZE] \
   __attribute__((section (".awe_heap_slow"), aligned(AWE_HEAP_ALIGN)));

UINT32 g_fastb_heap[FASTB_HEAP_SIZE] \
   __attribute__((section (".awe_heap_fastb"), aligned(AWE_HEAP_ALIGN)));

#ifdef  AWE_MULTI_INSTANCE
UINT32 g_shared_heap[SHARED_HEAP_SIZE] \
   __attribute__((section (".awe_shared_heap"), aligned(AWE_HEAP_ALIGN)));
#endif

/* Buffer used for Tuning Packet Processing*/
uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN] \
   __attribute__((section(".awe_tuning_pkt")));

volatile INT32 g_nPumpCount = 100;

/* ----------------------------------------------------------------------
** Module table
** ------------------------------------------------------------------- */

/* Array of pointers to module descriptors. This is initialized at compile time.
Each item is the address of a module descriptor that we need linked in. The
linker magic is such that only those modules referenced here will be in the
final program. */
const void *g_module_descriptor_table[] =
{
	// The suitably cast pointers to the module descriptors.
   (void*)LISTOFCLASSOBJECTS

};

/* AWE Layout0 Task */
#define AWELAYOUT0_TASK_PRI  4 //(configMAX_PRIORITIES-1)
#define AWELAYOUT0_TASK_STACKSIZE (65536U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gAweLayout0TaskStack[AWELAYOUT0_TASK_STACKSIZE] \
 __attribute__((section("audio_layout0_task_stack"), aligned(32)));
StaticTask_t gAweLayout0TaskObj;
TaskHandle_t gAweLayout0Task;

/* AWE Layout1 Task */
#define AWELAYOUT1_TASK_PRI  (3)
#define AWELAYOUT1_TASK_STACKSIZE (65536U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gAweLayout1TaskStack[AWELAYOUT1_TASK_STACKSIZE] __attribute__((aligned(32)));
StaticTask_t gAweLayout1TaskObj;
TaskHandle_t gAweLayout1Task;

/* AWE Layout2 Task */
#define AWELAYOUT2_TASK_PRI  (2)
#define AWELAYOUT2_TASK_STACKSIZE (65536U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gAweLayout2TaskStack[AWELAYOUT2_TASK_STACKSIZE] __attribute__((aligned(32)));
StaticTask_t gAweLayout2TaskObj;
TaskHandle_t gAweLayout2Task;

/* AWE Idle Task */
#define AWEIDLE_TASK_PRI  (tskIDLE_PRIORITY)
#define AWEIDLE_TASK_STACKSIZE (65536U/sizeof(configSTACK_DEPTH_TYPE))
StackType_t gAweIdleTaskStack[AWEIDLE_TASK_STACKSIZE] __attribute__((aligned(32)));
StaticTask_t gAweIdleTaskObj;
TaskHandle_t gAweIdleTask;

AWE_MOD_SLOW_DM_DATA UINT32 g_module_descriptor_table_size = sizeof(g_module_descriptor_table) / sizeof(g_module_descriptor_table[0]);

int32_t awe_uart_sendreply();
extern BOOL g_bPacketReceived;

volatile uint32_t  g_audioProcessing = 0;

extern INT32 awe_fwSetHeapAlignment(AWEInstance *pAWE, UINT32 alignmentWords);

//-----------------------------------------------------------------------------
// METHOD:  awe_createInstance
// PURPOSE: Initialize AWE Instance with target details
//-----------------------------------------------------------------------------
void awe_createInstance(void)
{
   int32_t i;
   memset(&g_AWEInstance, 0, sizeof(AWEInstance));

   g_AWEInstance.numProcessingInstances = NUM_INSTANCES;
   g_AWEInstance.instanceId            = INSTANCE_ID;
   g_AWEInstance.pName                 = TARGET_NAME_AMX;
   g_AWEInstance.userVersion           = USER_VER;
   g_AWEInstance.coreSpeed             = CORE_SPEED;
   g_AWEInstance.profileSpeed          = SAMPLE_SPEED;
	g_AWEInstance.numThreads            = NUM_AUDIO_THREADS; 
	g_AWEInstance.sampleRate            = AUDIO_SAMPLE_RATE;
	g_AWEInstance.fundamentalBlockSize  = AUDIO_BLOCK_SIZE;
	g_AWEInstance.pModuleDescriptorTable = g_module_descriptor_table;
	g_AWEInstance.numModules            = g_module_descriptor_table_size;
	g_AWEInstance.fastHeapASize         = MASTER_HEAP_SIZE;
	g_AWEInstance.slowHeapSize          = SLOW_HEAP_SIZE;
	g_AWEInstance.fastHeapBSize         = FASTB_HEAP_SIZE;
	g_AWEInstance.pFastHeapA            = g_master_heap;
	g_AWEInstance.pSlowHeap             = g_slow_heap;
	g_AWEInstance.pFastHeapB            = g_fastb_heap;
	g_AWEInstance.pPacketBuffer         = AWE_Packet_Buffer;
	g_AWEInstance.packetBufferSize      = MAX_COMMAND_BUFFER_LEN;
	g_AWEInstance.pReplyBuffer          = AWE_Packet_Buffer;
   g_AWEInstance.cbAudioStop 	         = &awe_cbAudioStop;
   g_AWEInstance.cbAudioStart	         = &awe_cbAudioStart;
	
#ifdef  AWE_MULTI_INSTANCE
	g_AWEInstance.pSharedHeap           = g_shared_heap;
	g_AWEInstance.sharedHeapSize        = SHARED_HEAP_SIZE;
	g_AWEInstance.numProcessingInstances = NUM_INSTANCES;	
#endif

#ifdef BUILD_C7X
   awe_fwSetHeapAlignment(&g_AWEInstance, 16);
#endif

   if(TI_AWE_PRIMARY_CORE == g_AWEInstance.instanceId)
   {
      g_AWEInstance.pInputPin = &s_InputPin;
      g_AWEInstance.pOutputPin = &s_OutputPin;
      awe_initPin(&s_InputPin, INPUT_CHANNEL_COUNT, NULL);
      awe_initPin(&s_OutputPin, OUTPUT_CHANNEL_COUNT, NULL);
   }

   // Initialize AWE signal processing instance
	awe_init(&g_AWEInstance);

   /* Initialize Lauout Arguments */
   for (i = 0; i < NUM_AUDIO_THREADS; i++)
   {
      SemaphoreP_constructBinary(&LayoutSem[i], 0);

      LayoutArgs[i].pAWE = &g_AWEInstance;
      LayoutArgs[i].LayoutSem = &LayoutSem[i];
      LayoutArgs[i].LayoutNum = i;
      LayoutArgs[i].layoutCycleCount = 0;
   }

   // Create AWE Layout0 Processing Task - mid priority
   gAweLayout0Task = xTaskCreateStatic( AWEPump,
                        "AWELayout0Task",
                        AWELAYOUT0_TASK_STACKSIZE,
                        (void*)&LayoutArgs[0],
                        AWELAYOUT0_TASK_PRI,
                        gAweLayout0TaskStack,
                        &gAweLayout0TaskObj );
   configASSERT(gAweLayout0Task != NULL);
#ifdef SMP_FREERTOS
   //Run the IPC task only on core0 for A53-SMP
   vTaskCoreAffinitySet(gAweLayout0Task, (1 << 0));
#endif /* SMP_FREERTOS */

   // Create AWE Layout1 Processing Task - low priority
   gAweLayout1Task = xTaskCreateStatic( AWEPump,
                        "AWELayout1Task",
                        AWELAYOUT1_TASK_STACKSIZE,
                        (void*)&LayoutArgs[1],
                        AWELAYOUT1_TASK_PRI,
                        gAweLayout1TaskStack,
                        &gAweLayout1TaskObj );
   configASSERT(gAweLayout1Task != NULL);
#ifdef SMP_FREERTOS
   //Run the IPC task only on core1 for A53-SMP
   vTaskCoreAffinitySet(gAweLayout1Task, (1 << 1));
#endif /* SMP_FREERTOS */

   // Create AWE Layout1 Processing Task - lower priority
   gAweLayout2Task = xTaskCreateStatic( AWEPump,
                        "AWELayout2Task",
                        AWELAYOUT2_TASK_STACKSIZE,
                        (void*)&LayoutArgs[2],
                        AWELAYOUT2_TASK_PRI,
                        gAweLayout2TaskStack,
                        &gAweLayout2TaskObj );
   configASSERT(gAweLayout2Task != NULL);
#ifdef SMP_FREERTOS
   //Run the IPC task only on core2
   vTaskCoreAffinitySet(gAweLayout2Task, (1 << 2));
#endif /* SMP_FREERTOS */

   // Create AWE Idle Task - lowest priority
   gAweIdleTask = xTaskCreateStatic( AWEIdleTask,
                        "AWEIdle",
                        AWEIDLE_TASK_STACKSIZE,
                        NULL,
                        AWEIDLE_TASK_PRI,
                        gAweIdleTaskStack,
                        &gAweIdleTaskObj );
   configASSERT(gAweIdleTask != NULL);
#ifdef SMP_FREERTOS
   //Run the IPC task only on core0
   vTaskCoreAffinitySet(gAweIdleTask, (1 << 0));
#endif /* SMP_FREERTOS */
}

//-----------------------------------------------------------------------------
// METHOD:  AWEIdleLoop
// PURPOSE: AWE Idle loop processing
//-----------------------------------------------------------------------------
void AWEIdleTask(void* Arg1)
{
   BOOL bMoreProcessingRequired = FALSE;
   Bool running = TRUE;

   while (running)
   {
      // Reset the pump count to show that the idle loop got serviced
      g_nPumpCount = 0;

      awe_process_tuner_command(&g_AWEInstance);

      // Process any local controls
      if ( awe_audioIsStarted(&g_AWEInstance) )
      {
         if (g_bDeferredProcessingRequired || bMoreProcessingRequired)
         {
            g_bDeferredProcessingRequired = FALSE;
            bMoreProcessingRequired = awe_deferredSetCall(&g_AWEInstance);
         }
      }
      
      taskYIELD();
   }
}
//----------------------------------------------------------------------------
// METHOD:  aweuser_getCycleCount
// PURPOSE: User defined function to return the cycle count to the AWECore
//          library to enable profiling. Must return a 32-bit integer.
//          The function can be inlined to improve performance if the target supports it
//----------------------------------------------------------------------------
UINT32 aweuser_getCycleCount(void)
{
   UINT64 cycles;

#ifdef BUILD_C7X
   cycles = CycleCounterP_getCount32();
#else
   cycles = CycleCounterP_getCount64();
#endif

   return (UINT32)(cycles/PROFILER_DIVIDER);
}

//----------------------------------------------------------------------------
// METHOD:  aweuser_getElapsedCycles
// PURPOSE: User defined function to return the elapsed cycles
//----------------------------------------------------------------------------
UINT32 aweuser_getElapsedCycles(UINT32 start_time, UINT32 end_time)
{
   return (end_time - start_time)& 0x03FFFFFF ;
}

//----------------------------------------------------------------------------
// METHOD:  awe_pltInvalidateCache
// PURPOSE: Invalidate cache region (used for FFS)
//----------------------------------------------------------------------------
void awe_pltInvalidateCache(const void* pStartAddr, const uint32_t length)
{
    CacheP_inv((char *)pStartAddr, length, CacheP_TYPE_ALL);
}

//----------------------------------------------------------------------------
// METHOD:  __disable_irq
// PURPOSE:  General function to disable interrupts, used by AWE libraries
//----------------------------------------------------------------------------
uintptr_t oldState = 0;

void __disable_irq()
{
   oldState = HwiP_disable();
}

//----------------------------------------------------------------------------
// METHOD:  __enable_irq
// PURPOSE:  General function to enable interrupts, used by AWE libraries
//----------------------------------------------------------------------------
void __enable_irq()
{
   HwiP_restore(oldState);
}

extern void audio_start();
extern void audio_stop();

uint32_t g_sendLayoutTime = 0;

INT32 awe_cbAudioStart(struct _AWEInstance *PAWE)
{
   if(g_audioProcessing == 0)
   {
      g_audioProcessing = 1;
      g_sendLayoutTime = 1;
      //audio_start();
   }
   return 1;
}

INT32 awe_cbAudioStop(struct _AWEInstance *PAWE)
{
   if(g_audioProcessing == 1)
   {
      g_audioProcessing = 0;
      //audio_stop();
   }
   
   return 1;
}
