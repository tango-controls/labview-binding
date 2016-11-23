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

#ifndef _YAT_REF_COUNTER_H_
#define _YAT_REF_COUNTER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <typeinfo> 
#include <yat/CommonHeader.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Utilities.h>

namespace yat 
{

#define PTR_DBG(s)
//#define PTR_DBG(s) std::cout << std::hex << "[" << yat::ThreadingUtilities::self() << "][" << (void*)(this) << "] - " << std::dec << s << std::endl;

//! Defines the default counter type.
typedef yat::uint32 counter_t;


// ============================================================================
//! \class DefaultDeleter 
//! \brief Default deleter function.
//!
//! This template structure provides a default destructor for \<T\> type object.
// ============================================================================
template <typename T>
struct DefaultDeleter
{
  //! \brief operator().
  //! \param object The object to delete.
  void operator()(T* object)
  {
    try
    {
      PTR_DBG( "Deleting object @" << (void*)object );
      delete object;
    }
    catch(...)
    {
    }
  }
};

// ============================================================================
//! \class CountBase 
//! \brief Base class for counter implementation.
//!
//! This template class is the base class for a generic \<C\> type counter
//! (see CountImpl for instance). At least, the virtual function *dispose()* must 
//! be implemented.
//! 
//! If counter lock is not necessary, use a yat::NullMutex type for example: 
//! \verbatim myCounter = new CountBase<yat::uint64, yat::NullMutex>(); \endverbatim
//! 
//! If counter lock is necessary, use a mutex type (default value in 
//! the template definition), for example:
//! \verbatim myCounter = new CountBase<yat::uint64>(); \endverbatim
//!
// ============================================================================
template <typename C = counter_t, typename L = yat::Mutex>
class CountBase
{
public:
  //! \brief Default constructor.
  CountBase() : m_use_count(1), m_weak_count(0)
  {
    PTR_DBG("CountBase::CountBase");
  }
  
  //! \brief Destructor.
  virtual ~CountBase()
  {
    PTR_DBG("CountBase::~CountBase");
  }

  //! \brief Object disposal method (i.e. delete function).
  virtual void dispose() = 0; // nothrow

  //! \brief Adds a reference to counter.
  //!
  //! Increments counter by 1.
  const C add_ref()
  {
    PTR_DBG("CountImpl::add_ref(->" << m_use_count+1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    ++m_use_count;
    return m_use_count;
  }
  
  //! \brief Adds a weak reference to counter.
  //!
  //! Increments weak counter by 1.
  const C add_weak_ref()
  {
    PTR_DBG("CountImpl::add_weak_ref(->" << m_weak_count+1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    ++m_weak_count;
    return m_weak_count;
  }

  //! \brief Removes a reference from counter.
  //!
  //! Decreases counter by 1.
  const C dec_ref()
  {
    PTR_DBG("CountImpl::dec_ref(->" << m_use_count-1 << ")");
    yat::AutoMutex<L> guard(this->m_lock);
    YAT_ASSERT(this->m_use_count > 0);
    --m_use_count;
    return m_use_count;
  }

  //! \brief Removes a weak reference from counter.
  //!
  //! Decreases weak counter by 1.
  const C dec_weak_ref()
  {
    PTR_DBG("CountImpl::dec_weak_ref(->" << m_weak_count-1 << ")");
    YAT_ASSERT(this->m_weak_count > 0);
    --m_weak_count;
    return m_weak_count;
  }
  
  //! \brief Gets the counter value.
  C & use_count()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return m_use_count;
  }

  //! \brief Gets the weak counter value.
  const C & weak_count()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return m_weak_count;
  }

  //! \brief Tests if there is any reference (weak or not).
  //!
  //! Returns true if reference counter AND weak reference counter 
  //! are empty, false otherwise.
  bool expired()
  {
    yat::AutoMutex<L> guard(this->m_lock);
    return 0 == m_use_count && 0 == m_weak_count;
  }
  
  //! \brief Explicitly locks the counter.
  void lock()
  {
    m_lock.lock();
  }

  //! \brief Explicitly unocks the counter.
  void unlock()
  {
    m_lock.unlock();
  }

protected:
  //! Use counter.
  C m_use_count;

  //! Weak counter.
  C m_weak_count;

