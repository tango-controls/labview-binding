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

#ifndef _DEVICE_TASK_H_
#define _DEVICE_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Task.h>
#include <yat4tango/Export.h>
#include <yat4tango/ExceptionHelper.h>

namespace yat4tango
{

// ============================================================================
//! \class DeviceTask
//! \brief Undetached thread in association with a message queue.
//!
//! The DeviceTask class implements the same functions as the yat::Task class with 
//! specific Tango Device features:
//! - The DeviceTask is associated to a Device and uses the Tango logging management.
//! - Exception handling : uses a Tango::devFailed type instead of a yat::Exception type.
//! - Message handling : uses process_message() function instead of handle_message() function.
//!   Thus, to use a DeviceTask class, inherits from yat4tango::DeviceTask to implement your 
//!   own device task and override the message manager function process_message() which is 
//!   called when a message is received in the message queue.\n
//!   Note that process_message() implementation must use Tango::devFailed exception only. 
//!
//! The DeviceTask doesn't inherits from yat::Task but implements a yat::Task member.
//! To sum up, the yat::Task provides a simplified thread concept which hides shared memory 
//! management by using stimulation messages.\n
//! See yat::Task documentation for behaviour details.
//!
// ============================================================================
class YAT4TANGO_DECL DeviceTask : public Tango::LogAdapter
{
public:
	//! \brief Default constructor.
	//! \param dev Pointer to associated device.
	DeviceTask (Tango::DeviceImpl * dev);
  
    //! \brief Config constructor.
    //! \param cfg Task configuration.
    //! \param dev Pointer to associated device.
	DeviceTask (const yat::Task::Config& cfg, Tango::DeviceImpl * dev);

	//! \brief Destructor.
	virtual ~DeviceTask ();

