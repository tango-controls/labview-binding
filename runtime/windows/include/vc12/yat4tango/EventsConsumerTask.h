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

#ifndef _EVENT_CONSUMER_TASK_H_
#define _EVENT_CONSUMER_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/any/Any.h>
#include <yat4tango/DeviceTask.h>

#if (TANGO_VERSION_MAJOR >= 8)

namespace yat4tango
{

// ============================================================================
//! \struct EventData
//! \brief The yat4tango version of the Tango EventData. This is the data struct
//! \brief attached to the messages asscoiated with PERIODIC, CHANGE and USER
//! \brief Tango events.
// ============================================================================
typedef struct EventData
{
  EventData ()
    : dev_proxy(0),
      attr_data_format(Tango::FMT_UNKNOWN),
      attr_data_type(0),
      evt_type(Tango::PERIODIC_EVENT),
      evt_id(0),
      has_error(false),
      attr_value(0),
      user_data(0)
  {}
  EventData (EventData& src)
  {
    *this = src;
  }
  ~EventData ()
  {
    //- delete attr_value;
  }
  inline void operator= (EventData& src)
  {
    dev_proxy        = src.dev_proxy;
    dev_name         = src.dev_name;
    attr_name        = src.attr_name;
    attr_data_format = src.attr_data_format;
    attr_data_type   = src.attr_data_type;
    evt_type         = src.evt_type;
    evt_id           = src.evt_id;
    has_error        = src.has_error;
    errors           = src.errors;
    user_data        = src.user_data;
    attr_value       = src.attr_value;
    src.attr_value   = 0;
  }
  //- event source: device proxy
  Tango::DeviceProxy* dev_proxy;
  //- event source: device name
  std::string dev_name;
  //- event source: attribute name
  std::string attr_name;
  //- event source: attribute data format
  Tango::AttrDataFormat attr_data_format;
  //- event source: attribute data type
  int attr_data_type;
  //- event type
  Tango::EventType evt_type;
  //- event identifier
  int evt_id;
  //- error flag
  bool has_error;
  //- errors list 
  Tango::DevErrorList errors;
  //- the actual event data (NULL in case has_error is true)
  Tango::DeviceAttribute* attr_value;
  //- user data
  void* user_data;
} EventData;

// ============================================================================
//! \struct AttributeConfigEventData
//! \brief The yat4tango version of the Tango EventData. This is the data struct
//! \brief attached to the messages asscoiated with ATTRIBUTE_CONFIG Tango event.
// ============================================================================
typedef struct AttributeConfigEventData
{
  AttributeConfigEventData ()
    : dev_proxy(0),
      evt_id(0),
      has_error(false),
      attr_conf(0),
      user_data(0)
  {}
  AttributeConfigEventData (AttributeConfigEventData& src)
  {
    *this = src;
  }
  ~AttributeConfigEventData ()
  {
    delete attr_conf;
  }
  inline void operator= (AttributeConfigEventData& src)
  {
    dev_proxy     = src.dev_proxy;
    dev_name      = src.dev_name;
    attr_name     = src.attr_name;
    evt_id        = src.evt_id;
    has_error     = src.has_error;
    errors        = src.errors;
    user_data     = src.user_data;
    attr_conf     = src.attr_conf;
    src.attr_conf = 0;
  }
  //- event source: device proxy
  Tango::DeviceProxy* dev_proxy;
  //- event source: device
  std::string dev_name;
  //- event source: attribute
  std::string attr_name;
  //- event identifier
  int evt_id;
  //- error flag
  bool has_error;
  //- errors list
  Tango::DevErrorList errors;
  //- the actual event data (NULL in case has_error is true)
  Tango::AttributeInfoEx* attr_conf;
  //- user data
  void* user_data;
} AttributeConfigEventData;

// ============================================================================
//! \struct DataReadyEventData
//! \brief The yat4tango version of the Tango EventData. This is the data struct
//! \brief attached to the messages associated with the DATA_READY Tango event.
// ============================================================================
typedef struct DataReadyEventData
{
  DataReadyEventData ()
    : dev_proxy(0),
      evt_id(0),
      has_error(false),
      ctr(0),
      user_data(0)
  {}
  DataReadyEventData (DataReadyEventData& src)
  {
    *this = src;
  }
  ~DataReadyEventData ()
  {
    //- noop
  }
  inline void operator= (DataReadyEventData& src)
  {
    dev_proxy        = src.dev_proxy;
    dev_name         = src.dev_name;
    attr_name        = src.attr_name;
    attr_data_format = src.attr_data_format;
    attr_data_type   = src.attr_data_type;
    evt_id           = src.evt_id;
    has_error        = src.has_error;
    errors           = src.errors;
    ctr              = src.ctr;
    user_data        = src.user_data;
  }
  //- event source: device proxy
  Tango::DeviceProxy* dev_proxy;
  //- event source: device
  std::string dev_name;
  //- event source: attribute
  std::string attr_name;
  //- event source: attribute data format
  Tango::AttrDataFormat attr_data_format;
  //- event source: attribute data type
  int attr_data_type;
  //- event identifier
  int evt_id;
  //- error flag
  bool has_error;
  //- errors list
  Tango::DevErrorList errors;
  //- the actual event data (data index, semantic is device impl dependent)
  int ctr;
  //- user data
  void* user_data;
} DataReadyEventData;

// ============================================================================
//! \brief EventSubscriptionForm data struct.
//!
//! \param dev_name The device name.
//! \param obj_name The object name (i.e. attribute or command name)
//! \param evt_type The Tango event type (see Tango::EventType for details)
//! \param msg_type The yat::Message type associated with this event.
//! \param user_data Abstract user data (pass back with the EventData)
//! \param stateless See remark.
//!
//! \remark With the stateless flag set to true, the event subscription will always succeed, even if the
//! \remark corresponding device server is not running. On Tango side, the keep alive thread will try every
//! \remark 10 seconds subscribe for the specified event. At every subscription retry, a callback is executed
//! \remark contains the corresponding exception.
// ============================================================================
typedef struct SubscriptionForm
{
  SubscriptionForm ()
    : evt_type(Tango::PERIODIC_EVENT),
      msg_type(0),
      user_data(0),
      stateless(true)
  {}
  //- device name
  std::string dev_name;
  //- object name (e.g. attribute name)
  std::string obj_name;
  //- event type (see Tango::EventType for details)
  Tango::EventType evt_type;
  //- yat::Message type (i.e. identifier) associated the Tango event
  size_t msg_type;
  //- abstract user data (pass back with the EventData, NULL means no data)
  void * user_data;
  //- see remark above.
  bool stateless;
} SubscriptionForm;


// ============================================================================
//! \brief SubscriptionAction data struct. Describes what to do in case the
//! \brief event is not configured into the Tango database.
//!
//! \param type The action type (default to 'do nothing').
//! \param polling_period_ms The polling period in ms.
//! \param period_ms The event period in ms (for PERIODIC_EVENT only)
//! \param absolute_change The event period in ms (for CHANGE_EVENT only)
//! \param relative_change The event period in ms (for CHANGE_EVENT only)
// ============================================================================
typedef struct SubscriptionAction
{
  typedef enum
  {
    DO_NOTHING,
    AUTO_CONFIGURE
  } ActionType;
  //- ctor
  SubscriptionAction ()
    : type(DO_NOTHING),
      polling_period_ms(1000),
      period_ms(1000),
      absolute_change(0.),
      relative_change(0.)
  {}
  //- action type
  ActionType type;
  //- polling period in ms
  size_t polling_period_ms;
  //- for periodic events: period in ms (ignored for any other event type)
  size_t period_ms;
  //- for change events: abasolute change value (ignored for any other event type)
  double absolute_change;
  //- for change events: abasolute change value (ignored for any other event type)
  double relative_change;
} SubscriptionAction;

// ============================================================================
//! \brief EventSubscription data struct.
//!
//! \param form The subscription main details
//! \param action The action to perform in case the event is not configured.
// ============================================================================
typedef struct EventSubscription
{
  EventSubscription ()
    : form(),
      action()
  {}
  //- the subscription main details
  SubscriptionForm form;
  //- the subscription action (performed in case the event is not configured)
  SubscriptionAction action;
} EventSubscription;

// ============================================================================
//! \class EventsConsumerTask
//! \brief A yat4tango::DeviceTask with Tango::CallBack capabilities.
//!
//! See yat4tango::DeviceTask & Tango::CallBack documentation for behaviour
//! details.
//!
// ============================================================================
class YAT4TANGO_DECL EventsConsumerTask : public yat4tango::DeviceTask, public Tango::CallBack
{
public:
  //! \brief Default constructor.
  //! \param dev Pointer to associated device.
  EventsConsumerTask (Tango::DeviceImpl * dev);

