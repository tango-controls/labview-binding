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

namespace yat
{

// ----------------------------------------------------------------------------
// Socket::operator>>
// ----------------------------------------------------------------------------
YAT_INLINE size_t Socket::operator>> (Socket::Data & ib)
{
  return this->receive(ib);
}

// ----------------------------------------------------------------------------
// Socket::operator>>
// ----------------------------------------------------------------------------
YAT_INLINE size_t Socket::operator>> (std::string & data_str)
{
  return this->receive(data_str);
}

// ----------------------------------------------------------------------------
// Socket::send
// ---------------------------------------------------------------------------- 
YAT_INLINE void Socket::operator<< (const Socket::Data & ob)
{
  this->send(ob);
}

// ----------------------------------------------------------------------------
// Socket::send
// ---------------------------------------------------------------------------- 
YAT_INLINE void Socket::operator<< (const std::string& os)
{
  this->send(os);
}

} // namespace
