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

#ifndef _YAT_SHARED_PTR_H_
#define _YAT_SHARED_PTR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/utils/ReferenceCounter.h>
#include <iostream>
#include <map>

    
namespace yat
{


// ============================================================================
// forward declaration class: WeakPtr
// ============================================================================
template <typename T, typename L = yat::Mutex> class WeakPtr;

// ============================================================================
//! \class SharedPtr 
//! \brief A shared pointer abstraction class.
//!
//! This template class provides a shared pointer on type \<T\> data with selectable 
//! locking strategy (type \<L\> parameter).
//!
//! If object lock is not necessary, use a yat::NullMutex type, for example : 
//! \verbatim myPointer = new SharedPtr<mySimpleObjectType, yat::NullMutex>(ptr); // defines a "simple" object pointer \endverbatim
//! 
//! If object lock is necessary, use a mutex type (default value in template definition
//! uses yat::Mutex), for example :
//! \verbatim myPointer = new SharedPtr<mySharedObjectType>(ptr); // defines a shared object pointer \endverbatim
//! 
// ============================================================================
template <typename T, typename L = yat::Mutex>
class SharedPtr
{
  template<typename U, typename V> friend class WeakPtr;
  template<typename U, typename V> friend class SharedPtr;

  typedef SharedPtr<T,L> ThisType;
  typedef SharedCounter<counter_t, L> ThisTypeRefCnt;

public:
  //! \brief Default constructor.
  //! 
  //! Data pointer and reference counter are initialized to null value.
  //! Default yat::Mutex type used for locking strategy.
  SharedPtr () 
    : m_data(0), m_ref_count((T*)0)
  {
    PTR_DBG("SharedPtr::SharedPtr()");
  }

  //! \brief Constructor from data pointer.
  //! \param p Pointer to type \<T\> data.
  //! Default yat::Mutex type used for locking strategy.
  SharedPtr (T* p) 
    : m_data(p), m_ref_count(p)
  {
    PTR_DBG("SharedPtr::SharedPtr(" << std::hex << (void*)p << ")");
  }

  //! \brief Constructor from data pointer and specific deleter.
  //! \param p Pointer to type \<T\> data.
  //! \param d Specific deleter. See yat::SharedCounter definition.
  //! Default yat::Mutex type used for locking strategy.
  template <typename D>
  SharedPtr (T* p, D d) 
    : m_data(p), m_ref_count(p, d)
  {
    PTR_DBG("SharedPtr::SharedPtr(" << std::hex << (void*)p << ")");
  }

