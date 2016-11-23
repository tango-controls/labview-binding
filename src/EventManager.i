// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   EventManager.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// EventManager::instance
//=============================================================================
LV_INLINE EventManager* EventManager::instance ()
{
  return EventManager::instance_;
}

//=============================================================================
// EventManager::events_have_been_used
//=============================================================================
LV_INLINE bool EventManager::events_have_been_used ()
{
  return events_have_been_used_;
}






