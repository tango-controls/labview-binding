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

#ifndef _MESSAGE_Q_H_
#define _MESSAGE_Q_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <iostream>
#include <yat/CommonHeader.h>
#include <list>
#if defined (YAT_WIN32)
# include <sys/timeb.h>
#else
# include <sys/time.h>
#endif
#include <yat/threading/Semaphore.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Message.h>

// ============================================================================
// CONSTs
// ============================================================================
//! Default post message timeout.
#define kDEFAULT_POST_MSG_TMO   1000
//-----------------------------------------------------------------------------
//! Default message queue low water mark.
#define kDEFAULT_LO_WATER_MARK  8
//! Default message queue high water mark.
#define kDEFAULT_HI_WATER_MARK  64
//-----------------------------------------------------------------------------
#define kMIN_LO_WATER_MARK      kDEFAULT_LO_WATER_MARK
//-----------------------------------------------------------------------------
#define kMIN_WATER_MARKS_DIFF   kDEFAULT_LO_WATER_MARK
//-----------------------------------------------------------------------------

namespace yat
{

// ============================================================================
//! \class MessageQ 
//! \brief %Message queue of Message messages.
//!
//! MessageQ is used to manage Message transmission/reception between Task objects.
//! It is a FIFO message queue for equal priority messages.
// ============================================================================
class YAT_DECL MessageQ
{
  friend class Task;

  typedef std::list<yat::Message *> MessageQImpl;

public:
  //! %Message queue state.
  typedef enum
  {
    OPEN,
    CLOSED
  } State;
  
  //! Low/high water marks unit.
  typedef enum
  {
    //! Number of messages in msgQ (default).
    NUM_OF_MSGS,
    //! Number of bytes in msgQ.
    NUM_OF_BYTES
  } WmUnit;
  
  //! %Message queue statistics.
  struct YAT_DECL Statistics
  {
    //! Default constructor.
    Statistics ();
    //! Dumps statistics to specified output.
    //! \param out Output
    void dump (std::ostream& out = std::cout) const;
    //! Number of times MessageQ reached the hi-water mark.
    size_t has_been_saturated_;
    //! Number of times MessageQ reached the low-water mark.
    size_t has_been_unsaturated_;
    //! Maximum pending charge reached.
    unsigned long max_pending_charge_reached_;
    //! Maximum number of pending messages reached.
    unsigned long max_pending_msgs_reached_;
    //! Total number of messages posted after "waiting for room in MsgQ".
    unsigned long posted_with_waiting_msg_counter_;
    //! Total number of messages posted without "waiting for room in MsgQ".
    unsigned long posted_without_waiting_msg_counter_;
    //! Total number of trashed messages.
    unsigned long trashed_msg_counter_;
    //! Total number of messages trashed on post timeout.
    unsigned long trashed_on_post_tmo_counter_;
    //! Current pending charge in bytes.
    unsigned long pending_charge_;
    //! Current pending charge in number of messages.
    unsigned long pending_mgs_;
    //! MessageQ unit. 
    WmUnit wm_unit_;
  };

  //! \brief Constructor with parameters.
  //! \param lo_wm Low water mark.
  //! \param hi_wm High water mark.
  //! \param throw_on_post_tmo Enables exception creation on post timeout expiration.
  MessageQ (size_t lo_wm = kDEFAULT_LO_WATER_MARK,
            size_t hi_wm = kDEFAULT_HI_WATER_MARK,
            bool throw_on_post_tmo = false);

  //! \brief Destructor.
  virtual ~ MessageQ ();

  //! \brief Posts a Message into the message queue.
  //!
  //! Returns 0 if the message was successfully posted, -1 otherwise. 
  //! \param msg %Message to send.
  //! \param tmo_msecs Timeout in ms.
  //! \remark %Message is destroyed (i.e. released) in case it could not be posted. 
  //! \remark Can NOT post any TIMEOUT or PERIODIC msg (yat::Task model violation).
  //!
  //! \exception TIMEOUT_EXPIRED Thrown on timeout expiration, if *throw_on_post_msg_timeout* is set 
  //! to true.
  int post (yat::Message * msg, size_t tmo_msecs = kDEFAULT_POST_MSG_TMO);

  //! \brief Extracts next message from the message queue. 
  //!
  //! Waits for a message the specified time.
  //! \param tmo_msecs Tiemout in ms.
  Message * next_message (size_t tmo_msecs);
  
