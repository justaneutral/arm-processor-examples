#ifndef TISP_SUPERNODE_COMMONS_HPP
#define TISP_SUPERNODE_COMMONS_HPP

#include "TISP_dmaUtils.hpp"
#include "TISP_superNode.hpp"
#if __C7X_VEC_SIZE_BITS__ == 512
#include "ti/drv/udma/dmautils/include/dmautils_autoincrement_3d.h"
#include "ti/drv/udma/include/udma_types.h"
#else
#include <drivers/dmautils/dmautils.h>
#include <drivers/dmautils/udma_standalone/udma.h>
#endif
#endif
