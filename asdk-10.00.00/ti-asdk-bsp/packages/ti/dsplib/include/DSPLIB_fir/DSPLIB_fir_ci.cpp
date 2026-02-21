/******************************************************************************/
/*!
 * \file DSPLIB_fir.c
 */
/* Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/

#include "../common/c71/DSPLIB_inlines.h"
#include "DSPLIB_fir_priv.h"

/*******************************************************************************
 *
 * DEFINES
 *
 ******************************************************************************/
#define SE_PARAM_BASE (0x0000)
#define SE_SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE_SE1_PARAM_OFFSET (SE_SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA0_PARAM_OFFSET (SE_SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define SE_SA1_PARAM_OFFSET (SE_SA0_PARAM_OFFSET + SE_PARAM_SIZE)

/* FIR mode flags */
#define FIR_MODE_SMALL_FIR                      ( 0 )
#define FIR_MODE_LARGE_FIR                      ( 1 )
#define FIR_MODE_SMALL_FIR_SS                   ( 2 )

/* Small FIR macros */
#define SMALL_FIR_LOOP_UNROLL_FACTOR            ( 4 )

/* Long FIR macros */
#define LONG_FIR_OUT_SAMPLES_PER_BLOCK          ( 8 )
#define LONG_FIR_NUM_OUT_SAMP_GROUPS            ( 2 )
#define LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK \
    ( LONG_FIR_OUT_SAMPLES_PER_BLOCK * LONG_FIR_NUM_OUT_SAMP_GROUPS )


void DSPLIB_fir_perfEst(DSPLIB_kernelHandle         handle,
                        const DSPLIB_bufParams2D_t *bufParamsIn,
                        const DSPLIB_bufParams2D_t *bufParamsFilter,
                        const DSPLIB_bufParams2D_t *bufParamsOut,
                        uint64_t                   *archCycles,
                        uint64_t                   *estCycles)
{
   return;
}

template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_getSizes(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                  DSPLIB_bufParams2D_t      *bufParamsFilter,
                                  DSPLIB_bufParams2D_t      *bufParamsOut)
{
   uint32_t                                               dataSize       = pKerInitArgs->dataSize;
   uint32_t                                               filterSize     = pKerInitArgs->filterSize;
   uint32_t                                               computeLoopOutputSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_fir_getSizes\n");
#endif

   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   /* Number of output points produced in the C7x compute loop for a single
      channel in the batch */
   computeLoopOutputSize = (dataSize - filterSize + 1);
   bufParamsOut->dim_x = computeLoopOutputSize;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_fir_getSizes bufParamsOut->dim_x %d\n", bufParamsOut->dim_x);
#endif
   return status;
}

template DSPLIB_STATUS DSPLIB_fir_getSizes<float>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                  DSPLIB_bufParams2D_t      *bufParamsIn,
                                                  DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                  DSPLIB_bufParams2D_t      *bufParamsOut);

template DSPLIB_STATUS DSPLIB_fir_getSizes<int16_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                    DSPLIB_bufParams2D_t      *bufParamsIn,
                                                    DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                    DSPLIB_bufParams2D_t      *bufParamsOut);
template DSPLIB_STATUS DSPLIB_fir_getSizes<int32_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                    DSPLIB_bufParams2D_t      *bufParamsIn,
                                                    DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                    DSPLIB_bufParams2D_t      *bufParamsOut);
template DSPLIB_STATUS DSPLIB_fir_getSizes<int8_t>(const DSPLIB_fir_InitArgs *pKerInitArgs,
                                                   DSPLIB_bufParams2D_t      *bufParamsIn,
                                                   DSPLIB_bufParams2D_t      *bufParamsFilter,
                                                   DSPLIB_bufParams2D_t      *bufParamsOut);

