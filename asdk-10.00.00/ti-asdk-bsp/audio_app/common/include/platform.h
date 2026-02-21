/*******************************************************************************
*
*               Audio Framework
*               ---------------
*
********************************************************************************
*     PlatformCore1.h
********************************************************************************
*
*     Description:  AWE Platform Interface Header File
*
*     Copyright:    (c) 2018 DSP Concepts, Inc. All rights reserved.
*                   3235 Kifer Road
*                   Santa Clara, CA 95054
*
*******************************************************************************/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <kernel/dpl/DebugP.h>
#include <kernel/dpl/SemaphoreP.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "AWECoreUtils.h"
#include "ti_avb_defs.h"

#ifndef MAX
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#define UArg  uint32_t*
#define Void  void
#define IArg  int32_t

typedef enum AweDebugGpio
{
#ifdef SOC_AM62AX
    mcasp_debug_pin   = 14,  /* User Expander, Pin#22 */
    uart_debug_pin    = 36,  /* User Expander, Pin#29 */
    awecore_debug_pin = 39,  /* User Expander, Pin#18 */
    generic_debug_pin = 40   /* User Expander, Pin#32 */
#elif SOC_AM62DX /* TBD */
    mcasp_debug_pin   = 0,  /* TBD */
    uart_debug_pin    = 1,  /* TBD */
    awecore_debug_pin = 2,  /* TBD */
    generic_debug_pin = 3   /* TBD  */
#elif SOC_AM275

#endif

}AweDebugGpio;

#include "AWECore.h"
#include "Errors.h"
#include "targetInfo.h"
#include "ti_bsp_defs.h"

#define DMA_AUDIO_SAMPLE_SIZE   (4)  /* audio sample: 32-bit */
#define AVB_AUDIO_SAMPLE_SIZE   (2)  /* audio samples: 16-bit */

#define BUFALIGN                (256)  /* alignment of buffer */

#ifdef SOC_AM62AX
#define ADC_DATA_PIN_CNT        1
#define DAC_DATA_PIN_CNT        1
#define ADC_CHANNEL_COUNT       2
#define DAC_CHANNEL_COUNT       2
#define ADC_CHANNEL_STRIDE      2
#define DAC_CHANNEL_STRIDE      2
#endif

#ifdef SOC_AM62DX
#define ADC_DATA_PIN_CNT            1
#define DAC_DATA_PIN_CNT            4
#ifdef MULTI_CHANNEL_AUDIO
    #define ADC_CHANNEL_COUNT       8
    #define DAC_CHANNEL_COUNT       8
    #define ADC_CHANNEL_STRIDE      8
    #define DAC_CHANNEL_STRIDE      8
#else
    #define ADC_CHANNEL_COUNT       2
    #define DAC_CHANNEL_COUNT       2
    #define ADC_CHANNEL_STRIDE      2
    #define DAC_CHANNEL_STRIDE      2
#endif
#endif

#ifdef SOC_AM275
#define ADC_DATA_PIN_CNT        4
#define DAC_DATA_PIN_CNT        2
#define ADC_CHANNEL_COUNT       8
#define DAC_CHANNEL_COUNT       8
#define ADC_CHANNEL_STRIDE      8
#define DAC_CHANNEL_STRIDE      8
#endif

#ifdef ENABLE_AVB_AUDIO
#define AVB_RX_CHANNEL_COUNT    (8)
#define AVB_TX_CHANNEL_COUNT    (8)
#define AVB_RX_CHANNEL_STRIDE   (8)
#define AVB_TX_CHANNEL_STRIDE   (8)
#else
#define AVB_RX_CHANNEL_COUNT    (0)
#define AVB_TX_CHANNEL_COUNT    (0)
#endif

#define INPUT_CHANNEL_COUNT     (ADC_CHANNEL_COUNT + AVB_RX_CHANNEL_COUNT)
#define OUTPUT_CHANNEL_COUNT    (DAC_CHANNEL_COUNT + AVB_TX_CHANNEL_COUNT)

#define BUFSIZE_PER_CHANNEL     (AUDIO_BLOCK_SIZE * DMA_AUDIO_SAMPLE_SIZE)
#define DMA_RX_BUF_SIZE         (BUFSIZE_PER_CHANNEL * ADC_CHANNEL_COUNT)
#define DMA_TX_BUF_SIZE         (BUFSIZE_PER_CHANNEL * DAC_CHANNEL_COUNT)

#define AVB_RX_FRAME_SIZE       (AVB_RX_CHANNEL_COUNT*AVB_AUDIO_SAMPLE_SIZE*AUDIO_BLOCK_SIZE)
#define AVB_TX_FRAME_SIZE       (AVB_TX_CHANNEL_COUNT*AVB_AUDIO_SAMPLE_SIZE*AUDIO_BLOCK_SIZE)

#define FAILURE 0
#define SUCCESS 1

#define STRIDE2 2
#define STRIDE4 4
#define STRIDE8 8

/** Create a word containing packed characters. */
#define MAKE_PACKED_STRING(c1, c2, c3, c4) \
	( (((c4) & 0xffU) << 24) | (((c3) & 0xffU) << 16) | (((c2) & 0xffU) << 8) | ((c1) & 0xffU) )

extern AWEInstance g_AWEInstance;

extern volatile INT32 g_nPumpCount;
extern volatile BOOL g_bDeferredProcessingRequired;

extern uint32_t AWE_Packet_Buffer[MAX_COMMAND_BUFFER_LEN];
extern SemaphoreP_Object semAudioDataReady;

typedef struct Awe_Data_Buf
{
    int32_t    *pDmaRxBuf;
    int32_t    *pDmaTxBuf;
    int16_t    *pIpcRxBuf;
    int16_t    *pIpcTxBuf;
}Awe_Data_Buf;

extern void awe_createInstance(void);

void awe_transferAudioBufs(AWEInstance *pAWE, Awe_Data_Buf* pAweBuf);
void awe_executeAudioPump(AWEInstance *pAWE);

void AWEPump(void* Arg1);
void AWETuning(uint32_t *pIn, uint32_t *pOut);
void AWEIdleTask(void* Arg1);
int32_t awe_cbAudioStart(struct _AWEInstance *PAWE);
int32_t awe_cbAudioStop(struct _AWEInstance *PAWE);

void debugGpio_toggle(AweDebugGpio pinNum);
void debugGpio_high(AweDebugGpio pinNum);
void debugGpio_low(AweDebugGpio pinNum);
void debugGpio_setup();

int16_t* ti_avb_getAvbRxDataPtr();
int16_t* ti_avb_getAvbRxDataPtr();

typedef struct
{
   AWEInstance          *pAWE;
   SemaphoreP_Object    *LayoutSem;
   SemaphoreP_Object    *AweLibFnMutex;
   int                  LayoutNum;
   uint32_t             layoutCycleCount;
} LayoutTaskArgs;

#endif
