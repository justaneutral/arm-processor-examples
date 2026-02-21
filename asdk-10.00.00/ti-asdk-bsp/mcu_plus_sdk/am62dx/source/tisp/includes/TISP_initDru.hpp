#ifndef TISP_INITDRU_H
#define TISP_INITDRU_H

#include <stdint.h>
#include <stdio.h>
#define DRU_LOCAL_EVENT_START_DEFAULT (192U) // Default for J721E and J721S2
#define DRU_LOCAL_EVENT_START_J784S4 (664U)

#if __C7X_VEC_SIZE_BITS__ == 512
#include "ti/drv/sciclient/sciclient.h"
#include "ti/drv/udma/include/udma_types.h"
#include <ti/drv/udma/dmautils/dmautils.h>
#include <ti/drv/udma/udma.h>
#else
#include <drivers/dmautils/dmautils.h>
#include <drivers/dmautils/udma_standalone/udma.h>
#include <drivers/sciclient.h>
#endif
namespace TISP {
void appC7xClecInitDru(void);

int32_t test_sciclientDmscGetVersion(char *version_str, uint32_t version_str_size);

} // namespace TISP
#endif
