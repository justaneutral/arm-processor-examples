/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
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

#include "c7x_scalable.h"
#define ELEMENT_COUNT(x) c7x::element_count_of<x>::value
/* the type of data that 'type x data structure' (i.e. vecType) holds */
#define ELEMENT_TYPE(x) typename c7x::element_type_of<x>::type

/******************************************************************************/
/*                                                                            */
/* Includes                                                                   */
/*                                                                            */
/******************************************************************************/

#include <cstddef>
#include <float.h>

#include "MATHLIB_asinh_scalar.h"
#include "MATHLIB_ilut.h"
#include "MATHLIB_lut.h"
#include "MATHLIB_permute.h"
#include "MATHLIB_types.h"
#include "MATHLIB_utility.h"

/******************************************************************************/
/*                                                                            */
/* MATHLIB_asinh                                                              */
/*                                                                            */
/******************************************************************************/

// this method performs iterative scalar asinh computation
template <typename T> void MATHLIB_asinh_scalar(size_t length, T *restrict pSrc, T *restrict pDst)
{
   for (uint32_t i = 0; i < length; i++) {
      pDst[i] = asinhsp_i(pSrc[i]);
   }
}

// this method is top level module of asinh, deploys scalar or vector version
template <typename T> MATHLIB_STATUS MATHLIB_asinh(size_t length, T *restrict pSrc, T *restrict pDst)
{
   MATHLIB_STATUS status = MATHLIB_SUCCESS; // return function status

   status = MATHLIB_checkParams(length, pSrc, pDst);

   if (status == MATHLIB_SUCCESS) {
      MATHLIB_asinh_scalar(length, pSrc, pDst);
   }

   return status;
}
/******************************************************************************/
/*                                                                            */
/* Explicit templatization for datatypes supported by MATHLIB_asinh           */
/*                                                                            */
/******************************************************************************/

// single precision
template MATHLIB_STATUS MATHLIB_asinh<float>(size_t length, float *pSrc, float *pDst);

/******************************************************************************/
/*                                                                            */
/* C-interface wrapper functions                                              */
/*                                                                            */
/******************************************************************************/

extern "C" {

// single-precision wrapper
MATHLIB_STATUS MATHLIB_asinh_sp(size_t length, float *pSrc, float *pDst)
{
   MATHLIB_STATUS status = MATHLIB_asinh(length, pSrc, pDst);
   return status;
}

} // extern "C"
