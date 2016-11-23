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

#ifndef _YAT4TANGO_TS_DEV_PROXY_H_
#define _YAT4TANGO_TS_DEV_PROXY_H_

#include <yat/threading/Mutex.h>
#include <yat4tango/CommonHeader.h>

namespace yat4tango
{

// ============================================================================
//! \deprecated ThreadSafeDeviceProxy
//! \brief The \b DEPRECATED thread safe Device proxy class.
//!
//! \remark Since Tango 7 (resp. 8), the Tango::DeviceProxy class is fully
//! (resp. optionally) thread safe.
// ============================================================================
class YAT4TANGO_DECL ThreadSafeDeviceProxy
{
public:

  //! \brief Constructor.
  //! 
  //! Creates a Tango::DeviceProxy to access the Device.
  //! \param dev_name The name of the Device.
  //! \exception Tango::DevFailed Thrown if Device's name is not correct.
  //! \exception Tango::DevFailed Thrown if Device proxy construction fails.
  ThreadSafeDeviceProxy (const std::string& dev_name);

  //! \brief Constructor.
  //! 
  //! Creates a Tango::DeviceProxy to access the Device.
  //! \param dev_name The name of the Device.
  //! \exception Tango::DevFailed Thrown if Device's name is not correct.
  //! \exception Tango::DevFailed Thrown if Device proxy construction fails.
  ThreadSafeDeviceProxy (const char * dev_name);

  //! \brief Destructor.
  virtual ~ThreadSafeDeviceProxy ();

  //! \brief Gets the Device's status.
  //! 
  //! Returns the Device's status as a string.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  const std::string status ();

  //! \brief Gets the Device's state.
  //! 
  //! Returns the Device's state as one of Tango::DevState.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::DevState state ();

  //! \brief Sends a ping to the Device.
  //! 
  //!  Returns the elapsed time in microseconds.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  int ping ();

  //! \brief Queries the Device for information about a command. 
  //! 
  //! Returns information as a CommandInfo type.
  //! \param cmd_name Command name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::CommandInfo command_query (const std::string&);

  //! \brief Queries the Device for information on all commands.
  //! 
  //! Returns a vector of CommandInfo types.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::CommandInfoList* command_list_query ();

  //! \brief Queries the Device for information about an attribute.
  //! 
  //! Returns a information as a Tango::AttributeInfoEx type (which inherits from the 
  //! Tango::AttributeInfo type).
  //! \param name Attribute's name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::AttributeInfoEx attribute_query (const std::string&);

  //! \brief Queries the Device for information on all attributes.
  //! 
  //!  Returns a vector of AttributeInfo types.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::AttributeInfoList* attribute_list_query ();

  //! \brief Reads an attribute. 
  //! 
  //! Returns the read value as a Tango::DeviceAttribute object. To extract the value, use 
  //! the operator of the class DeviceAttribute which corresponds to the data type 
  //! of the attribute.
  //! \param attr_name Attribute's name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::DeviceAttribute read_attribute (const std::string& attr_name);

  //! \brief Reads an attribute. 
  //! 
  //! Returns the read value as a Tango::DeviceAttribute object. To extract the value, use 
  //! the operator of the class DeviceAttribute which corresponds to the data type 
  //! of the attribute.
  //! \param attr_name Attribute's name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  Tango::DeviceAttribute read_attribute (const char *attr_name);

  //! \brief Reads asynchronously (callback model) an attribute. 
  //!
  //! The callback object should be an instance of a user class inheriting from the 
  //! Tango::CallBack class with the *attr_read()* method overloaded.
  //! \param attr_name Attribute's name.
  //! \param cb Reference to a callback object.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  void read_attribute_asynch (const char *attr_name, Tango::CallBack &cb);

  //! \brief Reads asynchronously (polling model) an attribute.
  //! 
  //! Returns an asynchronous call identifier which is needed to get the attribute's value.
  //! \param attr_name Attribute's name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  long read_attribute_asynch (const char *attr_name);

  //! \brief Reads the specified list of attributes.
  //! 
  //! Returns a vector of Tango::DeviceAttribute types. To extract the value, use the operator 
  //! of the class DeviceAttribute which corresponds to the data type of the attribute.
  //! \param attr_names List of attributes' name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  std::vector<Tango::DeviceAttribute>* read_attributes (const std::vector<std::string>& attr_names);

  //! \brief Reads asynchronously (callback model) an attribute list. 
  //!
  //! The callback object should be an instance of a user class inheriting from the 
  //! Tango::CallBack class with the *attr_read()* method overloaded.
  //! \param attr_names List of attributes' name.
  //! \param cb Reference to a callback object.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  void read_attributes_asynch (const std::vector<std::string>& attr_names, Tango::CallBack &cb);

  //! \brief Reads asynchronously (polling model) the specified list of attributes. 
  //!
  //! Returns an asynchronous call identifier which is needed to get attributes' value.
  //! \param attr_names List of attributes' name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  long read_attributes_asynch (const std::vector<std::string>& attr_names);

  //! \brief Writes an attribute.
  //! 
  //! To insert the value to write, use the operator of the class DeviceAttribute
  //! which corresponds to the data type of the attribute.
  //! \param attr_value The value to write.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  //! \exception Tango::DevFailed Thrown if Device is still locked.
  void write_attribute (const Tango::DeviceAttribute& attr_value);

