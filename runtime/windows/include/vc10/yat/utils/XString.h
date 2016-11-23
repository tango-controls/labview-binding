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

#ifndef _YAT_XSTRING_H_
#define _YAT_XSTRING_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
//! \class XString 
//! \brief Generic numeric string class.
//!
//! This template class provides conversion functions for a generic \<_T\> "numeric"
//! string.
// ============================================================================
template <typename _T>
class XString
{
public:  
  
  //! \brief Converts string content to numeric type \<_T\>.
  //! 
  //! Returns the converted string in numeric type \<_T\>.
  //! Should also work for any "istringstream::operator>>" supported type.
  //! \param _s The string to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  static _T to_num (const std::string& _s, bool _throw = true)
  {
    std::istringstream iss(_s.c_str());

    _T num_val;

    iss >> num_val;

    if ( iss.fail() )
    {
      if (_throw)
      {
        OSStream desc;
        desc << "conversion from string to num failed [" 
             << _s
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "XString::to_num");
      }
      return 0;
    }

    return num_val;
  } 

  //! \brief Converts from type \<_T\> to std::string.
  //!
  //! \param _t The \<_T\> type value to convert.
  //! \param _throw If set to true, throws an exception if conversion fails.
  //! \exception SOFTWARE_ERROR Thrown if conversion fails.
  static std::string to_string (const _T & _t, bool _throw = true)
  {
    std::ostringstream oss;

    oss << std::fixed << _t;
    
    if ( oss.fail() )
    {
      if (_throw)
      {
        OSStream desc;
        desc << "conversion from num to string failed [" 
             << _t
             << "]"
             << std::ends;
        THROW_YAT_ERROR ("SOFTWARE_ERROR",
                         desc.str().c_str(),
                         "XString::to_string");
      }
      return std::string("");
    }

    return oss.str();
  } 

};

} //- namespace

#endif // _XSTRING_H_
