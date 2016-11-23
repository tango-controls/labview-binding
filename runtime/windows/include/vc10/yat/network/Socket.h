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

#ifndef _YAT_SOCKET_H_
#define _YAT_SOCKET_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/memory/DataBuffer.h>
#include <yat/network/Address.h>
#include <yat/network/SocketException.h>

// ============================================================================
// CONSTs
// ============================================================================
//! DEFAULT NUMBER OF READ BYTES
#define DEFAULT_RD_BYTES 512

namespace yat { 
  
// ============================================================================
//! \class Socket 
//! \brief The YAT network socket class.
//!
//! This class provides an implementation of a TCP/IP and UDP/IP network socket.
//! It offers basic socket functions (open/close, read/write, bind, listen, ...).\n
//! It is the base class for both TCP client socket and UDP client socket.
// ============================================================================
class YAT_DECL Socket
{

public:
  //! \brief OS socket descriptor.
#if defined(YAT_WIN64)
  typedef yat::uint64 OSDescriptor;
#elif defined(YAT_WIN32)
  typedef yat::uint32 OSDescriptor;
#else
  typedef int OSDescriptor;
#endif

  //! \brief Create a dedicated type for socket I/O data.
  typedef Buffer<char> Data;

  //! \brief Supported socket protocols.
  enum Protocol 
  {
    //! Transfer Control Protocol.
    TCP_PROTOCOL,
    //! User Datagram Protocol.
    UDP_PROTOCOL
  };
    
  //!  \brief %Socket options.
  enum Option 
  {
    //! Keep connection alive.
    SOCK_OPT_KEEP_ALIVE,
    //! Time to linger on close (in seconds).
    SOCK_OPT_LINGER, 
    //! Disable the Nagle algorithm for packet coalescing.
    SOCK_OPT_NO_DELAY,
    //! Socket protocol type.
    SOCK_OPT_PROTOCOL_TYPE,
    //! Allow reuse of a TCP address without delay.
    SOCK_OPT_REUSE_ADDRESS,
    //! Size of receive buffer (in bytes).
    SOCK_OPT_IBUFFER_SIZE,
    //! Time out period for receive operations (in seconds).
    SOCK_OPT_ITIMEOUT,
    //! Size of send buffer (in bytes).
    SOCK_OPT_OBUFFER_SIZE,
    //! Time out period for send operations (in seconds).
    SOCK_OPT_OTIMEOUT,
    //! private option (do not use directly).
    SOCK_OPT_OPAQUE_1
  };
    
  //! \brief YAT socket internal intialization cooking.
  //!
  //! Must be called prior to any other yat::Socket or yat::Address call.
  //! Call this from your main function or dll entry point. 
  //! \exception YAT_INTERNAL_ERROR Thrown if socket library initialization fails 
  //! (WINDOWS plateform only).
  static void init ();

  //! \brief YAT socket internal termination cooking.
  //!
  //! Must be called afetr any other yat::Socket or yat::Address call.
  //! Call this at the end of your main function or dll exit point.  
  static void terminate ();

  //! \brief Returns socket error status.
  //!
  //! See yat::SocketException::SocketError enum for returned values.
  //! \exception SOCKET_ERROR Thrown if access to socket options fails.
  int status () const;
  
  //! \brief Returns the socket protocol.
  Protocol get_protocol () const;
      
  //! \brief Returns the connected peer address.
  //!
  //! \exception SOCKET_ERROR Thrown if unable to retrieve peer address. 
  Address get_address () const;
      
  //! \brief Returns current value of the specified socket option.
  //!
  //! \param op The option name.
  //! \exception SOCKET_ERROR Thrown if invalid socket option specified.
  int get_option (Socket::Option op) const;

  //! \brief Switches socket I/O to blocking mode.
  //!
  //! \exception SOCKET_ERROR Thrown if acess to socket I/O control fails.
  void set_blocking_mode ();

  //! \brief Switches socket I/O to non-blocking mode.
  //!
  //! \exception SOCKET_ERROR Thrown if acess to socket I/O control fails.
  void set_non_blocking_mode ();

