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
// Copyright (C) 2006-2012  The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// see AUTHORS file 
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


#ifndef __YAT_URI_H__
#define __YAT_URI_H__

#include <map>
#include <yat/utils/String.h>

namespace yat
{
// ============================================================================
//! \class URI 
//! \brief Unified Resource Identifier class.
//!
//! This class is aimed to allow parsing and building URLs.
//! \par The URI
//! A URI is a compact sequence of characters that identifies an abstract or physical 
//! resource.
//! \par
//! URI syntax is defined by RFC 3986. 
//! \par
//! The generic URI syntax consists of a hierarchical sequence of
//!  components referred to as the scheme, authority, path, query, and fragment.
//! \par
//! The basic syntax is:
//! \verbatim  URI = scheme ":" hier-part \[ "?" query \] \[ "\#" fragment \] \endverbatim
//! 
// ============================================================================
class YAT_DECL URI
{
public:
  //! \brief URI component types.
  enum Part
  {
    //! Scheme.
    SCHEME = 0,
    //! Authority.
    AUTHORITY,
    //! User information.
    USERINFO,
    //! Host.
    HOST,
    //! Port.
    PORT,
    //! Path.
    PATH,
    //! Query.
    QUERY,
    //! Fragment.
    FRAGMENT
  };
  
  //! \brief URI components structure.
  struct Fields
  {
    std::string scheme;
    std::string userinfo;
    std::string host;
    std::string port;
    std::string path;
    std::string query;
    std::string fragment;
  };
  
private:
  std::map<Part, std::string> m_part;
  
  void parse(const std::string& URI) throw ( Exception );
  std::string value(Part part) const;
  static bool check_value(const std::string& value, const std::string &accepted_chars, const std::string& error_desc, bool throw_exception) throw ( Exception );
  static void split_authority(const std::string& authority, std::string* userinfo_ptr, std::string* host_ptr, std::string* port_ptr);
  static bool check_authority(const std::string& authority, URI::Fields* fields_ptr, bool throw_exception) throw ( Exception );

public:
  //! \brief Constructor from URI.
  //!
  //! Creates an URI object from the regular representation of the URL: a string.
  //! \param uri_string The URI value.
  //! \exception BAD_URI_SYNTAX Thrown if URI syntax is not correct.
  URI(const std::string& uri_string) throw ( Exception );
  
  //! \brief Constructor from explicit values.
  //! 
  //! Creates an URI object from explicit URI fields.
  //! \param fields URI components structure.
  //! \exception BAD_URI_SYNTAX Thrown if URI syntax is not correct.
  URI(const URI::Fields& fields) throw ( Exception );
  
  //! \brief Default constructor.
  URI() { }
  
  //! \brief Gets the URI as a string.
  std::string get() const;
  
  //! \brief Gets a specific part of the URI as a string.
  //!
  //! \param part The URI part to retrieve.
  std::string get(Part part) const;

  //! \brief Sets a specific part of the URI.
  //!
  //! \param part The URI part to set.
  //! \param value Value.
  //! \exception BAD_URI_SYNTAX Thrown if the value syntax is not correct.
  void set(Part part, const std::string &value) throw ( Exception ); 
  
  //! \brief Sets the URI.
  //! 
  //! \param value The URI value.
  //! \exception BAD_URI_SYNTAX Thrown if the URI syntax is not correct.
  void set(const std::string &value) throw ( Exception ); 
  
  //! \brief Checks the syntax of the value of an URI part.
  //!
  //! Returns true if check is OK, false otherwise.
  //! \param part The URI part to check.
  //! \param value The value to check.
  //! \param throw_exception If set to true, throws an exception if an error occurs.
  //! \exception BAD_URI_SYNTAX Thrown if the value syntax is not correct.
  static bool check(Part part, const std::string &value, bool throw_exception=false) throw ( Exception );
};


} // namespace

#endif