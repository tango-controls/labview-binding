// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ThreadSafeDeviceProxy.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// ThreadSafeDeviceProxy::state
//=============================================================================
LV_INLINE const std::string
ThreadSafeDeviceProxy::status () 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->status();
}

//=============================================================================
// ThreadSafeDeviceProxy::state
//=============================================================================
LV_INLINE Tango::DevState
ThreadSafeDeviceProxy::state () 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->state();
}

//=============================================================================
// ThreadSafeDeviceProxy::ping
//=============================================================================
LV_INLINE int 
ThreadSafeDeviceProxy::ping () 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
toto
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->ping();
}

//=============================================================================
// ThreadSafeDeviceProxy::command_list_query
//=============================================================================
LV_INLINE Tango::CommandInfoList* 
ThreadSafeDeviceProxy::command_list_query () 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->command_list_query();
}

//=============================================================================
// ThreadSafeDeviceProxy::attribute_list
//=============================================================================
LV_INLINE std::vector<std::string>* 
ThreadSafeDeviceProxy::get_attribute_list () 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->get_attribute_list();
}

//=============================================================================
// ThreadSafeDeviceProxy::attribute_list_query
//=============================================================================
LV_INLINE Tango::AttributeInfoList* 
ThreadSafeDeviceProxy::attribute_list_query () 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->attribute_list_query();
}

//=============================================================================
// ThreadSafeDeviceProxy::read_attribute
//=============================================================================
LV_INLINE Tango::DeviceAttribute
ThreadSafeDeviceProxy::read_attribute (const std::string& attr_name) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->read_attribute(const_cast<std::string&>(attr_name));
}

//=============================================================================
// ThreadSafeDeviceProxy::read_attribute
//=============================================================================
LV_INLINE Tango::DeviceAttribute
ThreadSafeDeviceProxy::read_attribute (const char *attr_name) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->read_attribute(attr_name);
}

//=============================================================================
// ThreadSafeDeviceProxy:read_attributes
//=============================================================================
LV_INLINE std::vector<Tango::DeviceAttribute>*
ThreadSafeDeviceProxy::read_attributes (const std::vector<std::string>& attr_names) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif
  
  omni_thread::ensure_self es;

  return dp_->read_attributes(const_cast<std::vector<std::string>&>(attr_names));
}

//=============================================================================
// ThreadSafeDeviceProxy::write_attribute
//=============================================================================
LV_INLINE void
ThreadSafeDeviceProxy::write_attribute (const Tango::DeviceAttribute& attr_value) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  dp_->write_attribute(const_cast<Tango::DeviceAttribute&>(attr_value));
}

//=============================================================================
// ThreadSafeDeviceProxy::write_attributes
//=============================================================================
LV_INLINE void
ThreadSafeDeviceProxy::write_attributes (const std::vector<Tango::DeviceAttribute>& attr_values) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  dp_->write_attributes(const_cast<std::vector<Tango::DeviceAttribute>&>(attr_values));
}

//=============================================================================
// ThreadSafeDeviceProxy::command_inout
//=============================================================================
LV_INLINE Tango::DeviceData
ThreadSafeDeviceProxy::command_inout (const std::string& cmd_name) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->command_inout(const_cast<std::string&>(cmd_name));
}

//=============================================================================
// ThreadSafeDeviceProxy::attribute_history
//=============================================================================
LV_INLINE std::vector<Tango::DeviceAttributeHistory> * 
ThreadSafeDeviceProxy::attribute_history (const std::string & attr_name, int history_len) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->attribute_history(const_cast<std::string&>(attr_name), history_len);
}
 
//=============================================================================
// ThreadSafeDeviceProxy::attribute_history
//=============================================================================
LV_INLINE std::vector<Tango::DeviceAttributeHistory> * 
ThreadSafeDeviceProxy::attribute_history (const char * attr_name, int history_len) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->attribute_history(attr_name, history_len);
}
    
//=============================================================================
// ThreadSafeDeviceProxy::command_inout
//=============================================================================
LV_INLINE Tango::DeviceData
ThreadSafeDeviceProxy::command_inout (const char * cmd_name) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif
 
  omni_thread::ensure_self es;

  return dp_->command_inout(cmd_name);
}

//=============================================================================
// ThreadSafeDeviceProxy::command_inout
//=============================================================================
LV_INLINE Tango::DeviceData
ThreadSafeDeviceProxy::command_inout (const std::string& cmd_name, 
                                      const Tango::DeviceData &d) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->command_inout(const_cast<std::string&>(cmd_name), const_cast<Tango::DeviceData &>(d));
}

//=============================================================================
// ThreadSafeDeviceProxy::command_inout
//=============================================================================
LV_INLINE Tango::DeviceData
ThreadSafeDeviceProxy::command_inout (const char * cmd_name, 
                                      const Tango::DeviceData &d) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->command_inout(cmd_name, const_cast<Tango::DeviceData &>(d));
}

//=============================================================================
// ThreadSafeDeviceProxy::subscribe_event
//=============================================================================
LV_INLINE int
ThreadSafeDeviceProxy::subscribe_event(const std::string &attr_name, 
                                       Tango::EventType event, 
                                       Tango::CallBack *cb, 
                                       const vector<string> &filters) 
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif
  
  omni_thread::ensure_self es;

  return dp_->subscribe_event(attr_name, event, cb, filters);
}

//=============================================================================
// ThreadSafeDeviceProxy::subscribe_event
//=============================================================================
LV_INLINE int
ThreadSafeDeviceProxy::subscribe_event(const std::string &attr_name, 
                                       Tango::EventType event, 
                                       Tango::CallBack *cb)
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  return dp_->subscribe_event(attr_name, event, cb);
}

//=============================================================================
// ThreadSafeDeviceProxy::unsubscribe_event
//=============================================================================
LV_INLINE void
ThreadSafeDeviceProxy::unsubscribe_event (int event_id)
  throw (Tango::DevFailed)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  dp_->unsubscribe_event(event_id);
}

//=============================================================================
// ThreadSafeDeviceProxy::unsafe_proxy
//=============================================================================
LV_INLINE Tango::DeviceProxy&
ThreadSafeDeviceProxy::unsafe_proxy ()
{
  return *dp_;
}

//=============================================================================
// ThreadSafeDeviceProxy::proxy
//=============================================================================
LV_INLINE const std::string&
ThreadSafeDeviceProxy::dev_name () const
{
  return name_;
}

//=============================================================================
// ThreadSafeDeviceProxy::set_timeout
//=============================================================================
LV_INLINE void
ThreadSafeDeviceProxy::set_timeout (size_t tmo_msecs)
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  omni_thread::ensure_self es;

  dp_->set_timeout_millis(static_cast<int>(tmo_msecs));
}
