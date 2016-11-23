// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   EventManager.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#ifndef WIN32
#  include <sys/time.h> 
#endif 
#include "CommonHeader.h"
#include "LvDeviceProxyRepository.h"
#include "DataAdapter.h"
#include "EventManager.h"
#include "ErrorStack.h"
 
#if !defined (_LV_INLINE_)
  #include "EventManager.i"
#endif

//=============================================================================
// CONSTS
//=============================================================================
#define POST_ERROR_THRESHOLD 2

//=============================================================================
// STATIC MEMBERS
//=============================================================================
EventManager* EventManager::instance_ = 0;

//=============================================================================
// EventCallback::EventCallback
//=============================================================================
EventCallback::EventCallback (LVUserEventRef _lv_evt_ref)
  : Tango::CallBack(),
    lv_evt_ref_(_lv_evt_ref)
{
  //- noop
}

//=============================================================================
// EventCallback::EventCallback
//=============================================================================  
EventCallback::~EventCallback () 
{
  //- noop
}

//=============================================================================
// EventCallback::event_atached
//=============================================================================  
bool EventCallback::event_attached (tbfl::int32 _event_identifier)
{
  return attached_evts_.find(_event_identifier) != attached_evts_.end();
}

//=============================================================================
// EventCallback::event_atached
//=============================================================================
bool EventCallback::event_attached (const std::string & _fqn, tbfl::int32 * _event_identifier)
{
  EventInfoMapIterator it = attached_evts_.begin();
  EventInfoMapIterator end = attached_evts_.end();
  for (; it != end; ++ it )
  {
    std::string::size_type dev_pos = _fqn.find(it->second.device_name_);
    std::string::size_type obj_pos = _fqn.find(it->second.obj_name_);
    if ( dev_pos != std::string::npos && obj_pos != std::string::npos )
    {
      if ( _event_identifier )
        *_event_identifier = it->second.event_identifier_;
      return true;
    }
  }
  if (_event_identifier)
  {
	  *_event_identifier = -1;
  }
  return false;
}

//=============================================================================
// EventCallback::event_identifier
//=============================================================================
tbfl::int32 EventCallback::event_identifier (const std::string & _fqn)
{
  tbfl::int32 evt_id = -1;
  event_attached(_fqn, &evt_id);
  return evt_id;
}

//=============================================================================
// EventCallback::attach_event
//=============================================================================  
void EventCallback::attach_event (const EventInfo& _ei)
{
  attached_evts_[_ei.event_identifier_] = _ei;
}

//=============================================================================
// EventCallback::detach_event
//=============================================================================
void EventCallback::detach_event (const std::string & _fqn, bool _erase)
{
  tbfl::int32 evt_id = event_identifier(_fqn);
  detach_event(evt_id, _erase);
}

//=============================================================================
// EventCallback::event_error
//=============================================================================
void EventCallback::event_error (const std::string & _fqn)
{
  tbfl::int32 evt_id = event_identifier(_fqn);
  if ( evt_id == -1 )
  {
    return;
  }
              
  EventInfo& ei = attached_evts_[evt_id];

  TBFL_STDOUT("EventCallback::event_error::event handling error notified for evt "
              << ei.device_name_
              << "/"
              << ei.obj_name_
              << " [evt:"
              << evt_id
              << "]");

  if ( ++ei.post_event_to_lv_errors_ > POST_ERROR_THRESHOLD )
  {
    TBFL_STDOUT("EventCallback::event_error::error counter over threshold - detaching evt "
                << evt_id);
    
    detach_event(evt_id); 
  }
}
 
