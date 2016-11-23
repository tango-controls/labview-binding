// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceAppender.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#include "Config.h"

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "Types.h"
#include "ErrorStack.h"
#include "DataAdapter.h"
#include "LvDeviceProxy.h"
#include "LvDeviceServer.h"
#include "LvDevice.h"
#include "LvDeviceRepository.h"

// ============================================================================
// LvDeviceAppender
// ============================================================================
class LvDeviceAppender : public log4tango::Appender
{
  //- provide the LvDevice class with access to the ctor 
  friend class LvDevice;
  //- provide the log4tango::Logger with access to the dtor 
  friend class log4tango::Logger;
  //- provide the yat4tango::DynamicAttributeReadCallback with access to read_callback 
  friend class DynamicAttributeReadCallback;

  //- dedicate a type to the log buffer
  typedef std::deque<std::string> LogBuffer;

  //- ctor
  LvDeviceAppender (size_t max_log_buffer_depth = DEFAULT_LOG_BUFFER_DEPTH);

  //- ctor
  virtual ~LvDeviceAppender ();

  //- inherited from log4tango::Appender virtual interface.
  virtual bool requires_layout () const;
      
  //- inherited from log4tango::Appender virtual interface.
  virtual void set_layout (log4tango::Layout* layout);

  //- inherited from log4tango::Appender virtual interface.
  virtual void close ();

  //- log appender method.
  virtual int _append (const log4tango::LoggingEvent& event); 

  //- read callback of the associated dynamic attribute
  void read_callback (yat4tango::DynamicAttributeDynAttrCallbackData& cbd);

  //- thread safe access to the log buffer
  yat::Mutex lock_;

  //- log buffer
  LogBuffer log_buffer_;

  //- log buffer content changed flag
  bool log_buffer_changed_;

  //- max log buffer depth
  size_t max_log_buffer_depth_;
};

// ============================================================================
// LvDeviceAppender::LvDeviceAppender
// ============================================================================
LvDeviceAppender::LvDeviceAppender (size_t max_log_buffer_depth)
  : log4tango::Appender ("lv-device-appender"), 
    log_buffer_changed_ (true),
    max_log_buffer_depth_ (max_log_buffer_depth)
{
  TBFL_TRACE(get_name());
}

// ============================================================================
// LvDeviceAppender::~LvDeviceAppender
// ============================================================================
LvDeviceAppender::~LvDeviceAppender ()
{
  TBFL_TRACE(get_name());
}

// ============================================================================
// LvDeviceAppender::close
// ============================================================================
void LvDeviceAppender::close ()
{
  //- noop
}

// ============================================================================
// LvDeviceAppender::read_callback (yat4tango::DynamicAttributeReadCallback impl)
// ============================================================================
void LvDeviceAppender::read_callback (yat4tango::DynamicAttributeDynAttrCallbackData& d)
{
  static Tango::DevVarStringArray __log_array__;

  if (! d.tga)
  {
    THROW_DEVFAILED("DEVICE_ERROR",
                    "unexpected null pointer to Tango::Attribute [invalid yat4tango::DynamicAttributeDynAttrCallbackData]",
                    "yat4tango::LvDeviceAppender::read_callback");
  }

  {
    yat::MutexLock guard(lock_);

    if ( log_buffer_changed_ )
    {
      size_t l = log_buffer_.size();

      __log_array__.length(l);
      
      for (size_t i = 0; i < l; i++)
        __log_array__[i] = log_buffer_[i].c_str();

      log_buffer_changed_ = false;
    }

    d.tga->set_value(__log_array__.get_buffer(), __log_array__.length());
  }
}

// ============================================================================
// LvDeviceAppender::
// ============================================================================
int LvDeviceAppender::_append (const log4tango::LoggingEvent& event)
{
  //------------------------------------------------------------
  //- DO NOT LOG FROM THIS METHOD !!!
  //------------------------------------------------------------

  static const size_t max_time_str_len = 32;
  
  try
  {
    //- reformat the incomming LoggingEvent
    time_t raw_time;
    ::time(&raw_time);
    char c_date_string[max_time_str_len];
    struct tm * time_info = ::localtime(&raw_time);
    ::strftime (c_date_string, max_time_str_len, "%d:%m:%y@%H:%M:%S", time_info);
  
    std::ostringstream oss;
    oss << c_date_string
        << ": ["
        << log4tango::Level::get_name(event.level)
        << "] "
        << event.message;
        
    //- push the log into the log buffer
    {
      yat::MutexLock guard(lock_);
      //- push
      log_buffer_.push_back(oss.str());
      //- pop oldest log
      if ( log_buffer_.size() > max_log_buffer_depth_ )
        log_buffer_.pop_front();
      //- mark buffer content as changed
      log_buffer_changed_ = true;
    }
  }
  catch (...) 
  {
    return -1;
  }
  return 0;
} 