  //! \brief Copy constructor from same data type \<T\>.
  //! \param s The source pointer.
  SharedPtr (const ThisType & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    PTR_DBG("SharedPtr::SharedPtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("SharedPtr::SharedPtr(const ThisType & s) - use_count: " << use_count());
  }

  //! \brief Copy constructor from other data type \<Y\>.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template<typename Y>
  SharedPtr (const SharedPtr<Y,L> & s): m_ref_count(s.m_ref_count) 
  {
    cast_copy_data(s.m_data);
    PTR_DBG("SharedPtr::SharedPtr(const SharedPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("SharedPtr::SharedPtr(const SharedPtr<Y,L> & s) - use_count: " << use_count());
  }

  //! \brief Constructor from WeakPtr<T,L>.
  //! \param s Weak pointer with type <T,L> parameter.
  explicit SharedPtr(const WeakPtr<T,L>& s): m_ref_count(s.m_ref_count)
  {
    PTR_DBG("SharedPtr::SharedPtr(const WeakPtr<T,L>&) - use_count: " << use_count());
    if( m_ref_count.use_count() == 0 )
    { // pointer has expired -> returns a null pointer
      m_data = 0;
    }
    else
    {
      m_data = s.m_data;
    }
    PTR_DBG("Use_count: " << use_count());
  }

  //! \brief Constructor from WeakPtr\<Y,L\>.
  //! \param s Weak pointer with type \<Y,L\> parameter.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template<typename Y>
  SharedPtr(const WeakPtr<Y,L>& s)
  {
    PTR_DBG("SharedPtr::SharedPtr(const WeakPtr<Y,L>&) - use_count: " << use_count());
    if( s.m_ref_count.use_count() == 0 )
    { // pointer has expired -> returns a null pointer
      m_data = 0;
    }
    else
    {
      copy_cast_data(s.m_data);
      m_ref_count = s.m_ref_count;
    }
    PTR_DBG("Use_count: " << use_count());
  }

  //! Destructor.
  ~SharedPtr()
  {
    PTR_DBG("SharedPtr::~SharedPtr()");
  }

  //! \brief Operator=.
  //! \param s The source pointer.
  const ThisType& operator= (const ThisType& s)
  {
    if (this != &s)
    {
      PTR_DBG("SharedPtr::operator=(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
      m_ref_count = s.m_ref_count;
      m_data = s.m_data;
      PTR_DBG("SharedPtr::SharedPtr(const ThisType & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! \brief Operator= from pointer to type \<T\> data.
  //! \param p The source pointer.
  const ThisType& operator= (T* p)
  {
    PTR_DBG("SharedPtr::operator=(" << std::hex << (void*)p << ")");
    if (p)
      reset(p);
    else
      reset();
    return *this;
  }

  //! \brief Operator*. 
  //!
  //! Returns the pointed data (type \<T\>).
  T& operator* () const
  {
    YAT_ASSERT(m_data);
    return *m_data;
  }

  //! \brief Operator->.
  //!
  //! Returns a pointer to the data (type \<T\>*).
  T * operator-> () const
  {
    return m_data;
  }

  //! \brief Gets the data (idem operator ->).
  //!
  //! Returns a pointer to the data (type \<T\>*).
  T * get () const
  {
    return m_data;
  }

  //! \brief Resets pointer to null value.
  void reset ()
  {
    PTR_DBG("SharedPtr::reset()");
    ThisType().swap(*this);
  } 

  //! \brief Resets pointer with new type \<T\> data.
  //! \param p New pointed data.
  void reset (T * p)
  {
    PTR_DBG("SharedPtr::reset(" << std::hex << (void*)p << ")");
    ThisType(p).swap(*this);
  } 

  //! \brief Resets pointer with new type \<T\> data and new type \<D\> deleter.
  //! \param p New pointed data.
  //! \param d New deleter.
  template <typename D> 
  void reset (T * p, D d)
  {
    PTR_DBG("SharedPtr::reset(" << std::hex << (void*)p << ", d)");
    ThisType(p, d).swap(*this);
  } 

  //! \brief Resets pointer with new type \<Y\> data (\<Y\> must be \<T\> convertible).
  //! \param p New pointed data.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template <typename Y> 
  void reset (Y * p)
  {
    PTR_DBG("template <typename Y>SharedPtr::reset(" << std::hex << (void*)p << ")");
    ThisType(p).swap(*this);
  } 

  //! \brief Resets pointer with new type \<Y\> data and new type \<D\> deleter
  //! (\<Y\> must be \<T\> convertible).
  //! \param p New pointed data.
  //! \param d New deleter.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template <typename Y, typename D> 
  void reset (Y * p, D d)
  {
    PTR_DBG("template <typename Y>SharedPtr::reset(" << std::hex << (void*)p << ", d)");
    ThisType(p, d).swap(*this);
  } 

  //! \brief Swaps content with the specified pointer.
  //! \param s The source pointer.
  void swap (ThisType & s)
  {
    std::swap(m_data, s.m_data);
    m_ref_count.swap(s.m_ref_count);
  }

  //! \brief Does the shared pointer have only one user ?
  //!
  //! Returns true if *this* reference count equals to 1, false otherwise.
  bool unique () const
  {
    return m_ref_count->unique();
  }

  //! \brief Gets the shared pointer use count.
  //! 
  //! Returns the number of threads using the shared pointer.
  unsigned long use_count () const
  {
    return m_ref_count.use_count();
  }

  typedef T* ThisType::*anonymous_bool_type;

  //! \brief Implicit conversion to bool.
  operator anonymous_bool_type () const
  {
    return m_data == 0 ? 0 : &ThisType::m_data;
  }

  //! \brief Does *this* point to something ?
  //!
  //! Returns true if *this* points to a non allocated memory area, 
  //! false otherwise.
  bool is_null () const
  {
    return m_data ? false : true;
  }

  //! \brief Internal implementation of the '<' operator.
  //!
  //! Returns true if *this* pointed data is "less than" source pointed data, 
  //! false otherwise.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template<class Y> 
  bool less_than (SharedPtr<Y,L> const & s) const
  {
    return m_data < s.m_data;
  }

private:

  //- Tries to copy data pointer of foreign type.
  template<typename Y>
  void cast_copy_data(Y* data)
  {
    PTR_DBG("SharedPtr::cast_copy_data()");
    m_data = dynamic_cast<T*>(data);
    if( data != m_data )
    {
      throw Exception("BAD_CAST", "Trying to cast SharedPtr<T,L> to a uncompatible SharedPtr<Y,L>", "SharedPtr::cast_copy_data");
    }
  }

  //- Pointed data.
  T * m_data;

  //- Reference counter.
  ThisTypeRefCnt m_ref_count;
};


//! \brief Operator< on SharedPtr type, i.e. is \<a\> less than \<b\> ?
//! \param a Left shared pointer.
//! \param b Right shared pointer.
template <typename T, typename U, typename L> 
inline bool operator<(SharedPtr<T, L> const & a, SharedPtr<U, L> const & b)
{
 return a.less_than(b);
}

//! \brief Operator== on SharedPtr type, i.e. is \<a\> point to the same object as \<b\> ?
//! \param a Left shared pointer.
//! \param b Right shared pointer.
template < class T, class U >
inline bool operator==( const SharedPtr<T>& a, const SharedPtr<U>& b )
{
  return a.get() == b.get();
}

// ============================================================================
//! \class WeakPtr 
//! \brief A weak pointer abstraction class.
//!
//! This template class provides a weak pointer on type \<T\> data with selectable 
//! locking strategy (type \<L\> parameter).
//!
//! If object lock is not necessary, use a yat::NullMutex type, for example : 
//! \verbatim myPointer = new WeakPtr<mySimpleObjectType, yat::NullMutex>(ptr); // defines a "simple" object pointer \endverbatim
//! 
//! If object lock is necessary, use a mutex type (default value in template 
//! definition uses yat::Mutex), for example :
//! \verbatim myPointer = new WeakPtr<mySharedObjectType>(ptr); // defines a shared object pointer \endverbatim
//! 
// ============================================================================
template <typename T, typename L> class WeakPtr
{
  typedef WeakPtr<T,L> ThisType;
  typedef WeakCounter<counter_t, L> ThisTypeRefCnt;

  template<class U, class V> friend class WeakPtr;
  template<class U, class V> friend class SharedPtr;

public:
  //! \brief Default constructor.
  //! 
  //! Data pointer and reference counter are initialized to null value.
  //! Default yat::Mutex type used for locking strategy.
  WeakPtr () 
    : m_data(0)
  {
    PTR_DBG("WeakPtr::WeakPtr()");
  }

  //! \brief Copy constructor from same data type \<T\>.
  //! \param s The source pointer.
  WeakPtr (const WeakPtr<T,L> & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - use_count: " << use_count());
  }

  //! \brief Copy constructor from other data type \<Y\>. 
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template <typename Y>
  WeakPtr (const WeakPtr<Y,L> & s) 
  {
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::WeakPtr(const WeakPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const WeakPtr<Y,L> & s) - use_count: " << use_count());
  }

  //! \brief Constructor from SharedPtr<T,L>.
  //! \param s The source shared pointer.
  WeakPtr (const SharedPtr<T,L> & s) 
    : m_data(s.m_data), m_ref_count(s.m_ref_count) 
  {
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const ThisType & s) - use_count: " << use_count());
  }

  //! \brief Constructor from SharedPtr\<Y,L\>.
  //! \param s The source shared pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template <typename Y>
  WeakPtr (const SharedPtr<Y,L> & s) 
  {
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::WeakPtr(const SharedPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    PTR_DBG("WeakPtr::WeakPtr(const SharedPtr<Y,L> & s) - use_count: " << use_count());
  }

  //! \brief Destructor.
  ~WeakPtr()
  {
    PTR_DBG("WeakPtr::~WeakPtr()");
  }

  //! \brief Operator=.
  //! \param s The source pointer.
  const WeakPtr<T,L>& operator= (const WeakPtr<T,L>& s)
  {
    if (this != &s)
    {
      PTR_DBG("WeakPtr::operator=(const WeakPtr<T,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
      m_ref_count = s.m_ref_count;
      m_data = s.m_data;
      if( m_data )
      {
        PTR_DBG("WeakPtr::operator=(const WeakPtr<T,L> & s) - use_count: " << use_count());
      }
    }
    return *this;
  }

  //! \brief Operator= from data type \<Y\> weak pointer.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template <class Y>
  const WeakPtr<T,L>& operator= (const WeakPtr<Y,L>& s)
  {
    PTR_DBG("WeakPtr::operator=(const WeakPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::operator=(const WeakPtr<Y,L> & s) ref counter copied");
    if( m_data )
    {
      PTR_DBG("WeakPtr::operator=(const WeakPtr<Y,L> & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! \brief Operator= from shared pointer.
  //! \param s The source pointer.
  const WeakPtr<T,L>& operator= (const SharedPtr<T,L>& s)
  {
    PTR_DBG("WeakPtr::operator=(const SharedPtr<T,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    m_data = s.m_data;
    m_ref_count = s.m_ref_count;
    if( m_data )
    {
      PTR_DBG("WeakPtr::operator=(const SharedPtr<T,L> & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! \brief Operator= from data type \<Y\> shared pointer.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template <class Y>
  const WeakPtr<T,L>& operator= (const SharedPtr<Y,L>& s)
  {
    PTR_DBG("WeakPtr::operator=(const SharedPtr<Y,L> & s) - m_data: " << std::hex << (void*)m_data << ")");
    cast_copy_data(s.m_data);
    m_ref_count = s.m_ref_count;
    PTR_DBG("WeakPtr::operator=(const SharedPtr<Y,L> & s) ref counter copied");
    if( m_data )
    {
      PTR_DBG("WeakPtr::operator=(const SharedPtr<Y,L> & s) - use_count: " << use_count());
    }
    return *this;
  }

  //! \brief Resets pointer to null value.
  void reset ()
  {
    PTR_DBG("WeakPtr::reset()");
    ThisType().swap(*this);
  } 

  //! \brief Swaps content with the specified pointer.
  //! \param s The source pointer.
  void swap (ThisType & s)
  {
    std::swap(m_data, s.m_data);
    std::swap(m_ref_count, s.m_ref_count);
  }

  //! \brief Gets the weak pointer number of user.
  //! 
  //! Returns the number of threads using the weak pointer.
  unsigned long use_count () const
  {
    return m_ref_count.use_count();
  }

  //! \brief Returns a shared pointer in order to provide safe access to the pointed data.
  SharedPtr<T,L> lock() const
  {
    PTR_DBG("WeakPtr::lock()");      
    // Returns a sharedptr in order to get access to the pointed object
    return SharedPtr<T,L>(*this);
  }
  
  
  typedef T* ThisType::*anonymous_bool_type;

  //! \brief Implicit conversion to bool.
  operator anonymous_bool_type () const
  {
    return m_data == 0 || expired() ? 0 : &ThisType::m_data;
  }

  //! \brief Does the weak pointer point to something ?
  //!
  //! Returns true if *this* points to a non allocated memory area, 
  //! false otherwise.
  bool is_null () const
  {
    return m_data ? false : true;
  }

  //! \brief Does the weak pointer have no more user ?
  //!
  //! Returns true if *this* reference counter equals to 0, false otherwise.
  bool expired () const
  {
    return m_ref_count.use_count() == 0;
  }

  //! \brief Internal implementation of the '\<' operator.
  //!
  //! Returns true if *this* pointed data is "less than" source pointed data, 
  //! false otherwise.
  //! \param s The source pointer (on data type \<Y\>).
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template<class Y> 
  bool less_than (WeakPtr<Y,L> const & s) const
  {
    return m_data < s.m_data;
  }

private:
  //- Pointed data.
  T * m_data;

  //- Reference counter.
  ThisTypeRefCnt m_ref_count;

  //- Tries to copy foreign data pointer.
  template<typename Y>
  void cast_copy_data(Y* data)
  {
    PTR_DBG("WeakPtr::cast_copy_data()");
    m_data = dynamic_cast<T*>(data);
    PTR_DBG("WeakPtr::cast_copy_data(): copied");
    if( data != m_data )
    {
      throw Exception("BAD_CAST", "Trying to cast WeakPtr<T,L> to a uncompatible WeakPtr<Y,L>", "WeakPtr::cast_copy_data");
    }
  }
};

//! \brief Operator< on WeakPtr type, i.e. is \<a\> less than \<b\> ?
//! \param a Left weak pointer.
//! \param b Right weak pointer.
template <typename T, typename U, typename L> 
inline bool operator<(WeakPtr<T, L> const & a, WeakPtr<U, L> const & b)
{
 return a.less_than(b);
}

// ============================================================================
//! \class SharedObjectPtr 
//! \brief A shared object pointer abstraction class.
//!
//! This template class provides a shared object pointer on type \<T\>.
//! Template argument class \<T\> must inherit from yat::SharedObject.
//!
// ============================================================================
template <typename T> class SharedObjectPtr
{
public:

  //! \brief Default constructor.
  SharedObjectPtr() : m_so(0)
  {
    //- noop
  }

  //! \brief Constructor with initialization. 
  //!
  //! Takes ownership of the specified yat::SharedObject. 
  //! \<Y\> must inherit from yat::SharedObject.
  //! \param so Shared object.
  template <typename Y> explicit SharedObjectPtr( Y * so ) : m_so(so)
  {
    //- noop
  }
  
  //! \brief Destructor.
  ~SharedObjectPtr()
  {
    reset();
  }

  //! \brief Assignment operator.
  //!
  //! Releases currently pointed SharedObject, then points to the specified one.
  //! \param r New shared object pointer.
  SharedObjectPtr& operator= (SharedObjectPtr& r) 
  {
    T * new_so = r ? r.m_so->duplicate() : 0;
    if (m_so)
      m_so->release();
    m_so = new_so;
    return *this;
  }

  //! \brief Resets the shared object pointer.
  //! 
  //! Releases the currently pointed yat::SharedObject then takes ownership of 
  //! the specified one.
  //! \param so New shared object.
  void reset ( T* so = 0 )
  {
    if (m_so)
      m_so->release();
    m_so = so;
  }

  //! \brief Underlying yat::SharedObject accessor.
  T* get()
  {
    return m_so;
  }
  
  //! \brief Underlying yat::SharedObject accessor.
  T* operator-> ()
  {
    return m_so;
  }
  
  //! \brief Underlying yat::SharedObject data accessor.
  T& operator* ()
  {
    return *m_so;
  }
  
  typedef T* SharedObjectPtr::*anonymous_bool_type;

  //! \brief Implicit conversion to bool.
  operator anonymous_bool_type() const
  {
    return m_so ? &SharedObjectPtr::m_so : 0;
  }

private:
  //- Shared object pointer.
  T * m_so;
};

} //- namespace

//! Convenience declarations. To prepare a smooth move to C++ 11
#define YAT_SHARED_PTR(T) yat::SharedPtr<T, yat::NullMutex>
#define YAT_WEAK_PTR(T) yat::WeakPtr<T, yat::NullMutex>
#define YAT_THREADSAFE_SHARED_PTR(T) yat::SharedPtr<T, yat::Mutex>
#define YAT_THREADSAFE_WEAK_PTR(T) yat::WeakPtr<T, yat::Mutex>

#endif //- _YAT_SHARED_PTR_H_
