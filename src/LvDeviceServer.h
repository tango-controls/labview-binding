// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceServer.h
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
#include <tango.h>
#include <yat/threading/Mutex.h>
#include <yat/threading/Condition.h>
#include <yat/threading/Task.h>
#include "CommonHeader.h"
#include "LvDevice.h"
#include "xml/parser.h"

//=============================================================================
//  FORWARD DECLARATION
//=============================================================================
class LvDeviceClass;
class AsynchronousTasks;

//=============================================================================
// CLASS : TemporarySerializationModel
//=============================================================================
class TemporarySerializationModel
{
public:
  explicit TemporarySerializationModel (Tango::SerialModel sm = Tango::BY_CLASS)
  {
    sm_backup_ = Tango::Util::instance()->get_serial_model();
    Tango::Util::instance()->set_serial_model(sm);
  }

  ~TemporarySerializationModel ()
  {
    Tango::Util::instance()->set_serial_model(sm_backup_);
  }

private:
  Tango::SerialModel sm_backup_;
};

//=============================================================================
// CLASS : LvDeviceServer
//=============================================================================
class LvDeviceServer
{
  friend class AsynchronousTasks;
  
public:
  _DECLARE_MAP_(std::string, LvDevice*, LvDeviceByName);
  _DECLARE_MAP_(tbfl::uint32, LvDevice*, LvDeviceBySource);
  _DECLARE_MAP_(std::string, LvDeviceClass*, LvDeviceClassInstanceByName);
  _DECLARE_MAP_(std::string, std::vector<std::string>, LvServerClasses);
  _DECLARE_MAP_(std::string, std::string, LvServerDevices);
  _DECLARE_MAP_(std::string, std::vector<std::string>, LvTangoProperties);
  _DECLARE_MAP_(std::string, LvTangoPropertiesMap, LvClassProperties);
  _DECLARE_MAP_(std::string, LvTangoPropertiesMap, LvDeviceProperties);
  _DECLARE_MAP_(std::string, xmi::Class, LvDeviceClassInterface);
  _DECLARE_MAP_(std::string, yat::Mutex, LvDeviceLock);
  
  struct LvDeviceServerCache
  {
    void clear ()
    {
      ds_event = 0;
      executable_name.clear();
      instance_name.clear();
      ds_class_to_devices.clear();
      ds_class_properties.clear();
      ds_class_interface.clear();
      ds_device_to_class.clear();
      ds_device_to_original_class.clear();
      ds_device_properties.clear();
    }
    //- labview evt. ref. associated with the device server
    LVUserEventRef ds_event;
    //- executable name
    std::string executable_name;
    //- instance name
    std::string instance_name;
    //- map[class-name] -> list of devices belonging to 'class-name'
    LvServerClassesMap ds_class_to_devices;
    //- map[class-name] -> list of class properties
    LvClassPropertiesMap ds_class_properties;
    //- map[class-name] -> class interface description 
    LvDeviceClassInterfaceMap ds_class_interface;
    //- map[device-name] -> class-name
    LvServerDevicesMap ds_device_to_class;
    //- map[device-name] -> actual/initial/original class-name
    LvServerDevicesMap ds_device_to_original_class;
    //- map[device-name] -> list of device properties
    LvClassPropertiesMap ds_device_properties;
  };

  static LvDeviceServer* instance ();
  //- return the unique instance of <LvDeviceServer> or null if not initialized
  
  static void init (const std::string& executable_name,
                    const std::string& instance_name,
                    LVUserEventRef ds_evt,
                    LvStringArrayHdl device_list)
    throw (Tango::DevFailed);
  //- initialize the device server 

  static void cleanup ()
    throw (Tango::DevFailed);
  //- release the device server

  void set_client_request_timeout_by_class (const std::string& class_name, double tmo_in_sec);
  //- class timeout for client request handling 

  void set_client_request_timeout_by_device (const std::string& device_name, double tmo_in_sec);
  //- class timeout for client request handling

  void enable_state_machine (const std::string& device_name, bool enable);
  //- enable/disable the state machine of the specified device 
  
  void start_devices ()
    throw (Tango::DevFailed);
  //- start embedded Tango devices

  void stop_devices (bool about_to_exit = false)
    throw (Tango::DevFailed);
  //- stop embedded Tango devices_

  bool stop_devices_pending ();
  //- stop devices request pending?

  std::string device_server_name () const;
  //- returns the device server name components
  
  void device_server_name (std::string& executable_name, std::string& instance_name) const;
  //- ieturns the device server name components
    
  void class_factory (Tango::DServer* ds);
  //- instanciates each Tango registered Tango class and attaches it to the specified Tango::DServer

  LvDeviceImpl* device_factory (LvDeviceClass* dc, const std::string& dn);
  //- instanciates then returns the specified Tango device