// ============================================================================
// LvDeviceAppender::requires_layout
// ============================================================================
bool LvDeviceAppender::requires_layout () const
{
  return false; 
}

// ============================================================================
// LvDeviceAppender::set_layout
// ============================================================================
void LvDeviceAppender::set_layout (log4tango::Layout* l)
{
  delete l;
}

//=============================================================================
// DEVICE_TRY
//=============================================================================
#if defined(_TBFL_WINDOWS_)
#define DEVICE_TRY(ACT) \
  try \
  { \
    ACT; \
  } \
  catch ( Tango::DevFailed& df ) \
  { \
    std::ostringstream oss; \
    std::string location(__FUNCTION__); \
    oss << "Tango exception caught in  " << location; \
    ERROR_STREAM << oss.str() << std::endl; \
    Tango::Except::re_throw_exception(df, \
                                      _CPTC_("DEVICE_ERROR"), \
                                      _CPTC_(oss.str().c_str()), \
                                      _CPTC_(location.c_str())); \
  } \
  catch ( ... ) \
  { \
    std::ostringstream oss; \
    std::string location(__FUNCTION__); \
    oss << "unknown exception caught in  " << location; \
    ERROR_STREAM << oss.str() << std::endl; \
    Tango::Except::throw_exception(_CPTC_("DEVICE_ERROR"), \
                                   _CPTC_(oss.str().c_str()), \
                                   _CPTC_(location.c_str())); \
  } 
#else //- _TBFL_WINDOWS_
//=============================================================================
// DEVICE_TRY
//=============================================================================
#define DEVICE_TRY(ACT) \
  try \
  { \
    ACT; \
  } \
  catch ( Tango::DevFailed& df ) \
  { \
    std::ostringstream oss; \
    std::string location(__func__); \
    oss << "Tango exception caught in  " << location; \
    ERROR_STREAM << oss.str() << std::endl; \
    Tango::Except::re_throw_exception(df, \
                                      _CPTC_("DEVICE_ERROR"), \
                                      _CPTC_(oss.str().c_str()), \
                                      _CPTC_(location.c_str())); \
  } \
  catch ( ... ) \
  { \
    std::ostringstream oss; \
    std::string location(__func__); \
    oss << "unknown exception caught in  " << location; \
    ERROR_STREAM << oss.str() << std::endl; \
    Tango::Except::throw_exception(_CPTC_("DEVICE_ERROR"), \
                                   _CPTC_(oss.str().c_str()), \
                                   _CPTC_(location.c_str())); \
  } 
#endif //- _TBFL_WINDOWS_

//=============================================================================
// LvDevice::LvDevice
//=============================================================================
LvDevice::LvDevice (Tango::DeviceClass* dc, const std::string& device_name, bool restarting)
 : Tango::Device_4Impl(dc, const_cast<char*>(device_name.c_str())),
   initialized_(false),
   status_("unknwon device status"),
   lv_device_appender_(0)
{
  TBFL_TRACE(name());

  //- add a LvDeviceAppender to our log4tango appender
  lv_device_appender_ = new LvDeviceAppender();
  get_logger()->add_appender(lv_device_appender_);

  //- best effort init: state & status will indicate failure is required
  _BEST_EFFORT_TRY_(init_device(true));
}

//=============================================================================
// LvDevice::~LvDevice
//=============================================================================
LvDevice::~LvDevice ()
{
  TBFL_TRACE(name());

  //- we do not own <lv_device_appender_> since we pass it to our logger
  //- don't even try to delete it!

  //-in case of error: a leak is better than a crash...
  _BEST_EFFORT_TRY_(delete_device(true));
}

//=============================================================================
// LvDevice::init_device
//=============================================================================
void LvDevice::init_device ()
{
  init_device(false);
}

