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
* -----------------------------------------------------------------------------
*
*     Copyright:    DSP Concepts, Inc., 2007 - 2020
*                   3235 Kifer Road, Suite 100
*                   Santa Clara, CA 95051
*
*******************************************************************************/


#ifndef _TARGETINFO_H
#define _TARGETINFO_H

#ifdef	__cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------
** Specify the sizes of each of the heaps on the target, in 32-bit words
** ------------------------------------------------------------------- */
#define FASTA_HEAP_SIZE		    2000000
#define FASTB_HEAP_SIZE			2000000
#define SLOW_HEAP_SIZE			2000000
#define SHARED_HEAP_SIZE		2000000

/*Larger heaps for the linux examples*/
#define FASTA_HEAP_SIZE_LINUX   20000000
#define FASTB_HEAP_SIZE_LINUX	20000000
#define SLOW_HEAP_SIZE_LINUX	20000000
#define SHARED_HEAP_SIZE_LINUX	20000000

/* Define the rest of the target for the instance initialization */
#define MAX_COMMAND_BUFFER_LEN	(256 + 8)	//UINT32s - space for 256 samples + header
#define TARGET_NAME             "AWECore"
#define NUM_INPUT_CHANNELS      2
#define NUM_OUTPUT_CHANNELS     2
#define SAMPLE_RATE             48000
#define BASE_BLOCK_SIZE         32

#define CORE_SPEED              1.2e9f
#define PROFILE_SPEED           10e6f
#define NUM_THREADS             2
#define HAS_FLASHFILESYSTEM     NULL


#include "ModuleList.h"

#ifdef	__cplusplus
}
#endif

#endif	// _TARGETINFO_H
