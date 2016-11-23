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

#ifndef _YAT_ALLOCATOR_H_
#define _YAT_ALLOCATOR_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <deque>
#include <yat/threading/Mutex.h>
#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
//! \class NewAllocator 
//! \brief Managed memory allocation class of \<T\> type objects.
//!
//! The NewAllocator class is a template class that provides memory allocation 
//! management of specified \<T\> type objects.
// ============================================================================
template <typename T> class NewAllocator
{
public:
  //! \brief Default constructor.
  NewAllocator ();

  //! \brief Destructor.
  virtual ~NewAllocator ();
  
  //! \brief Allocates memory for a \<T\> type object.
  //!
  //! Returns a pointer to the allocated memory.
  //! \remark Can't allocate more than sizeof(T)
  virtual T * malloc ();

  //! \brief Releases memory pointed by \<p\>.
  //!
  //! \param p Pointer to the allocated memory to release
  //! \remark \<p\> must have been allocated by *this* malloc() function.
  virtual void free (T * p);
};

/*
// ============================================================================
//! The CachedAllocator class  
// ============================================================================
//! Implements an unbounded memory pool of T with selectable locking strategy 
//! ok but for "cachable" classes only!
// ============================================================================
#define CACHABLE_CLASS_DECL(CLASS_NAME, LOCKING_STRATEGY)
  typedef CachedAllocator<CLASS_NAME, LOCKING_STRATEGY> CLASS_NAME#Cache; \
  static CLASS_NAME#::Cache m_cache; \
  void * operator new (size_t); \
  void operator delete (void *); \
  static void pre_alloc (size_t _nobjs); \
  static void release_pre_alloc ();
#endif
  //-TODO: not usable for the moment - to be finished...
*/

// ============================================================================
//! \class CachedAllocator 
//! \brief Memory pool of \<T\> type objects.
//!
//! This class inherits from NewAllocator class. It implements an unbounded 
//! memory pool (cache) of \<T\> type objects with selectable locking strategy.\n
//!
//! If object lock is not necessary, use a yat::NullMutex type (default value in 
//! the template definition), for example : 
//! \verbatim myAllocator = new CachedAllocator<mySimpleObjectType>(10, 20); // defines a "simple" object pool \endverbatim
//! 
//! If object lock is necessary, use a mutex type, for example :
//! \verbatim myAllocator = new CachedAllocator<mySharedObjectType, yat::Mutex>(10, 20); // defines a shared object pool \endverbatim
//! 
// ============================================================================
template <typename T, typename L = yat::NullMutex> 
class CachedAllocator : public NewAllocator<T>
{
  //! Memory pool (or cache) implementation.
  typedef std::deque<T*> CacheImpl; 

public: 
  //! \brief Constructor.
  //!
  //! Preallocates \<nb_bunches * nb_objs_per_bunch\> slots of \<T\> type objects in a memory cache.
  //! \param nb_bunches Number of bunches in the cache.
  //! \param nb_objs_per_bunch Number of \<T\> type objects per bunch.
  //!
  //! \remark A bunch of \<nb_objs_per_bunch\> instances of \<T\> type object will be preallocated 
  //! each time the cache gets empty.
  //! \remark If \<nb_bunches * nb_objs_per_bunch\> is null, the CachedAllocator class works as 
  //! a single NewAllocator class.
  CachedAllocator (size_t nb_bunches = 0, size_t nb_objs_per_bunch = 0);

  //! \brief Destructor.
  //!
  //! Releases all allocated slots in memory cache.
  virtual ~CachedAllocator();
  
  //! \brief Allocates an instance of \<T\> type object in the cache.
  //!
  //! Gets an instance of \<T\> type object from the cache and returns pointer to 
  //! this instance.
  //! \remark Reallocates a bunch of \<T\> type objects in case the cache is empty.
  virtual T * malloc ();

  //! \brief Releases an instance of \<T\> type object.
  //!
  //! \param p Pointer to the instance to release
  //! \remark <p> must have been allocated by *this* malloc() function.
  //! \remark <p> returns to the cache.
  virtual void free (T * p);
  
  //! \brief Releases an instance of \<T\> type object.
  //!
  //! \param p Pointer to the instance to release
  //! \remark \<p\> must have been allocated by *this* malloc() function.
  //! \remark \<p\> does NOT return to the cache.
  virtual void release (T * p);
  
  //! \brief Returns the number of \<T\> type object instances currently stored into the cache.
  inline size_t length () const 
  {
    return m_cache.size();
  }

  //! \brief Releases the specified percentage of the cache.
  //!
  //! \param p Percentage of the cache to be released.
  inline void clear (const double p = 100.);

  //! \brief Returns the number of bytes currently stored into the cache.
  inline size_t size () const 
  {
    return m_cache.size() * sizeof(T);
  }
  
  //! \brief Returns the number of instances of \<T\> type object (pre)allocated each 
  //! time the cache gets empty.
  inline size_t bunch_length () const 
  {
    return m_nb_objs_per_bunch;
  }
  
  //! \brief Sets the number of instances of \<T\> type object (pre)allocated each time 
  //! the cache gets empty.
  //!
  //! \param n Number of \<T\> type objects per bunch
  inline void bunch_length (size_t n) 
  {
    m_nb_objs_per_bunch = n;
  }
  
protected:
  //! \brief Locking (i.e. thread safety) strategy.
  L m_lock;

  //! \brief Number of instances of \<T\> type object per bunch.
  size_t m_nb_objs_per_bunch;

  //! \brief The memory cache (i.e. memory pool).
  CacheImpl m_cache;
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/memory/Allocator.i>
#endif // YAT_INLINE_IMPL

#include <yat/memory/Allocator.tpp>

#endif // _YAT_ALLOCATOR_H_