template <typename dataType>
DSPLIB_STATUS DSPLIB_fir_init_ci(DSPLIB_kernelHandle         handle,
                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                 const DSPLIB_bufParams2D_t *bufParamsFilter,
                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                 const DSPLIB_fir_InitArgs  *pKerInitArgs)
{
    DSPLIB_STATUS    status = DSPLIB_SUCCESS;
    __SE_TEMPLATE_v1 se0Params =__gen_SE_TEMPLATE_v1(); 
    __SE_TEMPLATE_v1 se1Params =__gen_SE_TEMPLATE_v1();
    __SA_TEMPLATE_v1 sa0Params =__gen_SA_TEMPLATE_v1();
    __SA_TEMPLATE_v1 sa1Params =__gen_SA_TEMPLATE_v1();

    DSPLIB_fir_PrivArgs *pKerPrivArgs       = (DSPLIB_fir_PrivArgs *) handle;
    uint8_t             *pBlock             = pKerPrivArgs->bufPblock;
    int32_t              dataSize           = pKerPrivArgs->initArgs.dataSize;
    int32_t              batchSize          = pKerPrivArgs->initArgs.batchSize;
    int32_t              filterSize         = pKerPrivArgs->initArgs.filterSize;
    uint32_t             enableNchCoefs     = pKerPrivArgs->initArgs.enableNchCoefs;
    uint32_t             dataBufferInPitch  = pKerPrivArgs->dataBufferInPitch;
    uint32_t             dataBufferOutPitch = pKerPrivArgs->dataBufferOutPitch;
    uint32_t             filterPitch        = pKerPrivArgs->filterPitch;
    int32_t              outputSize         = pKerPrivArgs->outputSize;

    typedef typename c7x::make_full_vector<dataType>::type vec;
    __SE_VECLEN  SE_VECLEN  = c7x::se_veclen<vec>::value;
    __SA_VECLEN  SA_VECLEN  = c7x::sa_veclen<vec>::value;
    __SE_ELETYPE SE_ELETYPE = c7x::se_eletype<vec>::value;
    __SE_ELEDUP  SE_ELEDUP  = c7x::se_eledup<dataType>::value;
    int32_t eleCount = c7x::element_count_of<vec>::value;
    uint32_t flag;              /* flag indicating small or large FIR */
    int32_t blkElements;        /* number of outputs produce per loop iteration */
    int32_t dataLoopSize;       /* number of output data loops */
    int32_t filterLoopSize;     /* number of primary filter loops */
    int32_t remFilterLoopSize;  /* number of remaining filter loops */

#if DSPLIB_DEBUGPRINT
    printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
           "batchSize %d outputSize %d\n",
           SE_VECLEN, SA_VECLEN, SE_ELETYPE, batchSize, outputSize);
#endif

#if DSPLIB_DEBUGPRINT
    printf("DSPLIB_DEBUGPRINT SE_VECLEN: %d, SA_VECLEN: %d, SE_ELETYPE: %d "
           "SE_ELEDUP %d eleCount %d\n",
           SE_VECLEN, SA_VECLEN, SE_ELETYPE, SE_ELEDUP, eleCount);
