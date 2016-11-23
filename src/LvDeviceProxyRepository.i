// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceProxyRepository.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// LvDeviceProxyRepository::device_proxy
//=============================================================================
LV_INLINE LvDeviceProxy& 
LvDeviceProxyRepository::device_proxy (const char* _device_name)
    throw (Tango::DevFailed)
{
  return device_proxy(std::string(_device_name));
}

//=============================================================================
// LvDeviceProxyRepository::instance
//=============================================================================
LV_INLINE LvDeviceProxyRepository* LvDeviceProxyRepository::instance ()
{
  return LvDeviceProxyRepository::instance_;
}
