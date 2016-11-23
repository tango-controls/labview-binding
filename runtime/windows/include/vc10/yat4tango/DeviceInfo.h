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
// Copyright (C) 2006-2016 The Tango Community
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

#ifndef _DEVICE_INFO_H_
#define _DEVICE_INFO_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <deque>
#include <yat/Version.h>
#include <yat/utils/String.h>
#include <yat4tango/DynamicAttributeManager.h>
#include <yat/threading/Mutex.h>

namespace yat4tango
{

// ============================================================================
//! \class DeviceInfo 
//! \brief Add an 'info' attribute that provide system information about the device
//!
//! \b Usage: 
//! - Initialize the DeviceInfo in the device initialization function (i.e. the *init_device()*
//! function). This is the first thing to do before any other action. For instance:
//! \verbatim yat4tango::DeviceInfo::initialize(this); \endverbatim
//!
//! - Release the appender in the device destructor (i.e. the *delete_device()* function).
//! This is the last thing to do before removing the device. For instance:
//! \verbatim yat4tango::DeviceInfo::release(this); \endverbatim
//!
// ============================================================================
class YAT4TANGO_DECL DeviceInfo
{
public:
   //! \brief Initialization of the inner appender.
   //! 
   //! \param associated_device The Tango device to log.
   //! \param max_log_buffer_depth 
   //! \exception DEVICE_ERROR Thrown when an error occurs while initializing the logger.
   static void initialize (Tango::DeviceImpl* associated_device,
                           const std::string& project_name,
                           const std::string& project_version,
                           const std::string& attr_name = "info");

   //! \brief Termination.
   //! 
   //! \param associated_device The associated Tango device.
   //! \exception DEVICE_ERROR Thrown when an error occurs while releasing the logger.
   static void release (Tango::DeviceImpl* associated_device);

   //! \brief Add a project version.
   //! 
   //! \param version dependency version string
   static void add_dependency(Tango::DeviceImpl* hd, const std::string& name, const std::string& version);

  //! \brief Does nothing! 
  //!
  //! Inherited from log4tango::DeviceInfo virtual interface.
  virtual void close ();

private:

  typedef std::deque<std::string> StringBuffer;

  //- InnerAppender repository 
  typedef std::map<Tango::DeviceImpl*, DeviceInfo*> DeviceInfoRepository;
  typedef DeviceInfoRepository::value_type DeviceInfoEntry;
  typedef DeviceInfoRepository::iterator DeviceInfoIterator;
  typedef DeviceInfoRepository::const_iterator DeviceInfoConstIterator;

  //- provide the yat4tango::DynamicAttributeReadCallback with access to read_callback 
  friend class DynamicAttributeReadCallback;

  //- Ctor
  DeviceInfo ();

  //- Dtor
  virtual ~DeviceInfo ();

  //- Initialization
  void initialize_i (Tango::DeviceImpl* host_device,
                     const std::string& project_name,
                     const std::string& project_version,
                     const std::string& attribute_name);

  //- Termination
  void release_i ();

  //- Adds dependency version
  void add_dependency_i(const std::string& name, const std::string& version);

  //- Reads callback of the underlying dynamic attribute
  void read_callback (DynamicAttributeReadCallbackData& cbd);

  //- the host device
  Tango::DeviceImpl * m_dev;

  //- dynamic attribute manager
  DynamicAttributeManager * m_dam;

  //- thread safety
  yat::Mutex m_lock;

  //- string buffer
  StringBuffer m_buffer;

  //- InnerAppender repository
  static DeviceInfoRepository s_rep;

  //- thread safety
  static yat::Mutex s_rep_lock;

  bool m_buffer_changed;

  //- disabled methods
  DeviceInfo (const DeviceInfo&);
  const DeviceInfo& operator= (const DeviceInfo&);
};

} // namespace

#endif // _DEVICE_INFO_H_