  //! \brief Starts the task synchronously and wait the specified time
  //! (same as go_synchronously - backward compatibility).
  //!
  //! \param tmo_ms Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception DEVICE_ERROR Thrown when task instanciation fails.
  virtual void go (size_t tmo_ms = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Starts the task synchronously using the specified message 
  //! and wait the specified time
  //! (same as go_synchronously - backward compatibility).
  //!
  //! \param msg Message to send.
  //! \param tmo_ms Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception PROGRAMMING_ERROR Thrown in case the specified message:
  //!   * is not of type yat::TASK_INIT
  //!   * is not "waitable".
  //! \exception DEVICE_ERROR Thrown when task instanciation fails.
  virtual void go (yat::Message * msg, size_t tmo_ms = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Starts the task synchronously and wait the specified time
  //! (i.e. wait for the INIT message to be handled).
  //! \param tmo_msecs Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception DEVICE_ERROR Thrown when task instanciation fails.
  virtual void go_synchronously (size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);
    
  //! \brief Starts the task synchronously using the specified message and wait 
  //! the specified time
  //! (i.e. wait for the specified init message to be handled).
  //! \param msg Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception PROGRAMMING_ERROR Thrown in case the specified message:
  //!   * is not of type yat::TASK_INIT
  //!   * is not "waitable".
  //! \exception DEVICE_ERROR Thrown when task instanciation fails.
  virtual void go_synchronously (yat::Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Starts the task asynchronously 
  //! (i.e. does NOT wait for the INIT message to be handled, but still waits
  //! for the message to be posted).
  //! \param tmo_msecs Timeout for INIT message posting in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception DEVICE_ERROR Thrown when task instanciation fails.
  virtual void go_asynchronously (size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);
    
  //! \brief Starts the task asynchronously (i.e. does NOT wait for the specified init 
  //! message to be handled, but still waits for the message to be posted).
  //! \param msg Init message to send.
  //! \param tmo_msecs Timeout for message posting.
  //! \exception PROGRAMMING_ERROR Thrown in case the specified message is not of type yat::TASK_INIT
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception DEVICE_ERROR Thrown when task instanciation fails.
  virtual void go_asynchronously (yat::Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Aborts the task (joins with the underlying thread before returning).
  //!
  //! \exception SOFTWARE_ERROR Thrown when EXIT message allocation fails.
  //! \exception DEVICE_ERROR Thrown when task exit fails.
  virtual void exit ();

  //! \brief Posts a message to the task asynchronously (i.e. does NOT wait for the message 
  //! to be handled, but still waits for the message to be posted).
  //! \param msg Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  void post (yat::Message * msg, size_t tmo_msecs = kDEFAULT_POST_MSG_TMO);

  //! \brief Posts the specified message type to the task asynchronously (i.e. does NOT wait for 
  //! the message to be handled, but still waits for the message to be posted).
  //! \param msg_type Message type to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  void post (size_t msg_type, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Posts the specified message type with specified data to the task asynchronously
  //! (i.e. does NOT wait for the message to be handled, but still waits for the message to be posted).
  //! \param msg_type Message type to send.
  //! \param data Data buffer to send with the message.
  //! \param transfer_ownership True if message is to be deleted by the task.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  template <typename T> void post (size_t msg_type, T * data, bool transfer_ownership, size_t tmo_msecs);

  //! \brief Posts the specified message type with specified data to the task asynchronously
  //! (i.e. does NOT wait for the message to be handled, but still waits for the message to be posted).
  //! \param msg_type Message type to send.
  //! \param data Data buffer to send with the message.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  template <typename T> void post (size_t msg_type, const T & data, size_t tmo_msecs);

  //! \brief Posts the specified message to the task then waits for this message to be handled
  //! (synchronous approach).
  //! \param msg Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  void wait_msg_handled (yat::Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Posts the specified message type to the task then waits for this message to be handled
  //! (synchronous approach).
  //! \param msg_type Message type to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  void wait_msg_handled (size_t msg_type, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Posts the specified message type with specified data to the task then waits for the message to be handled 
  //! (synchronous approach).
  //! \param msg_type Message type to send.
  //! \param data Data buffer to send with the message.
  //! \param transfer_ownership True if message is to be deleted by the message.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  template <typename T> void wait_msg_handled (size_t msg_type, T * data, bool transfer_ownership, size_t tmo_msecs);

  //! \brief Posts the specified message type with specified data to the task then waits for the message to be handled 
  //! (synchronous approach).
  //! \param msg_type Message type to send.
  //! \param data Data buffer to send with the message.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  template <typename T> void wait_msg_handled (size_t msg_type, const T & data, size_t tmo_msecs);

  //! \brief Timeout message period mutator.
  //! \param p_msecs Timeout in ms.
  void set_timeout_msg_period (size_t p_msecs);
  
  //! \brief Periodic message period accessor.
  size_t get_timeout_msg_period () const;
  
  //! \brief Enable/disable timeout messages.
  //! \param enable True = enabled, false = disabled.
  void enable_timeout_msg (bool enable);

  //! \brief Returns timeout messages handling status.
  bool timeout_msg_enabled () const;

  //! \brief Periodic message period mutator.
  //! \param p_msecs Period in ms.
  void set_periodic_msg_period (size_t p_msecs);
  
  //! \brief Periodic message period accessor.
  size_t get_periodic_msg_period () const;

  //! \brief Enable/disable periodic messages.
  //! \param enable True = enabled, false = disabled.
  void enable_periodic_msg (bool enable);

  //! \brief Returns period messages handling status.
  bool periodic_msg_enabled () const;

  //! \brief %Message queue water marks unit mutator.
  //! \param _wmu %Message queue unit.
  void msgq_wm_unit (yat::MessageQ::WmUnit _wmu);

  //! \brief %Message queue water marks unit accessor.
  yat::MessageQ::WmUnit msgq_wm_unit () const;

  //! \brief %Message queue low water mark mutator.
  //! \param _lo_wm Low water mark (in message queue unit).
  void msgq_lo_wm (size_t _lo_wm);
  
  //! \brief %Message queue low water mark accessor
  //! (message queue unit dependent).
  size_t msgq_lo_wm () const;
  
  //! \brief %Message queue high water mark mutator.
  //! \param _hi_wm High water mark (in message queue unit).
  void msgq_hi_wm (size_t _hi_wm);
  
  //! \brief %Message queue high water mark accessor
  //! (message queue unit dependent).
  size_t msgq_hi_wm () const;
  
  //! \brief %Message queue Statistics accessor.
  const yat::MessageQ::Statistics & msgq_statistics ();

  //! \brief Resets the message queue statistics.
  void reset_msgq_statistics ();
  
  //! \brief Should the underlying message queue throw an exception
  //! on post message timeout expiration?
  //!
  //! Default value : false.
  //! \param _strategy True = exception thrown, False = no exception thrown.
  void throw_on_post_msg_timeout (bool _strategy);

  //! \brief Clears all pending messages in the message queue.
  size_t clear_pending_messages ();

  //! \brief Clears pending messages of type \<msg_type\> in the message queue.
  //! \param msg_type Message type to be cleared.
  size_t clear_pending_messages (size_t msg_type);
  
protected:
  //! \brief The yat::Message handler (pure virtual function).
  //!
  //! Must be implemented by derived classes.
  //! \param msg Message to handle.
  //! \remark After processing message, do NOT release the message (done by yat).
  //! \remark Be sure that your implementation only throws Tango::DevFailed exceptions.
  virtual void process_message (yat::Message& msg) = 0;

private:
  friend class Delegate;

  //- helper member
  yat::Task * yt () const;

  //- delegate all activity to the underlying yat::Task;
  yat::Task * yt_;
};

// ============================================================================
// DeviceTask::post
// ============================================================================
template <typename T> void DeviceTask::post (size_t msg_type, 
                                             T * data, 
                                             bool transfer_ownership,  
                                             size_t tmo_msecs)
{
  try
  {
    this->yt()->post(msg_type, data, transfer_ownership, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::post
// ============================================================================
template <typename T> void DeviceTask::post (size_t msg_type,  
                                             const T & data,  
                                             size_t tmo_msecs)
{
  try
  {
    this->yt()->post(msg_type, data, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::wait_msg_handled
// ============================================================================
template <typename T> void DeviceTask::wait_msg_handled (size_t msg_type, 
                                                         T * data, 
                                                         bool transfer_ownership,  
                                                         size_t tmo_msecs)
{
  try
  {
    this->yt()->wait_msg_handled(msg_type, data, transfer_ownership, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
// DeviceTask::wait_msg_handled
// ============================================================================
template <typename T> void DeviceTask::wait_msg_handled (size_t msg_type,  
                                                         const T & data,  
                                                         size_t tmo_msecs)
{
  try
  {
    this->yt()->wait_msg_handled(msg_type, data, tmo_msecs);
  }
  catch (const yat::Exception& ye)
  {
    throw YATDevFailed(ye);
  }
}

// ============================================================================
//! \struct DeviceTaskExiter
//! \brief 'Deleter' object to instanciate when using yat::SharedPtr<yat::Task>
//! \brief or with implicite instantiation whith yat::UniquePtr<yat::Task, yat::TaskExiter>
// ============================================================================
struct DeviceTaskExiter
{
  //! \brief operator().
  //! \param object The object to delete.
  void operator()(DeviceTask* task);
};


} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat4tango/DeviceTask.i>
#endif // YAT_INLINE_IMPL

#endif // _DEVICE_TASK_H_