#endif
   
    if ((dataSize >= (eleCount * LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK)) && 
        (filterSize <= (dataSize - LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount + 1))) {

        flag = FIR_MODE_LARGE_FIR; /* set flag for large FIR */

        blkElements = LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
        dataLoopSize = (outputSize + blkElements - 1 ) / blkElements;
        dataLoopSize = (dataLoopSize == 0) ? 1 : dataLoopSize;
      
#if DSPLIB_DEBUGPRINT
    printf("DSPLIB_DEBUGPRINT dataLoopSize: %d (dataSize - filterSize) %d 16*eleCount "
           "%d blkElements %d ((dataSize - filterSize) / blkElements) %d\n",
           dataLoopSize, (dataSize - filterSize), LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount, blkElements, ((dataSize - filterSize) / blkElements));
#endif
        
        filterLoopSize = filterSize;
        remFilterLoopSize = 0;
    }
    else {
        if (outputSize > 1)
        {
            flag = FIR_MODE_SMALL_FIR; /* set flag for small FIR */
        
            blkElements = (eleCount > filterSize) ? filterSize : eleCount;
            dataLoopSize = (outputSize + blkElements - 1 ) / blkElements;
            dataLoopSize = (dataLoopSize == 0) ? 1 : dataLoopSize;
            
            filterLoopSize = filterSize / SMALL_FIR_LOOP_UNROLL_FACTOR;
            remFilterLoopSize = filterSize % SMALL_FIR_LOOP_UNROLL_FACTOR;
        }
        else
        {
            flag = FIR_MODE_SMALL_FIR_SS; /* set flag for small FIR w/ single output sample */
        
            dataLoopSize = outputSize;
            
            int32_t numFilterVec = (filterSize + eleCount - 1)/eleCount;
            filterLoopSize = numFilterVec / SMALL_FIR_LOOP_UNROLL_FACTOR;
            remFilterLoopSize = numFilterVec % SMALL_FIR_LOOP_UNROLL_FACTOR;
        }
    }
    pKerPrivArgs->dataLoopSize = dataLoopSize;
    pKerPrivArgs->filterLoopSize = filterLoopSize;
    pKerPrivArgs->remFilterLoopSize = remFilterLoopSize;
    pKerPrivArgs->flag = flag;

#if DSPLIB_DEBUGPRINT
    printf("DSPLIB_DEBUGPRINT FIR size flag: %d\n", flag);
