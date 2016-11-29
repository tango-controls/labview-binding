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

#ifndef _YAT_IPLUGININFO_H_
#define _YAT_IPLUGININFO_H_

#include <yat/utils/NonCopyable.h>

namespace yat
{

// ============================================================================
//! \class IPlugInInfo 
//! \brief The information class for plugin object.
//!
//! This class is a base class providing access to informations on a plugin object. 
//! This abstract class can not be used as this and must be derived.
//!
//! Inherits from yat::NonCopyable class.
// ============================================================================
class YAT_DECL IPlugInInfo : private yat::NonCopyable
{
public:
  //! \brief Destructor.
  virtual ~IPlugInInfo();

  //! \brief Gets plugin identifier (as a string value).
  virtual std::string get_plugin_id () const = 0;

  //! \brief Gets plugin interface name (as a string value).
  virtual std::string get_interface_name () const = 0;

  //! \brief Gets plugin version number (as a string value).
  virtual std::string get_version_number () const = 0;
  
protected:
  //! \brief Protected constructor.
  IPlugInInfo();
};

} // namespace

#endif
