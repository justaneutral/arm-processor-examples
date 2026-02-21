#include "TISP_DSPLIB_common.hpp"
#include <TISP_matTrans.hpp>

using namespace TISP;
using namespace TISP::DSPLIB;

//! [DSPLIB_matTrans constructor]

template <typename T, dmaDir_t dmaDir>
matTrans<T, dmaDir>::matTrans(T *_pX, T *_pY, size_t _width, size_t _height, size_t _inPitch, size_t _outPitch)
    : pX(_pX), pY(_pY)
{

   /***********************************************************************/
   /* Initialize data structures required by DSPLIB_matrTrans from DSPLIB */
   /***********************************************************************/

   DSPLIB_matTransInitArgs kerInitArgs;
   kerInitArgs.dimX      = _width;
   kerInitArgs.dimY      = _height;
   kerInitArgs.funcStyle = DSPLIB_FUNCTION_OPTIMIZED;

   int32_t handleSize = DSPLIB_matTrans_getHandleSize(&kerInitArgs);
   handle             = memalign(128, handleSize);

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut;

   bufParamsIn.data_type = DSPLIB_element_dType<T>::value;
   bufParamsIn.dim_x     = _width;
   bufParamsIn.dim_y     = _height;
   bufParamsIn.stride_y  = _inPitch;

   bufParamsOut.data_type = DSPLIB_element_dType<T>::value;
   bufParamsOut.dim_x     = _height;
   bufParamsOut.dim_y     = _width;
   bufParamsOut.stride_y  = _outPitch;

   /******************************************************************************/
   /* Call init function from DSPLIB to for matrix transpose to set SE/SA params */
   /******************************************************************************/

   DSPLIB_matTrans_init(handle, &bufParamsIn, &bufParamsOut, &kerInitArgs);

   /***************************************************/
   /* Set pingPong offsets and flags to default value */
   /***************************************************/

   pingPongOffsetIn  = (_height * _inPitch) / sizeof(T);
   pingPongOffsetOut = (_width * _outPitch) / sizeof(T);

   pingPongFlagIn  = 0;
   pingPongFlagOut = 0;
}

//! [DSPLIB_matTrans constructor]

template <typename T, dmaDir_t dmaDir> void matTrans<T, dmaDir>::exec()
{
   DSPLIB_matTrans_exec(handle, pX + pingPongFlagIn * pingPongOffsetIn, pY + pingPongFlagOut * pingPongOffsetOut);

   setPingPongFlag<dmaDir>(pingPongFlagIn, pingPongFlagOut);
}

template class TISP::DSPLIB::matTrans<float, NO_DMA>;
template class TISP::DSPLIB::matTrans<float, DMA_IN>;
template class TISP::DSPLIB::matTrans<float, DMA_OUT>;
template class TISP::DSPLIB::matTrans<float, DMA_IN_OUT>;

template class TISP::DSPLIB::matTrans<double, NO_DMA>;
template class TISP::DSPLIB::matTrans<double, DMA_IN>;
template class TISP::DSPLIB::matTrans<double, DMA_OUT>;
template class TISP::DSPLIB::matTrans<double, DMA_IN_OUT>;