#endif

    if (flag == FIR_MODE_SMALL_FIR) {
        /**********************************************************************/
        /* Prepare streaming engine 1 to fetch the filter values              */
        /**********************************************************************/
        se1Params = __gen_SE_TEMPLATE_v1();
        se1Params.ICNT0 = 1;
        se1Params.ICNT1 = filterSize;
        se1Params.DIM1  = -(int32_t) 1;
        se1Params.ICNT2 = dataLoopSize;
        se1Params.DIM2  = 0;
        se1Params.ICNT3 = batchSize;
        se1Params.DIM3  = (enableNchCoefs == 0) ? 0 : filterPitch;
        se1Params.ELETYPE = SE_ELETYPE;
        se1Params.VECLEN  = SE_VECLEN;
        se1Params.ELEDUP  = SE_ELEDUP;
        se1Params.DIMFMT  = __SE_DIMFMT_4D;
        se1Params.DIR     = __SE_DIR_DEC;
        
        /**********************************************************************/
        /* Prepare streaming engine 0 to fetch data                           */
        /**********************************************************************/
        se0Params = __gen_SE_TEMPLATE_v1();
        se0Params.ICNT0   = blkElements;
        se0Params.ICNT1   = filterSize;
        se0Params.DIM1    = (int32_t) 1;
        se0Params.ICNT2   = dataLoopSize;
        se0Params.DIM2    = blkElements;
        se0Params.ICNT3   = batchSize;
        se0Params.DIM3    = dataBufferInPitch;
        se0Params.DECDIM1 = __SE_DECDIM_DIM2;
        se0Params.DECDIM1SD = __SE_DECDIMSD_DIM1;
        se0Params.DECDIM1_WIDTH = dataSize;
        se0Params.ELETYPE = SE_ELETYPE;
        se0Params.VECLEN  = SE_VECLEN;
        se0Params.DIMFMT  = __SE_DIMFMT_4D;

        /**********************************************************************/
        /* Prepare SA template to store output                                */
        /**********************************************************************/
        sa0Params = __gen_SA_TEMPLATE_v1();
        sa0Params.ICNT0 = blkElements;
        sa0Params.ICNT1 = dataLoopSize;
        sa0Params.DIM1  = blkElements;
        sa0Params.ICNT2         = batchSize;
        sa0Params.DIM2          = dataBufferOutPitch;
        sa0Params.DECDIM1_WIDTH = outputSize;
        sa0Params.DECDIM1       = __SA_DECDIM_DIM1;
        sa0Params.VECLEN        = SA_VECLEN;
        sa0Params.DIMFMT        = __SA_DIMFMT_3D;
        
        *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
        *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
        *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
    }
    else if (flag == FIR_MODE_LARGE_FIR) {
        /*********************************************
        * Prepare SA template to read filter values *
        *********************************************/
        sa1Params       = __gen_SA_TEMPLATE_v1();
        sa1Params.ICNT0 = (uint32_t) 1;
        sa1Params.ICNT1 = (uint32_t) filterSize;
        sa1Params.DIM1  = -(int32_t) 1;
        sa1Params.ICNT2 = dataLoopSize;
        sa1Params.DIM2  = 0;
        sa1Params.ICNT3 = batchSize;
        sa1Params.DIM3  = (enableNchCoefs == 0) ? 0 : filterPitch;
        sa1Params.DIMFMT = __SA_DIMFMT_4D;
        sa1Params.VECLEN = __SA_VECLEN_1ELEM;

        /**********************************************************************/
        /* Prepare streaming engine 0 to fetch data                           */
        /**********************************************************************/
        se0Params = __gen_SE_TEMPLATE_v1();
        se0Params.ICNT0   = LONG_FIR_OUT_SAMPLES_PER_BLOCK * eleCount;
        se0Params.ICNT1   = filterSize;
        se0Params.DIM1    = (int32_t) 1;
        se0Params.ICNT2   = dataLoopSize;
        se0Params.DIM2    = (int32_t) LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
        se0Params.ICNT3   = batchSize;
        se0Params.DIM3    = dataBufferInPitch;
        se0Params.DECDIM1 = __SE_DECDIM_DIM2;
        se0Params.DECDIM1SD = __SE_DECDIMSD_DIM1;
        se0Params.DECDIM1_WIDTH = dataSize;
        se0Params.ELETYPE = SE_ELETYPE;
        se0Params.VECLEN  = SE_VECLEN;
        se0Params.DIMFMT  = __SE_DIMFMT_4D;

        /**********************************************************************/
        /* Prepare streaming engine 2 to fetch data                           */
        /**********************************************************************/
        se1Params = se0Params;
        se1Params.DECDIM1_WIDTH -= LONG_FIR_OUT_SAMPLES_PER_BLOCK * eleCount; /* SE1 is advanced w.r.t. SE0 */

        /**********************************************************************/
        /* Prepare SA template to store output                                */
        /**********************************************************************/
        sa0Params = __gen_SA_TEMPLATE_v1();
        sa0Params.ICNT0 = eleCount;
        sa0Params.ICNT1 = LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK;
        sa0Params.DIM1  = (int32_t) eleCount;
        sa0Params.ICNT2 = dataLoopSize;
        sa0Params.DIM2  = (int32_t) LONG_FIR_TOT_OUT_SAMPLES_PER_BLOCK * eleCount;
        sa0Params.ICNT3         = batchSize;
        sa0Params.DIM3          = dataBufferOutPitch;
        sa0Params.DECDIM1       = __SA_DECDIM_DIM2;
        sa0Params.DECDIM1SD     = __SA_DECDIMSD_DIM1;
        sa0Params.DECDIM1_WIDTH = outputSize;
        sa0Params.VECLEN        = SA_VECLEN;
        sa0Params.DIMFMT        = __SA_DIMFMT_4D;

        *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
        *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
        *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
        *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET) = sa1Params;
    }
    else if (flag == FIR_MODE_SMALL_FIR_SS) {
        /**********************************************************************/
        /* Prepare streaming engine 1 to fetch the filter values              */
        /**********************************************************************/
        se1Params = __gen_SE_TEMPLATE_v1();
        se1Params.ICNT0 = filterSize;
        se1Params.ICNT1 = dataLoopSize;
        se1Params.DIM1  = 0;
        se1Params.ICNT2 = batchSize;
        se1Params.DIM2  = (enableNchCoefs == 0) ? 0 : filterPitch;
        se1Params.ELETYPE = SE_ELETYPE;
        se1Params.VECLEN  = SE_VECLEN;
        se1Params.DIMFMT  = __SE_DIMFMT_3D;
        se1Params.DIR     = __SE_DIR_DEC;
        
        /**********************************************************************/
        /* Prepare streaming engine 0 to fetch data                           */
        /**********************************************************************/
        se0Params = __gen_SE_TEMPLATE_v1();
        se0Params.ICNT0   = filterSize;
        se0Params.ICNT1   = dataLoopSize;
        se0Params.DIM1    = (int32_t) 1;
        se0Params.ICNT2   = batchSize;
        se0Params.DIM2    = dataBufferInPitch;
        se0Params.DECDIM1 = __SE_DECDIM_DIM1;
        se0Params.DECDIM1_WIDTH = dataSize;
        se0Params.ELETYPE = SE_ELETYPE;
        se0Params.VECLEN  = SE_VECLEN;
        se0Params.DIMFMT  = __SE_DIMFMT_3D;
        
        /**********************************************************************/
        /* Prepare SA template to store output                                */
        /**********************************************************************/
        sa0Params = __gen_SA_TEMPLATE_v1();
        sa0Params.ICNT0 = 1;
        sa0Params.ICNT1 = dataLoopSize;
        sa0Params.DIM1  = 1;
        sa0Params.ICNT2         = batchSize;
        sa0Params.DIM2          = dataBufferOutPitch;
        sa0Params.VECLEN        = SA_VECLEN;
        sa0Params.DIMFMT        = __SA_DIMFMT_3D;

        *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET) = se0Params;
        *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET) = se1Params;
        *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET) = sa0Params;
    }

    return status;
}

