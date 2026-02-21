/******************************************************************************/
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
#include "DSPLIB_matMul_fixed_priv.h"

#define SE_PARAM_BASE (0x0000)
#define SE0_PARAM_OFFSET (SE_PARAM_BASE)
#define SE1_PARAM_OFFSET (SE0_PARAM_OFFSET + SE_PARAM_SIZE)
#define SA0_PARAM_OFFSET (SE1_PARAM_OFFSET + SE_PARAM_SIZE)
#define DSPLIB_MATMUL_FIXED_UNROLL (128)

/***********************************
 *   Initialization
 ***********************************/
template <typename dataTypeIn, typename dataTypeOut>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci(__SE_TEMPLATE_v1 *se0Params, __SE_TEMPLATE_v1 *se1Params);
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                       __SE_TEMPLATE_v1 *se1Params)
{
   se0Params->PROMOTE   = __SE_PROMOTE_OFF;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_64BIT;
}
template <>
inline void DSPLIB_matMul_fixed_PromoteTranspose_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(__SE_TEMPLATE_v1 *se0Params,
                                                                                     __SE_TEMPLATE_v1 *se1Params)
{
   se0Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
   se1Params->TRANSPOSE = __SE_TRANSPOSE_32BIT;
   se1Params->PROMOTE   = __SE_PROMOTE_2X_SIGNEXT;
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS DSPLIB_matMul_fixed_init_ci(DSPLIB_kernelHandle                 handle,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs)
{
   DSPLIB_STATUS    status = DSPLIB_SUCCESS;
   __SE_TEMPLATE_v1 se0Params;
   __SE_TEMPLATE_v1 se1Params;
   __SA_TEMPLATE_v1 sa0Params;
   __SE_ELETYPE     SE_ELETYPE = c7x::se_eletype<dataTypeOut>::value;
   __SE_VECLEN      SE_VECLEN  = c7x::se_veclen<c7x::short_vec>::value;
   ;
   __SA_VECLEN                   SA_VECLEN    = c7x::sa_veclen<dataTypeOut>::value;
   int32_t                       elementCount = c7x::element_count_of<dataTypeOut>::value;
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;
   uint8_t                      *pBlock       = pKerPrivArgs->bufPblock;
   int32_t                       M            = pKerPrivArgs->M;
   int32_t                       K            = pKerPrivArgs->K;
   int32_t                       N            = pKerPrivArgs->N;
   int32_t                       strideIn0    = pKerPrivArgs->strideIn0Elements;
   int32_t                       strideIn1    = pKerPrivArgs->strideIn1Elements;
   int32_t                       strideOut    = pKerPrivArgs->strideOutElements;
   int32_t                       unrollFactor = 1;
   if (N >= 32) {
      unrollFactor = 2;
   }

   int32_t KBlocks = ((K + 4 - 1)) / (4);
   int32_t MBlocks = ((M + 2 - 1)) / (2);
   int32_t NBlocks = ((N + (elementCount * unrollFactor) - 1)) / (elementCount * unrollFactor);
   ;

   pKerPrivArgs->KBlocks = KBlocks;
   pKerPrivArgs->NBlocks = NBlocks;
   pKerPrivArgs->MBlocks = MBlocks;

   /**********************************************************************/
   /* Prepare streaming engine 0 to fetch A matrix                       */
   /**********************************************************************/
   se0Params           = __gen_SE_TEMPLATE_v1();
   se0Params.ELETYPE   = SE_ELETYPE;
   se0Params.DIMFMT    = __SE_DIMFMT_5D;
   se0Params.DECDIM1   = __SE_DECDIM_DIM2;
   se0Params.DECDIM2   = __SE_DECDIM_DIM4;
   se0Params.DECDIM1SD = __SE_DECDIMSD_DIM0;
   se0Params.DECDIM2SD = __SE_DECDIMSD_DIM1;

   se0Params.VECLEN        = __SE_VECLEN_4ELEMS;
   se0Params.GRPDUP        = __SE_GRPDUP_ON;
   se0Params.ICNT0         = 4;
   se0Params.ICNT1         = 2;
   se0Params.DIM1          = strideIn0;
   se0Params.ICNT2         = KBlocks;
   se0Params.DIM2          = 4;
   se0Params.ICNT3         = NBlocks;
   se0Params.DIM3          = 0;
   se0Params.DECDIM2_WIDTH = (uint32_t) M * strideIn0;
   se0Params.ICNT4         = MBlocks;
   se0Params.DIM4          = 2 * strideIn0;
   se0Params.DECDIM1_WIDTH = (uint32_t) K;

   /**********************************************************************/
   /* Prepare streaming engine 1 to fetch B matrix                       */
   /**********************************************************************/
   se1Params         = __gen_SE_TEMPLATE_v1();
   se1Params.ELETYPE = SE_ELETYPE;
   se1Params.VECLEN  = SE_VECLEN;
   se1Params.DIMFMT  = __SE_DIMFMT_5D;

   se1Params.ICNT0 = 4 * 4 * unrollFactor;
   se1Params.ICNT1 = 4;
   se1Params.DIM1  = strideIn1;
   se1Params.DIM2  = 4 * strideIn1;
   se1Params.ICNT2 = KBlocks;
   se1Params.DIM3  = 4 * 4 * unrollFactor;
   se1Params.ICNT3 = NBlocks;
   se1Params.DIM4  = 0;
   se1Params.ICNT4 = MBlocks;

   DSPLIB_matMul_fixed_PromoteTranspose_init_ci<dataTypeIn, dataTypeOut>(&se0Params, &se1Params);
   /**********************************************************************/
   /* Prepare SA template to write C matrix                              */
   /**********************************************************************/
   sa0Params           = __gen_SA_TEMPLATE_v1();
   sa0Params.VECLEN    = SA_VECLEN;
   sa0Params.DIMFMT    = __SA_DIMFMT_4D;
   sa0Params.DECDIM1   = __SA_DECDIM_DIM2;
   sa0Params.DECDIM1SD = __SA_DECDIMSD_DIM0;
   sa0Params.DECDIM2   = __SA_DECDIM_DIM3;
   sa0Params.DECDIM2SD = __SA_DECDIMSD_DIM1;

   sa0Params.ICNT0         = elementCount * unrollFactor;
   sa0Params.ICNT1         = 2;
   sa0Params.ICNT2         = NBlocks;
   sa0Params.DIM1          = strideOut;
   sa0Params.DECDIM1_WIDTH = N;
   sa0Params.DECDIM2_WIDTH = M * strideOut;
   sa0Params.DIM2          = elementCount * unrollFactor;
   sa0Params.ICNT3         = MBlocks;
   sa0Params.DIM3          = 2 * strideOut;

   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET) = se0Params;
   *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET) = se1Params;
   *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET) = sa0Params;

   return status;
}

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle                 handle,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                          const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                          const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                          const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);

