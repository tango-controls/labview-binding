// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DeviceGroup.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "DeviceGroup.h"

#if !defined (_LV_INLINE_)
# include "DeviceGroup.i"
#endif 
 
//=============================================================================
// DeviceGroup::DeviceGroup
//=============================================================================
DeviceGroup::DeviceGroup (Tango::Group* _g)
    : g(_g), dl(0)
{
	if ( g )
	{
	  dl = g->get_device_list(false);
	}
}

//=============================================================================
// DeviceGroup::~DeviceGroup
//=============================================================================
DeviceGroup::~DeviceGroup ()
{
  dl.clear();
  delete g;
}