//=============================================================================
// EventCallback::detach_event
//=============================================================================  
void EventCallback::detach_event (tbfl::int32 _event_identifier, bool _erase)
{
  EventInfoMapIterator it = attached_evts_.find(_event_identifier);
  if ( it == attached_evts_.end() )
    return;

  try
  {
    TBFL_STDOUT("EventManager::subscribe_attribute_event::unsubscribing from "
                << it->second.device_name_
                << "/"
                << it->second.obj_name_
                << " [evt:"
                << it->second.event_identifier_
                << "]");

    LvDeviceProxy& dvp = DEV_REP->device_proxy(it->second.device_name_);
    
    dvp.proxy()->unsubscribe_event(_event_identifier);
  }
  catch ( ... )
  {
    //- ignore error?
  }

  if ( _erase )
  {
    attached_evts_.erase(it);
  }
}

//=============================================================================
// EventCallback::detach_all_events
//=============================================================================  
void EventCallback::detach_all_events ()
{
  EventInfoMapIterator  it = attached_evts_.begin();
  EventInfoMapIterator end = attached_evts_.end();

  for (; it != end; ++it)
  {
    detach_event(it->first, false);
  }

  attached_evts_.clear();
}

//=============================================================================
// EventCallback::cmd_ended
//=============================================================================  
void EventCallback::cmd_ended (Tango::CmdDoneEvent*  _e)
{
  //-tango's future?
}

//=============================================================================
// EventCallback::attr_read
//=============================================================================  
void EventCallback::attr_read (Tango::AttrReadEvent*  _e)
{
  //-tango's future?
}

//=============================================================================
// EventCallback::attr_written
//=============================================================================  
void EventCallback::attr_written (Tango::AttrWrittenEvent*  _e)
{
  //-tango's future?
}

//=============================================================================
// EventCallback::push_event
//=============================================================================  
void EventCallback::push_event (Tango::AttrConfEventData*  _e)
{
  if ( ! _e || _e->err )
    return;
  
  //- initialize each LvAttributeInfo field to 0
  LvAttributeInfo lai;
  
  //- point to the i-th Tango::AttributeInfo in the Tango::AttributeInfoList 
  Tango::AttributeInfoEx * tai = _e->attr_conf;

  try
  {
    //- convert tango to labview attribute info
    DATA_ADAPTER->tg_to_lv_attribute_info(_e->device->dev_name(), *tai, lai);

    //- post event to labview
    //- no need to check error here cause we can't do anything to handle it properly!
    //- important note: PostLVUserEvent makes a deep copy of the data!
    MgErr err = ::PostLVUserEvent(lv_evt_ref_, static_cast<void*>(&lai));
    if ( err != noErr )
    {
      event_error( _e->event);
    }
  }
  catch (...)
  {
    event_error( _e->event);
  }
}

//=============================================================================
// EventCallback::push_event
//=============================================================================  
void EventCallback::push_event (Tango::DataReadyEventData*  _e)
{
  if ( ! _e )
    return;
  
  TBFL_STDOUT("EventCallback::evt::src::" << _e->device->dev_name() << "/" << _e->attr_name);
  TBFL_STDOUT("EventCallback::evt::typ::" << _e->event);
  TBFL_STDOUT("EventCallback::evt::err::" << _e->err);
  
  //- initialize each LvAttributeDataReady field to 0
  LvAttributeDataReady ladr;
  //- set device name
  tbfl::size_t l = static_cast<tbfl::size_t>(_e->device->dev_name().size());
  MgErr err = ::NumericArrayResize(iB, 1L, (UHandle*)&(ladr.dev_name), l);
  if ( err != noErr )
  {
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("EventCallback::push_event"));
  }
  LvStringHdl sh = reinterpret_cast<LvStringHdl>(ladr.dev_name);
  (*sh)->length = l;
  ::memcpy((*sh)->data, _e->device->dev_name().c_str(), l);

  //- set attr. name
  std::string::size_type pos = _e->attr_name.find_last_of("/");
  std::string attr_name = _e->attr_name.substr(pos + 1, _e->attr_name.size() - (pos + 1));
  l = static_cast<tbfl::size_t>(attr_name.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(ladr.attr_name), l);
  if ( err != noErr )
  {
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("EventCallback::push_event"));
  }
  sh = reinterpret_cast<LvStringHdl>(ladr.attr_name);
  (*sh)->length = l;
  ::memcpy((*sh)->data, attr_name.c_str(), l);

  //- init to error 
  ladr.ctr = -1;
  ladr.attr_data_type = -1;
  
  //- in case of error: just set error code and stack then return
  if ( _e->err )
  {
    ladr.error_code = -1;
    Tango::DevFailed df(_e->errors);
    ERROR_STACK->devfailed(df, ladr.error_stack);
    //- post event to LabVIEW
    //- important note: PostLVUserEvent makes a deep copy of the data!
    MgErr mg_err = ::PostLVUserEvent(lv_evt_ref_, static_cast<void*>(&ladr));
    if ( mg_err != noErr )
    {
      event_error(_e->attr_name);
    }
    return;
  }

  //- attribute data counter
  ladr.ctr = static_cast<tbfl::int32>(_e->ctr);

  //- attribute data type
  ladr.attr_data_type = static_cast<tbfl::int32>(_e->attr_data_type);
  
  //- reset error code and stack
  ladr.error_code = 0;
  ERROR_STACK->no_error("TangoBinding::EventManager", ladr.error_stack);

  //- post event to LabVIEW
  //- important note: PostLVUserEvent makes a deep copy of the data!
  MgErr mg_err = ::PostLVUserEvent(lv_evt_ref_, static_cast<void*>(&ladr));
  if ( mg_err != noErr )
  {
    event_error(_e->attr_name);
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("PostLVUserEvent failed!"),
                                   _CPTC_("EventCallback::push_event"));
  }
}