  void set_execute_command_argout (LvExecuteCommandReplyPtr ecr)
    throw (Tango::DevFailed);
  //- pass reply to execute command back to the client 

  void set_read_attribute_value (LvReadAttributeReplyPtr rar)
    throw (Tango::DevFailed);
  //- pass reply to read attribute back to the client 
    
  void ack_client_request (LvClientRequestReplyPtr _cl_rq_ack)
    throw (Tango::DevFailed);
  //- acknowledge client request (might indicate error)

  void push_data_ready_event (const std::string& device_name, const std::string& attribute_name, tbfl::int32 user_counter)
    throw (Tango::DevFailed);
  //- push data a ready event for the specified attribute

  void get_device_property (const std::string& device_name, const std::string&  property_name, LvDevicePropertyPtr property_value)
    throw (Tango::DevFailed);
  //- "return" the specified device property 

  void get_attribute_info (const std::string& device_name, const std::string& attribute_name, LvAttributeInfoPtr attribute_info)
    throw (Tango::DevFailed);
  //- "return" the info (i.e. the configuration) of the specified attribute

  void set_attribute_info (const std::string& device_name, const std::string& attribute_name, LvAttributeInfoPtr attribute_info)
    throw (Tango::DevFailed);
  //- change the r/w part of the attribute info

  void log (const std::string& device_name, LoggingLevel level, const std::string& text);
  //- log in the name of the specified device
  
private:  
  static LvDeviceServer * instance_;
  //- The unique instance of <LvDeviceServer>.

  static yat::Mutex lock_;
  //- singleton's guard
  
  void prepare_to_run (const std::string& executable_name,
                       const std::string& instance_name,
                       LVUserEventRef ds_evt,
                       LvStringArrayHdl device_list)
    throw (Tango::DevFailed);
  //- get ready to run

  void get_tango_db_address (std::string& host, int& port)
    throw (Tango::DevFailed);
  //- get tango db address
    
  void build_lv_device_list (LvStringArrayHdl device_list)
    throw (Tango::DevFailed);
  //- build the list of devices embedded in the device server
    
  void get_device_server_info (const std::string& executable, const std::string& instance, LvDeviceServerCache& cache)
    throw (Tango::DevFailed);
  //- get device server info from Tango database (dsi = 0 means just test/check)
    
  void get_devices_list (const std::string& ds, LvDeviceServerCache& cache, Tango::Database* tdb = 0)
    throw (Tango::DevFailed);
  //- get list of devices associated with the specified tango device server instance
    
  void parse_device_class_interfaces (LvDeviceServerCache& dsi)
    throw (Tango::DevFailed);
  //- Parse the device class interfaces
    
  void abort_client_requests ();
  //- abort client request on all tango devices

  void cleanup_devices_repository ();
  //- cleanup devices repository

  //- important note: two (or more) devices of the same device class adding
  //- the same dynamic interface causes a lot of troubles - since the interface
  //- of our LabVIEW devices is purely dynamic, we need to implement a workaround
  //- in order to be able to run two (or more) devices of the same class into
  //- the LabVIEW device server. the idea is to instance a "virtual" class per
  //- device - the trick is purely local and is supposed to have no side effect
  //- on Tango kernel and/or any client of the LabVIEW devices...
  void apply_per_device_virtual_class (LvDeviceServerCache& dsi)
    throw (Tango::DevFailed);

  //- empty empty client request trashes
  void empty_client_request_trashes ();

  //- check that the specified device exists (i.e. belongs to 'this' device server)
  void check_device_exists (const std::string& dn, const std::string& loc)
    throw (Tango::DevFailed);
    
  //- device server name: executable name + instance name
  std::string executable_name_;
  std::string instance_name_;

  Tango::DServer* device_server_;
  //- device server cache
  
  LvDeviceServerCache device_server_cache_;
  //- device server cache 
  
  LvDeviceClassInstanceByNameMap classes_by_names_;
  //- device class repository (see typedef above).
  
  LvDeviceByNameMap devices_by_name_;
  LvDeviceBySourceMap devices_by_src_;
  //- device instances repositories

  AsynchronousTasks* async_tasks_;
  //- initialize devices asynchronously
  
  bool tango_is_running_;
  //- embedded tango device server successfully run (or at least started) once

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  LvDeviceServer();
  LvDeviceServer (const LvDeviceServer&);
  virtual ~LvDeviceServer();
  void operator= (const LvDeviceServer&);
};

//=============================================================================
// SHORTCUT
//=============================================================================
#define LV_DEV_REP (LvDeviceServer::instance())

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "LvDeviceServer.i"
#endif 

#endif // _TBFL_HAS_DEVICE_SERVER_SUPPORT_
