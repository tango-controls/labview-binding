// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceClass.cpp
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
//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "LvDeviceImpl.h"
#include "LvDeviceServer.h"
#include "LvDeviceClass.h"

//=============================================================================
// LvDeviceClass::init
//=============================================================================
LvDeviceClass* LvDeviceClass::init (const std::string& class_name, const std::string& device_name)
{
  return new LvDeviceClass(class_name, device_name);
}

//=============================================================================
// LvDeviceClass::LvDeviceClass
//=============================================================================
LvDeviceClass::LvDeviceClass (const std::string& class_name, const std::string& device_name)
  : Tango::DeviceClass(const_cast<std::string&>(class_name)),
    device_name_(device_name)
{
  //- noop
}

//=============================================================================
// LvDeviceClass::LvDeviceClass
//=============================================================================  
LvDeviceClass::~LvDeviceClass ()
{
  //- noop
}

//=============================================================================
// LvDeviceClass::command_factory
//=============================================================================
void LvDeviceClass::command_factory ()
{
  TBFL_TRACE(get_name());
}

//=============================================================================
// LvDeviceClass::attribute_factory
//=============================================================================
void LvDeviceClass::attribute_factory (std::vector<Tango::Attr *>&)
{
  TBFL_TRACE(get_name());
}

//=============================================================================
// LvDeviceClass::device_factory
//=============================================================================
void LvDeviceClass::device_factory (const Tango::DevVarStringArray* dl)
{
  TBFL_TRACE(get_name());

  if ( ! LvDeviceServer::instance() )
  {
    return;
  }

  LvDeviceImpl* lv_device = LvDeviceServer::instance()->device_factory(this, device_name_);

  device_list.push_back(lv_device);

  if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
    export_device(lv_device);
  else
    export_device(lv_device, lv_device->get_name().c_str());
}

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_


