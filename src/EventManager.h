// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   EventManager.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <map>
#include <yat/threading/Mutex.h>

//=============================================================================
// STRUCT : EventInfo
//=============================================================================
struct EventInfo 
{
  EventInfo ()
    : event_type_(Tango::PERIODIC_EVENT),
      event_identifier_(0),
      post_event_to_lv_errors_(0)
  {}

  std::string device_name_;
  //- device name
  
  std::string obj_name_;
  //- command or attribute name

  Tango::EventType event_type_;
  //- event type

  tbfl::int32 event_identifier_;
  //- event identifier

  tbfl::int32 post_event_to_lv_errors_;
  //- post event errors
};

//=============================================================================
// TYPEDEFs
//=============================================================================
_DECLARE_MAP_(tbfl::int32, EventInfo, EventInfo);

//=============================================================================
// CLASS : EventCallback
//=============================================================================
class EventCallback : public Tango::CallBack 
{
  friend class EventManager;

public:
  //- Tango actual callback methods
	virtual void cmd_ended    (Tango::CmdDoneEvent       *);
	virtual void attr_read    (Tango::AttrReadEvent      *);
	virtual void attr_written (Tango::AttrWrittenEvent   *);
	virtual void push_event   (Tango::EventData          *);
	virtual void push_event   (Tango::AttrConfEventData  *);
	virtual void push_event   (Tango::DataReadyEventData *);

private:
  EventCallback (LVUserEventRef lv_evt_ref);
  //- ctor

  virtual ~EventCallback ();
  //- dtor
  
  void attach_event (const EventInfo& ei);
  //- attaches the specified event this callback

  void detach_event (tbfl::int32 _event_identifier, bool erase = true);
  //- detaches the specified event

  void detach_event (const std::string & _fully_qualified_name, bool _erase = true);
  //- detaches the specified event
   
  void detach_all_events ();
  //- detaches all events

  bool event_attached (tbfl::int32 _event_identifier);
  //- returns true if the specified event is attached to this callback

  bool event_attached (const std::string & _fully_qualified_name, tbfl::int32 * _event_identifier = 0);
  //- returns true if the specified event is attached to this callback

  tbfl::int32 event_identifier (const std::string & _fully_qualified_name);
  //- returns the event id of the specified event, or -1 if not itsn't attached to this callbacks

  void event_error (const std::string & _fully_qualified_name);
  //- increment the errors counter of the specicied event. in case the error counter is over the
  //- POST_ERROR_THRESHOLD, the event is detached. this acts as a trick to fight against a side
  //- effect due a Vi abort that skipped the event(s) unsubscription process.
   
  LVUserEventRef lv_evt_ref_;
  //- lv event identifier

  EventInfoMap attached_evts_;
  //- attached events
};

//=============================================================================
// TYPEDEFs
//=============================================================================
typedef EventCallback* EventCallbackPtr;
_DECLARE_MAP_(LVUserEventRef, EventCallbackPtr, EventCallback);

//=============================================================================
// CLASS : EventManager [SINGLETON]
//=============================================================================
class EventManager 
{
  friend class ObjectManager;

public:

  static EventManager* instance ();
  //- returns the unique instance of <EventManager>.
  
  void events_group_create (LVUserEventRef _lv_evt_ref);
  //- Tango events group events intialization

  void events_group_kill (LVUserEventRef _lv_evt_ref);
  //- Tango events group events termination

  tbfl::uint32 subscribe_event (const std::string& dev, 
                                const std::string& attr,
                                Tango::EventType evt_type,
                                LVUserEventRef lv_evt_refnum)
    throw (Tango::DevFailed);
  //- subscribes to the specified TANGO attribute event

  void unsubscribe_event (tbfl::uint32 evt_identifier)
    throw (Tango::DevFailed);
  //- unsubscribes to the specified TANGO attribute event

  bool events_have_been_used ();
  //- Returns true if TANGO events have been used, false otherwise.

private:

  static int init ();
  //- Instanciates the singleton.
  
  static void cleanup ();
  //- Releases the singleton.

  static EventManager* instance_;
  //- The unique instance of <EventManager>

  void clear ();
  //- clear the EventCallback repository

  yat::Mutex lock_;
  //- make this EventManager thread-safe

  EventCallbackMap evt_cb_map_;
  //- the callbacks repository (see typedef above).

  bool events_have_been_used_;
  //- true if TANGO events have been used, false otherwise.

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  EventManager();
  EventManager (const EventManager&);
  virtual ~EventManager();
  void operator= (const EventManager&); 
};

//=============================================================================
// #DEFINEs
//=============================================================================
#define EVT_MANAGER (EventManager::instance())

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "EventManager.i"
#endif 


   
   
