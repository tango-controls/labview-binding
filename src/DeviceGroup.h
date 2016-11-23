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

//=============================================================================
// TYPEDEFs
//=============================================================================
typedef std::vector<std::string> DeviceList;

//=============================================================================
// CLASS : DeviceGroup
//=============================================================================
class DeviceGroup
{
  friend class DeviceGroupRepository;

public:
  //- ctor
  DeviceGroup (Tango::Group * _g);

  //- underlying group access
  Tango::Group * operator-> ();

  //- underlying group access
  Tango::Group * group ();

  //- list of devices belonging to group
  const DeviceList & device_list ();

  //- default ctor
  bool operator< (const DeviceGroup & _src) const;

private:
  //- ref to group
  Tango::Group * g;

  //- list of devices belonging to group
  DeviceList dl;

  //- private/hidden/forbidden methods
  DeviceGroup (const DeviceGroup&);
  virtual ~DeviceGroup ();
  DeviceGroup & operator= (const DeviceGroup&);
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "DeviceGroup.i"
#endif 

   
   
