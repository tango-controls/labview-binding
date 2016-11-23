// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceRepository.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// LvDeviceRepository::instance
//=============================================================================
LV_INLINE LvDeviceRepository* LvDeviceRepository::instance ()
{
  return LvDeviceRepository::instance_;
}
