// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceImpl.h
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
#include <tango.h>
#include <yat4tango/DynamicInterfaceManager.h>
#include "CommonHeader.h"

//=============================================================================
// FORWARD DECLARATION
//=============================================================================
class LvDeviceProxy;
class LvDevice;

//=============================================================================
// TANGO DEVICE IMPL. VERSION
//=============================================================================
#if (TANGO_VERSION_MAJOR >= 9)
# define TANG0_DEV_IMPL Tango::Device_5Impl
#else
# define TANG0_DEV_IMPL Tango::Device_4Impl
#endif

//=============================================================================
// LvDeviceImpl
//=============================================================================
class LvDeviceImpl : public TANG0_DEV_IMPL
{
public:
  LvDeviceImpl (Tango::DeviceClass* dc, const std::string& device_name);
  //- ctor
  
  virtual ~LvDeviceImpl ();
  //- dtor

  void init_device ();
  //- initialization - supposed to only be called by the Tango core impl!

  void delete_device ();
  //- termination/cleanup

  Tango::DevState dev_state ();
  //- device state 

  Tango::ConstDevString dev_status ();
  //- device status

  void log (LoggingLevel level, const std::string& text);
  //- log the specified text at the specified level

private:
  //- initialization success?
  bool initialized_;

  //- current (or at least, last known) status
  std::string status_;

  //- initialization success?
  std::string initialization_error_;

  //- actual implementation delegated to an instance of LvDevice
  LvDevice* lv_device_;

  //- init counter (1st call is async, others are sync.)
  tbfl::int32 init_cnt_;
};

//=============================================================================
// AutoTangoMonitor
//=============================================================================
class AutoTangoMonitor
{
public:
  AutoTangoMonitor (LvDeviceImpl* d)
    : dev(d)
  {
    dev->get_dev_monitor().get_monitor();
  }
  ~AutoTangoMonitor ()
  {
    dev->get_dev_monitor().rel_monitor();
  }
private:
  LvDeviceImpl* dev;
};

#endif // _TBFL_HAS_DEVICE_SERVER_SUPPORT_
