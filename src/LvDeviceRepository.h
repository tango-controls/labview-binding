// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceRepository.h
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
#include <yat/threading/Mutex.h>
#include "CommonHeader.h"
#include "LvDevice.h"

//=============================================================================
// SHORTCUT
//=============================================================================
#define LV_DEV_INST_REP (LvDeviceRepository::instance())

//=============================================================================
// CLASS : LvDeviceRepository
//=============================================================================
class LvDeviceRepository
{
  friend class LvDeviceServer;

  _DECLARE_MAP_(std::string, LvDevice*,  LvDeviceByName);
  _DECLARE_MAP_(tbfl::uint32, LvDevice*,  LvDeviceBySource);
  
public:

  static LvDeviceRepository * instance ();
  //-return the unique instance of <LvDeviceRepository>.

  LvDevice* get_device_by_name (const std::string& device_name)
    throw (Tango::DevFailed);
  // return the LvDevice associated with the specified device (throw if no such device)

  LvDevice* get_device_by_source (tbfl::uint32)
    throw (Tango::DevFailed);
  // return the LvDevice associated with the specified device (throw if no such device)
    
private:
  static void init ()
    throw (Tango::DevFailed);
  //- instanciate the singleton.
  
  static void cleanup ()
    throw (Tango::DevFailed);
  //- release  the singleton.

  void register_device_instance (const std::string& dev_name, tbfl::uint32 src_id, LvDevice* dev_inst)
    throw (Tango::DevFailed);
  //- register the specified device instance
    
  static LvDeviceRepository* instance_;
  //- the unique instance of <LvDeviceRepository>.

  LvDeviceByNameMap devices_by_name_;
  LvDeviceBySourceMap devices_by_src_;
  //- device instances repositories

  yat::Mutex lock_;
  //- use a lock in order to make the repository thread-safe.

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  LvDeviceRepository();
  LvDeviceRepository (const LvDeviceRepository&);
  virtual ~LvDeviceRepository();
  void operator= (const LvDeviceRepository&);
};

#if defined (_LV_INLINE_)
#  include "LvDeviceRepository.i"
#endif 

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_

   
   
