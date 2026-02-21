#ifndef TISP_CORE_COMMON_HPP
#define TISP_CORE_COMMON_HPP

#include "stdint.h"
#include <stdlib.h>
#include <type_traits>

#include <TISP_types.hpp>
#include <TISP_virtualOp.hpp>

#if defined(HOST_EMULATION)
#include <malloc.h>
#elif __C7X_VEC_SIZE_BITS__ == 512
#include <ti/csl/arch/c7x/cslr_C7X_CPU.h>
#include <ti/csl/csl_clec.h>
#else
#include <drivers/hw_include/csl_clec.h>
#endif

#include "TISP_utils.hpp"

#endif
