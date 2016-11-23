// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceImpl.cpp
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
#include "LvDeviceProxy.h"
#include "DataAdapter.h"
#include "LvDeviceServer.h"
#include "LvDeviceImpl.h"
#include "LvDeviceRepository.h"

//=============================================================================
// LvDeviceImpl::LvDeviceImpl
//=============================================================================
LvDeviceImpl::LvDeviceImpl(Tango::DeviceClass* dc, const std::string& device_name)
  : TANG0_DEV_IMPL(dc, const_cast<char*>(device_name.c_str())),
    initialized_(false),
    status_("unknwon device status"),
    init_cnt_(0)
{
  TBFL_TRACE(name());

  //- best effort init: state & status will indicate failure is required
  _BEST_EFFORT_TRY_(init_device());
}

//=============================================================================
// LvDeviceImpl::~LvDeviceImpl
//=============================================================================
LvDeviceImpl::~LvDeviceImpl ()
{
  TBFL_TRACE(name());

  //- we do not own <lv_device_appender_> since we pass it to our logger
  //- don't even try to delete it!

  //- in case of error: a leak is better than a crash...
  _BEST_EFFORT_TRY_(delete_device());
}

//=============================================================================
// LvDeviceImpl::init_device
//=============================================================================
void LvDeviceImpl::init_device ()
{
  TBFL_TRACE(name());
  
  //- note: we never call this method directly
  //- note: consequently, the Tango device monitor is guarantee to be locked
  
  INFO_STREAM << "initializing LabVIEW device '" << name() << "'" << std::endl;

  //- point to associated LvDevice
  lv_device_ = LV_DEV_INST_REP->get_device_by_name(name());
  if ( ! lv_device_ )
  {
    std::ostringstream oss;
    oss << "LvDeviceImpl::init_device failed - could not obtain a valid reference to LvDevice [unexpected null pointer]";
    initialization_error_ = oss.str();
    return;
  }

  try
  {
    //- delegate the initialization job to the underlying LvDevice...
    lv_device_->init_device(this, init_cnt_++);
  }
  catch ( Tango::DevFailed& df )
  {
    std::string err;
    ERROR_STACK->devfailed(df, err);
    ERROR_STREAM << df << std::endl;
    std::ostringstream oss;
    oss << "device initialization failed: "
        << std::endl
        << err;
    initialization_error_ = oss.str();
    if ( init_cnt_ )
    {
      Tango::Except::re_throw_exception(df,
                                        _CPTC_("DEVICE_ERROR"),
                                        _CPTC_("device initialization failed"),
                                        _CPTC_("LvDeviceImpl::init_device"));
    }
    else
    {
      return;
    }
  }
  catch ( ... )
  {
    std::ostringstream oss;
    oss << "device initialization failed [unknown exception caught]";
    ERROR_STREAM << oss.str() << std::endl;
    initialization_error_ = oss.str();
    if ( init_cnt_ )
    {
      Tango::Except::throw_exception(_CPTC_("DEVICE_ERROR"),
                                     _CPTC_(oss.str().c_str()),
                                     _CPTC_("LvDeviceImpl::init_device"));
    }
    else
    {
      return;
    }
  } 

  //- ok, we are now properly initialized
  initialized_ = true;

  INFO_STREAM << "LabVIEW device '" << name() << "' successfully initialized" << std::endl;
}

//=============================================================================
// LvDeviceImpl::delete_device
//=============================================================================
void LvDeviceImpl::delete_device ()
{
  TBFL_TRACE(name());
     
  //- note: we never call this method directly
  //- note: consequently, the Tango device monitor is guarantee to be locked

  try
  {
    INFO_STREAM << "deleting LabVIEW device '" << name() << "'" << std::endl;

    if ( lv_device_ )
    {
      //- forward delete_device to our LvDeviceImpl
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
                                      _CPTC_("LvDeviceImpl::delete_device"));
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
                                   _CPTC_("LvDeviceImpl::delete_device"));
  }
}

//=============================================================================
// LvDeviceImpl::dev_state
//=============================================================================
Tango::DevState LvDeviceImpl::dev_state ()
{
  TBFL_TRACE(name());

  if ( ! initialized_ || ! lv_device_ )
  {
    return initialization_error_.empty() ? Tango::INIT : Tango::FAULT;
  }

  return lv_device_->dev_state();
}

//=============================================================================
// LvDeviceImpl::dev_status
//=============================================================================
Tango::ConstDevString LvDeviceImpl::dev_status ()
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
// LvDeviceImpl::set_client_request_timeout
//=============================================================================
void LvDeviceImpl::log (LoggingLevel level, const std::string& text)
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


