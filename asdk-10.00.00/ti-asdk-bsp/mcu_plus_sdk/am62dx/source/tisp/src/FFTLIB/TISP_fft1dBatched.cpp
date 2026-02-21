#include "TISP_FFTLIB_common.hpp"
#include <TISP_fft1dBatched.hpp>
#include <math.h>

using namespace TISP;
using namespace TISP::FFTLIB;

template <typename T, dmaDir_t dmaDir> void fft1dBatched<T, dmaDir>::tw_gen()
{

   uint32_t     i, j, k, t;
   const double PI = 3.141592654;
   t               = numPoints >> 2;
   for (j = 1, k = 0; j <= numPoints >> 2; j = j << 2) {
      for (i = 0; i < numPoints >> 2; i += j) {
         /* TODO: Big endian requires different format of Twiddle factors? */
         pW[k]             = (FFTLIB_F32) cos(2 * PI * i / numPoints);
         pW[k + 1]         = (FFTLIB_F32) -sin(2 * PI * i / numPoints);
         pW[k + 2 * t]     = (FFTLIB_F32) cos(4 * PI * i / numPoints);
         pW[k + 2 * t + 1] = (FFTLIB_F32) -sin(4 * PI * i / numPoints);
         pW[k + 4 * t]     = (FFTLIB_F32) cos(6 * PI * i / numPoints);
         pW[k + 4 * t + 1] = (FFTLIB_F32) -sin(6 * PI * i / numPoints);
         k += 2;
      }
      k += (t) *4;
      t = t >> 2;
   }
}

template <typename T, dmaDir_t dmaDir>
fft1dBatched<T, dmaDir>::fft1dBatched(T *_pX, T *_pY, uint32_t _numPoints, uint32_t _numChannels)
    : pX(_pX), pY(_pY), numPoints(_numPoints), numChannels(_numChannels)
{

   pBlock = new uint8_t[TISP_FFT1DBATCHED_I32FC_C32FC_O32FC_PBLOCK_SIZE];

   bufParamsX.dim_x = numPoints * numChannels * 2;
   bufParamsW.dim_x = numPoints * 2;

   pingPongFlagIn  = 0;
   pingPongFlagOut = 0;

   pW = (T *) memalign(128, numPoints * 2 * sizeof(T));

   if (std::is_same<float, T>::value) {

      bufParamsX.data_type = FFTLIB_FLOAT32;
      bufParamsW.data_type = FFTLIB_FLOAT32;

      tw_gen();

      FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_init((FFTLIB_F32 *) pX, &bufParamsX, (FFTLIB_F32 *) pW, &bufParamsW,
                                                 (FFTLIB_F32 *) pY, &bufParamsX, numPoints, numChannels,
                                                 (void *) pBlock);
   }
   else {
      // TBD: error out since not implemented!
   }
   return;
}

template <typename T, dmaDir_t dmaDir> void fft1dBatched<T, dmaDir>::exec()
{

   if (std::is_same<float, T>::value) {
      /* printf("\npX: %p\n", pX + pingPongFlagIn * numPoints * 2 * numChannels);
       */
      /* printf("\npY: %p\n", pY + pingPongFlagOut * numPoints * 2 * numChannels);
       */
      FFTLIB_fft1dBatched_i32fc_c32fc_o32fc_kernel((FFTLIB_F32 *) pX + pingPongFlagIn * numPoints * 2 * numChannels,
                                                   &bufParamsX, (FFTLIB_F32 *) pW, &bufParamsW,
                                                   (FFTLIB_F32 *) pY + pingPongFlagOut * numPoints * 2 * numChannels,
                                                   &bufParamsX, numPoints, numChannels, (void *) pBlock);
   }
   else {
      // TBD: error out since not implemented!
   }

   setPingPongFlag<dmaDir>(pingPongFlagIn, pingPongFlagOut);

   return;
}

/***************************/
/* Explicit instantiation  */
/***************************/

template class TISP::FFTLIB::fft1dBatched<float, NO_DMA>;
template class TISP::FFTLIB::fft1dBatched<float, DMA_IN>;
template class TISP::FFTLIB::fft1dBatched<float, DMA_OUT>;
template class TISP::FFTLIB::fft1dBatched<float, DMA_IN_OUT>;
