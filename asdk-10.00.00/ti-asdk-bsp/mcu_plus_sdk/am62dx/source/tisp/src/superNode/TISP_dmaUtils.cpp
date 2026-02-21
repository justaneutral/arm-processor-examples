#include "TISP_superNode_common.hpp"
#include <stdio.h>

using namespace TISP;

template <typename dType>
void dmaUtils<dType>::dmaAutoIncSetupXferPropIn2D(size_t                         width,
                                                  size_t                         height,
                                                  size_t                         blockWidth,
                                                  size_t                         blockHeight,
                                                  size_t                         inPitch,
                                                  size_t                         outPitch,
                                                  DmaUtilsAutoInc3d_TransferDim *transferDim)
{

   uint32_t pingPongOffsetIn = blockWidth * blockHeight * sizeof(dType);

   transferDim->sicnt0 = blockWidth * sizeof(dType);
   transferDim->sicnt1 = blockHeight;
   transferDim->sicnt2 = width / blockWidth;
   transferDim->sicnt3 = height / blockHeight;
   transferDim->sdim1  = inPitch;
   transferDim->sdim2  = blockWidth * sizeof(dType);
   transferDim->sdim3  = blockHeight * inPitch;

   transferDim->dicnt0 = blockWidth * sizeof(dType);
   transferDim->dicnt1 = blockHeight;
   transferDim->dicnt2 = 2;
   transferDim->dicnt3 = (transferDim->sicnt2 * transferDim->sicnt3 + 1) / 2;
   transferDim->ddim1  = blockWidth * sizeof(dType);
   transferDim->ddim2  = pingPongOffsetIn;
   transferDim->ddim3  = 0;

   return;
}

template <typename dType>
void dmaUtils<dType>::dmaAutoIncSetupXferPropOut2D(size_t                         width,
                                                   size_t                         height,
                                                   size_t                         blockWidth,
                                                   size_t                         blockHeight,
                                                   size_t                         inPitch,
                                                   size_t                         outPitch,
                                                   DmaUtilsAutoInc3d_TransferDim *transferDim)
{

   uint32_t pingPongOffset = blockWidth * blockHeight * sizeof(dType);

   transferDim->sicnt0 = blockWidth * sizeof(dType);
   transferDim->sicnt1 = blockHeight;
   transferDim->sicnt2 = 2;
   transferDim->sicnt3 = ((width / blockWidth) * (height / blockHeight) + 1) / 2;
   transferDim->sdim1  = blockWidth * sizeof(dType);
   transferDim->sdim2  = pingPongOffset;
   transferDim->sdim3  = 0;

   transferDim->dicnt0 = blockWidth * sizeof(dType);
   transferDim->dicnt1 = blockHeight;
   transferDim->dicnt2 = width / blockWidth;
   transferDim->dicnt3 = height / blockHeight;
   transferDim->ddim1  = outPitch;
   transferDim->ddim2  = blockWidth * sizeof(dType);
   transferDim->ddim3  = outPitch * blockHeight;

   return;
}

template <typename dType>
void dmaUtils<dType>::dmaAutoIncSetupXferPropOut2DTranspose(size_t                         width,
                                                            size_t                         height,
                                                            size_t                         blockWidth,
                                                            size_t                         blockHeight,
                                                            size_t                         inPitch,
                                                            size_t                         outPitch,
                                                            DmaUtilsAutoInc3d_TransferDim *transferDim)
{

   uint32_t pingPongOffset = blockWidth * blockHeight * sizeof(dType);

   transferDim->sicnt0 = blockHeight * sizeof(dType);
   transferDim->sicnt1 = blockWidth;
   transferDim->sicnt2 = 2;
   transferDim->sicnt3 = ((width / blockWidth) * (height / blockHeight) + 1) / 2;
   transferDim->sdim1  = blockHeight * sizeof(dType);
   transferDim->sdim2  = pingPongOffset;
   transferDim->sdim3  = 0;

   transferDim->dicnt0 = blockHeight * sizeof(dType);
   transferDim->dicnt1 = blockWidth;
   transferDim->dicnt2 = width / blockWidth;
   transferDim->dicnt3 = height / blockHeight;
   transferDim->ddim1  = outPitch;
   transferDim->ddim2  = outPitch * blockWidth;
   transferDim->ddim3  = blockHeight * sizeof(dType);

   return;
}

