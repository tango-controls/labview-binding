//----------------------------------------------------------------------------
// Copyright (c) 2004-2014 Synchrotron SOLEIL
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

#ifndef _TASK_H_
#define _TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Thread.h>
#include <yat/threading/MessageQ.h>

// ============================================================================
// CONSTs
// ============================================================================
#define kDEFAULT_TASK_TMO_MSECS         5000
#define kDEFAULT_THD_PERIODIC_TMO_MSECS 1000
//-----------------------------------------------------------------------------

namespace yat
{

// ============================================================================
//! \class Task
//! \brief Undetached thread in association with a message queue.
//!
//! A Task is an implemantation of the Thread class in association with a MessageQ class.
//! The task configuration (including message queue parameters) is set trough the Config configuration structure.
//! %Message types are defined in yat::MessageType enumeration.
//! \remark The yat4Tango::DeviceTask provides the same features than the yat::Task
//! but the former is better adapted to the context of TANGO Device (exception handling/conversion).
// ============================================================================
class YAT_DECL Task : public yat::Thread
{

public:

  //! Task configuration structure.
  struct YAT_DECL Config
  {
    //! Enables TIMEOUT messages.
    //!
    //! If timeout expires without any message received by the task, a TIMEOUT message is sent.
    //! Default value : false.
    bool enable_timeout_msg;
    //! Timeout message period in msec.
    size_t timeout_msg_period_ms;
    //! Enables PERIODIC messages.
    //!
    //! A PERIODIC message is sent every period (best effort).
    //! Default value : false.
    bool enable_periodic_msg;
    //! Periodic message period in msec.
    size_t periodic_msg_period_ms;
    //! \remark Obsolete attribute.
    //! Enables message processing under critical section.
    //! Not recommended! For backward compatibility only.
    //! Default value : false.
    bool lock_msg_handling;
    //! %Message queue low water mark.
    size_t lo_wm;
    //! %Message queue high water mark.
    size_t hi_wm;
    //! Enables throwing exception on post message timeout.
    bool throw_on_post_tmo;
    //! User data (passed back in all messages).
    Thread::IOArg user_data;
    //! Default constructor.
    Config ();
    //! Constructor with parameters.
    Config (bool   enable_timeout_msg,
            size_t timeout_msg_period_ms,
            bool   enable_periodic_msg,
            size_t periodic_msg_period_ms,
            bool   lock_msg_handling,
            size_t lo_wm,
            size_t hi_wm,
            bool   throw_on_post_tmo,
            Thread::IOArg user_data);
  };

  //! \brief Default constructor.
  Task ();
  
  //! \brief Config constructor.
  //! \param cfg Task configuration.
  Task (const Config& cfg);

  //! \brief Destructor.
  virtual ~Task ();

  //! \brief Starts the task synchronously and wait the specified time
  //! (same as go_synchronously - backward compatibility).
  //!
  //! \param tmo_msecs Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  virtual void go (size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);
    
  //! \brief Starts the task synchronously using the specified message 
  //! and wait the specified time
  //! (same as go_synchronously - backward compatibility).
  //!
  //! \param msg Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception PROGRAMMING_ERROR Thrown in case the specified message:
  //!   * is not of type TASK_INIT
  //!   * is not "waitable".
  virtual void go (Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);
  
  //! \brief Starts the task synchronously and wait the specified time
  //! (i.e. wait for the INIT message to be handled).
  //! \param tmo_msecs Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  virtual void go_synchronously (size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);
    
