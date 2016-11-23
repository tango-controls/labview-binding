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

#ifndef _MONITORED_DEVICE_TASK_H_
#define _MONITORED_DEVICE_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/DeviceTask.h>
#include <yat4tango/MonitoredDevice.h>

namespace yat4tango
{

// ============================================================================
//! \class MonitoredDeviceTask 
//! \brief The monitored task class.
//!
//! This class implements a threaded "polling Device" capability, i.e. the monitoring 
//! of a Device by a remote client (which can also be a Device), through a threaded task.
//!
//! The underlying task (yat::Task class) uses a synchronous mechanism of polling 
//! that can be suspended and resumed.
//!
//! Upon PERIODIC message reception, the monitored task polls the Device to get
//! the attributes value.
//!
//! Inherits from yat4tango::DeviceTask (for the threading capability) & from 
//! yat4tango::MonitoredDevice (for access to Device attributes).
//!
// ============================================================================
class YAT4TANGO_DECL MonitoredDeviceTask : private yat4tango::DeviceTask, 
                                           public yat4tango::MonitoredDevice
{
public:
  //! \brief Constructor.
  //!
  //! \param monitored_device The name of the Device to monitor.
  //! \param host_device The host Tango device (for logging purpose).
  MonitoredDeviceTask (const std::string & monitored_device, 
                       Tango::DeviceImpl * host_device = 0);

  //! \brief Destructor.
  virtual ~MonitoredDeviceTask ();

  //! \brief Starts the underlying task (i.e. starts polling the device).
  //!
  //! The default polling frequency is 1 Hz (i.e. 1000 msecs period).
  //! \exception TIMEOUT_EXPIRED Thrown when the task instantiation's timeout expires.
  //! \exception DEVICE_ERROR Thrown when task instantiation fails.
  void start ();

  //! \brief Suspends the polling mechanism.
  void suspend ();

  //! \brief Resumes the polling mechanism.
  void resume ();

  //! \brief Asks the underlying task to exit.
  //!
  //! \exception SOFTWARE_ERROR Thrown when EXIT message allocation fails.
  //! \exception DEVICE_ERROR Thrown when task exit fails.
  void quit ();
  
  //! \brief Sets the attributes' polling period (in milliseconds).
  //! 
  //! The task must be re-started to take the new value into account, no ? soso Question
  //! \param pp_msecs The new polling period (in milliseconds).
  void set_polling_period (size_t pp_msecs);

  //! \brief Gets the attributes' polling period (in milliseconds).
  //!
  //! Returns the current polling period (in milliseconds).
  size_t get_polling_period () const;

protected:
  //! \brief The yat::Message handler implementation.
  //!
  //! Polls the associated monitored Device.
  //! 
  //! This method can be overloaded in order to define a specific task behaviour.
  //! In this case, don't forget to call the MonitoredDevice::poll_attributes() 
  //! method upon reception of the yat::TASK_PERIODIC message.
  //!
  //! \param msg Message to handle.
  //! \remark After processing message, do NOT release the message (done by yat).
  //! \remark Be sure that your implementation only throws Tango::DevFailed exceptions.
  virtual void process_message (yat::Message& msg);

private:
  //- suspend/resume flag
  bool m_suspended;

  //- polling period
  size_t m_pp;
};

} // namespace

#endif // _MONITORED_DEVICE_TASK_H_
