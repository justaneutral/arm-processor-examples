#ifndef TISP_DMAUTILS_HPP
#define TISP_DMAUTILS_HPP

#if __C7X_VEC_SIZE_BITS__ == 512
#include <ti/drv/udma/dmautils/dmautils.h>
#include <ti/drv/udma/udma.h>
#else
#include <drivers/dmautils/dmautils.h>
#include <drivers/dmautils/udma_standalone/udma.h>
#endif

#if __C7X_VEC_SIZE_BITS__ == 512
#define DRU_LOCAL_EVENT_START_DEFAULT (192U) // Default for J721E and J721S2
#define DRU_LOCAL_EVENT_START_J784S4 (664U)
#endif

namespace TISP {

static void testDmaAutoIncPrintf(const char *str)
{
#ifdef ENABLE_PRINT
   print(str);
#endif
}

static uint64_t testDmaAutoIncVirtToPhyFxn(const void *virtAddr, uint32_t chNum, void *appData)
{
   return (uint64_t) virtAddr;
}

template <typename dType> class dmaUtils {

 public:
   static void dmaAutoIncSetupXferPropIn2D(size_t                         width,
                                           size_t                         height,
                                           size_t                         blockWidth,
                                           size_t                         blockHeight,
                                           size_t                         inPitch,
                                           size_t                         outPitch,
                                           DmaUtilsAutoInc3d_TransferDim *transferDim);
   static void dmaAutoIncSetupXferPropOut2D(size_t                         width,
                                            size_t                         height,
                                            size_t                         blockWidth,
                                            size_t                         blockHeight,
                                            size_t                         inPitch,
                                            size_t                         outPitch,
                                            DmaUtilsAutoInc3d_TransferDim *transferDim);

   static void dmaAutoIncSetupXferPropOut2DTranspose(size_t                         width,
                                                     size_t                         height,
                                                     size_t                         blockWidth,
                                                     size_t                         blockHeight,
                                                     size_t                         inPitch,
                                                     size_t                         outPitch,
                                                     DmaUtilsAutoInc3d_TransferDim *transferDim);

   static uint32_t init(uint8_t *dmaUtilsContext, int32_t numChannels, Udma_DrvHandle drvHandle);

   static uint32_t configure_channel(uint8_t                       *dmaUtilsContext,
                                     int32_t                        ch,
                                     uint8_t                       *pTrMemChs[],
                                     uint8_t                       *srcPtr,
                                     uint8_t                       *dstPtr,
                                     uint32_t                       syncType,
                                     DmaUtilsAutoInc3d_TransferDim *transferDim);

   static uint32_t trigger(uint8_t *dmaUtilsContext, int32_t chId[], size_t numChannels);
   static void     wait(uint8_t *dmaUtilsContext, int32_t chId[], size_t numChannels);

#if !defined(HOST_EMULATION) && __C7X_VEC_SIZE_BITS__ == 512
   static void    appC7xClecInitDru(void);
   static int32_t test_sciclientDmscGetVersion(char *version_str, uint32_t version_str_size);
#endif
};
} // namespace TISP
#endif