  //! \brief Writes asynchronously (callback model) an attribute.
  //! 
  //! The callback object should be an instance of a user class inheriting from the 
  //! Tango::CallBack class with the *attr_written()* method overloaded.
  //! \param attr_value The value to write.
  //! \param cb Reference to a callback object.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  void write_attribute_asynch (const Tango::DeviceAttribute& attr_value, Tango::CallBack &cb);

  //! \brief Writes asynchronously (polling model) an attribute.
  //! 
  //! Returns an asynchronous call identifier which is needed to get the server's reply.
  //! \param attr_value The value to write.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  long write_attribute_asynch (const Tango::DeviceAttribute& attr_value);

  //! \brief Writes the specified attributes.
  //! 
  //! To insert the values to write, use the operator of the DeviceAttribute class 
  //! which corresponds to the data type of the attributes.
  //! \param attr_values The list of attributes to write.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  //! \exception Tango::DevFailed Thrown if Device is still locked.
  void write_attributes (const std::vector<Tango::DeviceAttribute>& attr_values);

  //! \brief Writes asynchronously (callback model) the specified attributes. 
  //! 
  //! The callback object should be an instance of a user class inheriting from the 
  //! Tango::CallBack class with the *attr_written()* method overloaded.
  //! \param attr_values The list of attributes to write.
  //! \param cb Reference to a callback object.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  void write_attributes_asynch (const std::vector<Tango::DeviceAttribute>& attr_values, Tango::CallBack &cb);
  
  //! \brief Writes asynchronously (polling model) the specified attributes.
  //! 
  //! Returns an asynchronous call identifier which is needed to get the server's reply.
  //! \param attr_values The list of attributes to write.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  long write_attributes_asynch (const std::vector<Tango::DeviceAttribute>& attr_values);

  //! \brief Executes a command which takes no input argument.
  //! 
  //! Returns the result of the command as a DeviceData object.
  //! \param cmd_name Command's name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  //! \exception Tango::DevFailed Thrown if Device is still locked.
  Tango::DeviceData command_inout (const std::string& cmd_name);

  //! \brief Executes a command which takes no input argument.
  //! 
  //! Returns the result of the command as a DeviceData object.
  //! \param cmd_name Command's name.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  //! \exception Tango::DevFailed Thrown if Device is still locked.
  Tango::DeviceData command_inout (const char * cmd_name);

  //! \brief Executes a command which takes input arguments.
  //! 
  //! Returns the result of the command as a DeviceData object.
  //! \param cmd_name Command's name.
  //! \param d Input arguments of the command, passed in a DeviceData object.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  //! \exception Tango::DevFailed Thrown if Device is still locked.
  Tango::DeviceData command_inout (const std::string& cmd_name, const Tango::DeviceData &d);

  //! \brief Executes a command which takes input arguments.
  //! 
  //! Returns the result of the command as a DeviceData object.
  //! \param cmd_name Command's name.
  //! \param d Input arguments of the command, passed in a DeviceData object.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  //! \exception Tango::DevFailed Thrown if Device is still locked.
  Tango::DeviceData command_inout (const char *cmd_name, const Tango::DeviceData &d);

  //! \brief Subscription for event reception. 
  //!
  //! Returns an event identifier which has to be specified when un-subscribing from this
  //! event.
  //!
  //! The client implements a callback method which is triggered when the event is 
  //! received. Filtering is done based on the reason specified and the event's type.
  //! \param attr_name Device attribute's name which will be sent as an event.
  //! \param event The event's reason (one of Tango::EventType).
  //! \param cb Reference to a callback object.
  //! \param filters List of additional filtering reasons.
  //! \exception Tango::DevFailed Thrown if an error occurs during 
  //! the operation.
  int subscribe_event(const string &attr_name, 
                      Tango::EventType event, 
                      Tango::CallBack *cb, 
                      const std::vector<std::string> &filters);

  //! \brief Un-subscription from event reception.
  //! 
  //! Un-subscribe a client from receiving the specified event.
  //! \param event_id Event identifier (returned by the *subscribe_event()* method).
  //! \exception Tango::DevFailed Thrown if an error occurs during the operation.
  void unsubscribe_event(int event_id);

  //! \brief Sets the client side timeout for Device access. 
  //! 
  //! Any method which takes longer than this time to execute will throw an exception.
  //! \param tmo_ms Timeout in milliseconds. 
  void set_timeout_millis (int tmo_ms);

  //! \brief Gets the client side timeout.
  //! 
  //! Returns the current client timeout in milliseconds.
  int get_timeout_millis ();

  //! \brief Gets the last \<n\> commands executed on the Device server.
  //! 
  //!  Returns a pointer to a vector of strings containing the date, time, 
  //! command, and from which client computer the command was executed.
  //! \param n The number of commands to retrieve.
  //! \exception Tango::DevFailed Thrown if Device access fails.
  std::vector<std::string> * black_box (int n);

  //! \brief Gets the underlying Tango::DeviceProxy object.
  //! 
  //! Returns the internal Tango::DeviceProxy object.
  Tango::DeviceProxy& unsafe_proxy ();
  
  //! \brief Gets the Device's name.
  //! 
  //! Returns the Device's name as it is stored locally.
  std::string dev_name ();

private:

  Tango::DeviceProxy  dp_;

  yat::Mutex lock_;

  // = Disallow these operations (except for friends).
  //---------------------------------------------------------
  ThreadSafeDeviceProxy (const ThreadSafeDeviceProxy&);
  void operator= (const ThreadSafeDeviceProxy&); 
};

} // namespace 

#if defined (YAT_INLINE_IMPL)
  #include "ThreadSafeDeviceProxy.i"
#endif 

#endif // _TS_DEV_PROXY_H_
