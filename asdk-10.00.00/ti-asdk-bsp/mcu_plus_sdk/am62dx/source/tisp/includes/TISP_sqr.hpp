#ifndef TISP_SQR_HPP
#define TISP_SQR_HPP

#include "TISP_common.hpp"

namespace TISP {

namespace DSPLIB {

/**
 *  @ingroup dsplibNode
 */

/**@{*/

/**
 *  \class sqr
 *  @brief Provides node-level APIs for the DSPLIB_sqr kernel from DSPLIB.
 *  @tparam T        : Supports float
 *  @tparam dmaDir   : Supports @ref TISP::NO_DMA, @ref TISP::DMA_IN, @ref TISP::DMA_OUT, and @ref TISP::DMA_IN_OUT
 */

template <typename T, dmaDir_t dmaDir> class sqr : public virtualOp {

   T *pX; /*!< Pointer to address of input vector */
   T *pY; /*!< Pointer to address of result vector*/

   void  *handle; /*!< handle required by DSPLIB_addConstant kernel (stores SE/SA params for the kernel) */
   size_t size;   /*!< input length */

   bool pingPongFlagIn  = 0; /*!< Flag to pingPong input ptr; set based on @p dmaDir parameter */
   bool pingPongFlagOut = 0; /*!< Flag to pingPong output ptr; set based on @p dmaDir parameter*/

 public:
   /**
    * @brief Creates a DSPLIB_sqr node
    * @param [in]  *_pX      : Pointer to input vector; initializes @ref pX
    * @param [out] *_pY      : Pointer to output vector; initializes @ref pY
    * @param [in]  _size     : Length of the vector; initializes @ref size
    *
    */

   sqr(T *_pX, T *_pY, size_t _size);

   void exec();

   ~sqr() { free(handle); }
};

/** @} */

} // namespace DSPLIB

} // namespace TISP

#endif