template DSPLIB_STATUS
DSPLIB_matMul_fixed_init_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle                 handle,
                                                        const DSPLIB_bufParams2D_t         *bufParamsIn0,
                                                        const DSPLIB_bufParams2D_t         *bufParamsIn1,
                                                        const DSPLIB_bufParams2D_t         *bufParamsOut,
                                                        const DSPLIB_matMul_fixed_InitArgs *pKerInitArgs);
/***********************************
 *   Implementation
 ***********************************/
template <uint32_t id> static inline void loadMatSE(c7x::short_vec *a)
{
   *a = c7x::strm_eng<id, c7x::short_vec>::get_adv();
}
template <typename T, typename pVec, typename vecIn>
static inline void writeOutSA0(__vpred vPred, pVec *addr, T pOut, vecIn out1, vecIn out2)
{
   vPred = c7x::strm_agen<0, pVec>::get_vpred();
   addr  = c7x::strm_agen<0, pVec>::get_adv(pOut);
   __vstore_pred_packl_2src(vPred, addr, out1, out2);
   return;
}
template <typename T, typename vecIn>
static inline void writeOutSA0(__vpred vPred, c7x::char_hvec *addr, T pOut, vecIn out1, vecIn out2)
{
   vPred = c7x::strm_agen<0, c7x::char_hvec>::get_vpred();
   addr  = c7x::strm_agen<0, c7x::char_hvec>::get_adv(pOut);
   __vstore_pred_pack_byte_2src(vPred, addr, out1, out2);
   return;
}

