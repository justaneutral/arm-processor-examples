#include "../common/TISP_common.hpp"
#include <TISP_blockCopy.hpp>

#if defined(HOST_EMULATION)
#define RESTRICT
#else
#define RESTRICT restrict
#endif

using namespace TISP;
using namespace TISP::misc;

template <typename T, dmaDir_t dmaDir>
blockCopy<T, dmaDir>::blockCopy(T *_pX, T *_pY, size_t _width, size_t _height)
    : pX(_pX), pY(_pY), width(_width), height(_height)
{

   se0Param.ELETYPE = c7x::se_eletype<vec>::value;
   se0Param.VECLEN  = c7x::se_veclen<vec>::value;
   se0Param.DIMFMT  = __SE_DIMFMT_1D;

   se0Param.ICNT0  = width * height;
   sa0Param.VECLEN = c7x::sa_veclen<vec>::value;
   sa0Param.DIMFMT = __SA_DIMFMT_1D;

   sa0Param.ICNT0 = width * height;

   pingPongFlagIn  = 0;
   pingPongFlagOut = 0;
}

template <typename T, dmaDir_t dmaDir>
void blockCopy<T, dmaDir>::exec_ci(T *RESTRICT _pX, T *RESTRICT _pY, size_t _width, size_t _height)
{

   /* for (int32_t i = 0; i < height; i++) { */
   /*    printf("\nl2: %d\n", i); */
   /*    for (int32_t j = 0; j < width; j++) { */
   /*       printf(" %.f\n", _pX[i * width + j]); */
   /*       _pY[i * width + j] = _pX[i * width + j]; */
   /*    } */
   /* } */

   // open SE0 to fetch Input samples
   __SE0_OPEN(_pX, se0Param);

   // open SA0 to write output samples
   __SA0_OPEN(sa0Param);

   for (uint32_t i = 0; i < (_width * _height) / (c7x::max_simd<T>::value); i++) {
      __vpred tmp  = c7x::strm_agen<0, vec>::get_vpred();
      vec    *addr = c7x::strm_agen<0, vec>::get_adv(_pY);
      __vstore_pred(tmp, addr, c7x::strm_eng<0, vec>::get_adv());
   }

   // close SE/SA
   __SE0_CLOSE();
   __SA0_CLOSE();
}

template <typename T, dmaDir_t dmaDir> void blockCopy<T, dmaDir>::exec()
{

   exec_ci(pX + pingPongFlagIn * width * height, pY + pingPongFlagOut * width * height, width, height);

   setPingPongFlag<dmaDir>(pingPongFlagIn, pingPongFlagOut);
}

template class TISP::misc::blockCopy<float, NO_DMA>;
template class TISP::misc::blockCopy<float, DMA_IN>;
template class TISP::misc::blockCopy<float, DMA_OUT>;
template class TISP::misc::blockCopy<float, DMA_IN_OUT>;