template DSPLIB_STATUS DSPLIB_fir_init_ci<int16_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                   const DSPLIB_bufParams2D_t *bufParamsOut,
                                                   const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<int32_t>(DSPLIB_kernelHandle         handle,
                                                   const DSPLIB_bufParams2D_t *bufParamsIn,
                                                   const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                   const DSPLIB_bufParams2D_t *bufParamsOut,
                                                   const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<int8_t>(DSPLIB_kernelHandle         handle,
                                                  const DSPLIB_bufParams2D_t *bufParamsIn,
                                                  const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                  const DSPLIB_bufParams2D_t *bufParamsOut,
                                                  const DSPLIB_fir_InitArgs  *pKerInitArgs);

template DSPLIB_STATUS DSPLIB_fir_init_ci<float>(DSPLIB_kernelHandle         handle,
                                                 const DSPLIB_bufParams2D_t *bufParamsIn,
                                                 const DSPLIB_bufParams2D_t *bufParamsFilter,
                                                 const DSPLIB_bufParams2D_t *bufParamsOut,
                                                 const DSPLIB_fir_InitArgs  *pKerInitArgs);
/*******************************************************************************
 *
 * EXECUTION FUNCTIONS
 *
 ******************************************************************************/

template <typename dataType>
DSPLIB_STATUS
DSPLIB_fir_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut)
{
    __SE_TEMPLATE_v1 se0Params;
    __SE_TEMPLATE_v1 se1Params;
    __SA_TEMPLATE_v1 sa0Params;
    __SA_TEMPLATE_v1 sa1Params;

    DSPLIB_fir_PrivArgs *pKerPrivArgs = (DSPLIB_fir_PrivArgs *) handle;
    uint8_t             *pBlock       = pKerPrivArgs->bufPblock;
    int32_t              batchSize    = pKerPrivArgs->initArgs.batchSize;
    int32_t              filterSize   = pKerPrivArgs->initArgs.filterSize;
    uint32_t             flag         = pKerPrivArgs->flag;
    int32_t              dataLoopSize;
    int32_t              filterLoopSize;
    int32_t              remFilterLoopSize;

    dataType *pInLocal     = (dataType *) pIn;
    dataType *pFilterLocal = (dataType *) pFilter;
    dataType *pOutLocal    = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
    printf("Enter DSPLIB_fir_exec_ci\n");
#endif

    typedef typename c7x::make_full_vector<dataType>::type vec;
    int  eleCount = c7x::element_count_of<vec>::value;

    vec accumulator_0, accumulator_1, accumulator_2, accumulator_3;
    vec accumulator_x, accumulator_y;
    dataType acc;

    vec accumulator_0_0;
    vec accumulator_0_1;
    vec accumulator_0_2;
    vec accumulator_0_3;
    vec accumulator_0_4;
    vec accumulator_0_5;
    vec accumulator_0_6;
    vec accumulator_0_7;
    vec accumulator_1_0;
    vec accumulator_1_1;
    vec accumulator_1_2;
    vec accumulator_1_3;
    vec accumulator_1_4;
    vec accumulator_1_5;
    vec accumulator_1_6;
    vec accumulator_1_7;

#if DSPLIB_DEBUGPRINT
    printf("Enter pFilter %p\n", pFilter);
#endif

    if (flag == FIR_MODE_LARGE_FIR) {
        pFilterLocal = pFilterLocal + filterSize - 1;
    }
    else {
        pFilterLocal = pFilterLocal + filterSize;
    }

#if DSPLIB_DEBUGPRINT
    printf("Enter pFilter %p\n", pFilterLocal);
#endif

    se0Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE0_PARAM_OFFSET);
    __SE0_OPEN(pInLocal, se0Params);

    if ((flag == FIR_MODE_SMALL_FIR) || (flag == FIR_MODE_SMALL_FIR_SS)) {
        se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
        __SE1_OPEN(pFilterLocal, se1Params);
    }
    else {

        pInLocal  = pInLocal + 8 * eleCount;
        se1Params = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SE1_PARAM_OFFSET);
        __SE1_OPEN(pInLocal, se1Params);

        // Filter coefficient
        sa1Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA1_PARAM_OFFSET);
        __SA1_OPEN(sa1Params);
    }

    sa0Params = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE_SA0_PARAM_OFFSET);
    __SA0_OPEN(sa0Params);

    dataLoopSize = pKerPrivArgs->dataLoopSize;
    filterLoopSize = pKerPrivArgs->filterLoopSize;
    remFilterLoopSize = pKerPrivArgs->remFilterLoopSize;

