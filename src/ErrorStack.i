// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ErrorStack.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// ErrorStack::instance
//=============================================================================
LV_INLINE ErrorStack* ErrorStack::instance ()
{
  return ErrorStack::instance_;
}


