/*
 * Copyright (C) 2005-2011 by Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: MIT
 * 
 */

#include <bits/wordsize.h>

#if __WORDSIZE == 32

#ifdef _MIPS_SIM

#if _MIPS_SIM == _ABIO32
#include <freetype2/freetype/config/ftconfig-32.h>
#elif _MIPS_SIM == _ABIN32
#include <freetype2/freetype/config/ftconfig-n32.h>
#else
#error "Unknown _MIPS_SIM"
#endif

#else /* _MIPS_SIM is not defined */
#include <freetype2/freetype/config/ftconfig-32.h>
#endif

#elif __WORDSIZE == 64
#include <freetype2/freetype/config/ftconfig-64.h>
#else
#error "Unknown __WORDSIZE detected"
#endif /* matches #if __WORDSIZE == 32 */
  