  //! \brief Changes the value of the specified socket option.
  //!
  //! \param op The option name.
  //! \param value New value for the specified option.
  //! \exception SOCKET_ERROR Thrown if invalid or unsupported socket option specified.
  //! \remark Most options are associated to booleans. Some exceptions however...
  //!  SOCK_OPT_ITIMEOUT and SOCK_OPT_OTIMEOUT should be specified in milliseconds.
  void set_option (Socket::Option op, int value = 0);
  
  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! Returns the actual number of bytes received.
  //! \param ib The buffer in which the data will be placed.
  //! \param nb The number of bytes to read.
  //! \exception SOCKET_ERROR Thrown when:
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  size_t receive (char * ib, size_t nb);

  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! Returns the actual number of bytes received.
  //! \param ib The buffer in which the data will be placed.
  //! \exception SOCKET_ERROR Thrown when:
  //! - destination buffer has a null capacity or
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  //! \remarks Reads up to \c ib.capacity() bytes on the socket. Upon return,
  //!  \c ib.length() will give the actual number of bytes read.
  size_t receive (Socket::Data & ib);
      
  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! Returns the actual number of bytes received.
  //! \param is The string in which the data will be placed.
  //! \exception SOCKET_ERROR Thrown when:
  //! - buffer reallocation fails or
  //! - string assignment fails or
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  //! \remarks Read up to \c yat::Socket::MAX_RD_BYTES bytes.
  //! Upon return, the string size will equal the actual number of bytes read. 
  size_t receive (std::string & is);

  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! Returns the actual number of bytes received.
  //! \param ib The buffer in which the data will be placed.
  //! \param nb The number of bytes to read.
  //! \exception SOCKET_ERROR Thrown when:
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  size_t receive_from (char * ib, size_t nb, yat::Address * src_addr = 0);

  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! Returns the actual number of bytes received.
  //! \param ib The buffer in which the data will be placed.
  //! \exception SOCKET_ERROR Thrown when:
  //! - destination buffer has a null capacity or
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  //! \remarks Reads up to \c ib.capacity() bytes on the socket. Upon return,
  //!  \c ib.length() will give the actual number of bytes read.
  size_t receive_from (Socket::Data & ib, yat::Address * src_addr = 0);

  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! Returns the actual number of bytes received.
  //! \param is The string in which the data will be placed.
  //! \exception SOCKET_ERROR Thrown when:
  //! - buffer reallocation fails or
  //! - string assignment fails or
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  //! \remarks Read up to \c yat::Socket::MAX_RD_BYTES bytes.
  //! Upon return, the string size will equal the actual number of bytes read.
  size_t receive_from (std::string & is, yat::Address * src_addr = 0);
  
  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! \param ib The buffer in which the data will be placed.
  //! \exception SOCKET_ERROR Thrown when:
  //! - destination buffer has a null capacity or
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  //! \remarks Reads \c ib.length() bytes from the socket.
  size_t operator>> (Socket::Data & ib);

  //! \brief Receives (i.e. reads) data from the socket.
  //!
  //! \param is The string in which the data will be placed.
  //! \exception SOCKET_ERROR Thrown when:
  //! - buffer reallocation fails or
  //! - string assignment fails or
  //! - connection has been reset/closed by peer or
  //! - operation may block the caller (for non blocking socket) or
  //! - reception fails.
  //! \remarks Reads up to \c yat::Socket::MAX_RD_BYTES bytes.
  //! Upon return, the string size will equal the actual number of bytes read. 
  size_t operator>> (std::string& is);

  //! \brief Sends (i.e. writes) data to the socket.
  //!
  //! \param ob The buffer containing the data to be sent.
  //! \param nb The number of bytes to write.
  //! \exception SOCKET_ERROR Thrown when:
  //! - operation may block the caller (for non blocking socket) or
  //! - connection has been reset/closed by peer or
  //! - emission fails.
  void send (const char * ob, size_t nb);

  //! \brief Sends (i.e. writes) data to the socket.
  //!
  //! \param ob The buffer containing the data to be sent.
  //! \exception SOCKET_ERROR Thrown when:
  //! - operation may block the caller (for non blocking socket) or
  //! - connection has been reset/closed by peer or
  //! - emission fails.
  //! \remarks Writes \c ob.length() bytes onto the socket.
  void send (const Socket::Data & ob);
      
  //! \brief Sends (i.e. writes) data to the socket.
  //!
  //! \param os The string containing the data to be sent.
  //! \exception SOCKET_ERROR Thrown when:
  //! - operation may block the caller (for non blocking socket) or
  //! - connection has been reset/closed by peer or
  //! - emission fails.
  void send (const std::string & os);

  //! \brief Sends (i.e. writes) data to the socket.
  //!
  //! \param ob The buffer containing the data to be sent.
  //! \exception SOCKET_ERROR Thrown when:
  //! - operation may block the caller (for non blocking socket) or
  //! - connection has been reset/closed by peer or
  //! - emission fails.
  //! \remarks Sends \c ob.length() bytes on the socket.
  void operator<< (const Socket::Data & ob);

