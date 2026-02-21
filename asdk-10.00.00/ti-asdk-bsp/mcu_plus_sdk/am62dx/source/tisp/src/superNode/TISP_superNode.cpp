#include "TISP_superNode_common.hpp"
#include <TISP_util.hpp>
#include <stdio.h>

using namespace TISP;
using namespace TISP::SuperNode;

Udma_DrvHandle superNode::setUdmaDrvHandle(Udma_DrvObj &udmaDrvObj)
{
   Udma_InitPrms initPrms;

   uint32_t instId;
   uint32_t retVal;

   Udma_DrvHandle drvHandle = &udmaDrvObj;

   instId = UDMA_INST_ID_MAIN_0;
   UdmaInitPrms_init(instId, &initPrms);
   initPrms.printFxn     = &testDmaAutoIncPrintf;
   initPrms.virtToPhyFxn = &testDmaAutoIncVirtToPhyFxn;
#if __C7X_VEC_SIZE_BITS__ == 512
   initPrms.skipGlobalEventReg = 1;
#endif
   retVal = Udma_init(drvHandle, &initPrms);

   if (UDMA_SOK != retVal) {
      printf("[Error] UDMA init failed!!\n");
   }
   return drvHandle;
}

superNode::superNode(opVec &_myOpVec, size_t _numChannels, size_t _numInChannels, size_t _numOutChannels)
    : myOpVec(_myOpVec), numChannels(_numChannels), numInChannels(_numInChannels), numOutChannels(_numOutChannels)

{
   dmaUtilsContext = (uint8_t *) memalign(128, DmaUtilsAutoInc3d_getContextSize(numChannels));

   for (size_t ch = 0; ch < numChannels; ch++) {
      pTrMemCh[ch] = (uint8_t *) memalign(128, DmaUtilsAutoInc3d_getTrMemReq(1));
   }

   uint32_t retVal;

   // float is dummy as its not used by init function
   retVal = dmaUtils<float>::init(dmaUtilsContext, numChannels, appDrvHandle);

   if (retVal != UDMA_SOK) {
      printf("[Error] UDMA init failed!!\n");
   }
}

void superNode::exec()
{

   uint32_t blockIdx     = 0;
   uint32_t firstTrigger = 0;

   // DMA trigger for pipe-up, out transfer is dummy and handled inside DMA
   // utility; trigger and wait are datatype independent so using float

   dmaUtils<float>::trigger(dmaUtilsContext, chIdIn, numInChannels);

   // Wait for previous transfer of in
   dmaUtils<float>::wait(dmaUtilsContext, chIdIn, numInChannels);

   while (1) {

      if (firstTrigger != 0) {

         blockIdx = dmaUtils<float>::trigger(dmaUtilsContext, chIdOut, numOutChannels);
      }

      // DMA trigger for next in buffer
      if (blockIdx != 1) {
         dmaUtils<float>::trigger(dmaUtilsContext, chIdIn, numInChannels);
      }

      execute::graph(myOpVec);

      if (blockIdx != 1) {
         dmaUtils<float>::wait(dmaUtilsContext, chIdIn, numInChannels);
      }

      // Wait for previous transfer out
      if (firstTrigger != 0) {
         dmaUtils<float>::wait(dmaUtilsContext, chIdOut, numOutChannels);
      }
      else {
         firstTrigger = 1;
      }

      if (blockIdx == 1)
         break;
   }

   dmaUtils<float>::trigger(dmaUtilsContext, chIdOut, numOutChannels);

   // Need to wait for last out transfer
   dmaUtils<float>::wait(dmaUtilsContext, chIdOut, numOutChannels);

   return;
}
