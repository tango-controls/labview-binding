//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 The Tango Community
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (c) 2004-2016 The Tango Community
//
// The YAT4Tango library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation; either version 2 of the License, or (at 
// your option) any later version.
//
// The YAT4Tango library is distributed in the hope that it will be useful,
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
 * \authors See AUTHORS file
 */

#ifndef _YAT4TANGO_DYNAMIC_CMD_MANAGER_H_
#define _YAT4TANGO_DYNAMIC_CMD_MANAGER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>
#include <yat4tango/DynamicCommand.h>

namespace yat4tango
{

// ============================================================================
//! \class DynamicCommandManager
//! \brief The dynamic command manager.
//!
//! This class provides a manager of device dynamic commands. It manages a
//! repository of registered dynamic commands for a Tango Device.
//!
//! Adding or removing a dynamic command from the repository is equivalent to
//! adding or removing a dynamic command from the Tango Device interface.
//!
//! Inherits from Tango::LogAdapter class (for logging management).
//!
// ============================================================================
class YAT4TANGO_DECL DynamicCommandManager : public Tango::LogAdapter
{
public:
  //! \brief Constructor. 
  //! \param _dev The Tango Device handled by the instance.
  DynamicCommandManager (Tango::DeviceImpl * _dev = 0);
  
  //! \brief Destructor.
  //!
  //! Removes all registered dynamic commands from the device.
  ~DynamicCommandManager();

  //! \brief Sets host device.
  //!
  //! \param h The Tango device instance to which the dynamic commands are linked.
  //! \exception CONFIGURATION_ERROR Thrown if the manager is already linked to a device.
  void set_host_device (Tango::DeviceImpl * h);

  //! \brief Adds a dynamic command to the repository.
  //!
  //! \param dai A dynamic command information containing the command's description.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic command already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.
  void add_command (const yat4tango::DynamicCommandInfo& dai);

  //! \brief Adds a dynamic command to the repository.
  //!
  //! \param da The dynamic command to add.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic command already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR Thrown when adding fails.
  void add_command (yat4tango::DynamicCommand* dc);

  //! \brief Adds a list of dynamic commands to the repository.
  //!
  //! \param dai A list of command's information containing the commands' description.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic command already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.  
  void add_commands (const std::vector<DynamicCommandInfo>& dai);

  //! \brief Removes a dynamic command from the repository.
  //! 
  //! \param an The command's name.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception ATTRIBUTE_NOT_FOUND Thrown if command's name is not found.
  //! \exception INTERNAL_ERROR/UNKNOWN_ERROR Thrown when removing fails.  
  void remove_command (const std::string& an);

  //! \brief Removes the dynamic commands from the repository.
  //! 
  //! Removes all the dynamic commands registered.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception INTERNAL_ERROR/UNKNOWN_ERROR Thrown on the first error occurring during command's removing.
  void remove_commands ();

  //! \brief Gets a dynamic command by its name.
  //!
  //! \param an The command's name.
  //! \exception ATTR_NOT_FOUND Thrown if command's name is not found.
  DynamicCommand & get_command (const std::string& an)
  {
    DynamicCommandIterator it = this->rep_.find(an);
	  if (it == this->rep_.end() || ! (*it).second)
	  {
	    THROW_DEVFAILED("ATTR_NOT_FOUND",
	                    "the specified dynamic command does not exist",
                      "DynamicCommandManager::get_command");
	  }
	  return *((*it).second);
  }

private:
  typedef std::map <std::string, DynamicCommand*> DynamicCommandRepository;
  typedef DynamicCommandRepository::value_type DynamicCommandEntry;
  typedef DynamicCommandRepository::iterator DynamicCommandIterator;
  typedef DynamicCommandRepository::const_iterator DynamicCommandConstIterator;
    
  size_t num_pogo_cmd_;
  Tango::DeviceImpl * dev_;
  DynamicCommandRepository rep_;

  DynamicCommandManager(const DynamicCommandManager&);
  DynamicCommandManager& operator = (const DynamicCommandManager&);
};

} // namespace

#endif
