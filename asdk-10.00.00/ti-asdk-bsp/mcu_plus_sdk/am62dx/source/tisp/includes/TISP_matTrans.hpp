#ifndef TISP_MATRIXTRANS_HPP
#define TISP_MATRIXTRANS_HPP

#include "TISP_common.hpp"

namespace TISP {

namespace DSPLIB {

/**
 *  @ingroup dsplibNode
 */

/**@{*/

/**
 *  \class matTrans
 *  @brief Provides node-level APIs for the DSPLIB_matTrans kernel from DSPLIB.
 *  @tparam T        : Supports float and double
 *  @tparam dmaDir   : Supports @ref TISP::NO_DMA, @ref TISP::DMA_IN, @ref TISP::DMA_OUT, and @ref TISP::DMA_IN_OUT
 */

template <typename T, dmaDir_t dmaDir> class matTrans : public virtualOp {

   T *pX; /*!< Pointer to address of input vector */
   T *pY; /*!< Pointer to address of result vector*/

   void *handle; /*!< handle required by DSPLIB_addConstant kernel (stores SE/SA params for the kernel) */

   size_t pingPongOffsetIn  = 0; /*!< Calculate input pingPing offset */
   size_t pingPongOffsetOut = 0; /*!< Calculate output pingPing offset */

   bool pingPongFlagIn  = 0; /*!< Flag to pingPong input ptr; set based on @p dmaDir parameter */
   bool pingPongFlagOut = 0; /*!< Flag to pingPong output ptr; set based on @p dmaDir parameter*/

 public:
   /**
    * @brief Creates a DSPLIB_matTrans node
    * @param [in]  *_pX      : Pointer to input matrix; initializes @ref pX
    * @param [out] *_pY      : Pointer to output matrix; initializes @ref pY
    * @param [in]  _width    : Width of input matrix
    * @param [in]  _height   : Height of input matrix
    * @param [in]  _inPitch  : Pitch of input matrix
    * @param [in]  _outPitch : Pitch of output matrix
    *
    */
   matTrans(T *_pX, T *_pY, size_t _width, size_t _height, size_t _inPitch, size_t _outPitch);

   void exec();

   ~matTrans() { free(handle); }
};

/** @} */

} // namespace DSPLIB

} // namespace TISP

#endif
