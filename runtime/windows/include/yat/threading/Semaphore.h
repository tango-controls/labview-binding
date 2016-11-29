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

#ifndef _YAT_SEMAPHORE_H_
#define _YAT_SEMAPHORE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Implementation.h>
#if ! defined(WIN32)
# include <yat/threading/Mutex.h>
# include <yat/threading/Condition.h>
#endif

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_SEMAPHORE_IMPLEMENTATION)
# error "implementation header file incomplete [no semaphore implementation]"
#endif

namespace yat {

// ============================================================================
//! \enum yat::SemaphoreState 
//! \brief Semaphore state enumeration.
//!
//! The Semaphore::try_wait() may return one of the SemaphoreState value.
// ============================================================================
typedef enum
{
  //! semaphore is currently "decrementable".
  SEMAPHORE_DEC,
  //! no resource available (semaphore value is 0).
  SEMAPHORE_NO_RSC,
} SemaphoreState;

// ============================================================================
//! \class Semaphore
//! \brief The YAT basic semaphore implementation.
//!
//! This class is not supposed to be derived.
// ============================================================================
class YAT_DECL Semaphore
{

public:
  //! \brief Constructor (may throws an Exception).
  //! \param initial Initial semaphore value.
  Semaphore (unsigned int initial = 1);

  //! \brief Destructor.
  ~Semaphore ();

  //! \brief Waits for semaphore to be signaled.
  //!
  //! If semaphore value is > 0 then decrements it and carries on. 
  //! If semaphore value is already 0 then blocks until the semaphore 
  //! is "signaled" by another thread (with Semaphore::post() function).
  void wait ();

  //! \brief Waits for semaphore to be signaled until specified timeout elapses.
  //!
  //! If semaphore value is > 0 then decrements it and returns "true". 
  //! Returns "false" in case the specified timeout expired before the semaphore
  //! has been "signaled" by another thread.
  //! \param tmo_msecs Timeout in ms.
  bool timed_wait (unsigned long tmo_msecs);

  //! \brief Gets semaphore if it is signaled or gives up.
  //! 
  //! If the current semaphore value is > 0, then decrements it and returns 
  //! SEMAPHORE_DEC. In case the semaphore value is already 0, 
  //! this method does not block and "immediately" returns SEMAPHORE_NO_RSC.
  SemaphoreState try_wait ();

  //! \brief Signals the semaphore to other threads.
  //!
  //! If any threads are blocked in wait(), wakes one of them up. 
  //! Otherwise increments the value of the semaphore. 
  void post ();

private:
  //- Not implemented private member
  Semaphore (const Semaphore&);
  
  //- Not implemented private member
  Semaphore & operator= (const Semaphore&);
  
  //- platform specific implementation
  YAT_SEMAPHORE_IMPLEMENTATION;
};

// ============================================================================
//! \class AutoSemaphore
//! \brief The YAT "auto semaphore" class.
//!
//! An "auto semaphore" providing an auto wait/post mechanism implemented in the
//! object construction and destruction functions.
// ============================================================================
class YAT_DECL AutoSemaphore
{
public:
  //! \brief Constructor (waits on the associated Semaphore).
  AutoSemaphore (Semaphore & _sem)
    : m_sem (_sem)
  {
    m_sem.wait();
  }

  //! \brief Destructor (posts the associated Semaphore).
  ~AutoSemaphore ()
  {
    m_sem.post();
  }

private:
  //- The associated Mutex
  Semaphore & m_sem;

  //- Not implemented private member
  AutoSemaphore (const AutoSemaphore&);
  
  //- Not implemented private member
  AutoSemaphore & operator= (const AutoSemaphore&);
};

} // namespace yat

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtSemaphoreImpl.i>
# else
#  include <yat/threading/impl/PosixSemaphoreImpl.i>
# endif
#endif

#endif //- _YAT_SEMAPHORE_H_
