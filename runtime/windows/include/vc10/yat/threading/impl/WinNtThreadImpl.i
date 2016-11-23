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

namespace yat {

// ****************************************************************************
// YAT THREAD IMPL
// ****************************************************************************
// ----------------------------------------------------------------------------
// Thread::priority
// ----------------------------------------------------------------------------
YAT_INLINE Thread::Priority Thread::priority (void)
{
  //- enter critical section
  yat::AutoMutex<> guard(this->m_lock);

  return this->m_priority;
}
// ----------------------------------------------------------------------------
// Thread::state
// ----------------------------------------------------------------------------
YAT_INLINE Thread::State Thread::state (void)
{
  //- enter critical section
  yat::AutoMutex<> guard(this->m_lock);

  return this->m_state;
}
// ----------------------------------------------------------------------------
// Thread::state_i
// ----------------------------------------------------------------------------
YAT_INLINE Thread::State Thread::state_i (void) const
{
  return this->m_state;
}
// ----------------------------------------------------------------------------
// Thread::yield
// ----------------------------------------------------------------------------
YAT_INLINE void Thread::yield (void)
{
  ::Sleep(0);
}
// ----------------------------------------------------------------------------
// Thread::sleep
// ----------------------------------------------------------------------------
YAT_INLINE void Thread::sleep (unsigned long _msecs)
{
  ThreadingUtilities::sleep(0, 1000000 * _msecs);
}
// ----------------------------------------------------------------------------
// Thread::self
// ----------------------------------------------------------------------------
YAT_INLINE ThreadUID Thread::self (void) const
{
  return this->m_uid;
}

} // namespace yat
