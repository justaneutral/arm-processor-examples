#include "TISP_superNode.hpp"
#include "TISP_test.h"
#include "TISP_types.hpp"
#include "TISP_util.hpp"
#include <fftlib.h>
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

#define WIDTH (2048)
#define HEIGHT (2048)

#define BLOCK_WIDTH (1024)
#define BLOCK_HEIGHT (32)

#define TOTAL_DMA_CHANNELS (2)

#define NUM_REPS (1)

#define DDR_SIZE 64 * 1024 * 1024 // 64 MB

Udma_DrvHandle TISP::SuperNode::appDrvHandle;

#if defined(HOST_EMULATION)
#define RESTRICT
#else
#define RESTRICT restrict
#endif

#if !defined(HOST_EMULATION)
__attribute__((section(".ddrData"), aligned(128))) uint8_t ddrBuffer[DDR_SIZE];
#endif

void DSPLIB_sqr_exec_cn(float *RESTRICT pIn, float *RESTRICT pOut, size_t size)
{
   for (uint32_t i = 0; i < size; i++) {
      pOut[i] = pIn[i] * pIn[i];
   }
   return;
}

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

   uint32_t width  = WIDTH;
   uint32_t height = HEIGHT;

   uint32_t blockWidth  = BLOCK_WIDTH;
   uint32_t blockHeight = BLOCK_HEIGHT;

   float *input   = (float *) ddrBuffer;
   float *inputCn = input + width * height;
   float *output  = inputCn + width * height;

   float *refOut = output + width * height;

   uint32_t blockSize = blockHeight * blockWidth * sizeof(float);

   float *pInputBlock  = (float *) memalign(128, blockSize * 2);
   float *pCopyOutL2   = (float *) memalign(128, blockSize);
   float *pOutputBlock = (float *) memalign(128, blockSize * 2);

   for (uint32_t i = 0; i < width * height; i++) {
      input[i]   = (float) rand() / (float) (RAND_MAX / (2 * 8)) - 8;
      inputCn[i] = input[i];
   }

   printf("\n---------------------------------------------------");
   printf("\n Block Copy + Square Supernode Demo using TISP     ");
   printf("\n---------------------------------------------------\n");

   printf("\nCreating Graph:\n");
   printf("\nDDR-->L2-->blockCopy-->L2-->DSPLIB_sqr-->L2-->DDR\n");

   //! [block copy square supernode]
   auto k0 = new TISP::misc::blockCopy<float, TISP::DMA_IN>(pInputBlock, pCopyOutL2, blockWidth, blockHeight);
   auto k1 = new TISP::DSPLIB::sqr<float, TISP::DMA_OUT>(pCopyOutL2, pOutputBlock, blockHeight * blockWidth);

   TISP::opVec myOpVec;

   myOpVec.push_back(k0);
   myOpVec.push_back(k1);

   auto s0 = new TISP::SuperNode::i2d_to_o2d<float, float>(myOpVec, input, output, pInputBlock, pOutputBlock, width,
                                                           height, blockWidth, blockHeight, width * sizeof(float),
                                                           width * sizeof(float), (TOTAL_DMA_CHANNELS));

   printf("\nExecuting Graph\n");

#if (!HOST_EMULATION)
   uint64_t startTime, endTime;
   startTime = (__get_GTSC(1), __STSC);
#endif

   s0->exec();

#if (!HOST_EMULATION)
   endTime            = (__get_GTSC(1), __STSC);
   uint64_t totalTime = (endTime - startTime) / (NUM_REPS);
   printf("Total cycles to execute graph = %llu\n", totalTime);
   printf("\nPeformance: %.2f GB/sec\n", (sizeof(float) * WIDTH * HEIGHT) / ((float) totalTime));
#endif

   //! [block copy square supernode]

   DSPLIB_sqr_exec_cn(input, refOut, width * height);

   int32_t status = TI_compare_mem_2D_float((void *) output, (void *) refOut, 0, (double) powf(2, -2), width, height,
                                            width * sizeof(float), sizeof(float));

   if (status == TI_TEST_KERNEL_PASS) {
      printf("Opt and NatC match!  PASSED! \n");
   }
   else {
      printf("Opt and NatC DON'T match!  FAILED!!! \n");
   }

   printf("\nFreeing resources...\n");

   free(pInputBlock);
   free(pCopyOutL2);
   free(pOutputBlock);

   delete (k0);
   delete (k1);
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
