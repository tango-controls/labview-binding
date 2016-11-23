// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DataAdapter.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DataAdapter::instance
//=============================================================================
LV_INLINE DataAdapter* DataAdapter::instance ()
{
  return DataAdapter::instance_;
}
