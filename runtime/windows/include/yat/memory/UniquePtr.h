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

#ifndef _YAT_UNIQUE_PTR_H_
#define _YAT_UNIQUE_PTR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/utils/ReferenceCounter.h>
#include <iostream>
#include <map>

    
namespace yat
{

// ============================================================================
//! \class UniquePtr 
//! \brief A C++03 emulation of the C++11 unique_ptr
//! 
//! \notice Due to C++ 03 limitation some bad things can't be forbidden:
//! \notice The copy constructor exists but should not be used
//! 
//! \verbatim
//! Allocate an object and give ownership to an UniquePtr
//! yat::UniquePtr ptr(new Object);
//! ...
//! ptr->do_method();
//! ...
//! Store the UniquePtr object into a collection (vector, set, map,...).
//! The ownership is transferred to the object in the collection
//! std::set< yat::UniquePtr<Object> > collection;
//! collection.insert(ptr);
//! ptr->do_method(); // segmentation fault because ptr no longer own the object pointer
//! 
//! Transferring ownership
//! yat::UniquePtr ptr2;
//! YAT_MOVE(ptr, ptr2); // ownership is transferred from ptr to ptr2
//! 
//! Access the object pointer
//! Object* p = ptr.get();
//! 
//! Release the ownership. The UniquePtr object no longer own the pointer
//! Object* p = ptr.release();
//! delete p;
//! \endverbatim
// ============================================================================
template <typename T, typename D = DefaultDeleter<T> >
class UniquePtr
{
  template<typename U, typename E> friend class UniquePtr;

  typedef UniquePtr<T, D> ThisType;

public:
  //! \brief Default constructor.
  //! 
  //! Data pointer is initialized to null value.
  //! Deleter object is initialized to default value
  UniquePtr() 
    : m_p(0)
  {
    PTR_DBG("UniquePtr::UniquePtr()");
  }

  //! \brief Constructor from data pointer.
  //! \param p Pointer to type \<T\> data.
  UniquePtr ( T* p ) 
    : m_p(p)
  {

    PTR_DBG("UniquePtr::UniquePtr(" << std::hex << (void*)p << ")");
  }

  //! \brief Constructor from data pointer and specific deleter.
  //! \param p Pointer to type \<T\> data.
  //! \param d Specific deleter. See yat::SharedCounter definition.
  UniquePtr(T* p, D d) 
    : m_p(p), m_deleter(d)
  {
    PTR_DBG("UniquePtr::UniquePtr(" << std::hex << (void*)p << ", d)");
  }

  //! \brief Copy constructor from same data type \<T\>.
  //! \param s The source pointer.
  //! \notice the owner ship is transfered to the new object
  UniquePtr(const ThisType & s) 
    : m_p(s.m_p), m_deleter(s.m_deleter)
  {
    PTR_DBG("UniquePtr::UniquePtr(const ThisType & s) - m_p: " << std::hex << (void*)m_p << ")");
    T** p = const_cast<T**>(&(s.m_p));
    *p = 0;
  }

  //! \brief Copy constructor from other data type \<Y\>.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  //! \notice the owner ship is transfered to the new object
  //! \notice Deleter objects must be the same
  template<typename Y>
  UniquePtr(const UniquePtr<Y> & s)
  {
    cast_copy_data(s.m_p);
    m_deleter = s.m_deleter;
    Y** p = const_cast<Y**>(&(s.m_p));
    *p = 0;
    PTR_DBG("UniquePtr::UniquePtr(const UniquePtr<Y> & s) - m_p: " << std::hex << (void*)m_p << ")");
  }

  //! Destructor.
  ~UniquePtr()
  {
    PTR_DBG("UniquePtr::~UniquePtr()");
    if( m_p )
      m_deleter(m_p);
  }

  //! \brief Operator*. 
  //!
  //! Returns the pointed data (type \<T\>).
  T& operator*() const
  {
    YAT_ASSERT(m_p);
    return *m_p;
  }

  //! \brief Operator->.
  //!
  //! Returns a pointer to the data (type \<T\>*).
  T* operator->() const
  {
    YAT_ASSERT(m_p);
    return m_p;
  }

  template<typename Y> 
  ThisType& operator=( const UniquePtr<Y, D>& )
  {
    throw yat::Exception("NOT_ALLOWED", "operator=() not allowed with UniquePtr objects", "UniquePtr::operator=");
  }

  //! \brief Gets the data (idem operator ->).
  //!
  //! Returns a pointer to the data (type \<T\>*).
  T* get() const
  {
    return m_p;
  }

