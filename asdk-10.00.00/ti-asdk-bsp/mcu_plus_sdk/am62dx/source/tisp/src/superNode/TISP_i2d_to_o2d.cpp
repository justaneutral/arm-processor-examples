#include "TISP_superNode_common.hpp"
#include <stdio.h>

using namespace TISP;
using namespace TISP::SuperNode;

#define NUM_IN_CHANNELS (1)
#define NUM_OUT_CHANNELS (1)

//! [supernode example]

template <typename dTypeIn, typename dTypeOut>
i2d_to_o2d<dTypeIn, dTypeOut>::i2d_to_o2d(opVec    &_myOpVec,
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
                                          size_t    _numChannels)
    : superNode(_myOpVec, _numChannels, NUM_IN_CHANNELS, NUM_OUT_CHANNELS)

{

   int32_t retVal = UDMA_SOK;

   uint32_t transferSize = DMAUTILSAUTOINC3D_SYNC_2D;

   /***********************/
   /* Configure input DMA */
   /***********************/

   // set transfer dimenssion structure
   dmaUtils<dTypeIn>::dmaAutoIncSetupXferPropIn2D(_width, _height, _blockWidth, _blockHeight, _inPitch, _outPitch,
                                                  &transferDimIn);
   // assign channel number
   chIdIn[0] = 0;
   /* chIdIn[0] = dmaChOffset::globalChOffset; */
   /* dmaChOffset::globalChOffset += 1; */

   // configure channel
   retVal = dmaUtils<dTypeIn>::configure_channel(dmaUtilsContext, chIdIn[0], pTrMemCh, (uint8_t *) _pIn,
                                                 (uint8_t *) _pInBlock, transferSize, &transferDimIn);

   if (retVal != UDMA_SOK) {
      printf("[Error] UDMA init failed!!\n");
   }

   /***********************/
   /* Configure output DMA */
   /***********************/

   // set transfer dimenssion structure
   dmaUtils<dTypeOut>::dmaAutoIncSetupXferPropOut2D(_width, _height, _blockWidth, _blockHeight, _inPitch, _outPitch,
                                                    &transferDimOut);
   // assign channel number
   chIdOut[0] = 1;
   /* chIdOut[0] = dmaChOffset::globalChOffset; */
   /* dmaChOffset::globalChOffset += 1; */

   // configure channel
   retVal = dmaUtils<dTypeOut>::configure_channel(dmaUtilsContext, chIdOut[0], pTrMemCh, (uint8_t *) _pOutBlock,
                                                  (uint8_t *) _pOut, transferSize, &transferDimOut);

   if (retVal != UDMA_SOK) {
      printf("[Error] UDMA init failed!!\n");
   }
}
//! [supernode example]

template <typename dTypeIn, typename dTypeOut> void i2d_to_o2d<dTypeIn, dTypeOut>::exec() { superNode::exec(); }

template class TISP::SuperNode::i2d_to_o2d<float, float>;