  //! \brief Water marks unit mutator.
  //! \param _wmu Water mark unit.
  void wm_unit (WmUnit _wmu);

  //! \brief Water marks unit accessor.
  WmUnit wm_unit () const;
  
  //! \brief Low water mark mutator.
  //! \param _lo_wm Low water mark (in msgQ current unit).
  void lo_wm (size_t _lo_wm);

  //! \brief Low water mark accessor.
  size_t lo_wm () const;

  //! \brief High water mark mutator.
  //! \param _hi_wm High water mark (in msgQ current unit).
  void hi_wm (size_t _hi_wm);

  //! \brief High water mark accessor.
  size_t hi_wm () const;
  
  //! \brief Sets if the message queue should throw an exception on post message
  //! timeout expiration.
  //! \param _strategy True if exception to be thrown.
  void throw_on_post_msg_timeout (bool _strategy);

  //! \brief Clears message queue content.
  //!
  //! Returns number of removed messages.
  size_t clear();

  //! \brief Clears pending message of specified type.
  //!
  //! Returns number of removed messages.
  //! \param msg_type %Message type.
  size_t clear_pending_messages (size_t msg_type);
  
  //! \brief Closes the message queue.
  void close ();
  
  //! Returns the MessageQ Statistics.
  const Statistics & statistics ();
  
  //! \brief Resets the MessageQ Statistics.
  void reset_statistics ();

  //! \brief Enable/disable timeout messages.
  //! \param enable True = enabled, false = disabled.
  void enable_timeout_msg (bool enable);

  //! \brief Returns timeout messages handling status.
  bool timeout_msg_enabled () const;

  //! \brief Enable/disable periodic messages.
  //! \param enable True = enabled, false = disabled.
  void enable_periodic_msg (bool enable);

  //! \brief Returns period messages handling status.
  bool periodic_msg_enabled () const;
  
private:
  //- periodic msg tmo expired?
  bool periodic_tmo_expired_i (double _tmo_msecs);
  
  //- clears msgQ content (returns num of trashed messages).
  size_t clear_i (bool notify_waiters = true);

  //- waits for the msQ to contain at least one msg.
  //- returns false if tmo expired, true otherwise.
  bool wait_not_empty_i (size_t tmo_msecs);

  //- waits for the msQ to have room for new messages.
  //- returns false if tmo expired, true otherwise.
  bool wait_not_full_i (size_t tmo_msecs);

  //- inserts a msg according to its priority.
  void insert_i (Message * msg);

  //- increments the pending charge.
  void inc_pending_charge_i (Message * msg);
  
  //- decrements the pending charge.
  void dec_pending_charge_i (Message * msg);
  
  //- use a std::deque to implement msgQ
  MessageQImpl msg_q_;

  //- sync. object in order to make the msgQ thread safe
  Mutex lock_;

  //- Producer(s) synch object
  Condition msg_producer_sync_;

  //- Consumer synch object
  Condition msg_consumer_sync_;
  
  //- state
  MessageQ::State state_;

  //- timeout msg handling flag
  bool enable_timeout_msg_;

  //- periodic msg handling flag
  bool enable_periodic_msg_;

  //- PERIODIC msg timeout
  Timer periodic_msg_timer_;

  //- low water marks
  size_t lo_wm_;

  //- high water marks
  size_t hi_wm_;

  //- msqQ saturation flag
  volatile bool saturated_;

  //- expection activation flag
  bool throw_on_post_msg_timeout_;

  //- flag indicating whether or not the last returned msg was a periodic msg
  //- we use this flag in order to avoid PERIODIC event flooding in case
  //- the PERIODIC event frequency is really high - which could prevent other
  //- messages from being handled. reciprocally, a very high msg posting freq.
  //- could prevent the PERIODIC msg from being handled. the following tries
  //- to ensure that any msg is "finally" handled.
  bool last_returned_msg_periodic_;

  //- water marks unit
  MessageQ::WmUnit wm_unit_;

  //- depending on the water marks unit, this can be:
  //- 1. the number of pending messages (unit_ = NUM_OF_MSGS)
  //- 2. the number of pending bytes (unit_ = NUM_OF_BYTES)
  size_t pending_charge_;
  
  //- some task/msgQ stats 
  Statistics stats_;

  // = Disallow these operations.
  //--------------------------------------------
  MessageQ & operator= (const MessageQ &);
  MessageQ (const MessageQ &);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/threading/MessageQ.i>
#endif // YAT_INLINE_IMPL

#endif // _MESSAGE_Q_H_