//=============================================================================
// EventCallback::push_event
//=============================================================================
void EventCallback::push_event (Tango::EventData* _e)
{
  if ( ! _e )
    return;
  
  TBFL_STDOUT("EventCallback::evt::src::" << _e->device->dev_name() << "/" << _e->attr_name);
  TBFL_STDOUT("EventCallback::evt::typ::" << _e->event);
  TBFL_STDOUT("EventCallback::evt::err::" << _e->err);

  //- initialize each LvAttributeValue field to 0
  LvAttributeValue lvav;
  
  //- set device name
  tbfl::size_t l = static_cast<tbfl::size_t>(_e->device->dev_name().size());
  MgErr err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lvav.dev_name), l);
  if ( err != noErr ) 
  {
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("EventCallback::push_event"));
  }
  LvStringHdl sh = reinterpret_cast<LvStringHdl>(lvav.dev_name);
  (*sh)->length = l;
  ::memcpy((*sh)->data, _e->device->dev_name().c_str(), l);
  
  //- set attr. name
  std::string::size_type pos = _e->attr_name.find_last_of("/");
  std::string attr_name = _e->attr_name.substr(pos + 1, _e->attr_name.size() - (pos + 1));
  l = static_cast<tbfl::size_t>(attr_name.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lvav.attr_name), l);
  if ( err != noErr ) 
  {
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("EventCallback::push_event"));
  }
  sh = reinterpret_cast<LvStringHdl>(lvav.attr_name);
  (*sh)->length = l;
  ::memcpy((*sh)->data, attr_name.c_str(), l);
   
  //- in case of error: just set error code and stack then return
  if ( _e->err )
  {
    lvav.error_code = -1;
    Tango::DevFailed df(_e->errors);
    ERROR_STACK->devfailed(df, lvav.error_stack);
    //- post event to LabVIEW
    //- important note: PostLVUserEvent makes a deep copy of the data!
    MgErr mg_err = ::PostLVUserEvent(lv_evt_ref_, static_cast<void*>(&lvav));
    if ( mg_err != noErr )
    {
      event_error(_e->attr_name);
    }
    return;
  }

  //- access the Tango::DeviceAttribute
  Tango::DeviceAttribute *da = _e->attr_value;
  if ( ! da ) 
  {
    return;
  }

  //- set attribute quality (both value and string)
  std::string s;
  switch ( da->quality ) 
  {
    case Tango::ATTR_VALID:
      lvav.quality_val = 0;
      s = "VALID";
      break;
    case Tango::ATTR_INVALID:
      lvav.quality_val = 1;
      s = "INVALID";
      break;
    case Tango::ATTR_ALARM:
      lvav.quality_val = 2;
      s = "ALARM";
      break;
    default:
      lvav.quality_val = -1;
      s = "UNKNOWN";
      break;
  };
  l = static_cast<tbfl::size_t>(s.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lvav.quality_str), l);
  if (err != noErr)
  {
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("EventCallback::push_event"));

  }
  sh = reinterpret_cast<LvStringHdl>(lvav.quality_str);
  (*sh)->length = l;
  ::memcpy((*sh)->data, s.c_str(), l);
  
  //- set timestamp
  lvav.timestamp = kTIME_OFFSET +
                   (double)da->time.tv_sec +
                   1.E-6 * (double)da->time.tv_usec + 
                   1.E-9 * (double)da->time.tv_nsec;
  //- set dims
  Tango::AttributeDimension r_dim = da->get_r_dimension();
  lvav.dim_x = r_dim.dim_x;
  lvav.dim_y = r_dim.dim_y;

  //- reset error code
  lvav.error_code = 0;
  ERROR_STACK->no_error("TangoBinding::EventManager", lvav.error_stack);
  
  //- decode data
  try 
  {
    LvDeviceProxy& dvp = DEV_REP->device_proxy(_e->device->dev_name());

    const AttributeInfo& ai = dvp.attribute_info(attr_name);
    
    DATA_ADAPTER->decode_attr(ai, *da, &lvav);
  }
  catch (const Tango::DevFailed& e) 
  {
    //- set error code 
    lvav.error_code = -1;
    //- set error stack
    ERROR_STACK->devfailed(e, lvav.error_stack);
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("event data extraction failed!"),
                                   _CPTC_("EventCallback::push_event"));
  }
  catch (...) 
  {
    //- set error code 
    lvav.error_code = -1;
    //- set error stack 
    ERROR_STACK->unknown("EventManager::push_event", lvav.error_stack);
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("event data extraction failed!"),
                                   _CPTC_("EventCallback::push_event"));
  }

  //- post event to LabVIEW
  //- important note: PostLVUserEvent makes a deep copy of the data!
  MgErr mg_err = ::PostLVUserEvent(lv_evt_ref_, static_cast<void*>(&lvav));
  if ( mg_err != noErr )
  {
    event_error(_e->attr_name);
    //- this exception goes to hyper-space. anyway...
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("PostLVUserEvent failed!"),
                                   _CPTC_("EventCallback::push_event"));
  }
}

