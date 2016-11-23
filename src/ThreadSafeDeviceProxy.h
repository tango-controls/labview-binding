// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ThreadSafeDeviceProxy.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#if (TANGO_VERSION_MAJOR < 7)
#include <yat/threading/Mutex.h>
#endif

//=============================================================================
// Class ThreadSafeDeviceProxy (SINGLETON)
//=============================================================================
class ThreadSafeDeviceProxy
{
public:
  ThreadSafeDeviceProxy (const std::string& dev_name) 
    throw (Tango::DevFailed);

  ThreadSafeDeviceProxy (const char * dev_name)
    throw (Tango::DevFailed);

  ThreadSafeDeviceProxy (Tango::DeviceProxy * dp, bool ownership = false) 
    throw (Tango::DevFailed);

  virtual ~ThreadSafeDeviceProxy ();

  const std::string status () 
    throw (Tango::DevFailed);

  Tango::DevState state ()
    throw (Tango::DevFailed);

  int ping () 
    throw (Tango::DevFailed);

  Tango::CommandInfoList* command_list_query () 
    throw (Tango::DevFailed);

  std::vector<std::string>* get_attribute_list () 
    throw (Tango::DevFailed);

  Tango::AttributeInfoList* attribute_list_query () 
    throw (Tango::DevFailed);

  Tango::DeviceAttribute read_attribute (const std::string& attr_name) 
    throw (Tango::DevFailed);

  Tango::DeviceAttribute read_attribute (const char * attr_name)
    throw (Tango::DevFailed);

  std::vector<Tango::DeviceAttribute>* read_attributes (const std::vector<std::string>& attr_names) 
    throw (Tango::DevFailed);

  void write_attribute (const Tango::DeviceAttribute& attr_value)
    throw (Tango::DevFailed);

  void write_attributes (const std::vector<Tango::DeviceAttribute>& attr_values)
    throw (Tango::DevFailed);

  std::vector<Tango::DeviceAttributeHistory> * attribute_history (const std::string & attr_name, int history_len)
    throw (Tango::DevFailed);

  std::vector<Tango::DeviceAttributeHistory> * attribute_history (const char * attr_name, int history_len) 
    throw (Tango::DevFailed);

  Tango::DeviceData command_inout (const std::string& cmd_name) 
    throw (Tango::DevFailed);

  Tango::DeviceData command_inout (const char * cmd_name)
    throw (Tango::DevFailed);

  Tango::DeviceData command_inout (const std::string& cmd_name, const Tango::DeviceData &d) 
    throw (Tango::DevFailed);

  Tango::DeviceData command_inout (const char *cmd_name, const Tango::DeviceData &d)
    throw (Tango::DevFailed);

  int subscribe_event(const string &attr_name, 
                      Tango::EventType event, 
                      Tango::CallBack *cb, 
                      const std::vector<std::string> &filters) 
    throw (Tango::DevFailed);

  int subscribe_event(const string &attr_name, 
                      Tango::EventType event, 
                      Tango::CallBack *cb)
    throw (Tango::DevFailed);
    
  void unsubscribe_event(int event_id)
    throw (Tango::DevFailed);

  Tango::DeviceProxy& unsafe_proxy ();

  const std::string& dev_name () const;

  void set_timeout (size_t tmo_msecs);

private:
  Tango::DeviceProxy * dp_;

  bool ownership_;

  std::string name_;

#if (TANGO_VERSION_MAJOR < 7)
  yat::Mutex lock_;
#endif

  // = Disallow these operations (except for friends).
  //---------------------------------------------------------
  ThreadSafeDeviceProxy (const ThreadSafeDeviceProxy&);
  void operator= (const ThreadSafeDeviceProxy&); 
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "ThreadSafeDeviceProxy.i"
#endif 