  //! \brief Starts the task synchronously using the specified message and wait 
  //! the specified time
  //! (i.e. wait for the specified init message to be handled).
  //! \param msg Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  //! \exception PROGRAMMING_ERROR Thrown in case the specified message:
  //!   * is not of type TASK_INIT
  //!   * is not "waitable".
  virtual void go_synchronously (Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Starts the task asynchronously 
  //! (i.e. does NOT wait for the INIT message to be handled, but still waits
  //! for the message to be posted).
  //! \param tmo_msecs Timeout for INIT message posting in ms.
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  virtual void go_asynchronously (size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);
    
  //! \brief Starts the task asynchronously (i.e. does NOT wait for the specified init 
  //! message to be handled, but still waits for the message to be posted).
  //! \param msg Init message to send.
  //! \param tmo_msecs Timeout for message posting.
  //! \exception PROGRAMMING_ERROR Thrown in case the specified message is not of type TASK_INIT
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  virtual void go_asynchronously (Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

  //! \brief Aborts the task (join with the underlying thread before returning).
  //!
  //! Provides an implementation to the Thread::exit pure virtual method.
  //! \exception SOFTWARE_ERROR Thrown when EXIT message allocation fails.
  virtual void exit ();

  //! \brief Posts a message to the task asynchronously (i.e. does NOT wait for the message 
  //! to be handled, but still waits for the message to be posted).
  //! \param msg Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \exception INTERNAL_ERROR Thrown when message cannot be posted (msgQ error).
  //! \exception TIMEOUT_EXPIRED Thrown when timeout expires.
  void post (Message * msg, size_t tmo_msecs = kDEFAULT_POST_MSG_TMO);
    
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
  void wait_msg_handled (Message * msg, size_t tmo_msecs = kDEFAULT_MSG_TMO_MSECS);

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
  void msgq_wm_unit (MessageQ::WmUnit _wmu);

  //! \brief %Message queue water marks unit accessor.
  MessageQ::WmUnit msgq_wm_unit () const;

  //! \brief %Message queue low water mark mutator.
  //! \param _lo_wm Low water mark (in message queue unit).
  void msgq_lo_wm (size_t _lo_wm);
  
  //! \brief %Message queue low water mark accessor.
  //! (message queue unit dependent).
  size_t msgq_lo_wm () const;
  
  //! \brief %Message queue high water mark mutator.
  //! \param _hi_wm High water mark (in message queue unit).
  void msgq_hi_wm (size_t _hi_wm);
  
  //! \brief %Message queue high water mark accessor.
  //! (message queue unit dependent).
  size_t msgq_hi_wm () const;

  //! \brief %Message queue Statistics accessor.
  const MessageQ::Statistics & msgq_statistics ();
  
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
  //! \brief Run the task undetached.
  virtual Thread::IOArg run_undetached (Thread::IOArg);
  
  //! \brief Message handler (pure virtual function).
  //! \param msg Message to handle.
  //! \remark After processing message, do NOT release the message (done by yat).
  virtual void handle_message (yat::Message& msg) = 0;

  //! \brief Returns the underlying message queue.
  MessageQ & message_queue ();

private:
  //- actual_timeout
  size_t actual_timeout () const;

  //- the associated messageQ
  MessageQ msg_q_;

  //- timeout msg control flag
  bool timeout_msg_enabled_;

  //- timeout msg period
  size_t timeout_msg_period_ms_;

  //- periodic msg control flag
  bool periodic_msg_enabled_;

  //- periodic msg period 
  size_t periodic_msg_period_ms_;

  //- user data passed to entry point
  Thread::IOArg user_data_;

  //- should we process msg under critical section?
  bool lock_msg_handling_;
  
  //- true if TASK_INIT msg received, false ortherwise
  bool received_init_msg_;

#if defined (YAT_DEBUG)
  //- some statistics counter
  unsigned long next_msg_counter;
  unsigned long user_msg_counter;
  unsigned long ctrl_msg_counter;
#endif
};

// ============================================================================
// Task::post
// ============================================================================
template <typename T> void Task::post (size_t msg_type, 
                                       T * data, 
                                       bool transfer_ownership,  
                                       size_t tmo_msecs)
{
  Message * m = new (std::nothrow) Message(msg_type, DEFAULT_MSG_PRIORITY, false);
  if (! m)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "yat::Message allocation failed",
                    "Task::post");
  }
  m->attach_data(data);
  this->post(m, tmo_msecs);
}
// ============================================================================
// Task::post
// ============================================================================
template <typename T> void Task::post (size_t msg_type,  
                                       const T & data,  
                                       size_t tmo_msecs)
{
  Message * m = new (std::nothrow) Message(msg_type, DEFAULT_MSG_PRIORITY, false);
  if (! m)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "yat::Message allocation failed",
                    "Task::post");
  }
  m->attach_data(data);
  this->post(m, tmo_msecs);
}
// ============================================================================
// Task::wait_msg_handled
// ============================================================================
  template <typename T> void Task::wait_msg_handled (size_t msg_type, 
                                                     T * data, 
                                                     bool transfer_ownership,  
                                                     size_t tmo_msecs)
{
  Message * m = new (std::nothrow) Message(msg_type, DEFAULT_MSG_PRIORITY, true);
  if (! m)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "yat::Message allocation failed",
                    "Task::post");
  }
  m->attach_data(data);
  this->wait_msg_handled(m, tmo_msecs);
}
// ============================================================================
// Task::wait_msg_handled
// ============================================================================
template <typename T> void Task::wait_msg_handled (size_t msg_type,  
                                                   const T & data,  
                                                   size_t tmo_msecs)
{
  Message * m = new (std::nothrow) Message(msg_type, DEFAULT_MSG_PRIORITY, true);
  if (! m)
  {
    THROW_YAT_ERROR("OUT_OF_MEMORY",
                    "yat::Message allocation failed",
                    "Task::post");
  }
  m->attach_data(data);
  this->wait_msg_handled(m, tmo_msecs);
}

// ============================================================================
//! \struct TaskExiter
//! \brief 'Deleter' object to instanciate when using yat::SharedPtr<yat::Task>
//! \brief or with implicite instantiation whith yat::UniquePtr<yat::Task, yat::TaskExiter>
// ============================================================================
struct TaskExiter
{
  //! \brief operator().
  //! \param object The object to delete.
  void operator()(Task* task);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/Task.i>
#endif // YAT_INLINE_IMPL

#endif // _TASK_H_
