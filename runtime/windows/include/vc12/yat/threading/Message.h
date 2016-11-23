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

#ifndef _YAT_MESSAGE_H_
#define _YAT_MESSAGE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/CommonHeader.h>
#if defined(YAT_DEBUG)
# include <yat/time/Timer.h>
#endif
#include <yat/any/GenericContainer.h>
#include <yat/threading/SharedObject.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Mutex.h>
#if defined(_USE_MSG_CACHE_)
# include <yat/memory/Allocator.h>
#endif

namespace yat
{

// ============================================================================
//     MSG PRIORITIES - MSG PRIORITIES - MSG PRIORITIES - MSG PRIORITIES  
// ============================================================================
//-----------------------------------------------------------------------------
//! Highest message priority
//-----------------------------------------------------------------------------
#define HIGHEST_MSG_PRIORITY 0xFFFF
//-----------------------------------------------------------------------------
//! Lowest message priority
//-----------------------------------------------------------------------------
#define LOWEST_MSG_PRIORITY 0
//-----------------------------------------------------------------------------
//! INIT message priority
//-----------------------------------------------------------------------------
#define INIT_MSG_PRIORITY HIGHEST_MSG_PRIORITY
//-----------------------------------------------------------------------------
//! EXIT message priority
//-----------------------------------------------------------------------------
#define EXIT_MSG_PRIORITY HIGHEST_MSG_PRIORITY
//-----------------------------------------------------------------------------
//! MAX (i.e. highest) user message priority
//-----------------------------------------------------------------------------
#define MAX_USER_PRIORITY (HIGHEST_MSG_PRIORITY - 20)
//-----------------------------------------------------------------------------
//! Default message priority (lowest priority)
//-----------------------------------------------------------------------------
#define DEFAULT_MSG_PRIORITY LOWEST_MSG_PRIORITY
// ============================================================================

// ============================================================================
//! \enum yat::MessageType 
//! \brief Message type enumeration.
//! 
//! INIT, EXIT, TIMEOUT & PERIODIC types are predefined control types.
//! For user messages, use type number from FIRST_USER_MSG
//! (for example : \#define MY_MSG_TYPE (FIRST_USER_MSG+1)).
// ============================================================================
typedef enum
{
  //--------------------
  TASK_INIT = 0,
  TASK_TIMEOUT,
  TASK_PERIODIC,
  TASK_WAKEUP,
  TASK_EXIT,
  //--------------------
  FIRST_USER_MSG
} MessageType;

// ============================================================================
//! \class Message 
//! \brief Message exchanged between Task objects.
//!
//! Inherits from SharedObject class. Its main characteristics are : type, priority, 
//! waitable and associated data. \n
//! The waitable characteristic is based on a Condition object.
// ============================================================================
class YAT_DECL Message : private yat::SharedObject
{
#if defined(_USE_MSG_CACHE_)
  //- define what a message cache is
  typedef CachedAllocator<Message, Mutex> Cache;
  //- "global" message cache
  static Cache m_cache;
#endif

public:

#if defined (YAT_DEBUG)
  typedef unsigned long MessageID;
#endif

#if defined(_USE_MSG_CACHE_)
  //- overloads the new operator (makes class "cachable")
  void * operator new (size_t);

  //- overloads the delete operator (makes class "cachable")
  void operator delete (void *);
#endif

  //! \brief Message factory. 
  //!
  //! Creates a new message of specified type.
  //! \param msg_type %Message type.
  //! \param msg_priority %Message priority.
  //! \param waitable Is message is waitable or not.
  //! \exception OUT_OF_MEMORY Thrown if allocation fails due to lack of memory.
  static Message * allocate (size_t msg_type, 
                             size_t msg_priority = DEFAULT_MSG_PRIORITY,
                             bool waitable = false);

  //! \brief Default contructor.
  explicit Message ();

  //! \brief Constructor with parameters.
  //! \param msg_type %Message type.
  //! \param msg_priority %Message priority.
  //! \param waitable Is message is waitable or not.
  explicit Message (size_t msg_type, 
                    size_t msg_priority = DEFAULT_MSG_PRIORITY,
                    bool waitable = false);

  //! \brief Destructor.
  virtual ~Message ();

  //! \brief Returns a string containing information about the message.
  virtual const char * to_string () const;

  //! \brief Returns true if message type is a predefined control type.
  bool is_task_ctrl_message ();

  //! \brief Returns the "shallow" copy of the message.
  //!
  //! Does not really copy message but increase the reference count.
  //! Works as yat::SharedObject::duplicate function.
  //! \remark Useful when sending same message to multiple recipients.
  Message * duplicate ();

  //! \brief Decreases the shared reference count of the message.
  //!
  //! Works as yat::SharedObject::release function.
  void release ();

  //! \brief Returns message type.
  size_t type () const;

  //! \brief Sets message type.
  //! \param t %Message type (from yat::MessageType enumeration + 
  //! user defined types).
  void type (size_t t);

