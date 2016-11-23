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

#ifndef _YAT4TANGO_DEVICE_PLUGIN_H_
#define _YAT4TANGO_DEVICE_PLUGIN_H_

#include <yat/plugin/IPlugInObject.h>
#include <yat/plugin/PlugInTypes.h>
#include <yat4tango/DynamicAttribute.h>

namespace yat4tango
{

//! \brief List of plugin Tango Device properties.
//!
//! The 1st map parameter refers to the property name.
//! The 2nd map parameter refers to the property data type 
//! (from yat::PlugInPropType enumeration).
//!
//! The mapping between yat::PlugInPropType and C++ types is the following:
//! - PlugInPropType::BOOLEAN       --> bool
//! - PlugInPropType::UINT8         --> yat::uint8
//! - PlugInPropType::INT16         -->	yat::int16
//! - PlugInPropType::UINT16        -->	yat::uint16
//! - PlugInPropType::INT32         -->	yat::int32
//! - PlugInPropType::UINT32        -->	yat::uint32
//! - PlugInPropType::INT64         -->	yat::int64
//! - PlugInPropType::UINT64	      -->	yat::uint64
//! - PlugInPropType::FLOAT         -->	float
//! - PlugInPropType::DOUBLE	      -->	double
//! - PlugInPropType::STRING	      -->	std::string
//! - PlugInPropType::STRING_VECTOR --> std::vector<std::string>
//! - PlugInPropType::INT16_VECTOR	--> std::vector<yat::int16>
//! - PlugInPropType::UINT16_VECTOR	--> std::vector<yat::uint16>
//! - PlugInPropType::INT32_VECTOR	--> std::vector<yat::int32>
//! - PlugInPropType::UINT32_VECTOR	--> std::vector<yat::uint32>
//! - PlugInPropType::INT64_VECTOR	--> std::vector<yat::int64>
//! - PlugInPropType::UINT64_VECTOR	--> std::vector<yat::uint64>
//! - PlugInPropType::FLOAT_VECTOR	--> std::vector<float>
//! - PlugInPropType::DOUBLE_VECTOR	--> std::vector<double>
typedef std::map<std::string , int> PlugInPropertyInfoList;

//! \brief Plugin Tango Device property value.
//!
//! name = name of the property
//! value = value of the property
//! valid = true if value is valid, false otherwise
struct PlugInPropertyValue
{
  std::string name;
  yat::Any value;
  bool valid;
};

//! \brief List of plugin Tango Device property values.
typedef std::vector<PlugInPropertyValue> PlugInPropertyValueList;

//! \brief List of Tango Device dynamic attribute descriptions.
typedef std::vector<DynamicAttributeInfo> DynamicAttributeDescriptionList;

// ============================================================================
//! \class DevicePlugIn 
//! \brief Interface for a Plugin object with Tango Device properties and dynamic attributes.
//!
//! This class is a base class for a plugin object which can be configured with 
//! Tango Device properties and which can manage Tango Device dynamic attributes.
//! This abstract class can not be used as this and must be derived.
//!
//! Inherits from yat::IPlugInObject class.
// ============================================================================
class YAT4TANGO_DECL DevicePlugIn : public yat::IPlugInObject
{
public:
  //! \brief Enumerates the object's Tango Device properties.
  //! \param[out] propInfoList List of properties.
  virtual void enumerate_properties (yat4tango::PlugInPropertyInfoList& propInfoList) const = 0;
  
  //! \brief Sets the object's Tango Device properties.
  //! \param[in] propValueList List of property values to set.
  //!
  //! The <propValueList> list is filled with property values in the same order 
  //! than defined in the enumerate_properties() <propInfoList> list.
  virtual void set_properties (yat4tango::PlugInPropertyValueList& propValueList) = 0;

  //! \brief Enumerates the object's attributes.
  //! \param[out] attrDescList List of attribute descriptions.
  virtual void enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const = 0;
};

} // namespace

#endif // _YAT_DEVICE_PLUGIN_H_
