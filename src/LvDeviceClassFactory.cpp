// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceClassFactory.cpp
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
#include "LvDeviceServer.h"

//=============================================================================
// lv_binding_class_factory implementation
//=============================================================================
void lv_binding_class_factory (Tango::DServer* ds)
{
  TBFL_TRACE(ds);
  
  if ( ! ds )
  {
    TBFL_STDOUT("lv_binding_class_factory::unexpected null pointer to Tango::DServer [can't instanciate Tango device classes]");
    return;
  }
  
  if ( ! LvDeviceServer::instance() )
  {
    TBFL_STDOUT("lv_binding_class_factory::the LvDeviceServer is not properly initialized [can't instanciate Tango device classes]");
    return;
  }
  
  LvDeviceServer::instance()->class_factory(ds);
}

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_