  //! \brief Destructor.
  virtual ~EventsConsumerTask ();

  //! \brief Asks the underlying thread to quit (join).
  virtual void exit();
  
protected:  
  //! \brief Subscribes to the specified Tango event then returns its identifier.
  //! \param es The event subscription data struct.
  //! \exception Tango::DevFailed Thrown in case the event subscription fails.
  //!
  //! \remark With the stateless flag set to true, the event subscription will always succeed, even if the
  //! \remark corresponding device server is not running. On Tango side, the keep alive thread will try every
  //! \remark 10 seconds subscribe for the specified event. At every subscription retry, a callback is executed
  //! \remark contains the corresponding exception.
  int subscribe_event (const EventSubscription& es);
  
  //! \brief Tango event unsubscription by name and event type.
  //! \param dev_name The Tango device name.
  //! \param attr_name The attribute name.
  //! \param evt_type The event type.
  //! \param msg_type The yat::Message type associated with this event.
  //! \exception Tango::DevFailed Thrown in case the unsubscription fails.
  void unsubscribe_event (const std::string& dev_name,
                          const std::string& attr_name,
                          Tango::EventType evt_type
                         );

  //! \brief Tango event unsubscription by event identifier.
  //! \param evt_id The Tango event identifier (i.e. the value returned by EventsConsumerTask::subscribe_event).
  //! \exception Tango::DevFailed Thrown in case the unsubscription fails.
  void unsubscribe_event (int evt_id);

