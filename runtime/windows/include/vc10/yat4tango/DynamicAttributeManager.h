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

#ifndef _YAT4TANGO_DYNAMIC_ATTR_MANAGER_H_
#define _YAT4TANGO_DYNAMIC_ATTR_MANAGER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>
#include <yat4tango/DynamicAttribute.h>

namespace yat4tango
{

// ============================================================================
//! \class DynamicAttributeManager
//! \brief The dynamic attribute manager.
//!
//! This class provides a manager of device dynamic attributes. It manages a
//! repository of registered dynamic attributes for a Tango Device.
//!
//! Adding or removing a dynamic attribute from the repository is equivalent to
//! adding or removing a dynamic attribute from the Tango Device interface.
//!
//! Inherits from Tango::LogAdapter class (for logging management).
//!
// ============================================================================

class YAT4TANGO_DECL DynamicAttributeManager : public Tango::LogAdapter
{
public:
  //! \brief Constructor. 
  //! \param _dev The Tango Device handled by the instance.
  DynamicAttributeManager (Tango::DeviceImpl * _dev = 0);
  
  //! \brief Destructor.
  //!
  //! Removes all registered dynamic attributes from the device.
  ~DynamicAttributeManager();

  //! \brief Sets host device.
  //!
  //! \param h The Tango device instance to which the dynamic attributes are linked.
  //! \exception CONFIGURATION_ERROR Thrown if the manager is already linked to a device.
  void set_host_device (Tango::DeviceImpl * h);

  //! \brief Adds a dynamic attribute to the repository.
  //!
  //! \param dai A dynamic attribute information containing the attribute's description.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.
  void add_attribute (const yat4tango::DynamicAttributeInfo& dai);

  //! \brief Adds a dynamic attribute to the repository.
  //!
  //! \param da The dynamic attribute to add.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR Thrown when adding fails.
  void add_attribute (yat4tango::DynamicAttribute* da);

  //! \brief Adds a forwarded dynamic attribute to the repository.
  //!
  //! \param url A fully qualified Tango attribute's name (i.e. remote/dev/name/attr).
  //! \param lan Local attribute's name of the forwarded attribute. Default value is to yat4tango::KeepOriginalAttributeName.
  //! \param rdo If set to true, the forwarded attribute access is locally restricted to "read-only". Default value is false.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.
  void add_attribute (const std::string& url,
                      const std::string& lan = yat4tango::KeepOriginalAttributeName,
                      bool rdo = false);

  //! \brief Adds a forwarded dynamic attribute to the repository.
  //!
  //! \param fai A forwarded attribute information containing the attribute's description.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.
  void add_attribute (const yat4tango::ForwardedAttributeInfo& fai);
    
  //! \brief Adds a list of dynamic attributes to the repository.
  //!
  //! \param dai A list of attribute's information containing the attributes' description.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.  
  void add_attributes (const std::vector<DynamicAttributeInfo>& dai);

  //! \brief Adds a list of forwarded dynamic attributes to the repository.
  //!
  //! Provides a way to specify local attribute names.
  //! \param dai A list of forwarded attribute's information containing the attributes' description.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.  
  void add_attributes (const std::vector<ForwardedAttributeInfo>& dai);
    
  //! \brief Adds a list of forwarded dynamic attributes to the repository.
  //!
  //! Keeps original attribute names and accesses.
  //! \param urls A list of fully qualified Tango attribute names (i.e. remote/dev/name/attr).
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception OPERATION_NOT_ALLOWED Thrown if another dynamic attribute already exists with same name.
  //! \exception API_ERROR/INTERNAL_ERROR/UNKNOWN_ERROR Thrown when adding fails.  
  void add_attributes (const std::vector<std::string>& urls);

  //! \brief Removes a dynamic attribute from the repository.
  //! 
  //! \param an The attribute's name.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception ATTRIBUTE_NOT_FOUND Thrown if attribute's name is not found.
  //! \exception INTERNAL_ERROR/UNKNOWN_ERROR Thrown when removing fails.  
  void remove_attribute (const std::string& an);

  //! \brief Removes the dynamic attributes from the repository.
  //! 
  //! Removes all the dynamic attributes registered.
  //! \exception INITIALIZATION_ERROR Thrown if the manager is not linked to a Tango device.
  //! \exception INTERNAL_ERROR/UNKNOWN_ERROR Thrown on the first error occurring during attribute's removing.
  void remove_attributes ();

  //! \brief Gets a dynamic attribute by its name.
  //!
  //! \param an The attribute's name.
  //! \exception ATTR_NOT_FOUND Thrown if attribute's name is not found.
  DynamicAttribute & get_attribute (const std::string& an)
  {
    DynamicAttributeIterator it = this->rep_.find(an);
	  if (it == this->rep_.end() || ! (*it).second)
	  {
	    THROW_DEVFAILED("ATTR_NOT_FOUND",
	                    "the specified dynamic attribute does not exist",
                      "DynamicAttributeManager::get_attribute");
	  }
	  return *((*it).second);
  }

private:
  typedef std::map <std::string, DynamicAttribute*> DynamicAttributeRepository;
  typedef DynamicAttributeRepository::value_type DynamicAttributeEntry;
  typedef DynamicAttributeRepository::iterator DynamicAttributeIterator;
  typedef DynamicAttributeRepository::const_iterator DynamicAttributeConstIterator;

  Tango::DeviceImpl * dev_;
  DynamicAttributeRepository rep_;

  DynamicAttributeManager(const DynamicAttributeManager&);
  DynamicAttributeManager& operator = (const DynamicAttributeManager&);
};

} // namespace

#endif
