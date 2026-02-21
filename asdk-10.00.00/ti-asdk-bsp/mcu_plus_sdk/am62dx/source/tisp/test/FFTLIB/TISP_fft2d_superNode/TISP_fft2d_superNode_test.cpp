#include "TISP_blockCopy.hpp"
#include "TISP_superNode.hpp"
#include "TISP_test.h"
#include <cstdint>
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

#define NUM_REPS (1) // has to be 1 for now!

#define DDR_SIZE 64 * 1024 * 1024 // 64 MB

// Below cycle count assume flat memory model, i.e, no memory-related stalls
#define FFTLIB_FFT1DBATCHED_16X512_CYCLES (12904)
#define FFTLIB_FFT1DATCHED_512X512_CYCLES (FFTLIB_FFT1DBATCHED_16X512_CYCLES) * (512 / 16)
#define DSPLIB_MAT_TRANS_CYCLES ((NUM_POINTS * NUM_CHANNELS) / 4)

#define ESITMATED_TOTAL_CYCLE_COUNT ((2 * FFTLIB_FFT1DATCHED_512X512_CYCLES) + DSPLIB_MAT_TRANS_CYCLES)

/* size_t TISP::dmaChOffset::globalChOffset = 0; */
Udma_DrvHandle TISP::SuperNode::appDrvHandle;

#if !defined(HOST_EMULATION)
__attribute__((section(".ddrData"), aligned(128))) uint8_t ddrBuffer[DDR_SIZE];
#endif

void TISP_fft2d_test_cn(float *pX, float *pW, float *pTemp, float *pY, size_t numPoints, size_t numChannels);

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

   uint32_t numPoints   = NUM_POINTS;
   uint32_t numChannels = NUM_CHANNELS;

   uint32_t blockWidth  = BLOCK_WIDTH;
   uint32_t blockHeight = BLOCK_HEIGHT;

   /* size_t dataMemSize = (numPoints * 2) * (numChannels) * sizeof(float); */

   float *input   = (FFTLIB_F32 *) ddrBuffer;
   float *inputCn = input + (numPoints << 1) * numChannels;
   float *output  = inputCn + (numPoints << 1) * numChannels;

   float *refOut  = output + (numPoints << 1) * numChannels;
   float *tempOut = refOut + (numPoints << 1) * numChannels;
   float *pWCn    = tempOut + numPoints * 2 * numChannels;

   uint32_t blockSize = blockHeight * blockWidth * sizeof(float);

   float *pInputBlock  = (float *) memalign(128, blockSize * 2);
   float *pTempBlock   = (float *) memalign(128, blockSize * 2);
   float *pOutputBlock = (float *) memalign(128, blockSize * 2);
   float *pTw          = (float *) memalign(128, (numPoints << 1) * sizeof(float));

   /* float *pYCn = pXCn + (numPoints << 1) * numChannels; */

   for (uint32_t i = 0; i < numChannels * numPoints * 2; i++) {
      input[i]   = (float) rand() / (float) (RAND_MAX / (2 * 8)) - 8;
      inputCn[i] = input[i];
   }

   printf("\n------------------------------------------------");
   printf("\n         2D FFT Demo using TISP                 ");
   printf("\n------------------------------------------------\n");

   printf("\nCreating Graph:\n");
   printf("\nDDR-->L2-->1DBatchedFFT-->L2-->MatrixTranspose-->L2-->DDR-->L2-->1DBatchedFFT-->L2-->DDR\n");

   //! [2D FFT Example]
   auto k0 = new TISP::FFTLIB::fft1dBatched<float, TISP::DMA_IN>(pInputBlock, pTempBlock, numPoints, blockHeight);

   auto k1 = new TISP::DSPLIB::matTrans<double, TISP::DMA_OUT>(
       (double *) pTempBlock, (double *) pOutputBlock, blockWidth >> 1, blockHeight, blockWidth * sizeof(double) >> 1,
       blockHeight * sizeof(double));

   TISP::opVec myOpVec0;

   myOpVec0.push_back(k0);
   myOpVec0.push_back(k1);

   auto s0 = new TISP::SuperNode::i2d_to_o2dTranspose<double, double>(
       myOpVec0, (double *) input, (double *) output, (double *) pInputBlock, (double *) pOutputBlock, numPoints,
       numChannels, blockWidth >> 1, blockHeight, (numPoints) * sizeof(double), numChannels * sizeof(double),
       (TOTAL_DMA_CHANNELS));

   auto k2 =
       new TISP::FFTLIB::fft1dBatched<float, TISP::DMA_IN_OUT>(pInputBlock, pOutputBlock, numChannels, blockHeight);

   TISP::opVec myOpVec1;

   myOpVec1.push_back(k2);

   auto s1 = new TISP::SuperNode::i2d_to_o2d<float, float>(
       myOpVec1, output, input, pInputBlock, pOutputBlock, numChannels << 1, numPoints, numChannels << 1, blockHeight,
       (numChannels << 1) * sizeof(float), (numChannels << 1) * sizeof(float), (TOTAL_DMA_CHANNELS));

   TISP::SuperNode::superNodeVec mySuperNodeVec;

   mySuperNodeVec.push_back(s0);
   mySuperNodeVec.push_back(s1);

   printf("\nExecuting Graph\n");

#if (!HOST_EMULATION)
   uint64_t startTime, endTime;
   startTime = (__get_GTSC(1), __STSC);
#endif

   TISP::execute::graph(mySuperNodeVec);

   //! [2D FFT Example]

#if (!HOST_EMULATION)
   endTime            = (__get_GTSC(1), __STSC);
   uint64_t totalTime = (endTime - startTime) / (NUM_REPS);
   printf("Total cycles to execute graph = %llu\n", totalTime);

   printf("\nTheoritical cycle count (assuming flat memory w/ no stalls): %d\n", (ESITMATED_TOTAL_CYCLE_COUNT));
   printf("\nObeserved/Theoritical: %.2f\n", (((float) totalTime) / (ESITMATED_TOTAL_CYCLE_COUNT)));
   printf("\nMemory performance of 2D FFT: %.2f GB/sec\n", (8 * NUM_POINTS * NUM_CHANNELS * 2) / ((float) totalTime));
#endif

   TISP_fft2d_test_cn(inputCn, pWCn, tempOut, refOut, numPoints, numChannels);

   int32_t status = TI_compare_mem_2D_float((void *) input, (void *) refOut, 0, (double) powf(2, -2), numChannels << 1,
                                            numPoints, (numChannels << 1) * sizeof(float), sizeof(float));

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
   free(pTempBlock);

   delete (k0);
   delete (k1);
   delete (k2);
   delete (s0);
   delete (s1);

   printf("\nDemo complete!");

#ifdef HOST_EMULATION
#if defined(_MSC_VER)
   _aligned_free(ddrBuffer);
#else
   free(ddrBuffer);
#endif
#endif
   return 1;
}
#ifdef __cplusplus
}
#endif
