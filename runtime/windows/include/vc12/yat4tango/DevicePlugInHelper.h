//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (c) 2004-2016 Synchrotron SOLEIL
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

#ifndef _YAT4TANGO_DEVICE_PLUGIN_HELPER_H_
#define _YAT4TANGO_DEVICE_PLUGIN_HELPER_H_

#include <yat4tango/CommonHeader.h>
#include <yat4tango/DynamicAttributeManager.h>
#include <yat4tango/DevicePlugIn.h>

namespace yat4tango
{

  // ============================================================================
  //! \class DevicePlugInHelper 
  //! \brief Tango Device properties & attribute management from a plugin.
  //!
  //! This class provides an interface to manage Device properties & attributes 
  //! from a Device plugin object.
  //! It is useful, for instance, to implement specific plugin capabilities for a more 
  //! generic Tango Device.
  //!
  //! This class supplies a way to "register" the plugin in the Tango world:
  //! - the plugin properties are initialized with property values defined 
  //! in the Tango DataBase,
  //! - the plugin attributes are created as dynamic attributes of the associated 
  //! Tango Device.
  //!
  //! The plugin can also be "unregistered":
  //! - the plugin dynamic attributes are deleted from the associated Tango Device.
  // ============================================================================
  class YAT4TANGO_DECL DevicePlugInHelper
  {
  public:
    //! \brief Constructor.
    //! \param hostDevice The associated device.
    //! \param dynAttrManager The dynamic attribute manager.
    DevicePlugInHelper(Tango::DeviceImpl* hostDevice, 
                       yat4tango::DynamicAttributeManager* dynAttrManager);
    
    //! \brief Destructor.
    ~DevicePlugInHelper();

    //! \brief Registers a plugin.
    //!
    //! The registration means:
    //! - the plugin properties are initialized with property values defined  
    //! in he Tango DataBase,
    //! - the plugin attributes are created as dynamic attributes of the associated 
    //! Tango Device.
    //! \param[in,out] object The plugin object.
    //! \exception SOFTWARE_FAILURE/UNKNOWN_ERROR Thrown if an error occurs during registration.
    void register_plugin(yat4tango::DevicePlugIn* object);

    //! \brief Unregisters a plugin.
    //!
    //! The unregistration means:
    //! - the plugin dynamic attributes are deleted from the associated 
    //! Tango Device.
    //! \param[in] object The plugin object.
    //! \exception SOFTWARE_FAILURE/UNKNOWN_ERROR Thrown if an error occurs during unregistration.
    void unregister_plugin(yat4tango::DevicePlugIn* object);

  private:
    //- Registers plugin properties
    void register_properties(yat4tango::DevicePlugIn* object);

    //- Registers plugin dynamic attributes
    void register_attributes(yat4tango::DevicePlugIn* object);

    //- Tango Device (for DataBase access)
    Tango::DeviceImpl* m_hostDevice;

    //- Dynamic attribute manager
    yat4tango::DynamicAttributeManager* m_dynAttrMger;
  };

} // namespace

#endif // _YAT4TANGO_DEVICE_PLUGIN_HELPER_H_
