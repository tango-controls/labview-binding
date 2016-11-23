// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceProxyRepository.h
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
#include "CommonHeader.h"
#include "LvDeviceProxy.h"

//=============================================================================
// SHORTCUT
//=============================================================================
#define DEV_REP (LvDeviceProxyRepository::instance())

//=============================================================================
// CLASS : LvDeviceProxyRepository
//=============================================================================
class LvDeviceProxyRepository
{
  friend class ObjectManager;

  _DECLARE_MAP_(std::string, LvDeviceProxy*,  LvDeviceProxy);

public:

  static LvDeviceProxyRepository* instance ();
  //- Returns the unique instance of <LvDeviceProxyRepository>.

  LvDeviceProxy& device_proxy (const char* device_name)
    throw (Tango::DevFailed);
  // Providing a device name, returns its device descriptor.

  LvDeviceProxy& device_proxy (const std::string& device_name)
    throw (Tango::DevFailed);
  // Providing a device name, returns its device descriptor

  LvDeviceProxy& device_proxy (Tango::DeviceProxy* device_proxy, bool ownership = false)
    throw (Tango::DevFailed);
  // Providing a device proxy, returns its device descriptor

  void remove_device (const std::string& device_name);
  //- Providing a device name, removes it from the repository.

private:
  static void init ()
    throw (Tango::DevFailed);
  //- Instanciates the singleton.
  
  static void cleanup ()
    throw (Tango::DevFailed);
  //- Releases the singleton.

  static LvDeviceProxyRepository* instance_;
  //- The unique instance of <LvDeviceProxyRepository>.

  void remove_device_i (const std::string& device_name);
  //- Providing a device name, returns removes it from the repository.

  LvDeviceProxyMap dev_map_;
  //- The actual repository (see typedef above).

  yat::Mutex lock_;
  //- Use a lock in order to make the repository thread-safe.

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  LvDeviceProxyRepository();
  LvDeviceProxyRepository (const LvDeviceProxyRepository&);
  virtual ~LvDeviceProxyRepository();
  void operator= (const LvDeviceProxyRepository&);
};

#if defined (_LV_INLINE_)
#  include "LvDeviceProxyRepository.i"
#endif 


   
   
