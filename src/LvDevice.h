// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDevice.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#include "Config.h"

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)

#pragma once

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <map>
#include <deque>
#include <tango.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Condition.h>
#include <yat4tango/DynamicInterfaceManager.h>
#include "CommonHeader.h"
#include "xml/parser.h"

//=============================================================================
// FORWARD DECLARATION
//=============================================================================
class LvDeviceImpl;
class LvDeviceProxy;

//=============================================================================
// ClientRequest
//=============================================================================
struct ClientRequest
{
  ClientRequest ()
    : id(0),
      type(CLIENT_REQ_UNKNOWN),
      state(CLIENT_REQ_STATE_UNKNOWN),
      mtx(),
      cdt(mtx)
  {
    //- noop
  }
  ClientRequest (LvClientRequestType t, const std::string& o)
    : id(0),
      type(t),
      state(CLIENT_REQ_STATE_UNKNOWN),
      cmd_or_attr_name(o),
      mtx(),
      cdt(mtx)
  {
    //- noop
  }
  ClientRequest (const ClientRequest& src)
    : mtx(),
      cdt(mtx)
  {
    *this = src;
  }
  const ClientRequest& operator= (const ClientRequest& src)
  {
    if (&src == this) return *this;
    id = src.id;
    type = src.type;
    state = src.state;
    cmd_or_attr_name = src.cmd_or_attr_name;
    error = src.error;
    return *this;
  }
  ~ClientRequest ()
  {
    //- noop
  }
  inline bool pending () const
  {
    return state == CLIENT_REQ_STATE_PENDING_SYNC;
  }
  //- request identifier
  tbfl::uint32 id;
  //- request type
  LvClientRequestType type;
  //- request state
  LvClientRequestState state;
  //- name of command or attribute on which the request applies
  std::string cmd_or_attr_name;
  //- error (valid in case state is CLIENT_REQ_STATE_FAILED)
  Tango::DevFailed error;
  //- the associated mutex
  yat::Mutex mtx;
  yat::Condition cdt;
};
typedef ClientRequest* ClientRequestPtr;
typedef ClientRequest** ClientRequestHdl;

//=============================================================================
// TYPEDEFS FOR CLIENT REQUEST
//=============================================================================
_DECLARE_QUEUE_(ClientRequest*, ClientRequest);
_DECLARE_MAP_(tbfl::uint32, ClientRequest*, ClientRequest);
_DECLARE_MAP_(std::string, LvClientRequestReplyDataBuffer, LvClientRequestReplyDataBuffer);

//=============================================================================
// StateMachine
//=============================================================================
_DECLARE_MAP_(std::string, std::set<Tango::DevState>, ExcludedStates);
//-----------------------------------------------------------------------------
struct StateMachine
{
  StateMachine ()
   : enabled(true)
  {}
  //- state machine enabled?
  bool enabled;
  //- not allowed states for "execute command" 
  ExcludedStatesMap ec_nas;
  //- not allowed states for "read attribute"
  ExcludedStatesMap ra_nas;
  //- not allowed states for "write attribute" 
  ExcludedStatesMap wa_nas;
  //- register excluded states ------------------
  inline  void register_excluded_states (const std::string& n,
                                         const std::vector<Tango::DevState>& s,
                                         ExcludedStatesMap& m)
  {
    m[n] = std::set<Tango::DevState>();
    std::set<Tango::DevState>& es = m[n];
    for ( size_t i = 0; i < s.size(); i++ )
      es.insert(s[i]);
  }
  //- register excluded states ------------------
  inline  void register_read_attribute_excluded_states (const std::string& n,
                                                        const std::vector<Tango::DevState>& s)
  {
    register_excluded_states(n, s, ra_nas);
  }
  //- register excluded states ------------------
  inline  void register_write_attribute_excluded_states (const std::string& n,
                                                         const std::vector<Tango::DevState>& s)
  {
    register_excluded_states(n, s, wa_nas);
  }
  //- register excluded states ------------------
  inline  void register_execute_command_excluded_states (const std::string& n,
                                                         const std::vector<Tango::DevState>& s)
  {
    register_excluded_states(n, s, ec_nas);
  }
  //- is request allowed? -----------------------
  inline bool is_allowed (const std::string& n,
                          const Tango::DevState& s,
                          const ExcludedStatesMap& m) const
  {
    if ( ! enabled ) return true;
    ExcludedStatesMapConstIterator it = m.find(n);
    if ( it == m.end() ) return false;
    return ! ( it->second.find(s) != it->second.end() );
  }
  //- "execute command" allowed? ----------------
  inline bool is_command_allowed (const std::string& n,
                                  const Tango::DevState& s) const
  {
    return is_allowed(n, s, ec_nas);
  }
  //- "read attribute" allowed?
  inline bool is_read_attribute_allowed (const std::string& n,
                                         const Tango::DevState& s) const
  {
    return is_allowed(n, s, ra_nas);
  }
  //- "write attribute" allowed? ----------------
  inline bool is_write_attribute_allowed (const std::string& n,
                                          const Tango::DevState& s) const
  {
    return is_allowed(n, s, wa_nas);
  }
};

