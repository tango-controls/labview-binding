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
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
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

#ifndef _NETWORK_H_
#define _NETWORK_H_


// ============================================================================
//! \page networkPage TCP and UDP client documentation
//! \tableofcontents
//! The network utilities provide a basic implementation of a client/server communication 
//! based on sockets and using a TCP/IP or UDP/IP protocol.
//!
//! \section secN1 TCP and UDP client utilities
//! The network utilities implement the key concepts of a client/server communication :
//! - a network address,
//! - a TCP/IP and UDP/IP network socket with its basic functions (open/close, read/write, bind, ...),
//! - a client socket,
//! - specific exceptions.
//!
//! \remark The Yat network utilities only implement a client socket because a server socket is
//! not necessary in its original environment of use (TANGO environment): the server side is often 
//! supplied by the equipment to manage. Moreover, the TANGO/CORBA layer provides the communication 
//! protocol between TANGO "Devices".
//!
//! \section secN2 Network classes
//! Links to network classes :
//!   - yat::Address
//!   - yat::ClientSocket
//!   - yat::Socket
//!   - yat::SocketException
// ============================================================================

#endif // _NETWORK_H_
