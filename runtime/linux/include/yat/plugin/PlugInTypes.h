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

#ifndef _YAT_PLUGINTYPES_H_
#define _YAT_PLUGINTYPES_H_

#include <map>
#include <string>
#include <vector>
#include <yat/CommonHeader.h>
#include <yat/utils/Callback.h>
#include <yat/any/Any.h>

namespace yat
{
  //! \brief "Set attribute" callback definition.
  YAT_DEFINE_CALLBACK ( SetAttrCB, const yat::Any&  );
  //! \brief "Get attribute" callback definition.
  YAT_DEFINE_CALLBACK ( GetAttrCB, yat::Any& );

  struct PlugInDataType
  { 
    //! \brief Plugin attribute data types.
    enum
    {
      BOOLEAN,
      UINT8,
      INT16,
      UINT16,
      INT32,
      UINT32,
      INT64,
      UINT64,
      FLOAT,
      DOUBLE,
      STRING
    };
  };

  struct PlugInAttrWriteType
  { 
    //! \brief Plugin attribute access types.
    enum
    {
      READ,
      WRITE,
      READ_WRITE
    };
  };

  // ============================================================================
  //! \class PlugInAttrInfo 
  //! \brief Plugin attribute information.
  //!
  //! This class contains an attribute description.
  //! 
  // ============================================================================
  class PlugInAttrInfo
  {
  public:
    //! \brief Attribute name.
    std::string name;

    //! \brief Attribute data type.
    int data_type;

    //! \brief Attribute access type.
    int write_type;

    //! \brief Attribute label.
    std::string label;

    //! \brief Attribute description.
    std::string desc;

    //! \brief Attribute unit.
    std::string unit;

    //! \brief Attribute display format.
    std::string display_format;

    //! \brief "Set atrribute" callback.
    SetAttrCB set_cb;

    //! \brief "Get atrribute" callback.
    GetAttrCB get_cb;
  };

  //! \brief List of attribute informations.
  typedef std::vector<PlugInAttrInfo> PlugInAttrInfoList;

  struct PlugInPropType
  {
    //! \brief Plugin property types.
    enum
    {
      _UNDEFINED = -1,
      BOOLEAN,
      INT16,
      UINT8,
      UINT16,
      INT32,
      UINT32,
      INT64,
      UINT64,
      FLOAT,
      DOUBLE,
      STRING,
      STRING_VECTOR,
      INT16_VECTOR,
      UINT16_VECTOR,
      INT32_VECTOR,
      UINT32_VECTOR,
      INT64_VECTOR,
      UINT64_VECTOR,
      FLOAT_VECTOR,
      DOUBLE_VECTOR
    };
  };

  //! \brief List of property informations.
  typedef std::map<std::string , int> PlugInPropInfos;
  //! \brief List of property values.
  typedef std::map<std::string , yat::Any> PlugInPropValues;

} // namespace

#endif
