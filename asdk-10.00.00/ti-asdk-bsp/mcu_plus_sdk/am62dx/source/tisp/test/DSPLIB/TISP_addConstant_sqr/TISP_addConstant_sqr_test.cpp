
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

#define SIZE (65536)
#define NUM_REPS (1)

#define DDR_SIZE 64 * 1024 * 1024 // 64 MB

#define DSPLIB_ADDCONSTANT_CYCLES (SIZE / 8)
#define DSPLIB_SQR_CYCLES (SIZE / 8)
#define ESITMATED_TOTAL_CYCLE_COUNT ((DSPLIB_ADDCONSTANT_CYCLES) + DSPLIB_SQR_CYCLES)

#if !defined(HOST_EMULATION)
__attribute__((section(".ddrData"), aligned(128))) uint8_t ddrBuffer[DDR_SIZE];
#endif

void TISP_addConstant_sqr_test_cn(float *pIn0, float *pIn1, float *pOut0, float *pOut1, size_t size);

#ifdef __cplusplus
extern "C" {
#endif
int32_t test_main()
{

#if defined(HOST_EMULATION)
   uint8_t *ddrBuffer = (uint8_t *) memalign(128, DDR_SIZE);
#endif
   /* printf("\nTISP demo with DSPLIB_addConstant + DSPLIB_sqr\n"); */

   size_t dataMemSize = (SIZE) * sizeof(float);

   /* float in0 = 3.14; */

   float *pIn0 = (float *) memalign(128, sizeof(float));
   *pIn0       = 3.14;

   float *pIn1  = (float *) memalign(128, dataMemSize);
   float *pOut0 = (float *) memalign(128, dataMemSize);
   float *pOut1 = (float *) memalign(128, dataMemSize);

   float *pIn1Cn  = (float *) ddrBuffer;
   float *pOut0Cn = pIn1Cn + (SIZE);
   float *pOut1Cn = pOut0Cn + (SIZE);

   for (uint32_t i = 0; i < (SIZE); i++) {
      pIn1[i]   = (float) rand() / (float) (RAND_MAX / (2 * 8)) - 8;
      pIn1Cn[i] = pIn1[i];
   }

   printf("\n-------------------------------------------------");
   printf("\n  DSPLIB_addConstant + DSPLIBsqr Demo using TISP");
   printf("\n-------------------------------------------------\n");

   printf("\nCreating Graph:\n");
   printf("\nL2-->DSPLIB_addConstant-->DSPLIB_sqr-->L2\n");

   auto k0 = new TISP::DSPLIB::addConstant<float, TISP::NO_DMA>(pIn0, pIn1, pOut0, SIZE);

   auto k1 = new TISP::DSPLIB::sqr<float, TISP::NO_DMA>(pOut0, pOut1, SIZE);

   TISP::opVec myOpVec;
   myOpVec.push_back(k0);
   myOpVec.push_back(k1);

#if defined(LOKI_BUILD)
   /* printf("LOKI_BUILD is defined\n"); */
#endif

   printf("\nExecuting graph\n");

#if (!HOST_EMULATION)
   uint64_t startTime, endTime;
   startTime = (__get_GTSC(1), __STSC);
#endif

   for (size_t i = 0; i < (NUM_REPS); i++) {
      TISP::execute::graph(myOpVec);
   }

#if (!HOST_EMULATION)
   endTime            = (__get_GTSC(1), __STSC);
   uint64_t totalTime = (endTime - startTime) / (NUM_REPS);
   printf("Total cycles to execute graph = %llu\n", totalTime);
   printf("\nTheoritical cycle count (assuming flat memory w/ no stalls): %d\n", (ESITMATED_TOTAL_CYCLE_COUNT));
   printf("\nObeserved/Theoritical: %.2f\n", (((float) totalTime) / (ESITMATED_TOTAL_CYCLE_COUNT)));
#endif

   TISP_addConstant_sqr_test_cn(pIn0, pIn1Cn, pOut0Cn, pOut1Cn, SIZE);

   int32_t status =
       TI_compare_mem_2D_float((void *) pOut1, (void *) pOut1Cn, 0, (double) powf(2, -2), SIZE, 1, 1, sizeof(float));

   if (status == TI_TEST_KERNEL_PASS) {
      printf("Opt and NatC match!  PASSED! \n");
   }
   else {
      printf("Opt and NatC DON'T match!  FAILED!!! \n");
   }

   printf("\nFreeing resources...\n");

   free(pIn1);
   free(pOut0);
   free(pOut1);
   delete (k0);
   delete (k1);

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
