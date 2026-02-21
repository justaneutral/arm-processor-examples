/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            **** |**
 **|                            **** |**
 **|                            ******o*** |**
 **|                      ********_///_**** |**
 **|                      ***** /_//_/ **** |**
 **|                       ** ** (__/ **** |**
 **|                           ********* |**
 **|                            **** |**
 **|                            *** |**
 **| |**
 **|         Copyright (c) 2016 Texas Instruments Incorporated |**
 **|                        ALL RIGHTS RESERVED |**
 **| |**
 **| Permission to use, copy, modify, or distribute this software, |**
 **| whether in part or in whole, for any purpose is forbidden without |**
 **| a signed licensing agreement and NDA from Texas Instruments |**
 **| Incorporated (TI). |**
 **| |**
 **| TI makes no representation or warranties with respect to the |**
 **| performance of this computer program, and specifically disclaims |**
 **| any responsibility for any damages, special or consequential, |**
 **| connected with the use of this program. |**
 **| |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

#include "DSPLIB_fir_idat.h"

static fir_testParams_t testParams[] = {
/********************************************
 {
 testPattern,
 staticIn,
 filter,
 staticOut,
 dataType,
 dataSize,
 dataPitchIn,  // in bytes
 dataPitchOut, // in bytes
 batchSize,
 filterSize,
 shift,
 outputDataLocation,
 numReps, 
 testID
 },
 *********************************************/

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 1) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        64,                      // dataSize,
        64 * 4,                  // dataPitchIn, in bytes
        64 * 4,                  // dataPitchOut, in bytes
        1,                       // batchSize,
        32,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        1                        // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 2) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        256,                     // dataSize,
        256 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        64,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        2                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 3) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        128,                     // dataSize,
        128 * 4,                 // dataPitchIn, in bytes
        128 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        64,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        3                        // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 101) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        64,                      // dataSize,
        64 * 4,                  // dataPitchIn, in bytes
        64 * 4,                  // dataPitchOut, in bytes
        2,                       // batchSize,
        32,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        101                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 102) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        256,                     // dataSize,
        256 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        2,                       // batchSize,
        64,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        102                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 103) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        128,                     // dataSize,
        128 * 4,                 // dataPitchIn, in bytes
        128 * 4,                 // dataPitchOut, in bytes
        2,                       // batchSize,
        64,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        103                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 104) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        2048,                    // dataSize,
        2048 * 4,                // dataPitchIn, in bytes
        2048 * 4,                // dataPitchOut, in bytes
        1,                       // batchSize,
        1024,                    // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        104                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 105) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        271,                     // dataSize,
        320 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        16,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        105                      // testID
    },
#endif

#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 106) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        64,                      // dataSize,
        64 * 4,                  // dataPitchIn, in bytes
        64 * 4,                  // dataPitchOut, in bytes
        1,                       // batchSize,
        16,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation, 
        1,                       // numReps,
        106                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 107) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        287,                     // dataSize,
        320 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        32,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        107                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 108) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        287,                     // dataSize,
        320 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        10,                      // batchSize,
        32,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        108                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 109) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        319,                     // dataSize,
        320 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        64,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        109                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 110) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        383,                     // dataSize,
        384 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        128,                     // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        110                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 111) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        383,                     // dataSize,
        384 * 4,                 // dataPitchIn, in bytes
        256 * 4,                 // dataPitchOut, in bytes
        2,                       // batchSize,
        128,                     // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        111                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 112) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        639,                     // dataSize,
        640 * 4,                 // dataPitchIn, in bytes
        512 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        128,                     // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        112                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 113) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        639,                     // dataSize,
        640 * 4,                 // dataPitchIn, in bytes
        512 * 4,                 // dataPitchOut, in bytes
        2,                       // batchSize,
        128,                     // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        113                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 114) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        1151,                    // dataSize,
        1152 * 4,                // dataPitchIn, in bytes
        128 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        1024,                    // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        114                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 115) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        1151,                    // dataSize,
        1152 * 4,                // dataPitchIn, in bytes
        1024 * 4,                // dataPitchOut, in bytes
        1,                       // batchSize,
        128,                     // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        115                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 116) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        1087,                    // dataSize,
        1088 * 4,                // dataPitchIn, in bytes
        1024 * 4,                // dataPitchOut, in bytes
        1,                       // batchSize,
        64,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        116                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 117) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        1087,                    // dataSize,
        1088 * 4,                // dataPitchIn, in bytes
        64 * 4,                  // dataPitchOut, in bytes
        1,                       // batchSize,
        1024,                    // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        117                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 118) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        64,                      // dataSize,
        64 * 4,                  // dataPitchIn, in bytes
        64 * 4,                  // dataPitchOut, in bytes
        1,                       // batchSize,
        4,                       // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        118                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 119) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        64,                      // dataSize,
        64 * 4,                  // dataPitchIn, in bytes
        64 * 4,                  // dataPitchOut, in bytes
        1,                       // batchSize,
        17,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        119                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 120) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        33,                      // dataSize,
        33 * 4,                  // dataPitchIn, in bytes
        33 * 4,                  // dataPitchOut, in bytes
        1,                       // batchSize,
        17,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        120                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 121) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        159,                     // dataSize,
        159 * 4,                 // dataPitchIn, in bytes
        159 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        17,                      // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        121                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 122) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        159,                     // dataSize,
        159 * 4,                 // dataPitchIn, in bytes
        159 * 4,                 // dataPitchOut, in bytes
        1,                       // batchSize,
        5,                       // filterSize,
        0,                       // filterPitch, in bytes
        0,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        122                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 123) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        159,                     // dataSize,
        159 * 4,                 // dataPitchIn, in bytes
        159 * 4,                 // dataPitchOut, in bytes
        2,                       // batchSize,
        17,                      // filterSize,
        17 * 4,                  // filterPitch, in bytes
        1,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        123                      // testID
    },
#endif
#if (defined(ALL_TEST_CASES) || (TEST_CATEGORY == 3) || (TEST_CASE == 124) || (TEST_CATEGORY == 100))
    {
        RANDOM,                  // testPattern,
        NULL,                    // staticIn,
        NULL,                    // filter,
        NULL,                    // staticOut,
        DSPLIB_FLOAT32,          // dataType,
        159,                     // dataSize,
        159 * 4,                 // dataPitchIn, in bytes
        159 * 4,                 // dataPitchOut, in bytes
        2,                       // batchSize,
        17,                      // filterSize,
        DSPLIB_CALC_STRIDE(17 * sizeof(float), DSPLIB_ALIGN_SHIFT_64BYTES),  // filterPitch, in bytes
        1,                       // enableNchCoefs, 
        1,                       // shift,
        DSPLIB_TEST_OUTPUT_HEAP, // outputDataLocation,
        1,                       // numReps,
        124                      // testID
    },
#endif
};

/*
 *  Sends the test parameter structure and number of tests
 */
void fir_getTestParams(fir_testParams_t **params, int32_t *numTests)
{
   *params   = testParams;
   *numTests = sizeof(testParams) / sizeof(fir_testParams_t);
}

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2016 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
/* ======================================================================== */
