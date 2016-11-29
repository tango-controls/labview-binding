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

#ifndef _YAT_RW_MUTEX_H_
#define _YAT_RW_MUTEX_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Mutex.h>
#include <yat/threading/Semaphore.h>

namespace yat {

// ============================================================================
//! \class ReadersWriterMutex
//! \brief The YAT n-readers/1-writer mutex class.
//!
//! This class implements a n-readers/1-writer mutex. This is a synchronization 
//! tool for protecting resources that can be accessed for reading and writing. 
//! We want to allow multiple threads to have simultaneous read-only access, 
//! but as soon as one thread wants to write to the resource, all other threads 
//! must be blocked until the writing is complete.
//!
//! \remark This class is not supposed to be derived.
// ============================================================================
class YAT_DECL ReadersWriterMutex
{
public:
  //! \brief Constructor.
  ReadersWriterMutex (size_t max_readers = 32)
      : m_semaphore(max_readers), 
        m_max_readers(max_readers)
  {
    //- noop
  }

  //! \brief Destructor.
  virtual ~ReadersWriterMutex ()
  {
    //- noop
  }

  //! Locks the resource for reading
  inline void lock_for_reading () 
  { 
    this->m_semaphore.wait(); 
  }

  //! Unlocks the resource for reading
  inline void unlock_for_reading () 
  { 
    this->m_semaphore.post(); 
  }

  //! Locks the resource for writing
  inline void lock_for_writing () 
  { 
    yat::MutexLock guard(this->m_mutex);
    for (size_t i = 0; i < this->m_max_readers; ++i)
      m_semaphore.wait(); 
  }

  //! Unocks the resource for writing
  inline void unlock_for_writing () 
  { 
    for (size_t i = 0; i < this->m_max_readers; ++i)
      m_semaphore.post(); 
  }

private:
    size_t m_max_readers;
    yat::Semaphore m_semaphore;
    yat::Mutex m_mutex;
};

// ============================================================================
//! \class AutoReaderMutex
//! \brief The YAT "auto mutex" class for ReadersWriterMutex.
//!
//! The AutoReaderMutex class provides an auto lock/unlock mechanism.\n
//!
//! The AutoReaderMutex is ideal in context where some exceptions may be thrown. 
//! Whatever is the exit path of your code, the AutoMutex class will garantee 
//! that the associated mutex is properly unlock.\n
// ============================================================================
class YAT_DECL AutoReaderMutex
{
public:
  //! \brief Constructor (locks the associated r/w mutex for reading).
  //!
  //! Example : \n
  //!   AutoReadersWriterMutex myMutex;\n
  //!   AutoReaderMutex guard(myMutex); \n
  AutoReaderMutex (yat::ReadersWriterMutex & _rwlock)
    : m_rw_lock (_rwlock)
  {
    m_rw_lock.lock_for_reading();
  }

  //! \brief Destructor (unlocks the associated r/w mutex).
  ~AutoReaderMutex ()
  {
    m_rw_lock.unlock_for_reading();
  }

private:
  //- The associated Mutex
  ReadersWriterMutex & m_rw_lock;

  //- Not implemented private member
  AutoReaderMutex (const AutoReaderMutex&);

  //- Not implemented private member
  AutoReaderMutex & operator= (const AutoReaderMutex&);
};

// ============================================================================
//! \class AutoWriterMutex
//! \brief The YAT "auto mutex" class for ReadersWriterMutex.
//!
//! The AutoWriterMutex class provides an auto lock/unlock mechanism.\n
//!
//! The AutoWriterMutex is ideal in context where some exceptions may be thrown. 
//! Whatever is the exit path of your code, the AutoMutex class will garantee 
//! that the associated mutex is properly unlock.\n
// ============================================================================
class YAT_DECL AutoWriterMutex
{
public:
  //! \brief Constructor (locks the associated r/w mutex for writing).
  //!
  //! Example : \n
  //!   AutoReadersWriterMutex myMutex;\n
  //!   AutoWriterMutex guard(myMutex); \n
  AutoWriterMutex (yat::ReadersWriterMutex & _rwlock)
    : m_rw_lock (_rwlock)
  {
    m_rw_lock.lock_for_writing();
  }

  //! \brief Destructor (unlocks the associated r/w mutex).
  ~AutoWriterMutex ()
  {
    m_rw_lock.unlock_for_writing();
  }

private:
  //- The associated Mutex
  ReadersWriterMutex & m_rw_lock;

  //- Not implemented private member
  AutoWriterMutex (const AutoWriterMutex&);

  //- Not implemented private member
  AutoWriterMutex & operator= (const AutoWriterMutex&);
};

} // namespace yat 

#endif //- _YAT_RW_MUTEX_H_