template <typename dType>
uint32_t dmaUtils<dType>::init(uint8_t *dmaUtilsContext, int32_t numChannels, Udma_DrvHandle drvHandle)
{

   int32_t                     retVal = UDMA_SOK;
   DmaUtilsAutoInc3d_InitParam initParams;

   DmaUtilsAutoInc3d_ChannelInitParam chInitParams[numChannels];

   initParams.contextSize     = DmaUtilsAutoInc3d_getContextSize(numChannels);
   initParams.numChannels     = numChannels;
   initParams.traceLogLevel   = 1;
   initParams.udmaDrvHandle   = drvHandle;
   initParams.DmaUtilsVprintf = vprintf;

   for (int32_t ch = 0; ch < numChannels; ch++) {
      chInitParams[ch].dmaQueNo = 0;
      chInitParams[ch].druOwner = DMAUTILSAUTOINC3D_DRUOWNER_DIRECT_TR;
   }

   retVal = DmaUtilsAutoInc3d_init(dmaUtilsContext, &initParams, chInitParams);

   return retVal;
}

template <typename dType>
uint32_t dmaUtils<dType>::configure_channel(uint8_t                       *dmaUtilsContext,
                                            int32_t                        ch,
                                            uint8_t                       *pTrMemChs[],
                                            uint8_t                       *srcPtr,
                                            uint8_t                       *dstPtr,
                                            uint32_t                       syncType,
                                            DmaUtilsAutoInc3d_TransferDim *transferDim)
{
   uint32_t                         retVal = UDMA_SOK;
   DmaUtilsAutoInc3d_TrPrepareParam trPrepParam;
   DmaUtilsAutoInc3d_TransferProp   transferProp;

   trPrepParam.channelId = ch;
   trPrepParam.numTRs    = 1;
   trPrepParam.trMem     = pTrMemChs[ch];
   trPrepParam.trMemSize = DmaUtilsAutoInc3d_getTrMemReq(1);

   transferProp.circProp.circDir       = DMAUTILSAUTOINC3D_CIRCDIR_SRC;
   transferProp.circProp.circSize1     = 0;
   transferProp.circProp.circSize2     = 0;
   transferProp.circProp.addrModeIcnt0 = DMAUTILSAUTOINC3D_ADDR_LINEAR;
   transferProp.circProp.addrModeIcnt1 = DMAUTILSAUTOINC3D_ADDR_LINEAR;
   transferProp.circProp.addrModeIcnt2 = DMAUTILSAUTOINC3D_ADDR_LINEAR;
   transferProp.circProp.addrModeIcnt3 = DMAUTILSAUTOINC3D_ADDR_LINEAR;

   transferProp.syncType          = syncType;
   transferProp.dmaDfmt           = DMAUTILSAUTOINC3D_DFMT_NONE;
   transferProp.ioPointers.srcPtr = srcPtr;
   transferProp.ioPointers.dstPtr = dstPtr;
   transferProp.transferDim       = *transferDim;

   retVal = DmaUtilsAutoInc3d_prepareTr(&trPrepParam, &transferProp);

   if (UDMA_SOK != retVal) {
      printf("[Error] dma configure failed!!\n");
   }
   else {

      retVal = DmaUtilsAutoInc3d_configure(dmaUtilsContext, ch, pTrMemChs[ch], 1);
   }

   return retVal;
}

template <typename dType>
uint32_t dmaUtils<dType>::trigger(uint8_t *dmaUtilsContext, int32_t chId[], size_t numChannels)
{

   for (size_t ch = 0; ch < numChannels - 1; ch++) {
      DmaUtilsAutoInc3d_trigger(dmaUtilsContext, chId[ch]);
   }

   return DmaUtilsAutoInc3d_trigger(dmaUtilsContext, chId[numChannels - 1]);
}

template <typename dType> void dmaUtils<dType>::wait(uint8_t *dmaUtilsContext, int32_t chId[], size_t numChannels)
{
   for (size_t ch = 0; ch < numChannels; ch++) {

      DmaUtilsAutoInc3d_wait(dmaUtilsContext, chId[ch]);
   }

   return;
}

template class TISP::dmaUtils<float>;
template class TISP::dmaUtils<double>;