template <typename dataTypeIn, typename dataTypeOut>
DSPLIB_STATUS
DSPLIB_matMul_fixed_exec_ci(DSPLIB_kernelHandle handle, void *restrict pIn0, void *restrict pIn1, void *restrict pOut)
{
   DSPLIB_matMul_fixed_PrivArgs *pKerPrivArgs = (DSPLIB_matMul_fixed_PrivArgs *) handle;
   int16_t                      *pOutLocal    = (int16_t *) pOut;
   int32_t                       KBlocks      = pKerPrivArgs->KBlocks;
   int32_t                       NBlocks      = pKerPrivArgs->NBlocks;
   int32_t                       MBlocks      = pKerPrivArgs->MBlocks;
   int32_t                       qs           = pKerPrivArgs->qs;
   uint8_t                      *pBlock       = pKerPrivArgs->bufPblock;
   __SE_TEMPLATE_v1              se0Params    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE0_PARAM_OFFSET);
   __SE_TEMPLATE_v1              se1Params    = *(__SE_TEMPLATE_v1 *) ((uint8_t *) pBlock + SE1_PARAM_OFFSET);
   __SA_TEMPLATE_v1              sa0Params    = *(__SA_TEMPLATE_v1 *) ((uint8_t *) pBlock + SA0_PARAM_OFFSET);
   __vpred                       vPred;
   dataTypeOut                  *addr;
   uchar32 vMask = uchar32(0, 1, 8, 9, 16, 17, 24, 25, 2, 3, 10, 11, 18, 19, 26, 27, 4, 5, 12, 13, 20, 21, 28, 29, 6, 7,
                           14, 15, 22, 23, 30, 31);

   __SE0_OPEN(pIn0, se0Params);
   __SE1_OPEN((pIn1), se1Params);
   __SA0_OPEN(sa0Params);

   if (pKerPrivArgs->N >= 32) {
      for (int32_t mn = 0; mn < MBlocks * NBlocks; mn++) {
         c7x::long_vec  c00 = (c7x::long_vec) 0;
         c7x::long_vec  c01 = (c7x::long_vec) 0;
         c7x::long_vec  c02 = (c7x::long_vec) 0;
         c7x::long_vec  c03 = (c7x::long_vec) 0;
         c7x::long_vec  c04 = (c7x::long_vec) 0;
         c7x::long_vec  c05 = (c7x::long_vec) 0;
         c7x::long_vec  c06 = (c7x::long_vec) 0;
         c7x::long_vec  c07 = (c7x::long_vec) 0;
         c7x::long_vec  c10 = (c7x::long_vec) 0;
         c7x::long_vec  c11 = (c7x::long_vec) 0;
         c7x::long_vec  c12 = (c7x::long_vec) 0;
         c7x::long_vec  c13 = (c7x::long_vec) 0;
         c7x::long_vec  c14 = (c7x::long_vec) 0;
         c7x::long_vec  c15 = (c7x::long_vec) 0;
         c7x::long_vec  c16 = (c7x::long_vec) 0;
         c7x::long_vec  c17 = (c7x::long_vec) 0;
         c7x::short_vec b0, b1, b2, b3, b4, b5, b6, b7;
         c7x::short_vec a0, a1;
         for (int32_t k = 0; k < KBlocks; k++) {
            loadMatSE<0>(&a0);
            loadMatSE<0>(&a1);

            loadMatSE<1>(&b0);
            loadMatSE<1>(&b1);
            loadMatSE<1>(&b2);
            loadMatSE<1>(&b3);

            loadMatSE<1>(&b4);
            loadMatSE<1>(&b5);
            loadMatSE<1>(&b6);
            loadMatSE<1>(&b7);

            b0 = __as_short16(__permute(vMask, __as_uchar32(b0)));
            b1 = __as_short16(__permute(vMask, __as_uchar32(b1)));
            b2 = __as_short16(__permute(vMask, __as_uchar32(b2)));
            b3 = __as_short16(__permute(vMask, __as_uchar32(b3)));

            b4 = __as_short16(__permute(vMask, __as_uchar32(b4)));
            b5 = __as_short16(__permute(vMask, __as_uchar32(b5)));
            b6 = __as_short16(__permute(vMask, __as_uchar32(b6)));
            b7 = __as_short16(__permute(vMask, __as_uchar32(b7)));

            c00 += __dotp4_ext(a0, b0);
            c01 += __dotp4_ext(a0, b1);
            c02 += __dotp4_ext(a0, b2);
            c03 += __dotp4_ext(a0, b3);

            c04 += __dotp4_ext(a0, b4);
            c05 += __dotp4_ext(a0, b5);
            c06 += __dotp4_ext(a0, b6);
            c07 += __dotp4_ext(a0, b7);

            c10 += __dotp4_ext(a1, b0);
            c11 += __dotp4_ext(a1, b1);
            c12 += __dotp4_ext(a1, b2);
            c13 += __dotp4_ext(a1, b3);

            c14 += __dotp4_ext(a1, b4);
            c15 += __dotp4_ext(a1, b5);
            c16 += __dotp4_ext(a1, b6);
            c17 += __dotp4_ext(a1, b7);
         }
         c7x::long_vec c00shfited = __shift_right(c00, (c7x::long_vec) qs);
         c7x::long_vec c01shfited = __shift_right(c01, (c7x::long_vec) qs);
         c7x::long_vec c02shfited = __shift_right(c02, (c7x::long_vec) qs);
         c7x::long_vec c03shfited = __shift_right(c03, (c7x::long_vec) qs);
         c7x::long_vec c04shfited = __shift_right(c04, (c7x::long_vec) qs);
         c7x::long_vec c05shfited = __shift_right(c05, (c7x::long_vec) qs);
         c7x::long_vec c06shfited = __shift_right(c06, (c7x::long_vec) qs);
         c7x::long_vec c07shfited = __shift_right(c07, (c7x::long_vec) qs);
         c7x::long_vec c10shfited = __shift_right(c10, (c7x::long_vec) qs);
         c7x::long_vec c11shfited = __shift_right(c11, (c7x::long_vec) qs);
         c7x::long_vec c12shfited = __shift_right(c12, (c7x::long_vec) qs);
         c7x::long_vec c13shfited = __shift_right(c13, (c7x::long_vec) qs);
         c7x::long_vec c14shfited = __shift_right(c14, (c7x::long_vec) qs);
         c7x::long_vec c15shfited = __shift_right(c15, (c7x::long_vec) qs);
         c7x::long_vec c16shfited = __shift_right(c16, (c7x::long_vec) qs);
         c7x::long_vec c17shfited = __shift_right(c17, (c7x::long_vec) qs);

         c7x::int_vec out01 = __vwpackl_vvv(c7x::as_int_vec(c01shfited), c7x::as_int_vec(c00shfited));
         c7x::int_vec out02 = __vwpackl_vvv(c7x::as_int_vec(c03shfited), c7x::as_int_vec(c02shfited));
         c7x::int_vec out03 = __vwpackl_vvv(c7x::as_int_vec(c05shfited), c7x::as_int_vec(c04shfited));
         c7x::int_vec out04 = __vwpackl_vvv(c7x::as_int_vec(c07shfited), c7x::as_int_vec(c06shfited));
         c7x::int_vec out11 = __vwpackl_vvv(c7x::as_int_vec(c11shfited), c7x::as_int_vec(c10shfited));
         c7x::int_vec out12 = __vwpackl_vvv(c7x::as_int_vec(c13shfited), c7x::as_int_vec(c12shfited));
         c7x::int_vec out13 = __vwpackl_vvv(c7x::as_int_vec(c15shfited), c7x::as_int_vec(c14shfited));
         c7x::int_vec out14 = __vwpackl_vvv(c7x::as_int_vec(c17shfited), c7x::as_int_vec(c16shfited));

         writeOutSA0(vPred, addr, pOutLocal, out01, out02);
         writeOutSA0(vPred, addr, pOutLocal, out03, out04);
         writeOutSA0(vPred, addr, pOutLocal, out11, out12);
         writeOutSA0(vPred, addr, pOutLocal, out13, out14);
      }
   }
   else {
      for (int32_t mn = 0; mn < MBlocks * NBlocks; mn++) {
         c7x::long_vec  c00 = (c7x::long_vec) 0;
         c7x::long_vec  c01 = (c7x::long_vec) 0;
         c7x::long_vec  c02 = (c7x::long_vec) 0;
         c7x::long_vec  c03 = (c7x::long_vec) 0;
         c7x::long_vec  c10 = (c7x::long_vec) 0;
         c7x::long_vec  c11 = (c7x::long_vec) 0;
         c7x::long_vec  c12 = (c7x::long_vec) 0;
         c7x::long_vec  c13 = (c7x::long_vec) 0;
         c7x::short_vec b0, b1, b2, b3;
         c7x::short_vec a0, a1;
         // printf("Kloop\n");
         for (int32_t k = 0; k < KBlocks; k++) {
            loadMatSE<0>(&a0);
            loadMatSE<0>(&a1);

            loadMatSE<1>(&b0);
            loadMatSE<1>(&b1);
            loadMatSE<1>(&b2);
            loadMatSE<1>(&b3);

            b0 = __as_short16(__permute(vMask, __as_uchar32(b0)));
            b1 = __as_short16(__permute(vMask, __as_uchar32(b1)));
            b2 = __as_short16(__permute(vMask, __as_uchar32(b2)));
            b3 = __as_short16(__permute(vMask, __as_uchar32(b3)));

            c00 += __dotp4_ext(a0, b0);
            c01 += __dotp4_ext(a0, b1);
            c02 += __dotp4_ext(a0, b2);
            c03 += __dotp4_ext(a0, b3);

            c10 += __dotp4_ext(a1, b0);
            c11 += __dotp4_ext(a1, b1);
            c12 += __dotp4_ext(a1, b2);
            c13 += __dotp4_ext(a1, b3);
         }
         c7x::long_vec c00shfited = __shift_right(c00, (c7x::long_vec) qs);
         c7x::long_vec c01shfited = __shift_right(c01, (c7x::long_vec) qs);
         c7x::long_vec c02shfited = __shift_right(c02, (c7x::long_vec) qs);
         c7x::long_vec c03shfited = __shift_right(c03, (c7x::long_vec) qs);

         c7x::int_vec out01 = __vwpackl_vvv(c7x::as_int_vec(c01shfited), c7x::as_int_vec(c00shfited));
         c7x::int_vec out02 = __vwpackl_vvv(c7x::as_int_vec(c03shfited), c7x::as_int_vec(c02shfited));

         c7x::long_vec c10shfited = __shift_right(c10, (c7x::long_vec) qs);
         c7x::long_vec c11shfited = __shift_right(c11, (c7x::long_vec) qs);
         c7x::long_vec c12shfited = __shift_right(c12, (c7x::long_vec) qs);
         c7x::long_vec c13shfited = __shift_right(c13, (c7x::long_vec) qs);

         c7x::int_vec out11 = __vwpackl_vvv(c7x::as_int_vec(c11shfited), c7x::as_int_vec(c10shfited));
         c7x::int_vec out12 = __vwpackl_vvv(c7x::as_int_vec(c13shfited), c7x::as_int_vec(c12shfited));

         writeOutSA0(vPred, addr, pOutLocal, out01, out02);
         writeOutSA0(vPred, addr, pOutLocal, out11, out12);
      }
   }
   __SE0_CLOSE();
   __SE1_CLOSE();
   __SA0_CLOSE();
   // printStridedMat<dataTypeIn>("C OPT", pKerPrivArgs->M, pKerPrivArgs->strideOutElements, (dataTypeIn*)pOut);

   return DSPLIB_SUCCESS;
}
template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I8S_O8S>(DSPLIB_kernelHandle handle,
                                                                               void *restrict pIn0,
                                                                               void *restrict pIn1,
                                                                               void *restrict pOut);

template DSPLIB_STATUS DSPLIB_matMul_fixed_exec_ci<DSPLIB_MATMAPY_FXD_I16S_O16S>(DSPLIB_kernelHandle handle,
                                                                                 void *restrict pIn0,
                                                                                 void *restrict pIn1,
                                                                                 void *restrict pOut);