//=============================================================================
// EventManager::init
//=============================================================================
int EventManager::init ()
{
  TBFL_TRACE("EventManager");

  if ( EventManager::instance_ )
    return 0;

  EventManager::instance_ = new EventManager;

  return EventManager::instance_ ? 0 : -1;
}

//=============================================================================
// EventManager::cleanup
//=============================================================================
void EventManager::cleanup ()
{
  TBFL_TRACE("EventManager");

  delete EventManager::instance_;
  
  EventManager::instance_ = 0;
}

//=============================================================================
// EventManager::EventManager 
//=============================================================================
EventManager::EventManager () 
  : events_have_been_used_(false)
{
  //- noop
}

//=============================================================================
// EventManager::~EventManager 
//=============================================================================
EventManager::~EventManager ()
{
  clear();
}

//=============================================================================
// EventManager::events_group_create
//=============================================================================
void EventManager::events_group_create (LVUserEventRef _lv_evt_ref)
{
  TBFL_TRACE(this);

  yat::MutexLock guard(lock_);

  EventCallbackMapIterator it = evt_cb_map_.find(_lv_evt_ref);
  if ( it != evt_cb_map_.end() )
    return;

  EventCallback * ecb = new EventCallback(_lv_evt_ref);
  if ( ! ecb ) 
  {
    Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                   _CPTC_("failed to initialize attribute events"),
                                   _CPTC_("EventManager::initialize_attribute_events"));
  }

  evt_cb_map_[_lv_evt_ref] = ecb;
}

