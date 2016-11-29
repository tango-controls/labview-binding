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

#ifndef _YAT_CONFIG_WIN32_H_
#define _YAT_CONFIG_WIN32_H_

#define YAT_WIN32

#if defined (_WIN64) || defined (WIN64)
# define YAT_WIN64
# define YAT_NO_INLINE_ASM
#endif

/**
 *  MSVC++ only!
 */
#if ! defined (_MSC_VER)
# error Sorry but there is currently no support for this compiler - use Microsoft Visual C++
#endif

/**
 *  Check MSVC++ version
 */
#if (_MSC_VER < 1310)
#  error "Microsoft Visual C++ >= 7.0 required"
#elif (_MSC_VER >= 1600)
  //-> config-win32-msvc-10
# define YAT_HAS_INTRINSIC_BYTESWAP
#elif (_MSC_VER >= 1500)
  //-> config-win32-msvc-9
# define YAT_HAS_INTRINSIC_BYTESWAP
#elif (_MSC_VER >= 1400)
  //-> config-win32-msvc-8
# define YAT_HAS_INTRINSIC_BYTESWAP
#elif (_MSC_VER >= 1310)
  //-> config-win32-msvc-7
# define YAT_HAS_INTRINSIC_BYTESWAP
#else
# error This version of Microsoft Visual C++ is not supported.
#endif

/**
 *  Win32 base types
 */
#include <Basetsd.h>

/**
 *  Some integer types.
 *  Deprecated. Use new definitions below
 */
typedef char yat_int8_t;
typedef unsigned char yat_uint8_t;

typedef short yat_int16_t;
typedef unsigned short yat_uint16_t;

#if defined (_WIN64) || defined (WIN64)
  typedef int yat_int32_t;
  typedef unsigned int yat_uint32_t;
#else
  typedef long yat_int32_t;
  typedef unsigned long yat_uint32_t;
#endif
#if defined (_WIN64) || defined (WIN64)
  typedef long yat_int64_t;
  typedef unsigned long yat_uint64_t;
#else
  typedef long long yat_int64_t;
  typedef unsigned long long yat_uint64_t;
#endif
/**
 * New type definitions
 */
namespace yat
{
  typedef char int8;
  typedef unsigned char uint8;
  typedef unsigned char byte;

  typedef short int16;
  typedef unsigned short uint16;

  #if defined (_WIN64) || defined (WIN64)
    typedef int int32;
    typedef unsigned int uint32;
  #else
    typedef long int32;
    typedef unsigned long uint32;
  #endif
  
  typedef __int64 int64;
  typedef unsigned __int64 uint64;

  typedef SSIZE_T ssize_t;

 /**
   * Simulation of Posix types
   */
  typedef int  mode_t;
  typedef long uid_t;
  typedef long gid_t;
  typedef int  fsid_t;
}

/**
 *  Disable some annoying warnings
 */
//- 'identifier' : identifier was truncated to 'number' characters
#pragma warning(disable:4786) 
//- 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2' 
#pragma warning(disable:4251) 
//- non � DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier' 
#pragma warning(disable:4275) 
//- C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable:4290)
//- 'function': was declared deprecated
#pragma warning(disable:4996)
//- 'function': possible loss of data
#pragma warning(disable:4267)
//- 'function': forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable:4800)
//- decorated template name truncated 
#pragma warning(disable:4503)


#ifndef _WIN32_WINNT
//- the following macro must be set sa as ::SignalObjectAndWait() to be defined
#define _WIN32_WINNT 0x400
#endif

#include <windows.h>

/**
 *  <sstream> library related stuffs
 */
#define YAT_HAS_SSTREAM

/**
 *  Shared library related stuffs
 */
# if defined(YAT_DLL)
#   define YAT_DECL_EXPORT __declspec(dllexport)
#   define YAT_DECL_IMPORT __declspec(dllimport)
#   if defined (YAT_BUILD)
#     define YAT_DECL YAT_DECL_EXPORT
#   else
#     define YAT_DECL YAT_DECL_IMPORT
#   endif
# elif defined(YAT_PLUGIN)
#   define YAT_DECL_EXPORT __declspec(dllexport)
#   define YAT_DECL_IMPORT __declspec(dllimport)
#   define YAT_DECL
# else
#   define YAT_DECL_EXPORT
#   define YAT_DECL_IMPORT
#   define YAT_DECL
# endif

/**
 *  Endianness related stuffs
 */
# if (_M_IX86 > 400) || defined (_M_X64)
#  define YAT_HAS_PENTIUM 1
#  define YAT_LITTLE_ENDIAN_PLATFORM 1
# else
#  error "no support for this processor"
# endif

/**
 *  64 bits stuffs
 */
#if defined (_M_X64)
# define YAT_64BITS
#endif

/**
 *  Object manager
 */
#define YAT_HAS_STATIC_OBJ_MANAGER 0

#endif
