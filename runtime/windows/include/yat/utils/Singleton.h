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
 
#ifndef _YAT_SINGLETON_H_
#define _YAT_SINGLETON_H_

namespace yat
{
  // ============================================================================
  //! \class Singleton 
  //! \brief The YAT singleton class.
  //!
  //! This template class is a base class which provides a "single instance" capability.
  //! Inherit from this class to get this capability.
  //!
  // ============================================================================
  template <class T> class Singleton
  {
  protected: 
    //! \brief Protected constructor (this is a base class).
    Singleton ()
    {
      //-noop
    }
    
    //! \brief Protected destructor (this is a base class).
    virtual ~Singleton()
    {
      //-noop
    }

  public:
    //! \brief Static method to get the unique instance of the class.
    static T & instance( )
    {
      static T the_unique_instance;
      return the_unique_instance;
    }
  };
}

#endif