  //! \brief Sends (i.e. writes) data to the socket.
  //!
  //! \param os The string containing the data to be sent.
  //! \exception SOCKET_ERROR Thrown when:
  //! - operation may block the caller (for non blocking socket) or
  //! - connection has been reset/closed by peer or
  //! - emission fails.
  //! \remarks Send \c os.size() bytes on the socket.
  void operator<< (const std::string& os);
  
  //! \brief Sets default read buffer size (in bytes).
  //!
  //! \param dbs Default buffer size (in bytes).
  static void default_rd_buffer_size (size_t dbs);
  
  //! \brief Join the specified multicast group (on ANY network interface)
  //!
  //! \param multicast_group_addr Multicast address of the group to join.
  void join_multicast_group (const yat::Address& multicast_group_addr);
  
  //! \brief Join the specified multicast group on the specified local interface
  //!
  //! \param multicast_group_addr Multicast address of the group to join.
  //! \param local_interface_addr Local network interface.
  void join_multicast_group (const yat::Address& multicast_group_addr, const yat::Address& local_interface_addr);
  
  //! \brief Attach the socket to the specified network interface (i.e. set the IP_MULTICAST_IF socket option)
  //!
  //! \param local_interface_addr The local network interface.
  void attach_to_network_interface (const yat::Address& local_interface_addr);
  
protected:
  //! \brief Constructs new socket.
  //! 
  //! \param p The associated protocol. Defaults to \c yat::TCP_PROTOCOL.
  //! \exception SOCKET_ERROR Thrown when:
  //! - invalid protocol specified or
  //! - socket descriptor instanciation fails.
  explicit Socket (Protocol p = TCP_PROTOCOL);
      
  //! \brief Releases any allocated resource.
  virtual ~Socket ();
  
  //! \brief Instanciates the underlying OS socket descriptor.
  //!
  //! Called from Socket::Socket (constructor).
  //! \exception SOCKET_ERROR Thrown when:
  //! - invalid protocol specified or
  //! - socket descriptor instanciation fails.
  void open ();

  //! \brief Closes the underlying OS socket descriptor.
  //!
  //! \exception SOCKET_ERROR Thrown when socket descriptor closing fails.
  void close ();

  //! \brief Listens to incoming connection.
  //! \param n Maximum number of connections.
  //! \exception SOCKET_ERROR Thrown when socket listening fails.
  void listen_to_incoming_connections (size_t n = 1);

  //! \brief Accepts connection on bound port.
  //!
  //! \exception SOCKET_ERROR Thrown when connection acceptance fails.
  OSDescriptor accept_incoming_connections ();
      
  //! \brief Connects to peer socket.
  //!
  //! \param a The peer address.
  //! \exception SOCKET_ERROR Thrown when call for host fails.
  void connect (const Address & a);

  //! \brief Binds the socket to the specified port.
  //!
  //! \param p The port on which the socket is to be binded.
  //! \exception SOCKET_ERROR Thrown when call for host fails.
  //! \remarks May fail if port \<p\> is already reserved by another process or thread. 
  //! It may also fail after the connection is released (see option SOCK_OPT_REUSE_ADDRESS).
  void bind (size_t p);
      
  //! \brief Waits for activity on the socket (timeout in milliseconds).
  //!
  //! Returns true if there is some activity on the socket before timeout expires.
  //! \param _tmo_msecs The timeout in milliseconds (0 means no timeout - returns immediatly).
  //! \exception SOCKET_ERROR Thrown when select fails.
  bool select (size_t _tmo_msecs = 0);
    
  //! \brief Given a YAT socket option, returns the associated native socket option.
  //!
  //! \param o YAT socket option.
  //! \exception SOCKET_ERROR Thrown if invalid socket option specified.
  int yat_to_native_option (Option o) const;
    
  //! \brief Given a YAT socket option, returns its associated level.
  //!
  //! Returns the level to use for the specified option in the socket \c getsockopt() function.
  //! \param o YAT socket option.
  int option_level (Option o) const;
  
private:
  //- The associated protocol.
  Protocol m_protocol;

  //- The underlying OS socket descriptor.
  OSDescriptor m_os_desc;

  //- Returns true is the current operation would block, returns 
  //- false otherwise.
  bool current_op_is_blocking ();

  //- Data buffer used in some send/receive operations. 
  Socket::Data m_buffer;

  //- Init done flag.
  static bool init_done;

  //- Default read buffer size.
  static size_t m_default_rd_buffer_size;
 
  //- Not implemented private member.
  Socket (const Socket&);

  //- Not implemented private member.
  Socket& operator= (const Socket&);
};  
  
} //-  namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/network/Socket.i>
#endif // YAT_INLINE_IMPL

#endif //- _YAT_SOCKET_H_
