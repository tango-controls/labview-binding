// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DeviceGroupRepository.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "LvDeviceProxyRepository.h"
#include "DeviceGroupRepository.h"

#if !defined (_LV_INLINE_)
# include "DeviceGroupRepository.i"
#endif 

//=============================================================================
// STATIC MEMBERS
//=============================================================================
DeviceGroupRepository* DeviceGroupRepository::instance_ = 0;
//-----------------------------------------------------------------------------
tbfl::int32 DeviceGroupRepository::group_cnt = 0;

//=============================================================================
// CONSTs
//=============================================================================
#define kALL_GROUPS 0

//=============================================================================
// DeviceGroupRepository::init
//=============================================================================
void DeviceGroupRepository::init ()
    throw (Tango::DevFailed)
{
  if (DeviceGroupRepository::instance_ != 0)
    return;

  try
  {
    DeviceGroupRepository::instance_ = new DeviceGroupRepository;
    if ( ! DeviceGroupRepository::instance_ )
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			                       _CPTC_("Tango::DeviceGroupRepository instanciation failed"),
			                       _CPTC_("DeviceGroupRepository::init"));
  }
  catch (Tango::DevFailed& df)
  {
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("software error"),
			                          _CPTC_("Tango::DeviceGroupRepository instanciation failed [Tango exception caught]"),
			                          _CPTC_("DeviceGroupRepository::init"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("software error"),
			                       _CPTC_("Tango::DeviceGroupRepository instanciation failed [unknown exception caught]"),
			                       _CPTC_("DeviceGroupRepository::init"));
  }
}

//=============================================================================
// DeviceGroupRepository::cleanup
//=============================================================================
void DeviceGroupRepository::cleanup ()
    throw (Tango::DevFailed)
{
  if (DeviceGroupRepository::instance_)
  {
    delete DeviceGroupRepository::instance_;
    DeviceGroupRepository::instance_ = 0;
  }
}

//=============================================================================
// DeviceGroupRepository::DeviceGroupRepository
//=============================================================================
DeviceGroupRepository::DeviceGroupRepository ()
{
  //- noop
}

//=============================================================================
// DeviceGroupRepository::~DeviceGroupRepository
//=============================================================================
DeviceGroupRepository::~DeviceGroupRepository ()
{
  //- critical section
  yat::MutexLock guard(lock_);

  remove_group_i(kALL_GROUPS);
}

//=============================================================================
// DeviceGroupRepository::add
//=============================================================================
tbfl::int32 DeviceGroupRepository::add (const std::string& _name,
                                        const DeviceList & _devices_to_add,
                                        const DeviceList & _devices_to_remove)
    throw (Tango::DevFailed)
{ 
  Tango::Group * g = 0;

  if (! _devices_to_add.size())
  {
    Tango::Except::throw_exception(_CPTC_("invalid argument"),
			                       _CPTC_("can't create empty Tango::Group [empty managed device list]"),
                                   _CPTC_("DeviceGroupRepository::add"));
  }

  try
  {
    g = new Tango::Group(_name);
    if ( ! g ) 
      throw std::bad_alloc();
    
    g->add(_devices_to_add);

    if (_devices_to_remove.size())
      g->remove(_devices_to_remove);
  }
  catch (const std::bad_alloc&)
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			                       _CPTC_("Tango::Group initialization failed"),
			                       _CPTC_("DeviceGroupRepository::add"));
  }
  catch (Tango::DevFailed& df)
  {
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("software error"),
			                          _CPTC_("Tango::Group initialization failed [Tango exception caught]"),
			                          _CPTC_("DeviceGroupRepository::add"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("software error"),
			                       _CPTC_("Tango::Group initialization failed [unknown exception caught]"),
			                       _CPTC_("DeviceGroupRepository::add"));
  }

  DeviceGroup* dg = 0;

  try
  {
    dg = new DeviceGroup(g);
    if ( ! dg )
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc &)
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			                       _CPTC_("DeviceGroup instanciation failed"),
			                       _CPTC_("DeviceGroupRepository::add"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
			                       _CPTC_("DeviceGroup instanciation failed [unknown error]"),
			                       _CPTC_("DeviceGroupRepository::add"));
  }

  {
    //- critical section on repository
    yat::MutexLock guard(lock_);

    //- increment group cnt (i.e.group id)
    ++DeviceGroupRepository::group_cnt;

    //- insert the group into the repository
    group_map_[DeviceGroupRepository::group_cnt] = dg;
    
    //- push group devices into the dev. repository
    long num_device = g->get_size();
    for (long d = 1; d <= num_device; d++)
    {
      DEV_REP->device_proxy(g->get_device(d), false);
    }
  }

  return DeviceGroupRepository::group_cnt;
}

//=============================================================================
// DeviceGroupRepository::remove
//=============================================================================
void DeviceGroupRepository::remove (tbfl::int32 _group_id)
    throw (Tango::DevFailed)
{ 
  //- critical section
  yat::MutexLock guard(lock_);

  remove_group_i(_group_id);
}

//=============================================================================
// DeviceGroupRepository::remove_group_i
//=============================================================================
void DeviceGroupRepository::remove_group_i (tbfl::int32 _group_id)
{
  if ( kALL_GROUPS == _group_id ) 
  {
    GroupMapIterator cur = group_map_.begin();
    GroupMapIterator end = group_map_.end();
    for (; cur != end; cur++) 
    {
      DeviceGroup* dg = cur->second;
      if ( dg )
      {
        //- remove devices from repository
        const DeviceList& dl = dg->device_list();
		for (size_t d = 0; d < dl.size(); d++)
		{
		  DEV_REP->remove_device(dl[d]);
		}
        //- delete group descriptor
        delete dg;
      }
    } 
    group_map_.clear();
  }
  else 
  {
    GroupMapIterator it = group_map_.find(_group_id);
    if (it != group_map_.end()) 
    {
      DeviceGroup* dg = it->second;
      if ( dg )
      {
        //- remove device from repository
        const DeviceList& dl = dg->device_list();
		for (size_t d = 0; d < dl.size(); d++)
		{
			DEV_REP->remove_device(dl[d]);
		}
        group_map_.erase(_group_id);
        //- delete group descriptor
        delete dg;
      }
    }
  }
}