  //! Counter lock.
  L m_lock;
};

// ============================================================================
//! \class CountImpl 
//! \brief Counter implementation.
//!
//! This template class is a generic \<C\> type counter implementation. 
//! It inherits from CountBase class and provides:
//! - a generic \<D\> type deleter,
//! - a generic \<T\> type object.
//! 
//! If counter lock is not necessary, use a yat::NullMutex type for example: 
//! \verbatim myCounter = new CountImpl<myDeleterType, myObjType, yat::NullMutex>(); \endverbatim
//! 
//! If counter lock is necessary, use a mutex type (default value in 
//! the template definition), for example:
//! \verbatim myCounter = new CountImpl<myDeleterType, myObjType>(); \endverbatim
//!
// ============================================================================
template <typename T, typename D, typename C = counter_t, typename L = yat::Mutex>
class CountImpl: public CountBase<C,L>
{
public:
  //! \brief Constructor.
  //! \param p Object to manage.
  //! \param d Specific deleter.
  CountImpl(T* p, D d) : m_deleter(d), m_data(p)
  {
    PTR_DBG("CountImpl::CountImpl");
  }
  
  //! \brief A "do nothing" destructor.
  ~CountImpl()
  {
    PTR_DBG("CountImpl::~CountImpl");
  }

  //! \brief The specific deletion function.
  //!
  //! Calls for the specified deleter on the specified object.
  void dispose()
  {
    m_deleter(m_data);
  }

private:
  D  m_deleter;
  T* m_data;
};

template <typename C = counter_t, typename L = yat::Mutex> class WeakCounter;

// ============================================================================
//! \class SharedCounter 
//! \brief Shared counter implementation.
//!
//! This template class is a generic \<C\> type counter implementation. 
//! It uses a CountImpl counter.
//! 
//! If counter lock is not necessary, use a yat::NullMutex type for example: 
//! \verbatim myCounter = new SharedCounter<yat::NullMutex>(); \endverbatim
//! 
//! If counter lock is necessary, use a mutex type (default value in 
//! the template definition), for example:
//! \verbatim myCounter = new SharedCounter<>(); \endverbatim
//!
// ============================================================================
template <typename C = counter_t, typename L = yat::Mutex>
class SharedCounter
{
  friend class WeakCounter<C,L>;
public:

  //! Shared counter type.
  typedef SharedCounter<C,L> ThisType;
  
  //! \brief Constructor.
  //!
  //! uses the default deleter class: DefaultDeleter.
  //! \param p Object to manage.
  template <typename T>
  SharedCounter(T* p)
  {
    PTR_DBG("SharedCounter::SharedCounter(p)");
    this->m_count = new CountImpl<T,DefaultDeleter<T>,C,L>(p, DefaultDeleter<T>());
  }

  //! \brief Constructor with specific deleter.
  //! \param p Object to manage.
  //! \param d Specific deleter.
  template <typename T, typename D>
  SharedCounter(T* p, D d)
  {
    PTR_DBG("SharedCounter::SharedCounter(p, d)");
    this->m_count = new CountImpl<T,D,C,L>(p, d);
  }

  //! \brief Destructor.
  virtual ~SharedCounter()
  {
    PTR_DBG("SharedCounter::~SharedCounter()");
    release();
  };

  //! \brief Copy constructor from weak counter.
  //! \param cnt The source counter.
  explicit SharedCounter (const WeakCounter<C,L>& cnt)
  {
    PTR_DBG("SharedCounter::SharedCounter(const WeakCounter<T,L>&)");
    cnt.m_count->lock();
    if( cnt.m_count->use_count() > 0 )
    {
      this->m_count = cnt.m_count;
      this->m_count->add_ref();
    }
    cnt.m_count->unlock();
  }

  //! \brief copy constructor.
  //! \param cnt The source counter.
  SharedCounter (const ThisType& cnt)
  {
    PTR_DBG("SharedCounter::SharedCounter(const ThisType&)");
    this->m_count = cnt.m_count;
    this->m_count->add_ref();    
  }

  //! \brief Releases counter and underlying object.
  void release ()
  {
    PTR_DBG("SharedCounter::release()");
    m_count->dec_ref();
    if( m_count->expired() )
    {
      // No more reference to the counter object, we can destroy it
      this->m_count->dispose();
      delete this->m_count;
      this->m_count = 0;
    }
    else if( m_count->use_count() == 0 )
    {
      // At least one weak reference remains on the counter
      // We just destroy the referenced object
      this->m_count->dispose();
    }
  }

  //! \brief Resets counter's content.
  void reset ()
  {
    PTR_DBG("SharedCounter::reset()");
    ThisType().swap(*this);
  }

