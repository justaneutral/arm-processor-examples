#include "TISP_addConstant.hpp"
#include "TISP_DSPLIB_common.hpp"

using namespace TISP;
using namespace TISP::DSPLIB;

template <typename T, dmaDir_t dmaDir>
addConstant<T, dmaDir>::addConstant(T *_pIn0, T *_pIn1, T *_pOut, size_t _size)
    : pIn0(_pIn0), pIn1(_pIn1), pOut(_pOut), size(_size)
{

   DSPLIB_bufParams1D_t        bufParamsIn, bufParamsOut;
   DSPLIB_addConstant_InitArgs kerInitArgs;

   kerInitArgs.dataSize  = size;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   bufParamsIn.dim_x  = size;
   bufParamsOut.dim_x = size;

   bufParamsIn.data_type  = DSPLIB_element_dType<T>::value;
   bufParamsOut.data_type = DSPLIB_element_dType<T>::value;

   handle = memalign(128, DSPLIB_addConstant_getHandleSize(&kerInitArgs));
   DSPLIB_addConstant_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   pingPongFlagIn  = 0;
   pingPongFlagOut = 0;
}

template <typename T, dmaDir_t dmaDir> void addConstant<T, dmaDir>::exec()
{
   DSPLIB_addConstant_exec(handle, pIn0, pIn1 + pingPongFlagIn * size, pOut + pingPongFlagOut * size);
   setPingPongFlag<dmaDir>(pingPongFlagIn, pingPongFlagOut);
}

template class TISP::DSPLIB::addConstant<float, NO_DMA>;
template class TISP::DSPLIB::addConstant<float, DMA_IN>;
template class TISP::DSPLIB::addConstant<float, DMA_OUT>;
template class TISP::DSPLIB::addConstant<float, DMA_IN_OUT>;
