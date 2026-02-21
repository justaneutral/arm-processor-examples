#ifndef TISP_BLOCKCOPY_HPP
#define TISP_BLOCKCOPY_HPP

#include "TISP_common.hpp"

namespace TISP {

namespace misc {

/**
 *  @ingroup miscNode
 */

/**@{*/

/**
 *  \class blockCopy
 *  @brief Provides node-level APIs for the block copy kernel
 *  @tparam T        : Supports float
 *  @tparam dmaDir   : Supports @ref TISP::NO_DMA, @ref TISP::DMA_IN, @ref TISP::DMA_OUT, and @ref TISP::DMA_IN_OUT
 */

template <typename T, dmaDir_t dmaDir> class blockCopy : public virtualOp {

   T *pX; /*!< Pointer to address of input vector */
   T *pY; /*!< Pointer to address of result vector*/

   size_t width;  /*!< width of input block */
   size_t height; /*!< height of input block */

   __SE_TEMPLATE_v1 se0Param = __gen_SE_TEMPLATE_v1(); /*!< structure to hold SE params*/
   __SA_TEMPLATE_v1 sa0Param = __gen_SA_TEMPLATE_v1(); /*!< structure to hold SA params*/

   typedef typename c7x::make_full_vector<T>::type vec;

   bool pingPongFlagIn  = 0; /*!< Flag to pingPong input ptr; set based on @p dmaDir parameter */
   bool pingPongFlagOut = 0; /*!< Flag to pingPong output ptr; set based on @p dmaDir parameter*/

   /**
    * @brief Optimized function that uses SE/SA to perform block copy; this function is not exposed to user
    * @param [in]  *_pX    : Pointer to input 2D block
    * @param [out] *_pY    : Pointer to output 2D block
    * @param [in]  _width  : Width of input 2D block
    * @param [in]  _height : Height of input 2D block
    * @return void
    */

   void exec_ci(T *_pX, T *_pY, size_t _width, size_t _height);

 public:
   /**
    * @brief Creates a blockCopy node
    * @param [in]  *_pX    : Pointer to input 2D block; initializes @ref pX
    * @param [out] *_pY    : Pointer to output 2D block; initializes @ref pY
    * @param [in]  _width  : Width of input 2D block; initializes @ref width
    * @param [in]  _height : Height of input 2D block; initializes @ref height
    *
    */

   blockCopy(T *_pX, T *_pY, size_t _width, size_t _height);

   void exec();

   ~blockCopy() {}
};

/** @} */

} // namespace misc

} // namespace TISP

#endif
