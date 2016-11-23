// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DeviceGroupRepository.h
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
#include "DeviceGroup.h"

//=============================================================================
// DEFINEs
//=============================================================================
#define GRP_REP (DeviceGroupRepository::instance())

//=============================================================================
// CLASS : DeviceGroupRepository (SINGLETON)
//=============================================================================
class DeviceGroupRepository
{
  friend class ObjectManager;

  _DECLARE_MAP_(tbfl::int32, DeviceGroup*, Group);

public:

  static DeviceGroupRepository * instance ();
  // Returns the unique instance of <DeviceGroupRepository>.

  tbfl::int32 add (const std::string& name, 
                   const DeviceList & devices_to_add,
                   const DeviceList & devices_to_remove)
    throw (Tango::DevFailed);
  // Given a group name, create the group then insert it into the repository.

  void remove (tbfl::int32 group_id)
    throw (Tango::DevFailed);
  // Given a group identify, remove it from the repository.

  DeviceGroup * get (tbfl::int32 group_id);
  // Given a group id, returns a ref to this group or NULL if no such group

private:
  static tbfl::int32 group_cnt;
  //- group identifier 
  
  static void init ()
    throw (Tango::DevFailed);
  //- Instanciates the singleton.
  
  static void cleanup ()
    throw (Tango::DevFailed);
  //- Releases the singleton.

  static DeviceGroupRepository * instance_;
  //- The unique instance of <DeviceGroupRepository>.

  void remove_group_i (tbfl::int32 group_id);
  //- Providing a group id, removes it from the repository.

  GroupMap group_map_;
  //- The actual repository (see typedef above).

  yat::Mutex lock_;
  //- Use a lock in order to make the repository thread-safe.

  // = Disallow these operations (except for TangoClientXop).
  //---------------------------------------------------------
  DeviceGroupRepository();
  DeviceGroupRepository (const DeviceGroupRepository&);
  virtual ~DeviceGroupRepository();
  void operator= (const DeviceGroupRepository&);
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "DeviceGroupRepository.i"
#endif 

   
   
