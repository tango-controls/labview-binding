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
 
#ifndef _MONITORED_DEVICE_H_
#define _MONITORED_DEVICE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>
#include <yat4tango/MonitoredAttribute.h>

namespace yat4tango
{

// ============================================================================
// TYPEDEFs
// ============================================================================
//! Monitored attribute key (or identifier).
typedef unsigned long MonitoredAttributeKey;
  
//! Monitored attributes keys.
typedef std::vector<MonitoredAttributeKey> MonitoredAttributeKeyList;
  
// ============================================================================
//! \class MonitoredDevice 
//! \brief The monitored Device attribute class.
//!
//! This class provides an interface to read Tango Device attributes in a "thread 
//! safe" way. This class is used by the MonitoredDeviceTask class to 
//! implement a threaded "polling Device" capability, i.e. the monitoring of a Device 
//! by a remote client (which can also be a Device), through a threaded task.
//! 
//! The attributes to read must be registered with a *add_xxx()* function
//! and then, read with the *poll_attributes()* function.
//!
//! Inherits from Tango::LogAdapter class (for logging management).
//!
// ============================================================================
class MonitoredDevice : public Tango::LogAdapter
{
public:
  //! \brief Constructor.
  //! 
  //! \param dev_name The Device's name.
  //! \param host A pointer to the associated Device.
  MonitoredDevice (const std::string & dev_name, Tango::DeviceImpl * host = 0);

  //! \brief Destructor.
  virtual ~MonitoredDevice ();

  //! \brief Adds the specified attribute to the monitored attributes' list.
  //!
  //! Returns the registration key associated to the attribute.\n
  //! Registration order is kept until *this* is deleted. 
  //! \param attr_name Atribute's name.
  //! \exception INTERNAL_ERROR/MEMORY_ERROR Thrown if an error occurs during the registration.
  MonitoredAttributeKey add_monitored_attribute (const std::string &  attr_name);
    
  //! \brief Adds the specified attributes to the monitored attributes' list.
  //!
  //! Returns the registration keys' list associated to the added attributes.\n
  //! Registration order is kept until *this* is deleted.
  //! \param attr_names Attributes' name list.
  //! \exception INTERNAL_ERROR/MEMORY_ERROR Thrown if an error occurs during the registration.
  MonitoredAttributeKeyList add_monitored_attributes (const std::vector<std::string> &  attr_names);
    
  //! \brief Gets a monitored attribute by name.
  //! 
  //! Returns the associated MonitoredAttribute.
  //! \param attr_name The attribute's name.
  //! \exception DEVICE_ERROR Thrown if attribute not found in the monitored attributes' list list.
  MonitoredAttribute & get_monitored_attribute (const std::string & attr_name);
    
  //! \brief Gets a monitored attribute by registration key.
  //! 
  //! Returns the associated MonitoredAttribute.
  //! \param attr_key The attribute's registration key.
  //! \exception DEVICE_ERROR Thrown if attribute not found in the monitored attributes' list list.
  MonitoredAttribute & get_monitored_attribute (MonitoredAttributeKey attr_key);

  //! \brief Reads the monitored attributes' value.
  //! 
  //! The read values are stored in the associated monitored attributes.
  //! \exception DEVICE_ERROR/UNKNOWN_ERROR Thrown if an error occurs while trying 
  //! to read monitored attributes.
  void poll_attributes ();
    
  //! \brief Gets the monitored device's name.
  //!
  //! Returns the Device's name.
  const std::string& device_name () const;
  
private:
  //- connects to the device (instanciates the Tango::DeviceProxy)
  void connect (); 
  
  //- closes connection to the device (releases the Tango::DeviceProxy)
  void disconnect (); 

  //- monitored device name
  std::string	_devName;
  
  //- monitored device proxy
  Tango::DeviceProxy * _dp;
  
  //- Internal cooking...
  MonitoredAttributeKey _monitoredAttributeKeyGen;
  
  //- Internal cooking...
  typedef std::vector<std::string> MonitoredAttributesNames;
  MonitoredAttributesNames _monitoredAttributesNames;
  
  //- Internal cooking...
  typedef std::map<MonitoredAttributeKey, MonitoredAttribute*> MonitoredAttributes;
  typedef MonitoredAttributes::iterator MonitoredAttributesIt;
  MonitoredAttributes _monitoredAttributes;
  
  //- Internal cooking...
  typedef std::map<const std::string, MonitoredAttributeKey> MonitoredAttributesKeys;
  typedef MonitoredAttributesKeys::iterator MonitoredAttributesKeysIt;
  MonitoredAttributesKeys _monitoredAttributesKeys;
};

}	//- namespace


#endif //-	_MONITORED_DEVICE_H_
