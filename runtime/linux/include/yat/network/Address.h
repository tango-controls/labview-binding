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
 
#ifndef _YAT_ADDRESS_H_
#define _YAT_ADDRESS_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <string>
#include <yat/CommonHeader.h>

namespace yat { 
  
// ============================================================================
//! \class Address 
//! \brief The YAT network address class.
//!
//! This class provides an implementation of a basic IP network adress. 
//! This address is defined by the following features:
//! - IP address or host name,
//! - Port number.
// ============================================================================
class YAT_DECL Address
{

public:
  //! \brief Constructs a peer address.
  //!
  //! \param host The host name or IP address.
  //! \param port The associated port number.
  //! \exception INVALID_HOST Thrown if could not resolve IP address for host.
  Address (const std::string& host, size_t port);

  //! \brief Constructs a peer address by copy.
  //!
  //! \param addr The address to be cloned.
  Address (const Address& addr);

  //! \brief Copies a peer address.
  //!
  //! \param addr The address to be copied.
  Address & operator= (const Address& addr);

  //! \brief Destructor. 
  virtual ~Address();
  
  //! \brief Returns host name.
  const std::string& get_host_name () const;

  //! \brief Returns IP address.
  const std::string& get_ip_address () const;

  //! \brief Returns port number.
  size_t get_port_number () const;
      
protected:
  //! \brief Resolves host name <-> ip_address.
  //!
  //! \param host The host name to resolve.
  //! \exception INVALID_HOST Thrown if could not resolve IP address for host.
  void ns_lookup (const std::string& host);

  //! \brief Port number.
  size_t m_port;

  //! \brief IP address.
  std::string m_ip_addr;

  //! \brief Host name.
  std::string m_host_name;
};

} // namespace yat 

#if defined (YAT_INLINE_IMPL)
# include <yat/network/Address.i>
#endif

#endif //- _YAT_ADDRESS_H_

