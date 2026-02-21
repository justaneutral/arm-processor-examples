/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     TargetInfo.h
********************************************************************************
*
*     Description:  All target specific information
*
*     Copyright:    (c) 2016-2021 DSP Concepts Inc., All rights reserved
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/
#ifndef _TARGETINFO_H
#define _TARGETINFO_H

// Version Information
#define USER_VER                        (UINT32) 10000003
#define TI_AWE_PRIMARY_CORE	            0
#define TI_AWE_SECONDARY_CORE0	        1
#define TI_AWE_SECONDARY_CORE1	        2

// Number of Instances
#ifdef AWE_MULTI_INSTANCE
#define NUM_INSTANCES                   2
#else  /* AWE_MULTI_INSTANCE */
#define NUM_INSTANCES                   1
#endif /* AWE_MULTI_INSTANCE */

//Instance ID. 
// For multi-instance, C7x -> Primary, A53x secondary
#ifdef BUILD_C7X
#define INSTANCE_ID                     TI_AWE_PRIMARY_CORE
#else /* BUILD_C7X */
#ifdef AWE_MULTI_INSTANCE
#define INSTANCE_ID                     TI_AWE_SECONDARY_CORE0
#else /* AWE_MULTI_INSTANCE */
#define INSTANCE_ID                     TI_AWE_PRIMARY_CORE
#endif //AWE_MULTI_INSTANCE
#endif /* BUILD_C7X */

//--------------------------------------------
// AM62Ax EVM
//--------------------------------------------
#ifdef SOC_AM62AX
#define C7X_CORE_SPEED                  850e6f
#define A53_CORE_SPEED                  1000e6f
#ifdef BUILD_C7X
#define CORE_SPEED                      C7X_CORE_SPEED
#define TARGET_NAME_AMX                 "AM6x_C7"
#else /* BUILD_C7X */
#define CORE_SPEED                      A53_CORE_SPEED
#define TARGET_NAME_AMX                 "AM6x_A53"
#endif /* BUILD_C7X */
#endif /* SOC_AM62AX */
//--------------------------------------------

//--------------------------------------------
// AM62Dx EVM
//--------------------------------------------
#ifdef SOC_AM62DX
#define C7X_CORE_SPEED                  1000e6f
#define A53_CORE_SPEED                  1400e6f
#ifdef BUILD_C7X
#define CORE_SPEED                      C7X_CORE_SPEED
#define TARGET_NAME_AMX                 "AM6x_C7"
#else  /* BUILD_C7X */
#define CORE_SPEED                      A53_CORE_SPEED
#define TARGET_NAME_AMX                 "AM6x_A53"
#endif /* BUILD_C7X */
#endif /* SOC_AM62DX */

//--------------------------------------------
// AM275 EVM
//--------------------------------------------
#ifdef SOC_AM275
#define C7X1_CORE_SPEED                 1000e6f
#define C7X2_CORE_SPEED                 1000e6f
#ifdef BUILD_C7X1
#define CORE_SPEED                      C7X1_CORE_SPEED
#define TARGET_NAME_AMX                 "AM275C71"
#else  /* BUILD_C7X1 */
#define CORE_SPEED                      C7X2_CORE_SPEED
#define TARGET_NAME_AMX                 "AM275C72"
#endif /* BUILD_C7X1 */
#endif /* SOC_AM275 */
//--------------------------------------------

#define PROFILER_DIVIDER                64
#define SAMPLE_SPEED                    (CORE_SPEED/PROFILER_DIVIDER)
#define HAS_FLOAT_SUPPORT               1

#define NUM_AUDIO_THREADS               3
#define SAMPLE_SIZE_IN_BYTES            4
#define AUDIO_SAMPLE_RATE               48000

#define AWE_LOW_LATENCY_FRAME_SIZE      16
#define AWE_NORMAL_LATENCY_FRAME_SIZE   64

#ifdef LOWLATENCYCORE
#define AUDIO_BLOCK_SIZE                AWE_LOW_LATENCY_FRAME_SIZE
#else /* LOWLATENCYCORE */
#define AUDIO_BLOCK_SIZE                AWE_NORMAL_LATENCY_FRAME_SIZE
#endif /* LOWLATENCYCORE */

/* ----------------------------------------------------------------------
** Specify the size of each of the heaps on this target
** ------------------------------------------------------------------- */
#define MASTER_HEAP_SIZE                (197*1024)
#define SLOW_HEAP_SIZE                  (1024*1024)
#define FASTB_HEAP_SIZE                 (640*1024)

#ifdef  AWE_MULTI_INSTANCE
#define SHARED_HEAP_SIZE	            (SHM_C7A53_SHARED_HEAP_SIZE/4)
#endif /* AWE_MULTI_INSTANCE */

/* ----------------------------------------------------------------------
** Specify communication buffer size
** ------------------------------------------------------------------- */
#define MAX_COMMAND_BUFFER_LEN          264

/* ----------------------------------------------------------------------
** Specify which modules to include on this target
** ------------------------------------------------------------------- */
#include "ModuleList.h"
//#include "ModuleList_AutoPlayback.h"

#endif  // _TARGETINFO_H
