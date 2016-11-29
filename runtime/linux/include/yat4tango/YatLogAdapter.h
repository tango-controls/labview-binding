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
//    Nicolas Leclercq
//    Synchrotron SOLEIL 
//------------------------------------------------------------------------------
/*!
 * \authors See AUTHORS file
 */

#ifndef _YAT4TANGO_YAT_LOG_ADAPTER_H_
#define _YAT4TANGO_YAT_LOG_ADAPTER_H_

#include <yat/utils/Logging.h>
#include <yat/utils/Singleton.h>
#include <yat4tango/LogHelper.h>

namespace yat4tango
{
  
//=============================================================================
///
//! \class YatLogAdapter
//! \brief Redirecting log messages from YAT logging system to the Tango device layer
//!
//! This class provides a way redirect log messages emitted through the YAT logging service
//! to the Tango layer.
//! 
//! \b Usage: 
//! - Initialize the adapter in the device initialization function (i.e. the *init_device()*
//! function). This is the first thing to do before any other action. For instance:
//! \verbatim yat4tango::YatLogAdapter::initialize(this); \endverbatim
//!
//! - Release the adapter in the device destructor (i.e. the *delete_device()* function).
//! This is the last thing to do before removing the device. For instance:
//! \verbatim yat4tango::YatLogAdapter::release(); \endverbatim
//!
//! Inherits from yat::ILogTarget class.
//!
//=============================================================================
class YatLogAdapter: public yat::ILogTarget, yat::Singleton<YatLogAdapter>
{
public:
  static void initialize(Tango::DeviceImpl* device_p);
  static void release();

  void log(yat::ELogLevel log_level, yat::pcsz log_type, const std::string& log_msg);

private:
  Tango::DeviceImpl*            m_device_p;
	yat::LogCatcher*              m_log_catcher_p;
  yat4tango::TangoLogAdapter*   m_tango_log_adapter;
};

} // namespace

#endif