  //! \brief Unsubsribes all events.
  //! \exception Tango::DevFailed Thrown in case at least one unsubscription fails.
  void unsubscribe_events ();

  //! \brief Returns true if the specified event has been successfully associated with this task during a previous subscription,
  //! \brief returns false otherwise.
  //! \param dev_name The Tango device name.
  //! \param attr_name The attribute name.
  //! \param evt_type The event type.
  //! \param evt_id The event identifier or -1 if not such event registered.
  bool event_subscribed (const std::string& dev_name, const std::string& attr_name, Tango::EventType evt_type, int * evt_id = 0);

  //! \brief Returns true if the specified event has been successfully associated with this task during a previous subscription,
  //! \brief returns false otherwise.
  //! \param evt_id The event identifier.
  bool event_subscribed (int evt_id);

  //! \brief Sets the timeout (in ms) used when trying to post a Tango event to the task
  inline void set_tango_evts_post_tmo (size_t tmo_ms)
  {
    tango_evts_post_tmo_ms = tmo_ms;
  }
  
  //! \brief Returns the timeout (in ms) used when trying to post a Tango event to the task
  inline size_t get_tango_evts_post_tmo () const
  {
    return tango_evts_post_tmo_ms;
  }

  //! \brief Callback for the Tango CHANGE, PERIODOC & USER events
  //! \param evt_data The event data.
  virtual void push_event (Tango::EventData * evt_data);

  //! \brief Callback for the Tango ATTRIBUTE CONFIG event
  //! \param evt_data The event data.
  virtual void push_event (Tango::AttrConfEventData * evt_data);

  //! \brief Callback for the Tango DATA READY event
  //! \param evt_data The event data.
  virtual void push_event (Tango::DataReadyEventData * evt_data);
  
private:
  //- private struct ------------------------- 
  typedef struct DeviceProxy
  {
    //- ctor
    DeviceProxy ()
      : proxy(0), usage_counter(0) {}
    //- ctor
    DeviceProxy (Tango::DeviceProxy* tdp)
      : proxy(tdp), usage_counter(1) {}
    //- ctor
    DeviceProxy (const DeviceProxy& dp)
      : proxy(dp.proxy), usage_counter(dp.usage_counter) {}
    //- dtor
    ~DeviceProxy ()
      { /* do not delete proxy here */ }
    //- operator++
    void inc_usage_counter ( size_t inc = 1 )
      { usage_counter += inc; }
    //- operator--
    bool dec_usage_counter ( size_t dec = 1 )
      { usage_counter -= dec; return this->used(); }
    //- operator
    bool used ()
      { return usage_counter != 0 ? true : false; }
    //- operator
    void release ()
      { delete proxy; usage_counter = 0; }
    //- actual device proxy 
    Tango::DeviceProxy * proxy;
    //- usage counter
    size_t usage_counter;
  } DeviceProxy;

  //- private struct repository --------------
  typedef std::map<std::string, DeviceProxy> DeviceProxyMap;
  typedef DeviceProxyMap::iterator DeviceProxyMapIterator;
  
