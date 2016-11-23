// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceRepository.cpp
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
#include "CommonHeader.h"
#include "LvDeviceRepository.h"

#if !defined (_LV_INLINE_)
# include "LvDeviceRepository.i"
#endif 

//=============================================================================
// STATIC MEMBERS
//=============================================================================
LvDeviceRepository* LvDeviceRepository::instance_ = 0;

//=============================================================================
// LvDeviceRepository::cleanup
//=============================================================================
void LvDeviceRepository::init ()
    throw (Tango::DevFailed)
{
  if ( LvDeviceRepository::instance_ )
    return;

  try
  {
    LvDeviceRepository::instance_ = new LvDeviceRepository;
    if ( ! LvDeviceRepository::instance_)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			                             _CPTC_("LvDeviceRepository instanciation failed"),
			                             _CPTC_("LvDeviceRepository::init_device_repository"));
  }
  catch (Tango::DevFailed& df)
  {
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("software error"),
                                      _CPTC_("LvDeviceRepository instanciation failed [Tango exception caught]"),
                                      _CPTC_("LvDeviceRepository::init_device_repository"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("software error"),
                                   _CPTC_("LvDeviceRepository instanciation failed [unknown exception caught]"),
                                   _CPTC_("LvDeviceRepository::init_device_repository"));
  }
}

//=============================================================================
// LvDeviceRepository::cleanup
//=============================================================================
void LvDeviceRepository::cleanup ()
    throw (Tango::DevFailed)
{
  if ( LvDeviceRepository::instance_ )
  {
    delete LvDeviceRepository::instance_;
    LvDeviceRepository::instance_ = 0;
  }
}

//=============================================================================
// LvDeviceRepository::LvDeviceRepository
//=============================================================================
LvDeviceRepository::LvDeviceRepository ()
{
  //- noop
}

//=============================================================================
// LvDeviceRepository::~LvDeviceRepository
//=============================================================================
LvDeviceRepository::~LvDeviceRepository ()
{
  //- critical section
  //- yat::MutexLock guard(lock_);
}

//=============================================================================
// LvDeviceRepository::register_device_instance
//=============================================================================
void LvDeviceRepository::register_device_instance (const std::string& dev_name, tbfl::uint32 src_id, LvDevice* dev_inst)
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(lock_);
   
  devices_by_name_[dev_name] = dev_inst;
  devices_by_src_[src_id] = dev_inst;
}

//=============================================================================
// LvDeviceRepository::get_device_by_name
//=============================================================================
LvDevice* LvDeviceRepository::get_device_by_name (const std::string& dev_name)
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(lock_);
  
  LvDeviceByNameMapIterator it = devices_by_name_.find(dev_name);
  if ( it == devices_by_name_.end() )
  {
    std::ostringstream oss;
    oss << "unknown device name ["
        << dev_name
        << " is not the name of a registered Tango device]";
    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceRepository::get_device_by_name")); 
  }

  return it->second;
}

//=============================================================================
// LvDeviceRepository::get_device_by_source
//=============================================================================
LvDevice* LvDeviceRepository::get_device_by_source (tbfl::uint32 src_id)
  throw (Tango::DevFailed)
{
  yat::MutexLock guard(lock_);

  LvDeviceBySourceMapIterator it = devices_by_src_.find(src_id);
  if ( it == devices_by_src_.end() )
  {
    std::ostringstream oss;
    oss << "unknown evt. source identifier ["
        << src_id
        << " is not the source identifier of a registered Tango device]";
    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   _CPTC_(oss.str().c_str()),
                                   _CPTC_("LvDeviceRepository::get_device_by_source"));
  }

  return it->second;
}

#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_