  //! \brief Returns message priority.
  size_t priority () const;

  //! \brief Sets message priority.
  //! \param p %Message priority (from LOWEST_MSG_PRIORITY to HIGHEST_MSG_PRIORITY).
  void priority (size_t p);

  //! \brief Obsolete function.
  //!
  //! Returns message user data.
  void * user_data () const;

  //! \brief Obsolete function.
  //!
  //! Sets message user data.
  void user_data (void * ud);

  //! \brief Returns message size in bytes.
  //!
  //! Depends on associated data length.
  size_t size_in_bytes () const;

  //! \brief Sets message size in bytes.
  //! \param s Size of the message in bytes.
  void size_in_bytes (size_t s);
  
  //! \brief Template function to associate any type of data to a message.
  //!
  //! Example : 
  //! \verbatim m.attach_data<double>(pDouble); \endverbatim
  //! \param _data Data buffer pointer.
  //! \param _transfer_ownership True if data is deleted with message.
  //! \exception OUT_OF_MEMORY Thrown if allocation fails due to lack of memory.
  template <typename T> void attach_data (T * _data, bool _transfer_ownership = true);
  //! \brief Template function to associate any type of data to a message (makes a copy of _data).
  //!
  //! Example : 
  //! \verbatim m.attach_data<double>(myDouble); \endverbatim
  //! \param _data Data buffer.
  //! \exception OUT_OF_MEMORY Thrown if allocation fails due to lack of memory.
  template <typename T> void attach_data (const T & _data);
  //! \brief Template function that returns the message associated data.
  //!
  //! Data is left in message and will be deleted with message.
  //! Example : 
  //! \verbatim T &t = m.get_data<T>(); \endverbatim
  //! \exception RUNTIME_ERROR Thrown if wrong data type put in \<T\>.
  template <typename T> T& get_data () const;
  //! \brief Template function that returns the message associated data.
  //!
  //! Data is left in message and will be deleted with message.
  //! \exception RUNTIME_ERROR Thrown if wrong data type put in \<T\>.
  template <typename T> void get_data (T*& data) const;
  //! \brief Template function that detaches data from message,
  //! i.e. data is not left in the message and will not be deleted with message.
  //!
  //! Before extracting data, checks specified type and data type.
  //! Example : 
  //! \verbatim m.detach_data<double>(myDouble); \endverbatim
  //! \param _data Data buffer.
  //! \exception RUNTIME_ERROR Thrown if wrong data type specified.
  template <typename T> void detach_data (T*& _data) const;
  //! \brief Template function that detaches data from message, 
  //! i.e. data is not left in the message and will not be deleted with message.
  //!
  //! Example : 
  //! \verbatim myDouble = m.detach_data<double>(); \endverbatim
  //! \remark There is no data type check !
  template <typename T> T * detach_data () const;

  //! \brief Template function that checks the message associated data type.
  //!
  //! Returns true if wright type.
  //! Example : 
  //! \verbatim isDouble = m.check_attached_data_type<double>(); \endverbatim
  template <typename T> bool check_attached_data_type () const;

  //! \brief Makes a message waitable.
  //! 
  //! Default value = false.
  //! \exception MEMORY_ERROR Thrown if associated Condition allocation fails on lack of memory.
  void make_waitable ();
  //! \brief Is message waitable ?
  //! 
  //! Returns true if message is waitable, false otherwise.
  bool waitable () const;

  //! \brief Waits for the message to be processed by the associated consumer(s).
  //!
  //! Returns false in case the specified timeout expired before the 
  //! message was processed. Returns true otherwise. 
  //! \param tmo_ms Timeout in ms.
  //! \exception PROGRAMMING_ERROR Thrown in case the message is not "waitable".
  bool wait_processed (unsigned long tmo_ms);
  //! \brief Notifies to awaiting threads (on wait_processed())that the message is handled.
  void processed ();

  //! \brief Tells if attached errors (exceptions) on message.
  bool has_error () const;

  //! \brief Attaches an exception to the message.
  //! \param e Exception to attached to the message.
  void set_error (const Exception & e);

  //! \brief Gets attached errors (exceptions) on message.
  const Exception & get_error () const;

  //! \brief Writes in cout message dump.
  virtual void dump () const;

  //---------------------------------------------
  // Message::id
  //---------------------------------------------
#if defined (YAT_DEBUG)
  MessageID id () const;
#endif

  //---------------------------------------------
  // Message::elapsed_msec
  //---------------------------------------------
#if defined (YAT_DEBUG)
  double elapsed_msec ();
#endif

  //---------------------------------------------
  // Message::restart_timer
  //---------------------------------------------
#if defined (YAT_DEBUG)
  void restart_timer ();
#endif

protected:
  //! \brief Is message processed.
  bool processed_;

  //! \brief %Message type.
  size_t type_;

  //! \brief %Message priority.
  size_t priority_;

  //- the associated user data (same for all messages handled by a given task).
  void * user_data_;

  //! \brief Associated message data (specific to a given message).
  Container * msg_data_;

