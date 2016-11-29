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

#ifndef _YAT_IPLUGINFACTORY_H_
#define _YAT_IPLUGINFACTORY_H_

#include <yat/plugin/IPlugInObject.h>

namespace yat
{

//! Parameters to customize the object creation.
typedef std::vector<std::string> PlugInObjectParams;


// ============================================================================
//! \class IPlugInFactory 
//! \brief Abstract interface for a Plugin factory.
//!
//! This class is a base factory for "dynamic library" objects. At least the virtual 
//! function *create()* must be implemented by derived classes.
//! 
// ============================================================================
class YAT_DECL IPlugInFactory
{
  friend class PlugInManager;

public:
  //! \brief Creates a plugin object.
  //! \param[in,out] object A reference to a IPlugInObject pointer that will hold 
  //! the address of the created object.
  //! \param[in] params A set of parameters to customize the object creation if necessary.
  virtual void create(IPlugInObject*& object,
                      const PlugInObjectParams& params = PlugInObjectParams()) = 0;
protected:
  //! \brief Protected default constructor.
  IPlugInFactory();

  //! \brief Protected destructor.
  virtual ~IPlugInFactory();

private:
  IPlugInFactory(const IPlugInFactory&);
  IPlugInFactory& operator=(const IPlugInFactory&);
};

} // namespace

#endif
