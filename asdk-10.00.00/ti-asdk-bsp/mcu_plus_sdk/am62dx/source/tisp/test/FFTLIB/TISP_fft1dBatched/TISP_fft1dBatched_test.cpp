#include "TISP_test.h"
#include "TISP_types.hpp"
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

#define NUM_POINTS (512)
#define NUM_CHANNELS (32)

#define NUM_REPS (1)

#define DDR_SIZE 64 * 1024 * 1024 // 64 MB

#define FFTLIB_FFT1DBATCHED_32X512_CYCLES (25592)
#define DSPLIB_SQR_CYCLES ((NUM_POINTS * NUM_CHANNELS) / 4)
#define ESITMATED_TOTAL_CYCLE_COUNT ((FFTLIB_FFT1DBATCHED_32X512_CYCLES) + DSPLIB_SQR_CYCLES)

#if !defined(HOST_EMULATION)
__attribute__((section(".ddrData"), aligned(128))) uint8_t ddrBuffer[DDR_SIZE];
#endif

void TISP_1dBatched_test_cn(float *pX, float *pW, float *pY0, float *pY1, size_t numPoints, size_t numChannels);

#ifdef __cplusplus
extern "C" {
#endif
int32_t test_main()
{

#if defined(HOST_EMULATION)
   uint8_t *ddrBuffer = (uint8_t *) memalign(128, DDR_SIZE);
#endif
   /* printf("\nTISP demo with FFTLIB_1dBatched\n"); */

   uint32_t numPoints   = NUM_POINTS;
   uint32_t numChannels = NUM_CHANNELS;

   size_t dataMemSize = (numPoints * 2) * (numChannels) * sizeof(float);

   float *pX0 = (FFTLIB_F32 *) memalign(128, dataMemSize);
   float *pY0 = (FFTLIB_F32 *) memalign(128, dataMemSize);
   float *pY1 = (FFTLIB_F32 *) memalign(128, dataMemSize);

   float *pXCn  = (float *) ddrBuffer;
   float *pY0Cn = pXCn + numPoints * 2 * numChannels;
   float *pY1Cn = pY0Cn + numPoints * 2 * numChannels;
   float *pWCn  = pY1Cn + numPoints * 2 * numChannels;

   /* float *pYCn = pXCn + (numPoints << 1) * numChannels; */

   for (uint32_t i = 0; i < numChannels * numPoints * 2; i++) {
      pX0[i]  = (float) rand() / (float) (RAND_MAX / (2 * 8)) - 8;
      pXCn[i] = pX0[i];
   }

   printf("\n-------------------------------------------------");
   printf("\n        FFT 1D Batched + Sqr Demo using TISP     ");
   printf("\n-------------------------------------------------\n");

   printf("\nCreating Graph:\n");
   printf("\nL2-->fft1dBatched-->DSPLIB_sqr-->L2\n");

   //! [connecting nodes example]
   auto k0 = new TISP::FFTLIB::fft1dBatched<float, TISP::NO_DMA>(pX0, pY0, numPoints, numChannels);

   auto k1 = new TISP::DSPLIB::sqr<float, TISP::NO_DMA>(pY0, pY1, 2 * numPoints * numChannels);

   TISP::opVec myOpVec;

   myOpVec.push_back(k0);
   myOpVec.push_back(k1);

   /* k0->exec(); */
   /* k1->exec(); */

   printf("\nExecuting graph\n");

#if (!HOST_EMULATION)
   uint64_t startTime, endTime;
   startTime = (__get_GTSC(1), __STSC);
#endif

   for (int32_t i = 0; i < (NUM_REPS); i++) {
      TISP::execute::graph(myOpVec);
   }

#if (!HOST_EMULATION)
   endTime            = (__get_GTSC(1), __STSC);
   uint64_t totalTime = (endTime - startTime) / (NUM_REPS);
   printf("Total cycles to execute graph = %llu\n", totalTime);
   printf("\nTheoritical cycle count (assuming flat memory w/ no stalls): %d\n", (ESITMATED_TOTAL_CYCLE_COUNT));
   printf("\nObeserved/Theoritical: %.2f\n", (((float) totalTime) / (ESITMATED_TOTAL_CYCLE_COUNT)));
#endif

   //! [connecting nodes example]

   TISP_1dBatched_test_cn(pXCn, pWCn, pY0Cn, pY1Cn, numPoints, numChannels);

   int32_t status = TI_compare_mem_2D_float((void *) pY1, (void *) pY1Cn, 0, (double) powf(2, -2), numPoints << 1,
                                            numChannels, (numPoints << 1) * sizeof(float), sizeof(float));

   if (status == TI_TEST_KERNEL_PASS) {
      printf("Opt and NatC match!  PASSED! \n");
   }
   else {
      printf("Opt and NatC DON'T match!  FAILED!!! \n");
   }

   free(pX0);
   free(pY0);
   free(pY1);
   delete (k0);
   delete (k1);

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
