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

#ifndef _YAT_THREADING_UTILS_H_
#define _YAT_THREADING_UTILS_H_

// ----------------------------------------------------------------------------
// DEPENDENCIES
// ----------------------------------------------------------------------------
#include <yat/time/Timer.h>
#include <yat/threading/Implementation.h>

// ----------------------------------------------------------------------------
// CONSTs
// ----------------------------------------------------------------------------
#if defined(YAT_MACOSX)
# define YAT_INVALID_THREAD_UID 0
#else
# define YAT_INVALID_THREAD_UID 0xffffffff
#endif

namespace yat {

// ----------------------------------------------------------------------------
//! A dedicated type for thread identifier
// ----------------------------------------------------------------------------
#if defined(YAT_MACOSX)
  typedef _opaque_pthread_t * ThreadUID;
#else
  typedef unsigned long ThreadUID;
#endif
  
// ============================================================================
//! \class ThreadingUtilities 
//! \brief The YAT threading utilities.
//!
//! This class provides threading utilities such as thread Id and time management
//! functions.
// ============================================================================
class YAT_DECL ThreadingUtilities
{
public:
   //! \brief Returns the calling thread identifier.
  static ThreadUID self ();

  //! \brief Causes the caller to sleep for the given time.
  //! \param secs Time in seconds.
  //! \param nanosecs Nanoseconds precision of time.
  static void sleep (long secs, long nanosecs = 0);

  //! \brief Returns number of concurrent threads supported by the implementation.
  //! The value should be considered only a hint.
  //! If the value is not well defined or not computable, returns 0
  //! \note equivalent to c++11 std::thread::hardware_concurrency
  static unsigned int harware_concurrency();

  //! \brief Calculates an absolute time in seconds and nanoseconds, suitable for
  //! use in timed waits (ex: Condition, Semaphore), which is the current 
  //! time plus the given relative offset.
  //! \param abs_sec Absolute time in seconds.
  //! \param abs_nsec Nanoseconds precision of absolute time.
  //! \param offset_sec Offset in seconds.
  //! \param offset_nsec Nanoseconds precision of offset.
  static void get_time (unsigned long & abs_sec,
                        unsigned long & abs_nsec,
                        unsigned long offset_sec = 0,
                        unsigned long offset_nsec = 0);

  //! \brief Calculates an absolute time in seconds and nanoseconds, suitable for
  //! use in timed waits (ex: Condition, Semaphore), which is the current time 
  //! plus the given relative offset.
  //! \param abs_time Absolute time.
  //! \param offset_msecs Offset in milliseconds.
  static void get_time (Timespec & abs_time, unsigned long offset_msecs);

private:

#if defined (YAT_WIN32)
  //- internal impl
  static void ThreadingUtilities::get_time_now (unsigned long & abs_sec_, 
                                                unsigned long & abs_nano_sec_);
#endif  
};

} // namespace yat 

#endif //- _YAT_THREADING_UTILS_H_