#if DSPLIB_DEBUGPRINT
    printf("DSPLIB_DEBUGPRINT dataSize %d filterSize %d dataLoopSize %d\n", pKerPrivArgs->initArgs.dataSize, filterSize, dataLoopSize);
#endif

#if DSPLIB_DEBUGPRINT
    printf("DSPLIB_DEBUGPRINT filterSize %d\n", filterSize);
#endif

    if (flag == FIR_MODE_SMALL_FIR_SS) {
        /* small FIR, single sample */
        for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
            for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
                accumulator_0 = (vec) 0;
                accumulator_1 = (vec) 0;
                accumulator_2 = (vec) 0;
                accumulator_3 = (vec) 0;

                for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
                    vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_0 += tmp0 * tmp1;
        
                    vec tmp2 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp3 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1 += tmp2 * tmp3;
        
                    vec tmp4 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp5 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_2 += tmp4 * tmp5;
        
                    vec tmp6 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp7 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_3 += tmp6 * tmp7;
                }
            
                for (int32_t filterLoopCount = 0; filterLoopCount < remFilterLoopSize; filterLoopCount++) {
                    vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_0 += tmp0 * tmp1;
                }
            
                accumulator_x = accumulator_0 + accumulator_1;
                accumulator_y = accumulator_2 + accumulator_3;
                accumulator_x = accumulator_x + accumulator_y;
                
                /* horizontal add */
                accumulator_x.lo() = accumulator_x.hi() + accumulator_x.lo();
                accumulator_x.lo().lo() = accumulator_x.lo().hi() + accumulator_x.lo().lo();
                #if defined(__C7100__) || defined(__C7120__)
                   accumulator_x.lo().lo().lo() = accumulator_x.lo().lo().hi() + accumulator_x.lo().lo().lo();
                #endif
                acc = (float) accumulator_x.s[0] + (float) accumulator_x.s[1];

                /* write output */
                __vpred tmp   = c7x::strm_agen<0, dataType>::get_vpred();
                dataType    *VB1   = c7x::strm_agen<0, dataType>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, acc);
            }
        }
    }
    else if (flag == FIR_MODE_SMALL_FIR) {
        /* small FIR */
        for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
            for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
                accumulator_0 = (vec) 0;
                accumulator_1 = (vec) 0;
                accumulator_2 = (vec) 0;
                accumulator_3 = (vec) 0;

                for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
                    vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
#if DSPLIB_DEBUGPRINT
//                    DSPLIB_debugPrintVector(tmp0);
//                    DSPLIB_debugPrintVector(tmp1);
#endif
                    accumulator_0 += tmp0 * tmp1;
    
                    vec tmp2 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp3 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1 += tmp2 * tmp3;
        
                    vec tmp4 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp5 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_2 += tmp4 * tmp5;
        
                    vec tmp6 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp7 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_3 += tmp6 * tmp7;
                }
            
                for (int32_t filterLoopCount = 0; filterLoopCount < remFilterLoopSize; filterLoopCount++) {
                    vec tmp0 = c7x::strm_eng<0, vec>::get_adv();
                    vec tmp1 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_0 += tmp0 * tmp1;
                }
            
                accumulator_x = accumulator_0 + accumulator_1;
                accumulator_y = accumulator_2 + accumulator_3;
                accumulator_x = accumulator_x + accumulator_y;
                __vpred tmp   = c7x::strm_agen<0, vec>::get_vpred();
                vec    *VB1   = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_x);

