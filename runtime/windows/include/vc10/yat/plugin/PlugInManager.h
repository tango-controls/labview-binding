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

#ifndef _YAT_PLUGIN_MANAGER_H_
#define _YAT_PLUGIN_MANAGER_H_

#include <yat/plugin/PlugIn.h>
#include <deque>

namespace yat
{

// ============================================================================
//! \class PlugInManager 
//! \brief The plugin manager class.
//!
//! This class provides an interface to manage dynamic library (plugin) objects: 
//! - load a plugin,
//! - unload a plugin.
//!
// ============================================================================
class YAT_DECL PlugInManager
{
public:
  //! \brief Information about a specific plug-in.
  struct PlugInEntry
  {
    //! \brief %File name.
    std::string     m_fileName;
    //! \brief Plugin pointer.
    PlugIn*         m_plugin;
    //! \brief Plugin informations.
    IPlugInInfo*    m_info;
    //! \brief Plugin factory.
    IPlugInFactory* m_factory;
  };

  //! \brief Default constructor.
  //!
  //! Constructs a PlugInManager object.
  PlugInManager();

  //! \brief Destructor.
  virtual ~PlugInManager();

  //! \brief Loads the specified plug-in.
  //!
  //! After being loaded, the OnLoad() method is called.
  //! Returns the pointer on the IPlugInFactory associated to the library.
  //! Valid until the library is unloaded. Never \c NULL.
  //!
  //! \param library_file_name Name of the file that contains the PlugIn.
  //! \exception SHAREDLIBRARY_ERROR Thrown if an error occurs during loading.
  std::pair<IPlugInInfo*, IPlugInFactory*> load( const std::string &library_file_name );

  //! \brief Loads the specified plug-in. Newer version!
  //!
  //! After being loaded, the OnLoad() method is called.
  //!
  //! \param library_file_name Name of the file that contains the PlugIn.
  //! \param entry_ptr Pointer to a previously allocated PlugInEntry structure.
  //! \exception NULL_POINTER Thrown if entry pointer is NULL.
  //! \exception SHAREDLIBRARY_ERROR Thrown if an error occurs during loading.
  void load( const std::string &library_file_name, PlugInEntry* entry_ptr );

  //! \brief Unloads the specified plug-in.
  //! \param library_file_name Name of the file that contains the TestPlugIn passed
  //! to a previous call to load().
  void unload( const std::string &library_file_name );
  
  //! \brief Unloads all loaded plug-in.
  void unload_all( void );

protected:

  //! \brief Unloads the specified plug-in.
  //! \param plug_in Information about the plug-in.
  void unload( PlugInEntry &plug_in );

private:
  //- Prevents the use of the copy constructor.
  PlugInManager( const PlugInManager &copy );

  //- Prevents the use of the copy operator.
  void operator =( const PlugInManager &copy );

private:
  typedef std::deque<PlugInEntry> PlugIns;
  PlugIns m_plugIns;
};

} // namespace

#endif