//=============================================================================
// DEVICE PROPERTY REPOSITORY
//=============================================================================
_DECLARE_MAP_(std::string, std::vector<std::string>, DeviceProperty);

//=============================================================================
// LvDevice
//=============================================================================
class LvDevice
{
  friend class LvDeviceServer;
  friend class LvDeviceImpl;

  //- ctor
  LvDevice ();

  //- dtor
  ~LvDevice ();

  //- init_device: called when the device is restarted from labview
  void init_device ();

  //- init_device: called by Tango kernel notification (device startup or explicit init request)
  void init_device (LvDeviceImpl* lvd, tbfl::int32 init_cnt);

  //- delete_device called by Tango kernel notification
  void delete_device ();
    
  //- device state 
  Tango::DevState dev_state ();

  //- device status
  std::string dev_status ();

  //- (re)start device activity (requested from labview side)
  void start ();

  //- stop device activity (requested from labview side)
  void stop ();

  //- enable the device (accept incoming requests)
  void enable ();

  //- disable the device (reject incoming requests)
  void disable ();

  //- log text on behalf of the underlying tango 
  void log (LoggingLevel level, const std::string& text);

  //- changes the ""wait client request handled timeout" to the specified value (unit is ms)
  //- for safety reasons, the timeout will be set to a minimum value of 1 sec
  void set_client_request_timeout (double tmo_in_sec);

  //- enable/disable the state machine
  void enable_state_machine (bool enable);
  
  //- reply to 'execute command' client request
  void set_execute_command_argout (LvExecuteCommandReplyPtr ao)
    throw (Tango::DevFailed);

  //- reply to 'read attribute' client request
  void set_read_attribute_value (LvReadAttributeReplyPtr av)
    throw (Tango::DevFailed);

  //- acknowledge client request (might indicate error)
  void ack_client_request (LvClientRequestReplyPtr _cl_rq_ack)
    throw (Tango::DevFailed);

  //- push data a ready event for the specified attribute
  void push_data_ready_event (const std::string& attribute_name, tbfl::int32 user_counter)
    throw (Tango::DevFailed);
  
  //- empty the client request to trash
  void empty_client_request_trash ();

  //- wakes up the client thread (in case it's waiting for client request handled notification)
  void abort_client_requests ();

  //- initialize a client request notification for posting...
  void intialize_client_request_notification (ClientRequest& pcr, LvClientRequest& crn)
    throw (Tango::DevFailed);

  //- post client request to labview
  void post_lv_user_event (LvClientRequest& crn)
    throw (Tango::DevFailed);

  //- post request to labview then return immediately (pcr_out = addr of the request actually posted)
  void post_client_request (ClientRequest& pcr)
    throw (Tango::DevFailed);

  //- post request to labview then wait up to 'ack-tmo' for a response from labVIEW
  void wait_client_request_handled (ClientRequest& pcr)
    throw (Tango::DevFailed);

  //- post request to labview then wait up to 'ack-tmo' for a response from labVIEW
  void wait_client_request_handled (ClientRequest& in_pcr, LvClientRequest& crn)
    throw (Tango::DevFailed);
    
