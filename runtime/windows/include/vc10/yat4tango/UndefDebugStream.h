//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 The Tango Community
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2016 The Tango Community
//
// The YAT4Tango library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation; either version 2 of the License, or (at 
// your option) any later version.
//
// The YAT4Tango library is distributed in the hope that it will be useful,
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
 * \authors See AUTHORS file
 */

#ifndef _YAT4TANGO_UNDEF_DEBUG_STREAM_H_
#define _YAT4TANGO_UNDEF_DEBUG_STREAM_H_

// Undefine the DEBUG_STREAM macro because of too abusive use by Pogo
#ifdef DEBUG_STREAM
  #undef DEBUG_STREAM
#endif

namespace yat4tango
{
  // re-define the DEBUG_STREAM
  struct NullDebugStream : std::ostream
  {
    NullDebugStream(): std::ios(0), std::ostream(0) {}
  };
  template <typename T>
  NullDebugStream& operator<<(NullDebugStream& ns, T) { return ns; }
}

  yat4tango::NullDebugStream g_null_debug_stream;

  #define DEBUG_STREAM g_null_debug_stream

// DEBUG_STREAM substitution
#define Y4T_DEBUG_STREAM \
  if (get_logger()->is_debug_enabled()) \
    get_logger()->debug_stream() << log4tango::LogInitiator::_begin_log


#endif //- _YAT4TANGO_UNDEF_DEBUG_STREAM_H_ 
