//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2014 The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
// The YAT library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details 
//
// Contact:
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \author See AUTHORS file
 */
#ifndef _YAT_COMMON_H_
#define _YAT_COMMON_H_

#if (defined (_DEBUG) || defined (DEBUG))
# define YAT_DEBUG
#endif

#ifndef YAT_PROJECT_VERSION
  #define YAT_PROJECT_VERSION "version not provided!"
#endif

#include <assert.h>
#include <yat/Portability.h>
#include <yat/Inline.h>
#include <yat/LogHelper.h>
#include <yat/Exception.h>

namespace yat 
{

// ============================================================================
// IMPL OPTION
// ============================================================================
#define ENABLE_TASK_STATS

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_MSG_TMO_MSECS 2000

//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define SAFE_DELETE_PTR(P) if (P) { delete P; P = 0; } else (void)0
//-----------------------------------------------------------------------------
#define SAFE_DELETE_ARRAY(P) if (P) { delete[] P; P = 0; } else (void)0
//-----------------------------------------------------------------------------
#define SAFE_RELEASE(P) if (P) { P->release(); P = 0; } else (void)0
//-----------------------------------------------------------------------------
#define _CPTC(X) static_cast<const char*>(X)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Free function fuzzy float point numbers comparison
//-----------------------------------------------------------------------------
template<typename T>
inline bool fp_is_equal(T a, T b, T precision)
{
  if( std::abs(a-b) < std::abs(precision) ) // work better near zero
    return true;

  if( std::abs(a) > std::abs(b) )
    return std::abs(a-b) <= ( std::abs(precision) * std::abs(a) );
  else
    return std::abs(b-a) <= ( std::abs(precision) * std::abs(b) );
}

//-----------------------------------------------------------------------------
// Expanding Macros into string constants
// The MACRO_TO_STRING macro calls _STR_EXPAND with its argument. The parameter is checked for macro
// expansions and evaluated by the preprocessor before being passed to _STR_EXPAND which quotes it.
//-----------------------------------------------------------------------------
#define __STR_EXPAND__(tok) #tok
#define YAT_MACRO_TO_STRING(tok) __STR_EXPAND__(tok)

//-----------------------------------------------------------------------------
// ASSERTION
//-----------------------------------------------------------------------------
#if defined (YAT_DEBUG)
# define DEBUG_ASSERT(EXP) assert(EXP)
#else
# define DEBUG_ASSERT(EXP)
#endif
#define YAT_ASSERT(EXP) assert(EXP)

}

#endif
