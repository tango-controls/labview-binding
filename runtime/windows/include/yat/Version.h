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
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
//
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

#ifndef _YAT_VERSION_H_
#define _YAT_VERSION_H_


#include <string>
#include <vector>

#include <yat/CommonHeader.h>
#include <yat/utils/Singleton.h>

namespace yat
{

#define _YAT_STR_(s) #s
#define YAT_XSTR(s) _YAT_STR_(s)

// ============================================================================
//! \class Version 
//! \brief Class aimed to manage versions numbers information about a project's modules
//!
// ============================================================================
class YAT_DECL Version : public yat::Singleton<Version>
{
public:
  
  /// ------------------------------------------
  /// Sets the project informations
  /// ------------------------------------------
  static void set(const std::string& name, const std::string& version);

  /// ------------------------------------------
  /// Sets informations about a dependency
  /// ------------------------------------------
  static void add_dependency(const std::string& name, const std::string& version);

  /// ------------------------------------------
  /// Get all informations as a string
  /// ------------------------------------------
  static std::string get();

  /// ------------------------------------------
  /// Clear informations
  /// ------------------------------------------
  static void clear();

private:

  struct Module
  {
    std::string name;
    std::string version;

    Module(const std::string& _name, const std::string& _version) : name(_name), version(_version) {}
    Module() {}
  };

  std::vector<Module> m_dependencies;
  Module              m_main_module;
};

}

#endif // _YAT_VERSION_H_
