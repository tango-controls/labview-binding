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

#ifndef _YAT_MUTEX_H_
#define _YAT_MUTEX_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/time/Timer.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Implementation.h>

// ----------------------------------------------------------------------------
// Implementation-specific header file.
// ----------------------------------------------------------------------------
#if ! defined(YAT_MUTEX_IMPLEMENTATION)
# error "implementation header file incomplete [no mutex implementation]"
#endif

namespace yat {

// ============================================================================
//! \enum yat::MutexState 
//! \brief Mutex state enumeration.
//! The Mutex::try_lock() function may return one of the MutexState value.
// ============================================================================
typedef enum
{
  MUTEX_LOCKED,
  MUTEX_BUSY,
} MutexState;

// ============================================================================
//! \class NullMutex
//! \brief The YAT NullMutex class.
//!
//! Provides a "do nothing" Mutex implementation. May be used as template argument
//! in order to control the template instanciation and avoiding locking
//! overhead where thread safety is not required.\n
//!    template <typename LOCK> class OptionalThreadSafetyImpl\n
//!    {\n
//!    public:\n
//!      inline void do_something ()\n
//!      {\n
//!         yat::AutoMutex<LOCK>(this->m_mutex);\n
//!         ...\n
//!      }\n
//!    private:\n
//!      LOCK m_mutex;\n
//!    };\n
//!
//! OptionalThreadSafetyImpl<yat::Mutex> will be thread safe while...
//! OptionalThreadSafetyImpl<yat::NullMutex> will not be!
//!
//! \remark This class is not supposed to be derived.
// ============================================================================
class YAT_DECL NullMutex
{

public:
  //! \brief Constructor.
  NullMutex ();

  //! \brief Destructor.
  ~NullMutex ();

  //! \brief Locks (i.e. acquires) the mutex.
  void lock ();

  //! \brief Locks (i.e. acquires) the mutex. Synonym of lock().
  void acquire ();

  //! \brief Tries to lock (i.e. acquire) the mutex. 
  //!
  //! Always returns MUTEX_LOCKED.
  MutexState try_lock ();

  //! \brief Tries to lock (i.e. acquire) the mutex. Synonym of try_lock().
  //!
  //! Always returns MUTEX_LOCKED.
  MutexState try_acquire ();

  //! \brief Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! Always returns MUTEX_LOCKED.
  //! \param tmo_msecs The "dummy" timeout in milliseconds.
  MutexState timed_try_lock (unsigned long tmo_msecs);

  //! \brief Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! Synonym of timed_try_lock().
  //! Always returns MUTEX_LOCKED.
  //! \param tmo_msecs The "dummy" timeout in milliseconds.
  MutexState timed_try_acquire (unsigned long tmo_msecs);

  //! \brief Unlocks (i.e. releases) the mutex.
  void unlock ();

  //! \brief Unlocks (i.e. releases) the mutex. Synonym of unlock().
  void release ();

private:
  //- Not implemented private member
  NullMutex (const NullMutex&);
  
  //- Not implemented private member
  NullMutex & operator= (const NullMutex&);
};

// ============================================================================
//! \class Mutex
//! \brief The yat basic mutex implementation.
//!
//! This class is not supposed to be derived (no virtual destructor).
// ============================================================================
class YAT_DECL Mutex
{

public:
  //! \brief Constructor.
  Mutex ();

  //! \brief Destructor.
  ~Mutex ();

  //! \brief Locks (i.e. acquires) the mutex.
  void lock ();

  //! \brief Locks (i.e. acquires) the mutex. Synonym of lock().
  void acquire ();

  //! \brief Tries to lock (i.e. acquire) the mutex.
  //!
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if it is already owned by another thread.
  MutexState try_lock ();
  
  //! \brief Tries to lock (i.e. acquire) the the mutex. Synonym of try_lock().
  //!
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if it is already owned by another thread.
  MutexState try_acquire ();

  //! \brief Tries to lock (i.e. acquire) the mutex within the specified time.
  //!
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if still owned by another thread after tmo expiration.
  //!
  //! \param tmo_msecs The timeout in milliseconds.
  MutexState timed_try_lock (unsigned long tmo_msecs);

