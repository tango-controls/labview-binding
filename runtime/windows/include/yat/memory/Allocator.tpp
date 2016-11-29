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

#ifndef _ALLOCATOR_TPP_
#define _ALLOCATOR_TPP_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include  <yat/memory/Allocator.h>

#if !defined (YAT_INLINE_IMPL)
# include <yat/memory/Allocator.i>
#endif // YAT_INLINE_IMPL

namespace yat 
{

// ============================================================================
// NewAllocator::NewAllocator
// ============================================================================
template <typename T>
NewAllocator<T>::NewAllocator ()
{
  //- noop
}

// ============================================================================
// NewAllocator::~NewAllocator
// ============================================================================
template <typename T>
NewAllocator<T>::~NewAllocator ()
{
  //- noop
}

#define USE_NEW_OPERATOR 1

#if USE_NEW_OPERATOR

// ============================================================================
// NewAllocator::malloc
// ============================================================================
template <typename T>
T * NewAllocator<T>::malloc ()
{
  //- use default new "array" operator - infinite loop otherwise!
  //- that's the C++ trick os this class guys! ACE impl. made me understand!
  //- the compiler will take care of alignment for us.
   
  return new T[1];
}

// ============================================================================
// NewAllocator::free
// ============================================================================
template <typename T>
void NewAllocator<T>::free (T * p)
{
  //- used <new T[1]> to allocate space so...
  delete[] p;
}

#else //- USE_NEW_OPERATOR
  
// ============================================================================
// VERY NICE TRICK STOLEN FROM THE ACE LIB FOR OBJS MEMORY SPACE ALLOCATION 
// TAKING INTO ACCOUNT THAT MEMORY IS ALLOCATED USING <char> WHILE OBJ STORAGE
// NEEDS ALIGNMENT...
// ============================================================================
union yat_max_align_info
{
  int (*i)();
  void* p;
  long l;
  double d;
};
// ============================================================================
// PART OF THE ACE LIB MEMORY ALIGNMENT TRICK
// ============================================================================
#define ROUNDUP(X, Y) ((X) + ((Y) - 1) & ~((Y) - 1))

// ============================================================================
// NewAllocator::malloc
// ============================================================================
template <typename T>
T * NewAllocator<T>::malloc ()
{
  //- use the generic byte type (i.e. char) to allocate space
  //- in this case we have to take care of alignment. the space
  //- required is >= sizeof (T). using the ACE lib trick, it gives...
  
  size_t chunk_size = sizeof(T);
  chunk_size = ROUNDUP(chunk_size, sizeof(yat_max_align_info));
  return (T *) new char[chunk_size];
}

// ============================================================================
// NewAllocator::free
// ============================================================================
template <typename T>
void NewAllocator<T>::free (T * p)
{
  //- used the generic byte type (i.e. char) to allocate space so...
  delete[] (char *)p;
}

#endif  //- USE_NEW_OPERATOR

// ============================================================================
// CachedAllocator::CachedAllocator
// ============================================================================
template <typename T, typename L> 
CachedAllocator<T,L>::CachedAllocator (size_t nb_bunches, size_t nb_objs_per_bunch)
  : m_nb_objs_per_bunch(nb_objs_per_bunch)
{
  //- total num of objs to preallocate
  size_t nb_preallocated_objs = nb_bunches * nb_objs_per_bunch;

  //- prealloc objects
  for (size_t i = 0; i < nb_preallocated_objs; i++)
    m_cache.push_back( NewAllocator<T>::malloc() );
}

// ============================================================================
// CachedAllocator::~CachedAllocator
// ============================================================================
template <typename T, typename L> 
CachedAllocator<T,L>::~CachedAllocator () 
{   
   this->clear(100.);
}
  
// ============================================================================
// CachedAllocator::malloc
// ============================================================================
template <typename T, typename L> 
T * CachedAllocator<T,L>::malloc ()
{ 
  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);
  
  //- reallocate a bunch of T in case the cache is empty
  if ( m_cache.empty() && m_nb_objs_per_bunch )
  {
    for (size_t i = 0; i < m_nb_objs_per_bunch; i++)
      m_cache.push_back( NewAllocator<T>::malloc() );
  }
  
  //- return first available T in cache
  if ( ! m_cache.empty() )
  {
    //- get ref. to the first available chunk of memory
    T * p = m_cache.front();
    //- remove chunk for list of available 
    m_cache.pop_front();
    //- return storage to caller
    return p;
  }
    
  //- cache empty && m_nb_objs_per_bunch == 0
  return NewAllocator<T>::malloc();
}

// ============================================================================
// CachedAllocator::free
// ============================================================================
template <typename T, typename L> 
void CachedAllocator<T,L>::free (T * p)
{   
  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  //- push T back into the cache
  this->m_cache.push_back(p);
}
  
// ============================================================================
// CachedAllocator::release
// ============================================================================
template <typename T, typename L> 
void CachedAllocator<T,L>::release (T * p)
{   
  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  //- free p
  NewAllocator<T>::free(p);
}

// ============================================================================
// CachedAllocator::clear
// ============================================================================
template <typename T, typename L> 
void CachedAllocator<T,L>::clear (const double p)
{   
  //- enter critical section
  yat::AutoMutex<L> guard(this->m_lock);

  if ( p < 0. ||  this->m_cache.empty() )
	return;

  size_t n = 0;
  
  if ( p >= 100. )
    n = this->m_cache.size();
  else
    n = static_cast<size_t>( (p / 100.) * this->m_cache.size() );

  //- release objects
  for (size_t i = 0 ; i < n; i++)
  {
    T * o = m_cache.front();
    m_cache.pop_front();
    NewAllocator<T>::free(o);
  }
}

} // namespace 

#endif // _ALLOCATOR_TPP_