  //- private struct ------------------------- 
  typedef struct EventInfo
  {
    //------------------------------
    EventInfo ()
      : evt_type(Tango::PERIODIC_EVENT),
        evt_id(0),
        msg_type(0),
        attr_data_format (Tango::FMT_UNKNOWN),
        attr_data_type(0),
        user_data(0)
    {}
    //------------------------------
    EventInfo (const EventInfo& ei)
    {
      *this = const_cast<EventInfo&>(ei);
    }
    //------------------------------
    ~EventInfo ()
    {
      /* do not delete dp here */
    }
    //------------------------------
    void operator= (EventInfo& ei)
    {
      dev_name = ei.dev_name;
      obj_name = ei.obj_name;
      evt_src = ei.evt_src;
      evt_type = ei.evt_type;
      evt_id = ei.evt_id;
      msg_type = ei.msg_type;
      attr_data_format = ei.attr_data_format;
      attr_data_type = ei.attr_data_type;
      user_data = ei.user_data;
    }
    //------------------------------
    std::string dev_name;
    std::string obj_name;
    std::string evt_src;
    Tango::EventType evt_type;
    int evt_id;
    size_t msg_type;
    Tango::AttrDataFormat attr_data_format;
    int attr_data_type;
    void * user_data;
  } EventInfo;

  //- initialize EventData from a EventInfo
  inline void initialize_event_data (EventData & ed, const EventInfo & ei)
  {
    ed.dev_proxy        = 0;
    ed.dev_name         = ei.dev_name;
    ed.attr_name        = ei.obj_name;
    ed.evt_type         = ei.evt_type;
    ed.evt_id           = ei.evt_id;
    ed.attr_data_format = ei.attr_data_format;
    ed.attr_data_type   = ei.attr_data_type;
    ed.has_error        = false;
    ed.attr_value       = 0;
    ed.user_data        = ei.user_data;
  }

  //- initialize AttributeConfigEventData from a EventInfo
  inline void initialize_event_data (AttributeConfigEventData & ed, const EventInfo & ei)
  {
    ed.dev_proxy = 0;
    ed.dev_name  = ei.dev_name;
    ed.attr_name = ei.obj_name;
    ed.evt_id    = ei.evt_id;
    ed.has_error = false;
    ed.attr_conf = 0;
    ed.user_data = ei.user_data;
  }

  //- initialize AttributeConfigEventData from a EventInfo
  inline void initialize_event_data (DataReadyEventData & ed, const EventInfo & ei)
  {
    ed.dev_proxy        = 0;
    ed.dev_name         = ei.dev_name;
    ed.attr_name        = ei.obj_name;
    ed.evt_id           = ei.evt_id;
    ed.attr_data_format = ei.attr_data_format;
    ed.attr_data_type   = ei.attr_data_type;
    ed.has_error        = false;
    ed.ctr              = 0;
    ed.user_data        = ei.user_data;
  }
  
  //- EventInfoMap: a STL map of <EventInfo*> indexed by Tango evt identifier
  typedef std::map<int, EventInfo> EventInfoMap;
  //- EventInfoMapIterator: an EventInfoMap iterator
  typedef EventInfoMap::iterator EventInfoMapIterator;

  //- thread unsafe impl of event_subscribed
  bool event_subscribed_i (const std::string& dev_name,
                           const std::string& attr_name,
                           Tango::EventType evt_type,
                           int * evt_id = 0);

  //- thread unsafe impl of unsubscribe_event
  void unsubscribe_event_i (int evt_id, bool erase = true);

  //- returns the yat::Message identifier associated with the specified event
  int message_identifier_i (const std::string& dev_name,
                            const std::string& attr_name,
                            Tango::EventType evt_type);

  //- returns the yat::Message identifier associated with the specified event
  int message_identifier_i (const std::string& fully_qualified_name,
                            Tango::EventType evt_type);

  //- returns the EventInfo data struct of the specified event
  const EventInfo & event_info_i (const std::string& dev_name,
                                  const std::string& attr_name,
                                  Tango::EventType evt_type);

  //- returns the EventInfo data struct of the specified event
  const EventInfo & event_info_i (const std::string& fully_qualified_name,
                                  Tango::EventType evt_type);
    
  //- timeout (in ms) used when trying to post a Tango event to the underlying task
  size_t tango_evts_post_tmo_ms;

  //- device proxy repository
  DeviceProxyMap proxies;
  
  //- subscribed events
  yat::Mutex subscribed_events_lock;
  EventInfoMap subscribed_events;
};

} // namespace

#endif //- #if (TANGO_VERSION_MAJOR >= 8)

#endif // _EVENT_CONSUMER_TASK_H_
