/* ----------------------------------------------------------------------------
 *	 Preprocessor Definitions
 * ------------------------------------------------------------------------- */

#ifndef TI_C7X_TARGET_PROCESSOR_H_
#define TI_C7X_TARGET_PROCESSOR_H_

extern void __disable_irq();
extern void __enable_irq();

#define DISABLE_INTERRUPTS() __disable_irq()
#define ENABLE_INTERRUPTS()  __enable_irq()

#include "Generic_TargetProcessor.h"

#define VEC_RESTRICT    restrict
#define VEC_INLINE      inline

/* ----------------------------------------------------------------------
** Memory segments for various processors
** ------------------------------------------------------------------- */

#define AWE_MOD_SLOW_CODE	__attribute__((section(".awe_mod_slowcode")))
#define AWE_MOD_FAST_CODE	__attribute__((section(".awe_mod_fastcode")))
#define AWE_FW_SLOW_CODE	__attribute__((section(".awe_fw_slowcode")))
#define AWE_FW_FAST_CODE	__attribute__((section(".awe_fw_fastcode")))

#define AWE_USB_INTERNAL_CODE   __attribute__((section(".awe_usb_internalcode")))

#define AWE_PRIO0_FAST_CODE		__attribute__((section(".awe_prio0_fastcode")))

#define AWE_MOD_SLOW_DM_DATA    __attribute__((section(".awe_mod_slowdmdata")))
#define AWE_MOD_FAST_DM_DATA    __attribute__((section(".awe_mod_fastdmdata")))
#define AWE_MOD_SLOW_PM_DATA    __attribute__((section(".awe_mod_slowpmdata")))
#define AWE_MOD_FAST_PM_DATA    __attribute__((section(".awe_mod_fastpmdata")))

#endif	  /* !defined  TI_C7X_TARGET_PROCESSOR_H_ */
