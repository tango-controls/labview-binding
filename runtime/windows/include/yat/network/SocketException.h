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

#ifndef _YAT_SOCK_EX_H_
#define _YAT_SOCK_EX_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/Exception.h>

namespace yat 
{

//! \brief Socket specific error codes.
enum SocketError 
{
  //! No error.
  SoErr_NoError, 
  //! The receive buffer pointer points outside the process address space.
  SoErr_BadMemAddress,
  //! %Address is already in use (bind & connect).
  SoErr_AddressInUse, 
  //! %Address not available on machine (bind & connect).
  SoErr_AddressNotAvailable, 
  //! Invalid socket descriptor (socket).
  SoErr_BadDescriptor, 
  //! %Message signature is invalid.
  SoErr_BadMessage,
  //! Connection was closed (or broken) by other party.
  SoErr_ConnectionClosed, 
  //! Connection refused by server.
  SoErr_ConnectionRefused, 
  //! Datagram too long to send atomically.
  SoErr_DatagramTooLong, 
  //! Invalid option for socket protocol.
  SoErr_InvalidOption, 
  //! %Socket is already connected.
  SoErr_IsConnected, 
  //! %Socket is not connected.
  SoErr_NotConnected,
  //! Operation is not supported for this socket.
  SoErr_OpNotSupported, 
  //! User does not have access to privileged ports (bind).
  SoErr_PrivilegedPort, 
  //! %Time out was reached for operation (receive & send).
  SoErr_TimeOut, 
  //! Current operation is blocking (non-blocking socket).
  SoErr_WouldBlock,
  //! Operation in progress.
  SoErr_InProgress,
  //! Operation interrupted by OS event (signal).
  SoErr_OSInterrupt,
  //! Memory allocation failed.
  SoErr_OutOfMemory, 
  //! %Any other OS specific error.
  SoErr_Other
};

// ============================================================================
//! \class SocketException 
//! \brief The specific socket exception class.
//!
//! This class provides an implementation of specific socket exception.
//! It offers pre-formated socket error messages and error code conversion (native/YAT).\n
//! Inherits from Exception class.
// ============================================================================

class YAT_DECL SocketException : public Exception
{
public:
  //! \brief Constructor from an application error.
  //! \param reason %Error reason.
  //! \param desc %Error description.
  //! \param origin %Error origin.
  //! \param err_code %Native error code.
  //! \param severity %Error severity.
  SocketException ( const char *reason,
                    const char *desc,
                    const char *origin,
                    int err_code = -1,
                    int severity = yat::ERR);

  //! \brief Constructor from an application error.
  //! \param reason %Error reason.
  //! \param desc %Error description.
  //! \param origin %Error origin.
  //! \param err_code %Native error code.
  //! \param severity %Error severity.
  SocketException ( const std::string& reason,
                    const std::string& desc,
                    const std::string& origin, 
                    int err_code = -1, 
                    int severity = yat::ERR);
  
  //! \brief Destructor.
  virtual ~SocketException ();
  
  //! \brief Returns the YAT error code.
  int code () const;
  
  //! \brief Tests if specified code equals the exception error code.
  //!
  //! Returns true if \<_code\> is equal to the YAT error code, false otherwise.
  //! \param _code Code to test.
  bool is_a (int _code) const;

  //! \brief Returns the error text.
  std::string text () const;
  
  //! \brief Dumps the error towards logging output.
  virtual void dump () const;
    
  //! \brief Given a YAT yat::SocketError code, returns its associated text.
  //! \param _yat_err_code The error code.
  static std::string get_error_text (int _yat_err_code);

  //! \brief Converts from native to YAT yat::SocketError code.
  //! \param _os_err_code The native error code.
  static SocketError native_to_yat_error (int _os_err_code);
  
  //! \brief Converts from YAT to native yat::SocketError code.
  //! \param _yat_err_code The YAT error code.
  static int yat_to_native_error (SocketError _yat_err_code);
  
private:
  //- The native (i.e. platform specific error code).
  int m_yat_err_code;
};

} // namespace

#endif //- _YAT_SOCK_EX_H_
