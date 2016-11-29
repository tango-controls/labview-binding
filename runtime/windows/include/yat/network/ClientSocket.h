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

#ifndef _YAT_CLIENT_SOCKET_H_
#define _YAT_CLIENT_SOCKET_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/network/Socket.h>

namespace yat { 
  
// ============================================================================
//! \class ClientSocket 
//! \brief The YAT client socket class.
//!
//! This class provides an implementation of a TCP/IP and UDP/IP client socket.
//! It offers basic client socket functions (open/close, read/write, bind, ...).\n
//! It inherits from Socket class.
// ============================================================================
class YAT_DECL ClientSocket : public Socket
{

public:
  //! \brief ClientSocket connection status.
  typedef enum {
    //! Connected.
    CONNECTED_YES,
    //! Not connected.
    CONNECTED_NO
  } ConnectionStatus;

  //! \brief Constructs new ClientSocket.
  //! 
  //! \param p The associated protocol. Defaults to \c yat::TCP_PROTOCOL.
  //! \exception SOCKET_ERROR Thrown when:
  //! - invalid protocol specified or
  //! - socket descriptor instanciation fails.
  ClientSocket (Protocol p = TCP_PROTOCOL);
      
  //! \brief Releases any allocated resource.
  virtual ~ClientSocket ();

  //! \brief Binds socket to specified port.
  //! 
  //! \param _p The port on which the socket is to be binded
  //! \exception SOCKET_ERROR Thrown when call for host fails.
  //! \remarks May fail if port \<p\> is already reserved by another process or thread. 
  //! It may also fail after the connection is released (see option SOCK_OPT_REUSE_ADDRESS).
  void bind (size_t _p = 0);
    
  //! \brief Connects to peer socket.
  //!
  //! \param a The peer address.
  //! \exception SOCKET_ERROR Thrown when call for host fails.
  void connect (const Address & a);

  //! \brief Disonnects from peer socket.
  //!
  //! \exception SOCKET_ERROR Thrown when socket descriptor closing fails.
  void disconnect ();
    
  //! \brief Could we read without blocking?
  //!
  //! Returns true if there is some input data pending, false otherwise.
  //! \exception SOCKET_ERROR Thrown when select fails.
  bool can_read_without_blocking ();

  //! \brief Waits till some data is available for reading or timeout expires.
  //!
  //! \param _tmo_msecs The timeout in milliseconds (0 means no timeout - return immediatly).
  //! \param _throw_exception Throws an exception in case timeout expires.
  //! \exception SOCKET_ERROR Thrown when:
  //! - select fails or 
  //! - timeout expires (if \<_throw_exception\> set to true).
  bool wait_input_data (size_t _tmo_msecs, bool _throw_exception = true);

  //! \brief Is this client socket currently connected?
  //!
  //! Returns the connection status: ClientSocket::CONNECTED_YES or ClientSocket::CONNECTED_NO.
  inline ConnectionStatus connection_status () const 
  {
    return this->m_connection_status;
  }

private:
  //- Connection status.
  ConnectionStatus m_connection_status;
};  
  
} //-  namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/network/ClientSocket.i>
#endif // YAT_INLINE_IMPL

#endif //- _YAT_CLIENT_SOCKET_H_