  //- throw client request error 
  void throw_client_request_error (ClientRequest& pcr)
    throw (Tango::DevFailed);

  //- clear client request queues (pending & trash)
  void clear_client_request_queues ();

  //- clear client request queues (pending & trash)
  void clear_pending_client_request_map ();

  //- our own device interface (allows to reuse the DataAdapter features)
  LvDeviceProxy* interface ();

  //- reload the dynamic interface management (does a [remove/add]_dynamic_interface)
  void reload_dynamic_interface();

  //- the dynamic interface management
  void add_dynamic_interface ();

  //- the dynamic interface management
  void remove_dynamic_interface ();

  //- exec cmd callback
  void execute_callback (yat4tango::DynamicCommandExecuteCallbackData& cbd);

  //- read attr callback
  void read_callback (yat4tango::DynamicAttributeReadCallbackData& cbd);

  //- write attr callback
  void write_callback (yat4tango::DynamicAttributeWriteCallbackData& cbd);

  //- get tango db address
  void get_tango_db_address (std::string& host, int& port)
    throw (Tango::DevFailed);

  //- get the device properties from tango db
  void get_device_properties ()
    throw (Tango::DevFailed);

  //- "return" the specified property value
  void get_device_property (const std::string&  pn, LvDevicePropertyPtr pv)
    throw (Tango::DevFailed);
  
  //- "return" the info (i.e. the configuration) of the specified attribute
  void get_attribute_info (const std::string& attribute_name, LvAttributeInfoPtr attribute_info)
    throw (Tango::DevFailed);

  //- change the r/w part of the attribute info
  void set_attribute_info (const std::string& attribute_name, LvAttributeInfoPtr attribute_info)
    throw (Tango::DevFailed);

  //- device enabled?
  bool enabled_;

  //- device interface changed?
  bool interface_changed_;

  //- self init requested?
  bool self_init_requested_;

  //- device state machine
  StateMachine state_machine_;
  
  //- tango device server: executable name (i.e. labview application name)
  std::string ds_executable_name_;

  //- tango device server: instance name  (i.e. labview application instance name)
  std::string ds_instance_name_;

  //- tango device class name
  std::string class_name_;

  //- initial/original/... tango device class name
  std::string original_class_name_;

  //- tango device name
  std::string device_name_;

  //- device class interface
  const xmi::Class* class_interface_;

  //- device class instance
  const Tango::DeviceClass* device_class_;

  //- device instance
  LvDeviceImpl* device_impl_;

  //- device properties
  DevicePropertyMap device_prop_;
 
  //- labview event ref associated with the tango device
  LVUserEventRef lv_evt_ref_;

  //- the event identifier of this device
  tbfl::uint32 lv_evt_source_id_;

  //- client request counter (used to generates client request identifier)
  tbfl::uint32 client_request_cnt_;

  //- our own device interface (allow to reuse the DataAdapter features)
  LvDeviceProxy* tg_device_intf_;

  //- condition variable used to emulate synchronous request processing
  //- the semantic is: wait for client request to be handles on LabVIEW
  yat::Mutex cl_rq_mtx_;
  yat::Condition cl_rq_cdt_;

  //- pending_client request repository and its associated mutex
  yat::Mutex pending_cl_rq_mtx_;
  ClientRequestMap pending_cl_rq_;

  //- pending_client request trash and its associated mutex
  yat::Mutex cl_rq_trash_mtx_;
  ClientRequestQueue cl_rq_trash_;

  //- the mutex used to protect the client request data against race conditions
  //- CORBA thread propagating the client request versus LabVIEW thread handling it
  yat::Mutex cl_rq_data_mtx_;

  //- client request ack. timeout in ms
  tbfl::uint32 cl_rq_ack_tmo_in_ms_;

  //- command argout data buffer repository for 'execute command' client request
  LvClientRequestReplyDataBufferMap exc_data_buffers_;

  //- attribute data buffer repository for 'read attribute' client request
  LvClientRequestReplyDataBufferMap rda_data_buffers_;

  //- the dynamic interface manager
  yat4tango::DynamicInterfaceManager* dim_;
};

#endif // _TBFL_HAS_DEVICE_SERVER_SUPPORT_
