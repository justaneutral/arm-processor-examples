#include "TISP_test.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tisp.hpp>

#if defined(HOST_EMULATION)
#include <malloc.h>
#elif __C7X_VEC_SIZE_BITS__ == 512
#include <ti/csl/arch/c7x/cslr_C7X_CPU.h>
#include <ti/csl/csl_clec.h>
#else
#include <drivers/hw_include/csl_clec.h>
#endif

#include <vector>

#define NUM_POINTS (512)
#define NUM_CHANNELS (512)

#define BLOCK_WIDTH (NUM_POINTS << 1)
#define BLOCK_HEIGHT (16)

#define TOTAL_DMA_CHANNELS (2)

#define NUM_REPS (1)

#define DDR_SIZE 64 * 1024 * 1024 // 64 MB

// Below cycle count assume flat memory model, i.e, no memory-related stalls
#define FFTLIB_FFT1DBATCHED_16X512_CYCLES (12904)
#define FFTLIB_FFT1DATCHED_512X512_CYCLES (FFTLIB_FFT1DBATCHED_16X512_CYCLES) * (512 / 16)
#define ESITMATED_TOTAL_CYCLE_COUNT ((FFTLIB_FFT1DATCHED_512X512_CYCLES))

Udma_DrvHandle TISP::SuperNode::appDrvHandle;

#if !defined(HOST_EMULATION)
__attribute__((section(".ddrData"), aligned(128))) uint8_t ddrBuffer[DDR_SIZE];
#endif

void TISP_fft1dBatched_test_cn(float *pX, float *pW, float *pY, size_t numPoints, size_t numChannels);

#ifdef __cplusplus
extern "C" {
#endif
int32_t test_main()
{

#ifdef HOST_EMULATION
#if defined(_MSC_VER)
   /* pIntMmeBase = (uint8_t *) _aligned_malloc(L2SRAM_SIZE, L2SRAM_SIZE); */
#else
   /* pIntMmeBase     = (uint8_t *) memalign(128, L2SRAM_SIZE); */
#endif
#elif __C7X_VEC_SIZE_BITS__ == 512
   int32_t                retVal = 0;
   Sciclient_ConfigPrms_t sciClientCfg;
   Sciclient_configPrmsInit(&sciClientCfg);
   retVal = Sciclient_init(&sciClientCfg);
   if (retVal != 0) {
      printf("Sciclient Init Failed \n");
   }

   TISP::test_sciclientDmscGetVersion(NULL, 0);
   TISP::appC7xClecInitDru();
#endif

   struct Udma_DrvObj appUdmaDrvObj;

   TISP::SuperNode::appDrvHandle = TISP::SuperNode::superNode::setUdmaDrvHandle(appUdmaDrvObj);

#if defined(HOST_EMULATION)
   uint8_t *ddrBuffer = (uint8_t *) memalign(128, DDR_SIZE);
#endif
   /* printf("\nTISP demo with FFTLIB_1dBatched\n"); */

   uint32_t numPoints   = NUM_POINTS;
   uint32_t numChannels = NUM_CHANNELS;

   uint32_t blockWidth  = BLOCK_WIDTH;
   uint32_t blockHeight = BLOCK_HEIGHT;

   float *input   = (FFTLIB_F32 *) ddrBuffer;
   float *inputCn = input + (numPoints << 1) * numChannels;
   float *output  = inputCn + (numPoints << 1) * numChannels;

   float *refOut = output + (numPoints << 1) * numChannels;
   float *pWCn   = refOut + numPoints * 2 * numChannels;

   uint32_t blockSize = blockHeight * blockWidth * sizeof(float);

   float *pInputBlock  = (float *) memalign(128, blockSize * 2);
   float *pOutputBlock = (float *) memalign(128, blockSize * 2);
   float *pTw          = (float *) memalign(128, (numPoints << 1) * sizeof(float));

   /* float *pYCn = pXCn + (numPoints << 1) * numChannels; */

   for (uint32_t i = 0; i < numChannels * numPoints * 2; i++) {
      input[i]   = (float) rand() / (float) (RAND_MAX / (2 * 8)) - 8;
      inputCn[i] = input[i];
   }

   printf("\n------------------------------------------------------");
   printf("\n        FFT 1D Batched Demo using TISP                 ");
   printf("\n------------------------------------------------------\n");

   printf("\nCreating Graph:\n");
   printf("\nDDR-->L2-->fft1dBatched-->L2-->DDR\n");

   auto k0 = new TISP::FFTLIB::fft1dBatched<float, TISP::DMA_IN_OUT>(pInputBlock, pOutputBlock, numPoints, blockHeight);

   TISP::opVec myOpVec;

   myOpVec.push_back(k0);

   auto s0 = new TISP::SuperNode::i2d_to_o2d<float, float>(
       myOpVec, input, output, pInputBlock, pOutputBlock, numPoints << 1, numChannels, blockWidth, blockHeight,
       blockWidth * sizeof(float), blockWidth * sizeof(float), (TOTAL_DMA_CHANNELS));

   /* k0->exec(); */
   /* k1->exec(); */
   TISP::SuperNode::superNodeVec mySuperNodeVec;

   mySuperNodeVec.push_back(s0);

   printf("\nExecuting graph\n");

#if (!HOST_EMULATION)
   uint64_t startTime, endTime;
   startTime = (__get_GTSC(1), __STSC);
#endif

   /* for (uint32_t i = 0; i < NUM_REPS; i++) { */
   /*    s0->exec(); */
   /* } */
   TISP::execute::graph(mySuperNodeVec);

#if (!HOST_EMULATION)
   endTime            = (__get_GTSC(1), __STSC);
   uint64_t totalTime = (endTime - startTime) / (NUM_REPS);
   printf("Total cycles to execute graph = %llu\n", totalTime);

   printf("\nTheoritical cycle count (assuming flat memory w/ no stalls): %d\n", (ESITMATED_TOTAL_CYCLE_COUNT));
   printf("\nObeserved/Theoritical: %.2f\n", (((float) totalTime) / (ESITMATED_TOTAL_CYCLE_COUNT)));

   printf("\nPeformance of 1D Bathced FFT: %.2f GB/sec\n", (8 * NUM_POINTS * NUM_CHANNELS) / ((float) totalTime));
#endif

   TISP_fft1dBatched_test_cn(inputCn, pWCn, refOut, numPoints, numChannels);

   int32_t status = TI_compare_mem_2D_float((void *) output, (void *) refOut, 0, (double) powf(2, -2), numPoints << 1,
                                            numChannels, (numPoints << 1) * sizeof(float), sizeof(float));

   if (status == TI_TEST_KERNEL_PASS) {
      printf("Opt and NatC match!  PASSED! \n");
   }
   else {
      printf("Opt and NatC DON'T match!  FAILED!!! \n");
   }

   printf("\nFreeing resources...\n");

   free(pInputBlock);
   free(pOutputBlock);
   free(pTw);

   delete (k0);
   /* delete (k1); */
   delete (s0);

   printf("\nDemo complete!");

#ifdef HOST_EMULATION
#if defined(_MSC_VER)
   _aligned_free(ddrBuffer);
/* _aligned_free(pIntMmeBase); */
#else
   free(ddrBuffer);
/* free(pIntMmeBase); */
#endif
#endif
   return 1;
}
#ifdef __cplusplus
}
#endif