  //! \brief Gets the deleter object
  //!
  D& get_deleter()
  {
    return m_deleter;
  }

  //! \brief Gets the deleter object
  //!
  const D& get_deleter() const
  {
    return m_deleter;
  }

  //! \brief Resets pointer with new type \<T\> data.
  //! \param p New pointed data.
  T* release()
  {
    PTR_DBG("UniquePtr<T>::release(" << std::hex << (void*)m_p << ")");
    T* p = m_p;
    m_p = 0;
    return p;
  } 

  //! \brief Resets pointer with new type \<T\> data.
  //! \param p New pointed data.
  void reset(T * p = 0)
  {
    PTR_DBG("UniquePtr<T>::reset(" << std::hex << (void*)p << ")");
    if( m_p )
      m_deleter( m_p );
    m_p = p;
  } 

  //! \brief Resets pointer with new type \<T\> data.
  //! \param p New pointed data.
  template<typename Y>
  void reset(Y * p)
  {
    PTR_DBG("UniquePtr<Y>::reset(" << std::hex << (void*)p << ")");
    if( m_p )
      m_deleter(m_p);
    cast_copy_data(p);
  } 

  //! \brief Swaps content with the specified pointer.
  //! \param s The source pointer.
  void swap(ThisType & s)
  {
    std::swap(m_p, s.m_p);
    std::swap(m_deleter, s.m_deleter);
  }

  typedef T* ThisType::*anonymous_bool_type;

  //! \brief Implicit conversion to bool.
  operator anonymous_bool_type() const
  {
    return m_p == 0 ? 0 : &ThisType::m_p;
  }

  //! \brief Does *this* point to something ?
  //!
  //! Returns true if *this* points to a non allocated memory area, 
  //! false otherwise.
  bool is_null() const
  {
    return m_p ? false : true;
  }

  //! \brief Internal implementation of the '<' operator.
  //!
  //! Returns true if *this* pointed data is "less than" source pointed data, 
  //! false otherwise.
  //! \param s The source pointer.
  //! \exception BAD_CAST Thrown if type \<Y\> is not compatible with type \<T\>.
  template<class Y> 
  bool less_than(UniquePtr<Y> const & s) const
  {
    return m_p < s.m_p;
  }

private:

  //! Duplication not allowed
  template<typename Y, typename E> 
  ThisType& operator=( UniquePtr<Y, E>& );

  //- Tries to copy data pointer of foreign type.
  template<typename Y>
  void cast_copy_data(Y* p)
  {
    PTR_DBG("UniquePtr::cast_copy_data()");
    m_p = dynamic_cast<T*>(p);
    if( p != m_p )
    {
      throw Exception("BAD_CAST", "Trying to cast UniquePtr<T, D> to a uncompatible UniquePtr<Y, D>", "UniquePtr::cast_copy_data");
    }
  }

  //- Pointed data.
  T * m_p;

  // Deleter object
  D  m_deleter;
};

//! \brief Transfer ownership (equivalent to std::move in C++ 11)
//! \param from_p source pointer
//! \param to_p destination pointer
template <typename T, typename D> 
inline void move(UniquePtr<T, D>& from_p, UniquePtr<T, D>& to_p)
{
  to_p.reset( from_p.release() );
}

//! \brief Transfer ownership (equivalent to std::move in C++ 11)
//! \param from_p source pointer
//! \param to_p destination pointer
template <typename T, typename Y, typename D> 
inline void move(UniquePtr<T, D>& from_p, UniquePtr<Y, D>& to_p)
{
  to_p.reset( from_p.release() );
}

//! \brief Operator< on UniquePtr type, i.e. is \<a\> less than \<b\> ?
//! \param a Left shared pointer.
//! \param b Right shared pointer.
template <typename T, typename Y, typename D> 
inline bool operator<(UniquePtr<T, D> const & a, UniquePtr<Y, D> const & b)
{
 return a.less_than(b);
}

//! \brief Operator== on SharedPtr type, i.e. is \<a\> point to the same object as \<b\> ?
//! \param a Left shared pointer.
//! \param b Right shared pointer.
template <typename T, typename U, typename D, typename E> 
inline bool operator==( const UniquePtr<T, D>& a, const UniquePtr<U, E>& b )
{
  return a.get() == b.get();
}

// To ease transition to C++11
#define YAT_MOVE(from, to) yat::move(from, to)


} //- namespace

//! Convenience declaration in order to prepare a smooth move to C++ 11
#define YAT_UNIQUE_PTR(T) yat::UniquePtr<T>

#endif //- _YAT_UNIQUE_PTR_H_
