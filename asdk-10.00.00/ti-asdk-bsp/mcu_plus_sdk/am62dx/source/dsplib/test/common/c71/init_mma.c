//*****************************************************************************
//  Copyright (C) 2020 Texas Instruments Incorporated - http://www.ti.com/
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//      Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//      Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the
//      distribution.
//
//      Neither the name of Texas Instruments Incorporated nor the names of
//      its contributors may be used to endorse or promote products derived
//      from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************

//*****************************************************************************
//  init_mma.c
//
//  Description
//      Initialize the MMA prior to processing
//
//*****************************************************************************

#include "init_mma.h"

/* -------------------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not being         */
/*  addressed so as not  to lose portability across different platforms.      */
/* -------------------------------------------------------------------------- */
#ifdef WIN32
#define ASSIGN(param, value) value
#else
#define ASSIGN(param, value) .param = value
#endif

const __HWA_OFFSET_REG zerosOffsetRegStruct = {
    ASSIGN(offset0, 0),  ASSIGN(offset1, 0),  ASSIGN(offset2, 0),  ASSIGN(offset3, 0),  ASSIGN(A_LUT_VAL_0, 0),
    ASSIGN(offset4, 0),  ASSIGN(offset5, 0),  ASSIGN(offset6, 0),  ASSIGN(offset7, 0),  ASSIGN(A_LUT_VAL_1, 0),
    ASSIGN(offset8, 0),  ASSIGN(offset9, 0),  ASSIGN(offset10, 0), ASSIGN(offset11, 0), ASSIGN(A_LUT_VAL_2, 0),
    ASSIGN(offset12, 0), ASSIGN(offset13, 0), ASSIGN(offset14, 0), ASSIGN(offset15, 0), ASSIGN(A_LUT_VAL_3, 0),
    ASSIGN(offset16, 0), ASSIGN(offset17, 0), ASSIGN(offset18, 0), ASSIGN(offset19, 0), ASSIGN(A_LUT_VAL_4, 0),
    ASSIGN(offset20, 0), ASSIGN(offset21, 0), ASSIGN(offset22, 0), ASSIGN(offset23, 0), ASSIGN(A_LUT_VAL_5, 0),
    ASSIGN(offset24, 0), ASSIGN(offset25, 0), ASSIGN(offset26, 0), ASSIGN(offset27, 0), ASSIGN(A_LUT_VAL_6, 0),
    ASSIGN(offset28, 0), ASSIGN(offset29, 0), ASSIGN(offset30, 0), ASSIGN(offset31, 0), ASSIGN(A_LUT_VAL_7, 0),
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void init_mma()
{
#if !defined(_HOST_BUILD)
#if defined(EVM_TEST)
   // Is this still needed here, or does it get handled elsewhere in the system
   __HWA_CONFIG_REG_v1 mmaConfig = __gen_HWA_CONFIG_REG_v1();
   __HWA_OFFSET_REG    mmaOffset = zerosOffsetRegStruct;
   __HWAOPEN(mmaConfig, mmaOffset, __MMA_OPEN_FSM_RESET);
   __HWACLOSE(0);
#endif

#endif
}

#ifdef __cplusplus
}
#endif // extern "C"
