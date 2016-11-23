// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceClass.h
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
#include "CommonHeader.h"

//=============================================================================
// CLASS: LvDeviceClass
//=============================================================================
class LvDeviceClass : public Tango::DeviceClass
{
  friend class Tango::DServer;
  
public:
  static LvDeviceClass* init (const std::string& class_name, const std::string& device_name);
  //- class factory

  void command_factory ();
  //- command factory

  void attribute_factory (std::vector<Tango::Attr*>&);
  //- attribute factory
  
  void device_factory (const Tango::DevVarStringArray*);
  //- command factory

private:
  std::string device_name_;
  //- unique device belonging to the class 
  
  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  LvDeviceClass (const std::string& class_name, const std::string& device_name);
  LvDeviceClass (const LvDeviceClass&);
  virtual ~LvDeviceClass ();
  void operator= (const LvDeviceClass&);
};

#endif // _TBFL_HAS_DEVICE_SERVER_SUPPORT_
