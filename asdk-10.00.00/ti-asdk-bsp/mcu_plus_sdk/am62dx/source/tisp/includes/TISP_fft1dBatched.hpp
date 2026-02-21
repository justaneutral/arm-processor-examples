#ifndef TISP_FFT1DBATCHED_HPP
#define TISP_FFT1DBATCHED_HPP

#include "TISP_common.hpp"
#include <fftlib.h>

#define TISP_FFT1DBATCHED_I32FC_C32FC_O32FC_PBLOCK_SIZE (13 * SE_PARAM_SIZE)

namespace TISP {
namespace FFTLIB {

/**
 *  @ingroup fftlibNode
 */

/**@{*/

/**
 *  \class fft1dBatched
 *  @brief Provides node-level APIs for the FFTLIB_fft1dBatched kernel from DSPLIB. Assumes complex float data that is
 *         stored in interleaved fashion.
 *  @tparam T        : Supports float
 *  @tparam dmaDir   : Supports @ref TISP::NO_DMA, @ref TISP::DMA_IN, @ref TISP::DMA_OUT, and @ref TISP::DMA_IN_OUT
 */

template <typename T, dmaDir_t dmaDir> class fft1dBatched : public virtualOp {

   T *pX; /*!< Pointer to input vector (2D matrix flattened to numChannels * numPoints) */
   T *pW; /*!< Pointer to input twiddle factor vector */
   T *pY; /*!< Pointer to FFT output */

   FFTLIB_bufParams1D_t bufParamsX; /*!< FFTLIB bufParams structure of input data */
   FFTLIB_bufParams1D_t bufParamsW; /*!< FFTLIB bufParams structure of twiddles */
   FFTLIB_bufParams1D_t bufParamsY; /*!< FFTLIB bufParams structure of output data */

   uint32_t numPoints;   /*!< FFT size */
   uint32_t numChannels; /*!< number of channels */
   uint8_t *pBlock;      /*!< Pointer with address location of SE and SA params */

   bool pingPongFlagIn  = 0; /*!< Flag to pingPong input ptr; set based on @p dmaDir parameter */
   bool pingPongFlagOut = 0; /*!< Flag to pingPong output ptr; set based on @p dmaDir parameter*/

   void tw_gen(); /*!< Utility function to generate the twiddle factors */

 public:
   /**
    * @brief Creates a FFTLIB_fft1dBatched node
    * @param [in]  *_pX        : Pointer to input; initializes @ref pX
    * @param [out] *_pY        : Pointer to FFT output; initializes @ref pY
    * @param [in]  _numPoints  : FFT size
    * @param [in]  _numChannels: Number of FFT channels
    *
    */

   fft1dBatched(T *_pX, T *_pY, uint32_t _numPoints, uint32_t _numChannels);

   void exec();

   ~fft1dBatched()
   {
      free(pW);
      delete[] pBlock;
   }
};

/** @} */

} // namespace FFTLIB
} // namespace TISP

#endif