//=============================================================================
// EventManager::events_group_kill
//============================================================================= 
void EventManager::events_group_kill (LVUserEventRef _lv_evt_ref) 
{
  TBFL_TRACE(this);

  yat::MutexLock guard(lock_);

  EventCallbackMapIterator it = evt_cb_map_.find(_lv_evt_ref);
  if ( it == evt_cb_map_.end() )
    return;

  it->second->detach_all_events();

  delete it->second;

  evt_cb_map_.erase(it);
}

//=============================================================================
// EventManager::subscribe_event
//=============================================================================
tbfl::uint32 EventManager::subscribe_event (const std::string& dev, 
                                            const std::string& attr,
                                            Tango::EventType evt_type,
                                            LVUserEventRef lv_evt_refnum)
  throw (Tango::DevFailed)
{ 
  TBFL_TRACE(this);

  yat::MutexLock guard(lock_);

  TBFL_STDOUT("EventManager::subscribe_attribute_event::subscribing to "
              << dev
              << "/"
              << attr);

  EventCallbackMapIterator it = evt_cb_map_.find(lv_evt_refnum);

  if ( it == evt_cb_map_.end() )
  {
    std::string d = "failed to subscribe to " + dev + "/" + attr + " - invalid events group specified";
    TBFL_STDOUT(d);
    Tango::Except::throw_exception(_CPTC_("Tango event subscription failed"),
                                   d.c_str(),
                                   _CPTC_("EventManager::subscribe_attribute_event"));
  }


  EventInfo ei;
  ei.device_name_ = dev;
  ei.obj_name_ = attr;
  ei.event_type_  = evt_type;
  
  try 
  {
    LvDeviceProxy& dvp = DEV_REP->device_proxy(dev);
    
    ei.event_identifier_ = dvp.proxy()->subscribe_event(attr, evt_type, it->second);
    
    it->second->attach_event(ei);

    TBFL_STDOUT("EventManager::subscribe_attribute_event::evt ref. is " << ei.event_identifier_);
  }
  catch (const Tango::DevFailed& e)
  {
    std::string d = "failed to subscribe to " + dev + "/" + attr;
    Tango::Except::re_throw_exception(const_cast<Tango::DevFailed&>(e),
                                      _CPTC_("TANGO event subscription failed [TANGO exception caught]"),
                                      d.c_str(),
                                      _CPTC_("EventManager::subscribe"));
  }
  catch (...) 
  {
    std::string d = "failed to subscribe to " + dev + "/" + attr;
    Tango::Except::throw_exception(_CPTC_("TANGO event subscription failed [unknown error]"),
                                   d.c_str(),
                                   _CPTC_("EventManager::subscribe"));
  }

  events_have_been_used_ = true;

  return static_cast<tbfl::int32>(ei.event_identifier_);
} 

//=============================================================================
// EventManager::unsubscribe_event
//=============================================================================
void EventManager::unsubscribe_event (tbfl::uint32 evt_identifier)
  throw (Tango::DevFailed)
{ 
  TBFL_TRACE(this);

  yat::MutexLock guard(lock_);
 
  EventCallbackMapIterator  it = evt_cb_map_.begin();
  EventCallbackMapIterator end = evt_cb_map_.end();
  for (; it != end; ++it )
  {
    if ( it->second->event_attached(evt_identifier) )
    {
       it->second->detach_event(evt_identifier);
       break;
    }
  }
}

//=============================================================================
// EventManager::clear
//=============================================================================
void EventManager::clear ()
{
  TBFL_TRACE(this);

  yat::MutexLock guard(lock_);

  EventCallbackMapIterator cur = evt_cb_map_.begin();
  EventCallbackMapIterator end = evt_cb_map_.end();

  for (; cur != end; cur++) 
  {
    //- this can cause a seg. fault at exit...
    //- cur->second->detach_all_events();
    delete cur->second;
  }

  evt_cb_map_.clear();
}



