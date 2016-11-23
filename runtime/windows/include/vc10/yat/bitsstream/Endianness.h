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

//=============================================================================
// original authors..Aniruddha Gokhale, Carlos O'Ryan, ... (ACE lib)
// hackers...........N. Leclercq & J. Malik - SOLEIL

#ifndef  _YAT_ENDIANNESS_H_
#define  _YAT_ENDIANNESS_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/CommonHeader.h>

namespace yat
{

// ============================================================================
//! \class Endianness 
//! \brief Endianness management class.
//!
//! This class provides endianness swaping functions for different data types.
//! \remark The current implementation assumes that the host has 1-byte, 2-byte and 
//! 4-byte integral types, and that it has single precision and double precision 
//! IEEE floats. Those assumptions are pretty good these days, with Crays being
//! the only known exception.
// ============================================================================
class YAT_DECL Endianness
{
public:
  //! \brief Bytes ordering types.
  typedef enum
  {
    //! Little endian.
    BO_LITTLE_ENDIAN,
    //! Big endian.
    BO_BIG_ENDIAN
  } ByteOrder;
  
  //! \brief Host bytes ordering.
  static const Endianness::ByteOrder host_endianness;

  //! \brief Not host bytes ordering.
  static const Endianness::ByteOrder not_host_endianness;

  //! \brief Tests if bytes ordering is required for host platform.
  //!
  //! Returns true if the specified data bytes ordering type is different
  //! from local platform bytes ordering type, false otherwise.
  //! \param _data_bo Data bytes ordering type.
  inline static bool data_needs_bytes_reordering (const Endianness::ByteOrder & _data_bo)
  {
    return _data_bo != Endianness::host_endianness;
  }

  //! \brief Byte swapping for a 2-bytes scalar.
  //! \param orig The source scalar.
  //! \param target The swaped scalar.
  static void swap_2  (const char *orig, char *target);

  //! \brief Byte swapping for a 4-bytes scalar.
  //! \param orig The source scalar.
  //! \param target The swaped scalar.
  static void swap_4  (const char *orig, char *target);
  
  //! \brief Byte swapping for a 8-bytes scalar.
  //! \param orig The source scalar.
  //! \param target The swaped scalar.
  static void swap_8  (const char *orig, char *target);

  //! \brief Byte swapping for a 16-bytes scalar.
  //! \param orig The source scalar.
  //! \param target The swaped scalar.
  static void swap_16 (const char *orig, char *target);

  //! \brief Byte swapping for an array of 2-bytes elements.
  //! \param orig The source array.
  //! \param target The swaped array.
  //! \param length The array size.
  static void swap_2_array  (const char *orig, char *target, size_t length);

  //! \brief Byte swapping for an array of 4-bytes elements.
  //! \param orig The source array.
  //! \param target The swaped array.
  //! \param length The array size.
  static void swap_4_array  (const char *orig, char *target, size_t length);

  //! \brief Byte swapping for an array of 8-bytes elements.
  //! \param orig The source array.
  //! \param target The swaped array.
  //! \param length The array size.
  static void swap_8_array  (const char *orig, char *target, size_t length);

  //! \brief Byte swapping for an array of 16-bytes elements.
  //! \param orig The source array.
  //! \param target The swaped array.
  //! \param length The array size.
  static void swap_16_array (const char *orig, char *target, size_t length);
};

} //- namespace 


#endif //- _ENDIANNESS_H_
