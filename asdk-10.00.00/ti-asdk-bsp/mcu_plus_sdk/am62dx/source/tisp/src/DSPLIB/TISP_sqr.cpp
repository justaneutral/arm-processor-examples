#include "TISP_sqr.hpp"
#include "TISP_DSPLIB_common.hpp"

using namespace TISP;
using namespace TISP::DSPLIB;

template <typename T, dmaDir_t dmaDir> sqr<T, dmaDir>::sqr(T *_pX, T *_pY, size_t _size) : pX(_pX), pY(_pY), size(_size)
{

   DSPLIB_bufParams1D_t bufParamsIn, bufParamsOut;
   DSPLIB_sqr_InitArgs  kerInitArgs;

   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   bufParamsIn.dim_x  = size;
   bufParamsOut.dim_x = size;

   bufParamsIn.data_type  = DSPLIB_element_dType<T>::value;
   bufParamsOut.data_type = DSPLIB_element_dType<T>::value;

   handle = memalign(128, DSPLIB_sqr_getHandleSize(&kerInitArgs));
   DSPLIB_sqr_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   pingPongFlagIn  = 0;
   pingPongFlagOut = 0;
}

template <typename T, dmaDir_t dmaDir> void sqr<T, dmaDir>::exec()
{
   DSPLIB_sqr_exec(handle, pX + pingPongFlagIn * size, pY + pingPongFlagOut * size);
   setPingPongFlag<dmaDir>(pingPongFlagIn, pingPongFlagOut);
}

template class TISP::DSPLIB::sqr<float, NO_DMA>;
template class TISP::DSPLIB::sqr<float, DMA_IN>;
template class TISP::DSPLIB::sqr<float, DMA_OUT>;
template class TISP::DSPLIB::sqr<float, DMA_IN_OUT>;
