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
//    Nicolas Leclercq
//    Synchrotron SOLEIL 
//------------------------------------------------------------------------------
/*!
 * \authors See AUTHORS file
 */

#ifndef _PROPERTY_HELPER_H_
#define _PROPERTY_HELPER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>

namespace yat4tango
{

// ============================================================================
//! \class PropertyHelper
//! \brief Helper class for properties storage
//!
//! The PropertyHelper class implements static methods that help to read and store
//! class/device/attribute properties
//! See yat::Task documentation for behaviour details.
//!
// ============================================================================
class YAT4TANGO_DECL PropertyHelper : public Tango::LogAdapter
{
public:
	//! \brief Store property value
	//! \param dev_p Pointer to associated device.
	//! \param name Property's name
	//! \param value Property's value
  template <class T>
  static void set_property(Tango::DeviceImpl* dev_p, const std::string& name, T value);

	//! \brief Read property value
	//! \param dev_p Pointer to associated device.
	//! \param name Property's name
  template <class T>
  static T get_property(Tango::DeviceImpl* dev_p, const std::string& name);

	//! \brief Creates a device property if it's doesn't exists in the Tango database
	//! \param dev_p Pointer to associated device.
	//! \param name Property's name
  template <class T>
  static void create_property_if_empty(Tango::DeviceImpl* dev_p, Tango::DbData& dev_prop,
                                       T value, std::string property_name);

	//! \brief Store the current value of an attribut as a attribute property
	//! \param dev_p Pointer to associated device.
	//! \param name Attribute's name
	//! \param val Attribute's value
  template <class T>
  static void set_memorized_attribute(Tango::DeviceImpl* dev_p, const std::string& name, T val);

	//! \brief Read an attributes's memorized value
	//! \param dev_p Pointer to associated device.
	//! \param name Attribute's name
  template <class T>
  static T get_memorized_attribute(Tango::DeviceImpl* dev_p, const std::string& name);

  //! \brief Read an attributes's memorized value
  //! \param dev_p Pointer to associated device.
  //! \param name Attribute's name
  //! \param default_value defaul value if there is no memorized value
  template <class T>
  static T get_memorized_attribute(Tango::DeviceImpl* dev_p, const std::string& name, T default_value);
};

} // namespace

#include <yat4tango/PropertyHelper.hpp>

#endif