#if DSPLIB_DEBUGPRINT
                DSPLIB_debugPrintVector(accumulator_x);
#endif
            }
        }
    }
    else {
        /* large FIR */
        for (int32_t batchSizeCount = 0; batchSizeCount < batchSize; batchSizeCount++) {
            for (int32_t dataLoopCount = 0; dataLoopCount < dataLoopSize; dataLoopCount++) {
                accumulator_0_0 = (vec) 0;
                accumulator_0_1 = (vec) 0;
                accumulator_0_2 = (vec) 0;
                accumulator_0_3 = (vec) 0;
                accumulator_0_4 = (vec) 0;
                accumulator_0_5 = (vec) 0;
                accumulator_0_6 = (vec) 0;
                accumulator_0_7 = (vec) 0;
                accumulator_1_0 = (vec) 0;
                accumulator_1_1 = (vec) 0;
                accumulator_1_2 = (vec) 0;
                accumulator_1_3 = (vec) 0;
                accumulator_1_4 = (vec) 0;
                accumulator_1_5 = (vec) 0;
                accumulator_1_6 = (vec) 0;
                accumulator_1_7 = (vec) 0;

                for (int32_t filterLoopCount = 0; filterLoopCount < filterLoopSize; filterLoopCount++) {
                    dataType *VB1Dup = c7x::strm_agen<1, dataType>::get_adv(pFilterLocal);
                    vec       flt = __vload_dup(VB1Dup);
        
                    vec tmp0_0 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_0 += tmp0_0 * flt;
        
                    vec tmp0_1 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_1 += tmp0_1 * flt;
        
                    vec tmp0_2 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_2 += tmp0_2 * flt;
        
                    vec tmp0_3 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_3 += tmp0_3 * flt;
        
                    vec tmp0_4 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_4 += tmp0_4 * flt;
        
                    vec tmp0_5 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_5 += tmp0_5 * flt;
        
                    vec tmp0_6 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_6 += tmp0_6 * flt;
        
                    vec tmp0_7 = c7x::strm_eng<0, vec>::get_adv();
                    accumulator_0_7 += tmp0_7 * flt;
        
                    vec tmp1_0 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_0 += tmp1_0 * flt;
        
                    vec tmp1_1 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_1 += tmp1_1 * flt;
        
                    vec tmp1_2 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_2 += tmp1_2 * flt;
        
                    vec tmp1_3 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_3 += tmp1_3 * flt;
        
                    vec tmp1_4 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_4 += tmp1_4 * flt;
        
                    vec tmp1_5 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_5 += tmp1_5 * flt;
        
                    vec tmp1_6 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_6 += tmp1_6 * flt;
        
                    vec tmp1_7 = c7x::strm_eng<1, vec>::get_adv();
                    accumulator_1_7 += tmp1_7 * flt;
#if DSPLIB_DEBUGPRINT
//                    DSPLIB_debugPrintVector(flt);
//                    DSPLIB_debugPrintVector(tmp0_0);
//                    DSPLIB_debugPrintVector(tmp0_1);
//                    DSPLIB_debugPrintVector(tmp0_2);
//                    DSPLIB_debugPrintVector(tmp0_3);
//                    DSPLIB_debugPrintVector(tmp0_4);
//                    DSPLIB_debugPrintVector(tmp0_5);
//                    DSPLIB_debugPrintVector(tmp0_6);
//                    DSPLIB_debugPrintVector(tmp0_7);
//                    DSPLIB_debugPrintVector(tmp1_0);
//                    DSPLIB_debugPrintVector(tmp0_1);
//                    DSPLIB_debugPrintVector(tmp1_2);
//                    DSPLIB_debugPrintVector(tmp1_3);
//                    DSPLIB_debugPrintVector(tmp1_4);
//                    DSPLIB_debugPrintVector(tmp1_5);
//                    DSPLIB_debugPrintVector(tmp1_6);
//                    DSPLIB_debugPrintVector(tmp1_7);
#endif
                }
                __vpred tmp = c7x::strm_agen<0, vec>::get_vpred();
                vec    *VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_0);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_1);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_2);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_3);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_4);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_5);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_6);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_0_7);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_0);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_1);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_2);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_3);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_4);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_5);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_6);
    
                tmp = c7x::strm_agen<0, vec>::get_vpred();
                VB1 = c7x::strm_agen<0, vec>::get_adv(pOutLocal);
                __vstore_pred(tmp, VB1, accumulator_1_7);

