#ifndef TISP_SUPERNODE_HPP
#define TISP_SUPERNODE_HPP

/* #include "common.h" */

#include "TISP_virtualOp.hpp"

#if __C7X_VEC_SIZE_BITS__ == 512
#include "ti/drv/udma/dmautils/dmautils.h"
#include "ti/drv/udma/udma.h"
#else
#include <drivers/dmautils/dmautils.h>
#include <drivers/dmautils/udma_standalone/udma.h>
#endif

#include <vector>

#if defined(HOST_EMULATION)
#include <malloc.h>
#elif __C7X_VEC_SIZE_BITS__ == 512
#include <ti/csl/arch/c7x/cslr_C7X_CPU.h>
#include <ti/csl/csl_clec.h>
#else
#include <drivers/hw_include/csl_clec.h>
#endif

#define TISP_MAX_DMA_CHANNEL_SUPER_NODE (4)

namespace TISP {
namespace SuperNode {

extern Udma_DrvHandle appDrvHandle;

/**
 *  @ingroup SUPERNODE
 */

/**@{*/

/**
 *  \class superNode
 *  @brief Provides the base class for supernode. Contains member functions to create dma context and allocate memory
 *         required to store DRU-related parameters.
 */

class superNode {

 private:
   /* struct Udma_DrvObj udmaDrvObj; */
   opVec &myOpVec;
   size_t numChannels;
   size_t numInChannels;
   size_t numOutChannels;

 protected:
   int32_t  chIdIn[TISP_MAX_DMA_CHANNEL_SUPER_NODE];  // Input channel IDs
   int32_t  chIdOut[TISP_MAX_DMA_CHANNEL_SUPER_NODE]; // Output channel IDs
   uint8_t *dmaUtilsContext;
   uint8_t *pTrMemCh[TISP_MAX_DMA_CHANNEL_SUPER_NODE];

   /**
    * @brief Supernode constructor that is called to create the supernode with appropriate parameters. Typically, used
    *        by classes derived from the @ref superNode as part of their constructor c
    * @param [in] &_myOpVec       : Operator vector with list of nodes to be executed inside the supernode
    * @param [in] _numChannels    : Total number of DMA channels to be used by the supernode
    * @param [in] _numInChannels  : Total number of input DMA channels
    * @param [in] _numOutChannels : Total number of output DMA channels
    * @return None
    */
   superNode(opVec &_myOpVec, size_t _numChannels, size_t _numInChannels, size_t _numOutChannels);

 public:
   /**
      @brief This function executes a given supernode. It contains the boiler plate template for  block-based DMA and
      compute flow. The function calls the @ref execute::graph function which in turn executes the correct compute flow.
      For data movement, appropriate APIs are called around the @ref execute::graph function.
      * @return None.
    */

   void exec();

   virtual ~superNode()
   {
      free(dmaUtilsContext);
      for (uint32_t ch = 0; ch < numChannels; ch++) {
         free(pTrMemCh[ch]);
      }
      /* Udma_deinit((Udma_DrvHandle) (&udmaDrvObj)); */
   }

   static Udma_DrvHandle setUdmaDrvHandle(Udma_DrvObj &udmaDrvObj);
};

typedef std::vector<TISP::SuperNode::superNode *> superNodeVec;
/** @} */

/**
 *  @ingroup  SUPERNODE
 */

/**@{*/

/**
 *  \class i2d_to_o2d
 *  @brief Provides the supernode style when input data is transferred in 2D form and output is transferred out in 2D
 *         form.
 */

template <typename dTypeIn, typename dTypeOut> class i2d_to_o2d : public superNode {

 private:
   DmaUtilsAutoInc3d_TransferDim transferDimIn;  /*!< Structure to hold transfer properties of input DMA channel */
   DmaUtilsAutoInc3d_TransferDim transferDimOut; /*!< Structure to hold transfer properties of output DMA channel */

 public:
   /**
    * @brief Supernode constructor to create a 2D input to 2D output DMA supernode.
    * @param [in]  &_myOpVec    : Operator vector with list of nodes to be executed inside the supernode
    * @param [in]  *_pIn        : Input pointer in DDR space
    * @param [out] *_pOut       : Output pointer in DDR space
    * @param [in]  *_pInBlock   : Input pointer in L2 space
    * @param [out] *_pOutBlock  : Output pointer in L2 space
    * @param [in]  _width       : Width of 2D block in DDR
    * @param [in]  _height      : height of 2D block in DDR
    * @param [in]  _blockWidth  : Width of 2D block in L2
    * @param [in]  _blockHeight : Height of 2D block in L2
    * @param [in]  _inPitch     : Input pitch in DDR
    * @param [in]  _outPitch    : Output pitch in DDR
    * @param [in]  _numChannels : Total number of DMA channels to be used by the supernode (always 2)
    *
    */

   i2d_to_o2d(opVec    &_myOpVec,
              dTypeIn  *_pIn,
              dTypeOut *_pOut,
              dTypeIn  *_pInBlock,
              dTypeOut *_pOutBlock,
              size_t    _width,
              size_t    _height,
              size_t    _blockWidth,
              size_t    _blockHeight,
              size_t    _inPitch,
              size_t    _outPitch,
              size_t    _numChannels);

   void exec();
};

/** @} */

/**
 *  @ingroup  SUPERNODE
 */

/**@{*/

/**
 *  \class i2d_to_o2dTranspose
 *  @brief Provides the supernode style when input data is transferred in 2D form and output blocks are transferred in a
 *         transposed fashion in 2D form.
 */

template <typename dTypeIn, typename dTypeOut> class i2d_to_o2dTranspose : public superNode {
 private:
   DmaUtilsAutoInc3d_TransferDim transferDimIn;  /*!< Structure to hold transfer properties of input DMA channel */
   DmaUtilsAutoInc3d_TransferDim transferDimOut; /*!< Structure to hold transfer properties of output DMA channel */

 public:
   /**
    * @brief Supernode constructor to create a 2D input to 2D output (tranposed) DMA supernode.
    * @param [in]  &_myOpVec    : Operator vector with list of nodes to be executed inside the supernode
    * @param [in]  *_pIn        : Input pointer in DDR space
    * @param [out] *_pOut       : Output pointer in DDR space
    * @param [in]  *_pInBlock   : Input pointer in L2 space
    * @param [out] *_pOutBlock  : Output pointer in L2 space
    * @param [in]  _width       : Width of 2D block in DDR
    * @param [in]  _height      : height of 2D block in DDR
    * @param [in]  _blockWidth  : Width of 2D block in L2
    * @param [in]  _blockHeight : Height of 2D block in L2
    * @param [in]  _inPitch     : Input pitch in DDR
    * @param [in]  _outPitch    : Output pitch in DDR
    * @param [in]  _numChannels : Total number of DMA channels to be used by the supernode (always 2)
    *
    */

   i2d_to_o2dTranspose(opVec    &_myOpVec,
                       dTypeIn  *_pIn,
                       dTypeOut *_pOut,
                       dTypeIn  *_pInBlock,
                       dTypeOut *_pOutBlock,
                       size_t    _width,
                       size_t    _height,
                       size_t    _blockWidth,
                       size_t    _blockHeight,
                       size_t    _inPitch,
                       size_t    _outPitch,
                       size_t    _numChannels);

   void exec();
};

/** @} */

} // namespace SuperNode
} // namespace TISP
#endif
