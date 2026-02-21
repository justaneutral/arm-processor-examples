#ifndef TISP_H
#define TISP_H

#include "TISP_addConstant.hpp"
#include "TISP_blockCopy.hpp"
#include "TISP_common.hpp"
#include "TISP_fft1dBatched.hpp"
#include "TISP_initDru.hpp"
#include "TISP_matTrans.hpp"
#include "TISP_sqr.hpp"
#include "TISP_superNode.hpp"
#include "TISP_util.hpp"

/**
 * \defgroup NODE Nodes
 * \brief Node-level APIs
 */

/**
 *  @ingroup NODE
 */

/**@{*/

/**
 * \defgroup dsplibNode DSPLIB Nodes
 * \brief Node-level APIs supported from DSPLIB.
 * \details Currently, we support only three kernels (DSPLIB_addConstant, DSPLIB_matTrans, and DSPLIB_sqr). We will be
 *          adding more in future releases.
 */

/**
 * \defgroup miscNode Miscellaneous Nodes
 * \brief Node-level APIs supported for miscellaneous kernels implemented within TISP, e.g., block copy kernel.
 * \details Currently, we support only one kernel. We will be adding more in future releases.
 */

/**
 * \defgroup fftlibNode FFTLIB Nodes
 * \brief Node-level APIs supported for FFT-style kernels from FFTLIB
 * \details Currently, we support only one kernel. We will be adding more in future releases.
 */

/** @} */

/**
 * \defgroup SUPERNODE Supernodes
 * \brief SuperNode-level APIs
 */

#endif