  //! \brief operator=.
  //! \param s The source counter.
  const ThisType& operator= (const ThisType& s)
  {
    PTR_DBG("SharedCounter::operator=(const ThisType&)");
    if (this != &s)
    {
      release();
      this->m_count = s.m_count;
      this->m_count->add_ref();
    }
    return *this;
  }
  
  //! \brief Swaps counters content.
  //! \param s The source counter.
  void swap (ThisType& s)
  {
    std::swap(this->m_count, s.m_count);
  }

  //! \brief Tests if there is only one reference or more.
  //!
  //! Returns true if there is only one reference, false otherwise.
  bool unique () const
  {
    return this->m_count->use_count() == 1;
  }

  //! \brief Gets use counter value.
  const C use_count () const
  {
    return this->m_count ? this->m_count->use_count() : 0;
  }

  //! Implicit conversion to bool.
  typedef C* SharedCounter::*unspecified_bool_type;

  //! \brief Implicit conversion to bool.
  operator unspecified_bool_type() const
  {
    return this->m_count ? m_count->use_count() : 0;
  }

private:
  CountBase<C, L>* m_count; // The shared counter
  L m_lock;
};

// ============================================================================
//! \class WeakCounter 
//! \brief Weak counter implementation.
//!
//! This template class is a generic \<C\> type counter implementation. 
//! It uses a SharedCounter counter.
//! 
//! If counter lock is not necessary, use a yat::NullMutex type for example: 
//! \verbatim myCounter = new WeakCounter<yat::uint16, yat::NullMutex>(); \endverbatim
//! 
//! If counter lock is necessary, use a mutex type , for example:
//! \verbatim myCounter = new WeakCounter<yat::uint16, yat::Mutex>(); \endverbatim
//!
//! This class is used by the WeakPtr pointer type. 
//!
// ============================================================================
template <typename C, typename L>
class WeakCounter
{
  friend class SharedCounter<C,L>;

public:

  typedef WeakCounter<C,L> ThisType;
  
  //! \brief Default constructor.
  WeakCounter ()
  {
    PTR_DBG("WeakCounter::WeakCounter()");
    this->m_count = 0;
  }

  //! \brief Copy constructor.
  //! \param cnt The source counter.
  WeakCounter (const ThisType& cnt)
  {
    PTR_DBG("WeakCounter::WeakCounter(const ThisType&)");
    this->m_count = cnt.m_count;
    this->m_count->add_weak_ref();
  }

  //! \brief Copy constructor from shared pointer.
  //! \param cnt The source counter.
  WeakCounter (const SharedCounter<C,L>& cnt)
  {
    PTR_DBG("WeakCounter::WeakCounter(const SharedCounter<T,L>&)");
    this->m_count = cnt.m_count;
    this->m_count->add_weak_ref();
  }

  //! \brief Destructor.
  virtual ~WeakCounter()
  {
    PTR_DBG("WeakCounter::~WeakCounter()");
    release();
  };

  //! \brief operator=.
  //! \param s The source counter.
  const ThisType& operator= (const ThisType& s)
  {
    PTR_DBG("WeakCounter::operator=(const ThisType&)");
    if (this != &s)
    {
      release();
      this->m_count = s.m_count;
      this->m_count->add_weak_ref();
    }
    return *this;
  }
  
  //! \brief Releases counter.
  void release()
  {
    PTR_DBG("WeakCounter::release()");
    if( m_count )
    {
      m_count->dec_weak_ref();
      if( m_count->expired() )
      {
        delete this->m_count;
        this->m_count = 0;
      }
    }
  }

  //! \brief Resets counter's content.
  void reset ()
  {
    PTR_DBG("WeakCounter::reset()");
    ThisType().swap(*this);
  }

  //! \brief Swaps counters content.
  //! \param s The source counter.
  void swap (const WeakCounter & s)
  {
    std::swap(this->m_count, s.m_count);
  }

  //! \brief Gets the use counter value.
  const C use_count () const
  {
    return this->m_count ? this->m_count->use_count() : 0;
  }

  //! Implicit conversion to bool.
  typedef C* WeakCounter::*unspecified_bool_type;

  //! \brief Implicit conversion to bool.
  operator unspecified_bool_type() const
  {
    return this->m_count ? m_count->use_count() : 0;
  }

private:
  CountBase<C,L>* m_count; // The shared counter
};

} // namespace

#endif



