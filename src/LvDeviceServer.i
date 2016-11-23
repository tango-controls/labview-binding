// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DeviceManager.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// LvDeviceServer::instance
//=============================================================================
LV_INLINE LvDeviceServer* LvDeviceServer::instance ()
{
  return LvDeviceServer::instance_;
}