//=============================================================================
// LvDevice::init_device
//=============================================================================
void LvDevice::init_device (bool restarting)
{
  TBFL_TRACE(name());
  
  //- note: we never call this method directly
  //- note: consequently, the Tango device monitor is guarantee to be locked

  //- point to associated LvDevice
  lv_device_ = LV_DEV_INST_REP->get_device_by_name(name());
  if ( ! lv_device_ )
  {
    std::ostringstream oss;
    oss << "LvDevice::init_device failed - could not obtain a valid reference to LvDevice [unexpected null pointer]";
    initialization_error_ = oss.str();
    return;
  }

  //- we might have been reinstanciated by the Tango kernel
  lv_device_->init_device(this);

  INFO_STREAM << "initializing LabVIEW device '" << name() << "'" << std::endl;

  //-post 'init' notification to our labview implementation
  ClientRequest pcr(CLIENT_REQ_EXEC_CMD, "Init");
  DEVICE_TRY(lv_device_->post_client_request(pcr));

  //- ok, we are now properly initialized
  initialized_ = true;

  INFO_STREAM << "LabVIEW device '" << name() << "' successfully initialized" << std::endl;
}

//=============================================================================
// LvDevice::delete_device
//=============================================================================
void LvDevice::delete_device ()
{
  delete_device(false);
}

//=============================================================================
// LvDevice::delete_device
//=============================================================================
void LvDevice::delete_device (bool restarting)
{
  TBFL_TRACE(name());
     
  //- note: we never call this method directly
  //- note: consequently, the Tango device monitor is guarantee to be locked

  try
  {
    INFO_STREAM << "deleting LabVIEW device '" << name() << "'" << std::endl;

    if ( lv_device_ )
    {
      //- forward delete_device to our LvDevice
      _BEST_EFFORT_TRY_(lv_device_->delete_device());
      lv_device_ = 0;
    }

    //- log one more time before deleting the yat4tango::LvDeviceAppender
    INFO_STREAM << "LabVIEW device '" << name() << "' deleted" << std::endl;

    //- reset initialization flag and error
    initialized_ = false;
    initialization_error_.clear();
  }
  catch ( Tango::DevFailed& df )
  {
    initialized_ = false;
    initialization_error_.clear();
    std::ostringstream oss;
    oss << "Tango exception caught while trying to delete device "
        << name();
    ERROR_STREAM << oss.str() << std::endl;
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("DEVICE_ERROR"),
                                      _CPTC_(oss.str().c_str()),
                                      _CPTC_("LvDevice::delete_device"));
  }
  catch ( ... )
  {
    initialized_ = false;
    initialization_error_.clear();
    std::ostringstream oss;
    oss << "unknown exception caught while trying to delete device "
        << name();
    ERROR_STREAM << oss.str() << std::endl;
    Tango::Except::throw_exception(_CPTC_("DEVICE_ERROR"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDevice::delete_device"));
  }
}

//=============================================================================
// LvDevice::dev_state
//=============================================================================
Tango::DevState LvDevice::dev_state ()
{
  TBFL_TRACE(name());

  if ( ! initialized_ || ! lv_device_ )
  {
    return initialization_error_.empty() ? Tango::INIT : Tango::FAULT;
  }

  return lv_device_->dev_state();
}

//=============================================================================
// LvDevice::dev_status
//=============================================================================
Tango::ConstDevString LvDevice::dev_status ()
{
  TBFL_TRACE(name());

  if ( ! initialized_  || ! lv_device_ )
  {
    status_ = initialization_error_.empty()
            ? "LabVIEW device initialization in progress..."
            : initialization_error_;
    return status_.c_str();
  }

  status_ = lv_device_->dev_status();

  return status_.c_str();
}

//=============================================================================
// LvDevice::set_client_request_timeout
//=============================================================================
void LvDevice::log (LoggingLevel level, const std::string& text)
{
  switch ( level )
  {
    case LV_DEBUG_STREAM:
      DEBUG_STREAM << text << std::endl;
      break;
    case LV_INFO_STREAM:
      INFO_STREAM << text << std::endl;
      break;
    case LV_WARN_STREAM:
      WARN_STREAM << text << std::endl;
      break;
    case LV_ERROR_STREAM:
      ERROR_STREAM << text << std::endl;
      break;
    case LV_FATAL_STREAM:
      FATAL_STREAM << text << std::endl;
      break;
    default:
      DEBUG_STREAM << text << std::endl;
      break;
  }
}

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_


