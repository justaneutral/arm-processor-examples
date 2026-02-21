#ifndef TISP_ADDCONSTANT_HPP
#define TISP_ADDCONSTANT_HPP

#include "TISP_common.hpp"

namespace TISP {
namespace DSPLIB {

/**
 *  @ingroup dsplibNode
 */

/**@{*/

/**
 *  \class addConstant
 *  @brief Provides node-level APIs for the DSPLIB_addConstant kernel from DSPLIB.
 *  @tparam T        : Supports float
 *  @tparam dmaDir   : Supports @ref TISP::NO_DMA, @ref TISP::DMA_IN, @ref TISP::DMA_OUT, and @ref TISP::DMA_IN_OUT
 */

template <typename T, dmaDir_t dmaDir> class addConstant : public virtualOp {

   T *pIn0; /*!< Pointer to input constant */
   T *pIn1; /*!< Pointer to address of input vector */
   T *pOut; /*!< Pointer to address of result vector*/

   void  *handle; /*!< handle required by DSPLIB_addConstant kernel (stores SE/SA params for the kernel) */
   size_t size;   /*!< length of vector */

   bool pingPongFlagIn  = 0; /*!< Flag to pingPong input ptr; set based on @p dmaDir parameter */
   bool pingPongFlagOut = 0; /*!< Flag to pingPong output ptr; set based on @p dmaDir parameter*/

 public:
   /**
    * @brief Creates an addConstant node
    * @param [in]  *_pIn0 : Pointer to input constant; initializes @ref pIn0
    * @param [in]  *_pIn1 : Pointer to address of input vector; initializes @ref pIn1
    * @param [out] *_pOut : Pointer to address of output vector; initializes @ref pOut
    * @param [in]  _size  : length of vector; initializes @ref size
    *
    */

   addConstant(T *_pIn0, T *_pIn1, T *_pOut, size_t _size);
   void exec();
   ~addConstant() { free(handle); }
};

/** @} */
} // namespace DSPLIB
} // namespace TISP

#endif
