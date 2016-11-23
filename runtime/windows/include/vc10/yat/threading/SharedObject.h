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

#ifndef _YAT_SHARED_OBJECT_H_
#define _YAT_SHARED_OBJECT_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#include <yat/threading/Mutex.h>

namespace yat
{

// ============================================================================
//! \class SharedObject
//! \brief A reference counted object abstraction.
//!
//! Base class for any reference counted (i.e. shared) object.
// ============================================================================
class YAT_DECL SharedObject
{
public:

  //! \brief Constructor.
  SharedObject ();
  
  //! \brief Destructor.
  virtual ~SharedObject ();
  
  //! \brief Returns a "shallow" copy of the shared object (avoids deep copy).
  //!
  //! Increments the shared reference count by 1.
  SharedObject * duplicate ();

  //! \brief Decreases the shared reference count by 1. 
  //! 
  //! If the reference count equals to 0 and \<commit_suicide\> is true, 
  //! then delete *this* and return 0. Returns 1 otherwise. 
  //! \param commit_suicide Indicates if object is to be deleted if
  //! no one uses it.
  //! \remark Behaviour is undefined if reference count < 0.
  int release (bool commit_suicide = true);

  //! \brief Returns the current reference count.
  int reference_count () const;
  
  //! \brief Locks the underlying mutex.
  void lock ();
  
  //! \brief Unlocks the underlying mutex.
  void unlock ();

  //! \brief Returns the underlying synchronization object (mutex).
  Mutex & mutex ();
  
protected:
  //! \brief Mutex to protect the data during any operation on them.
  Mutex lock_;

private:
  //- Internal release implementation.
  SharedObject * release_i ();

  //- Reference count for "shallow" copies (used to avoid deep copies).
  int reference_count_;

  //- Disallow these operations.
  //--------------------------------------------
  SharedObject & operator= (const SharedObject &);
  SharedObject (const SharedObject &);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/SharedObject.i>
#endif  // YAT_INLINE_IMPL

#endif  // _SHARED_OBJECT_H_
