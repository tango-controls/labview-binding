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

#ifndef _YAT_STRING_TOKENIZER_H_
#define _YAT_STRING_TOKENIZER_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "yat/CommonHeader.h"

namespace yat 
{

// ============================================================================
//! \class StringTokenizer 
//! \brief String "tokenizer" class.
//!
//! This class provides functions to read a string containing tokens separated by
//! a separator.\n
//! For instance: "Kilo:Lima:Mike:November"
// ============================================================================
class YAT_DECL StringTokenizer
{
public:
  //! \brief Constructor from string and separator.
  //! \param str The string.
  //! \param delim The separator.
  StringTokenizer (const std::string & str, const std::string & delim);

  //! \brief Destructor.
  ~StringTokenizer () {};

  //! \brief Counts remaining tokens in string.
  int count_tokens () const;

  //! \brief Tests if string has remaining tokens.
  bool has_more_tokens () const;

  //! \brief Gets next token as a string value.
  std::string next_token ();

  //! \brief Gets next token as an integer value.
  int next_int_token ();

  //! \brief Gets next token as a long value.
  long next_long_token ();

  //! \brief Gets next token as a double value.
  double next_fp_token ();

  //! \brief Gets next token as a string value, with specified separator.
  //!
  //! Searches fro next token using the specified separator.
  //! \param delim The separator.
  std::string next_token (const std::string & delim);

  //! \brief Gets the remaining string.
  std::string remaining_string ();

  //! \brief Erases the specified filter from next token.
  //!
  //! Returns the filtered token.
  //! \param filterStr %String to filter.
  std::string filter_next_token (const std::string & filterStr);

private:
  std::string m_delim_str;
  std::string m_token_str;
};

} //- namespace

#endif