  //! \brief Has an error occurred during message handling.
  bool has_error_;

  //! \brief Local storage exception.
  Exception exception_;

  //! \brief Condition variable (for waitable messages).
  Condition * cond_;

  //! \brief Size of message content in bytes.
  size_t size_in_bytes_;

#if defined (YAT_DEBUG)
  //- msg id
  MessageID id_;
  //- timer: provides a way to evaluate <dt> between msg <post> and <processing>
  yat::Timer timer_;
  //- msg counter
  static MessageID msg_counter;
  //- ctor/dtor counters
  static size_t ctor_counter;
  static size_t dtor_counter;
#endif

  // = Disallow these operations.
  //--------------------------------------------
  Message & operator= (const Message &);
  Message (const Message &);
};

//---------------------------------------------
// Message::attach_data 
//---------------------------------------------
template <typename T> void Message::attach_data (T * _data, bool _transfer_ownership)
{
  //- try to avoid GenericContainer<T> reallocation
  if (this->msg_data_)
  {
    //- is <msg_data_> content a <T>?
    if (! this->check_attached_data_type<T>())
    {
      delete this->msg_data_;
      this->msg_data_ = 0;
    }
  }
  //- no existing or deleted data
  if (! this->msg_data_)
  {
    //- (re)allocate the underlying generic container
    Container * md = new GenericContainer<T>(_data, _transfer_ownership);
    if (md == 0)
    {
      THROW_YAT_ERROR("OUT_OF_MEMORY",
                      "MessageData allocation failed",
                      "Message::attach_data");
    }
    //- point to <new> data
    this->msg_data_ = md;
  }
  //- simply change message GenericContainer<T> content
  else 
  {
    GenericContainer<T> * c = reinterpret_cast<GenericContainer<T>*>(this->msg_data_); 
    c->set_content(_data, _transfer_ownership);
  }
}

//---------------------------------------------
// Message::attach_data (makes a copy of _data)
//---------------------------------------------
template <typename T> void Message::attach_data (const T & _data)
{
  //- try to avoid GenericContainer<T> reallocation
  if (this->msg_data_)
  {
    //- is <msg_data_> content a <T>?
    if (! this->check_attached_data_type<T>())
    {
      delete this->msg_data_;
      this->msg_data_ = 0;
    }
  }
  //- no existing or deleted data
  if (! this->msg_data_)
  {
    //- (re)allocate the underlying generic container
    Container * md = new GenericContainer<T>(_data);
    if (md == 0)
    {
      THROW_YAT_ERROR("OUT_OF_MEMORY",
                      "MessageData allocation failed",
                      "Message::attach_data");
    }
    //- point to <new> data
    this->msg_data_ = md;
  }
  //- simply change message GenericContainer<T> content
  else 
  {
    GenericContainer<T> * c = reinterpret_cast<GenericContainer<T>*>(this->msg_data_); 
    c->set_content(_data);
  }
}

//---------------------------------------------
// Message::get_data
//---------------------------------------------
template <typename T> T& Message::get_data () const
{
  GenericContainer<T> * c = 0;
  try
  {
    c = dynamic_cast<GenericContainer<T>*>(this->msg_data_);
    if (c == 0)
    {
      THROW_YAT_ERROR("RUNTIME_ERROR",
                      "could not extract data from message [attached data type is not the requested type]",
                      "Message::get_data");
    }
  }
  catch(const std::bad_cast&)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from message [attached data type is not the requested type]",
                    "Message::get_data");
  }
  return c->get_content();
}

//---------------------------------------------
// Message::get_data
//---------------------------------------------
template <typename T> void Message::get_data (T*& data) const
{
  GenericContainer<T> * c = 0;
  try
  {
    c = dynamic_cast<GenericContainer<T>*>(this->msg_data_);
    if (c == 0)
    {
      THROW_YAT_ERROR("RUNTIME_ERROR",
                      "could not extract data from message [attached data type is not the requested type]",
                      "Message::get_data");
    }
  }
  catch(const std::bad_cast&)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from message [attached data type is not the requested type]",
                    "Message::get_data");
  }
  data = c->get_content(false);
}

//---------------------------------------------
// Message::detach_data
//---------------------------------------------
template <typename T> void Message::detach_data (T*& _data) const
{
  _data = yat::any_cast<T>(this->msg_data_, true);
  if (! _data)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from message [attached data type is not the requested type]",
                    "Message::detach_data");
  }
}

//---------------------------------------------
// Message::detach_data
//---------------------------------------------
template <typename T> T * Message::detach_data () const
{
  return yat::any_cast<T>(this->msg_data_, true);
}

//---------------------------------------------
// Message::check_attached_data_type
//---------------------------------------------
template <typename T> bool Message::check_attached_data_type () const
{
  return yat::any_cast<T>(this->msg_data_)
       ? true
       : false;
}

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/Message.i>
#endif // YAT_INLINE_IMPL

#endif // _MESSAGE_H_