#if DSPLIB_DEBUGPRINT
//                DSPLIB_debugPrintVector(accumulator_0_0);
//                DSPLIB_debugPrintVector(accumulator_0_1);
//                DSPLIB_debugPrintVector(accumulator_0_2);
//                DSPLIB_debugPrintVector(accumulator_0_3);
//                DSPLIB_debugPrintVector(accumulator_0_4);
//                DSPLIB_debugPrintVector(accumulator_0_5);
//                DSPLIB_debugPrintVector(accumulator_0_6);
//                DSPLIB_debugPrintVector(accumulator_0_7);
//                
//                DSPLIB_debugPrintVector(accumulator_1_0);
//                DSPLIB_debugPrintVector(accumulator_1_1);
//                DSPLIB_debugPrintVector(accumulator_1_2);
//                DSPLIB_debugPrintVector(accumulator_1_3);
//                DSPLIB_debugPrintVector(accumulator_1_4);
//                DSPLIB_debugPrintVector(accumulator_1_5);
//                DSPLIB_debugPrintVector(accumulator_1_6);
//                DSPLIB_debugPrintVector(accumulator_1_7);
#endif
            }
        }
    }
    __SE0_CLOSE();
    __SE1_CLOSE();
    __SA0_CLOSE();

    return DSPLIB_SUCCESS;
}

template DSPLIB_STATUS DSPLIB_fir_exec_ci<int16_t>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn,
                                                   void *restrict pFilter,
                                                   void *restrict pOut);

template DSPLIB_STATUS DSPLIB_fir_exec_ci<int32_t>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn,
                                                   void *restrict pFilter,
                                                   void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_fir_exec_ci<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_fir_exec_ci<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pFilter, void *restrict pOut);