  //! \brief Tries to lock (i.e. acquire) the mutex within the specified time.
  //! Synonym of timed_try_lock().
  //!
  //! Returns MUTEX_LOCKED in case the mutex was successfully locked.
  //! Returns MUTEX_BUSY if still owned by another thread after tmo expiration.
  //!
  //! \param tmo_msecs The timeout in milliseconds.
  MutexState timed_try_acquire (unsigned long tmo_msecs);

  //! \brief Unlocks (i.e. releases) the mutex.
  void unlock ();
  
  //! \brief Unlocks (i.e. releases) the mutex.
  //! Synonym of unlock().
  void release ();

private:
  //- Not implemented private member
  Mutex (const Mutex&);
  
  //- Not implemented private member
  Mutex & operator= (const Mutex&);

  //- platform specific implementation
  YAT_MUTEX_IMPLEMENTATION;
};

// ============================================================================
//! \class AutoMutex
//! \brief The YAT "auto mutex" class.
//!
//! The AutoMutex class provides an auto lock/unlock mechanism.\n
//!
//! The AutoMutex is ideal in context where some exceptions may be thrown.
//! Whatever is the exit path of your code, the AutoMutex class will garantee
//! that the associated mutex is properly unlock.\n
//!
//! This class is a template since it may be used in contexts in which the
//! thread safety is optionnal (see yat::NullMutex for an example).
//!
//! AutoMutex provides an efficient and safe alternative to:\n
//!    { //- enter critical section\n
//!      my_mutex.lock();\n
//!      ...your critical section code goes here (may throw an exception)...\n
//!      my_mutex.unlock();\n
//!    } //- leave critical section \n
//!
//! In such a context, you can use a instance AutoMutex as follows:\n
//!    { //- enter critical section\n
//!      yat::AutoMutex<> guard(my_mutex);\n
//!      ...your critical section code goes here (may throw an exception)...\n
//!    } //- leave critical section \n
//! \n
//! This has the advantage that my_mutex.unlock() will be called automatically
//! even if an exception is thrown. Since the AutoMutex is created on the stack,
//! its destructor will be called whatever is the exit path of critical section.\n
//!
//! \remark Note that the AutoMutex type can be used with any "LOCK_TYPE" whose 
//! interface contains both a lock() and a unlock() method. 
//! For example, the yat::SharedObject class offers such a compatible "LOCK_TYPE".
// ============================================================================
template <typename LOCK_TYPE = yat::Mutex> class AutoMutex
{

public:
  //! \brief Constructor (locks the associated Mutex).
  //!
  //! Example : \n
  //!   myMutexType myMutex;\n
  //!   AutoMutex<myMutexType> guard(myMutex); \n
  AutoMutex (LOCK_TYPE & _lock)
    : m_lock (_lock)
  {
    m_lock.lock();
  }

  //! \brief Destructor (unlocks the associated Mutex).
  ~AutoMutex ()
  {
    m_lock.unlock();
  }

private:
  //- The associated Mutex
  LOCK_TYPE & m_lock;

  //- Not implemented private member
  AutoMutex (const AutoMutex&);

  //- Not implemented private member
  AutoMutex & operator= (const AutoMutex&);
};

// ----------------------------------------------------------------------------
//! MutexLock: an AutoMutex specialisation (for backforward compatibility).
// ----------------------------------------------------------------------------
typedef AutoMutex<Mutex> MutexLock;

// ----------------------------------------------------------------------------
//! A global mutex used for thread safe logging.
// ----------------------------------------------------------------------------
#if defined (YAT_DEBUG)
  extern Mutex g_logging_mux;
#endif

} // namespace yat 

#if defined (YAT_INLINE_IMPL)
# if defined (YAT_WIN32)
#  include <yat/threading/impl/WinNtMutexImpl.i>
# else
#  include <yat/threading/impl/PosixMutexImpl.i>
# endif
#endif

#endif //- _YAT_MUTEX_H_
