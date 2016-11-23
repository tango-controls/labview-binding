// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DeviceGroupRepository.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DeviceGroupRepository::instance
//=============================================================================
LV_INLINE DeviceGroupRepository* DeviceGroupRepository::instance ()
{
  return DeviceGroupRepository::instance_;
}

//=============================================================================
// DeviceGroupRepository::get
//=============================================================================
LV_INLINE DeviceGroup * DeviceGroupRepository::get (tbfl::int32 group_id)
{
  //- critical section
  yat::MutexLock guard(lock_);

  //- search for <group_id> in the repository
  GroupMapIterator it = group_map_.find(group_id);
  if (it != group_map_.end() && it->second) 
    return it->second;

  Tango::Except::throw_exception(_CPTC_("invalid argument"),
			                           _CPTC_("invalid group id specified [no such group]"),
			                           _CPTC_("DeviceGroupRepository::get"));

	//- make gcc happy...
	return 0;
}

