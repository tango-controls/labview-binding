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
 
#pragma once

#include <list>
#include <yat/utils/Callback.h>
#include <yat/threading/Mutex.h>



namespace yat 
{
// ============================================================================
//! \class Signal 
//! \brief The YAT signal class.
//!
//! This class is a template class to which different callbacks can be registered and 
//! then run when they are triggered. The generic type \<ParamType_\> is the callback
//! argument type.
//!
//! It is useful to implement observers: the run() function will be called on the signal
//! object without knowing who and how many clients are interested in this.
//!
//! The callbacks are defined as yat::Callback objects.
//!
//! If signal lock is not necessary, use a yat::NullMutex type (default value in 
//! the template definition), for example: 
//! \verbatim mySignal = new Signal<myCallbackArgType>(); \endverbatim
//! 
//! If signal lock is necessary, use a mutex type, for example:
//! \verbatim mySignal = new Signal<myCallbackArgType, yat::Mutex>(); \endverbatim
//!
// ============================================================================
template <typename ParamType_, typename LockType_ = yat::NullMutex> 
class Signal
{
public:
  //! \brief Macro to define the callback function type (Slot).
  YAT_DEFINE_CALLBACK(Slot, ParamType_);

protected:
  //! \brief %Signal lock.
  mutable LockType_ lock_;

  //! \brief List of observers (contains callback functions).
  std::list<Slot> observers_;

  //! \brief Internal run function.
  //! 
  //! Calls for each connected observer's callback function.
  //! Returns true if no problem occurs, false otherwise.
  //! \param param %Callback parameter.
  //! \param observers Connected observers list.
  bool _run(ParamType_ param, std::list<Slot> & observers)
  {
    typename std::list<Slot>::iterator 
                i(observers.begin()),
                e(observers.end());
    bool everythingOk = true;
    for (; i!=e; ++i) {
      try {
        (*i)(param);
      } catch(...) {
        everythingOk = false;
      }
    }
    return everythingOk;
  }

public:

  //! \brief Run function.
  //!
  //! Calls for each connected observer's callback function.
  //! Returns true if no problem occurs, false otherwise.
  //! \param param %Callback parameter.
  bool run(ParamType_ param)
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    return this->_run(param, this->observers_);
  }

  //! \brief Tests whether the connected observers list is empty.
  //! 
  //! Returns true if the connected observers list is empty, false otherwise.
  bool connected() const
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    return !this->observers_.empty();
  }

  //! \brief Connects an observer to the signal object.
  //!
  //! The observer is added in the observers list.
  //! \param cb Observer's callback.
  void connect(Slot cb)
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    this->observers_.remove(cb);
    this->observers_.push_front(cb);
  }

  //! \brief Disconnects an observer from the signal object.
  //!
  //! The observer is removed from the observers list.
  //! \param cb Observer's callback.
  void disconnect(Slot cb)
  {
    yat::AutoMutex<LockType_> guard(this->lock_);
    this->observers_.remove(cb);
  }
};

} // namespace yat
