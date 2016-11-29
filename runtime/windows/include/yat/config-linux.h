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
// Copyright (C) 2006-2009  The Tango Community
//
// Part of the code comes from the ACE Framework
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
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

#ifndef _YAT_CONFIG_LINUX_H_
#define _YAT_CONFIG_LINUX_H_

#define YAT_LINUX

/**
 *  Some integer types.
 *  Deprecated. Use new definitions below
 */
#include <stdint.h>
typedef int8_t   yat_int8_t;            //CORBA::Char
typedef uint8_t  yat_uint8_t;           //CORBA::UChar
typedef int16_t  yat_int16_t;           //CORBA::Short
typedef uint16_t yat_uint16_t;          //CORBA::UShort
#if __WORDSIZE == 64
typedef int yat_int32_t;                //CORBA::Long
typedef unsigned int yat_uint32_t;      //CORBA::ULong
#else
typedef long yat_int32_t;               //CORBA::Long
typedef unsigned long yat_uint32_t;     //CORBA::ULong
#endif

#if __WORDSIZE == 64
typedef long yat_int64_t;                //CORBA::LongLong
typedef unsigned long yat_uint64_t;      //CORBA::ULongLong
#else
typedef long long yat_int64_t;           //CORBA::LongLong
typedef unsigned long long yat_uint64_t; //CORBA::ULongLong
#endif

/**
 * New type definitions
 */
namespace yat
{
  typedef int8_t   int8;
  typedef uint8_t  uint8;
  typedef uint8_t  byte; 
  typedef int16_t  int16;
  typedef uint16_t uint16;
  typedef int32_t  int32;
  typedef uint32_t uint32;
  typedef int64_t  int64;
  typedef uint64_t uint64;
}

/**
 *  Shared library related stuff
 */
#define YAT_DECL_EXPORT
#define YAT_DECL_IMPORT
#define YAT_DECL

/**
 *  GCC only!
 */
#if ! defined (__GNUC__)
# error Sorry but there is currently no support for this compiler
#endif

/**
 *  <sstream> library related stuffs
 */
#undef YAT_HAS_SSTREAM
#if __GNUC__ >= 3
# if __GNUC__ == 3
#   if __GNUC_MINOR__ >= 2
#     define YAT_HAS_SSTREAM
#   endif
# else
#   define YAT_HAS_SSTREAM
# endif
#endif

/**
 *  Pthread related stuffs
 */
#define YAT_HAS_PTHREAD_YIELD 1

/**
 *  Endianness related stuffs
 */
# if defined(i386) || defined(__i386__) || defined(__amd64__) || defined (__x86_64__)
#  define YAT_HAS_PENTIUM 1
#  define YAT_LITTLE_ENDIAN_PLATFORM 1
# else
#  error "no support for this processor"
# endif

/**
 *  64 bits stuffs
 */
#if defined (__amd64__) || defined (__x86_64__)
# define YAT_64BITS
#endif

/**
 *  ASM related stuffs
 */
#if defined (YAT_HAS_PENTIUM) || defined (__amd64__) || defined (__x86_64__)
# define YAT_HAS_INTEL_ASSEMBLY
#endif

/**
 *  Bytes swapping related stuffs
 */
#define YAT_HAS_BYTESWAP_H
#define YAT_HAS_BSWAP_16
#define YAT_HAS_BSWAP_32
#define YAT_HAS_BSWAP_64

/**
 *  Object manager
 */
#define YAT_HAS_STATIC_OBJ_MANAGER 0

#endif
