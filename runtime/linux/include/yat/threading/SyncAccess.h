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

#ifndef __SYNCACCESS_H__
#define __SYNCACCESS_H__

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#include <yat/threading/Thread.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Utilities.h>
#include <yat/threading/Task.h>
#include <map>

#define LOCK(ptr) yat::SyncAccess _lock(ptr);

namespace yat
{
// ============================================================================
//! \class SyncAccess 
//! \brief Synchronize access to an object.
//!
//! The synchronization is implemented in the object constructor and destructor
//! functions.
// ============================================================================
class YAT_DECL SyncAccess
{
public:
  //! \brief Constructor.
  //! \param pObj Pointer to the object or resource to synchronize.
  SyncAccess(void *pObj);

  //! \brief Destructor.
  ~SyncAccess();

private:
  typedef void* obj_ptr;
  
  //- Synchronized object class.
  class SyncObject
  {
  public:
    //- Pointer to the synchronized object.
    void*          m_pObj;            
    
    //- Thread access count.
    uint32         m_uiThreadCount;    
    
    //- Current lock count.
    uint32         m_uiLockCount;     
    
    //- Locked by using thread.
    yat::Mutex     m_Mutex;           
    
    //- Thread currently locking access.
    yat::ThreadUID m_CurrentThreadId; 
    
    //- Constructor.
    SyncObject(void *ptr) : m_pObj(ptr), m_uiThreadCount(0), m_uiLockCount(0), 
      m_CurrentThreadId(YAT_INVALID_THREAD_UID) { }
  };

  //- Synchronized object.
  SyncObject *m_pSyncObj;
  
  //- Synchronization map.
  static std::map<void *, SyncObject *> m_sSynObjMap;
  
  //- To lock synchronization map access.
  static yat::Mutex m_sLock; 

  //- Locks synchronized object.
  void lock_obj(void *pObj);

  //- Unlocks synchronized object.
  void unlock_obj(void *pObj);
  
};

} // namespace gdshare

#endif // __THREADUTIL_H__
