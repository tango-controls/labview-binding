// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceProxyRepository.cpp
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

#if !defined (_LV_INLINE_)
# include "LvDeviceProxyRepository.i"
#endif 

//=============================================================================
// STATIC MEMBERS
//=============================================================================
LvDeviceProxyRepository* LvDeviceProxyRepository::instance_ = 0;

//=============================================================================
// LvDeviceProxyRepository::cleanup
//=============================================================================
void LvDeviceProxyRepository::init ()
    throw (Tango::DevFailed)
{
  if (LvDeviceProxyRepository::instance_ != 0)
    return;

  try
  {
    LvDeviceProxyRepository::instance_ = new LvDeviceProxyRepository;
    if (LvDeviceProxyRepository::instance_ == 0)
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc&)
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			           _CPTC_("LvDeviceProxyRepository instanciation failed"),
			           _CPTC_("LvDeviceProxyRepository::init_device_repository"));
  }
  catch (Tango::DevFailed& df)
  {
    Tango::Except::re_throw_exception(df,
                    _CPTC_("software error"),
			              _CPTC_("LvDeviceProxyRepository instanciation failed [Tango exception caught]"),
			              _CPTC_("LvDeviceProxyRepository::init_device_repository"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("software error"),
			           _CPTC_("LvDeviceProxyRepository instanciation failed [unknown exception caught]"),
			           _CPTC_("LvDeviceProxyRepository::init_device_repository"));
  }
}

//=============================================================================
// LvDeviceProxyRepository::cleanup
//=============================================================================
void LvDeviceProxyRepository::cleanup ()
    throw (Tango::DevFailed)
{
  if (LvDeviceProxyRepository::instance_)
  {
    delete LvDeviceProxyRepository::instance_;
    LvDeviceProxyRepository::instance_ = 0;
  }
}

//=============================================================================
// LvDeviceProxyRepository::LvDeviceProxyRepository
//=============================================================================
LvDeviceProxyRepository::LvDeviceProxyRepository ()
{
  //- noop
}

//=============================================================================
// LvDeviceProxyRepository::~LvDeviceProxyRepository
//=============================================================================
LvDeviceProxyRepository::~LvDeviceProxyRepository ()
{
  //- critical section
  yat::MutexLock guard(lock_);

  remove_device_i("*");
}

//=============================================================================
// LvDeviceProxyRepository::device_proxy
//=============================================================================
LvDeviceProxy& LvDeviceProxyRepository::device_proxy (const std::string& _device_name)
    throw (Tango::DevFailed)
{
  //- critical section
  yat::MutexLock guard(lock_);

  //- convert to lower case
  std::string dev_name(_device_name);
  std::transform(dev_name.begin(), dev_name.end(), dev_name.begin(), ::tolower);

  //- search for <device_name> in the repository
  LvDeviceProxyMapIterator it = dev_map_.find(dev_name);
  if ( it != dev_map_.end() && it->second ) 
    return *(it->second);
          
  LvDeviceProxy* dvp = 0;

  try
  {
    //- search failed, create the descriptor
    dvp = new LvDeviceProxy(dev_name);
    if ( ! dvp ) 
      throw std::bad_alloc();
  }
  catch (const std::bad_alloc &)
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			           _CPTC_("memory allocation failed"),
			           _CPTC_("LvDeviceProxyRepository::device_proxy"));
  }
  catch (Tango::DevFailed & df)
  {
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("object instanciation failed"),
			              _CPTC_("LvDeviceProxy instanciation failed [Tango exception caught]"),
			              _CPTC_("LvDeviceProxyRepository::device_proxy"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("object instanciation failed"),
			           _CPTC_("LvDeviceProxy instanciation failed [unknown error]"),
			           _CPTC_("LvDeviceProxyRepository::device_proxy"));
  }

  //- insert the device proxy into the repository
  dev_map_[dev_name] = dvp;

  //- return the device proxy
  return *dvp;
}

//=============================================================================
// LvDeviceProxyRepository::device_proxy
//=============================================================================
LvDeviceProxy& LvDeviceProxyRepository::device_proxy (Tango::DeviceProxy * _dp, bool _ownership)
    throw (Tango::DevFailed)
{
  //- critical section
  yat::MutexLock guard(lock_);

  if ( ! _dp )
  {
    Tango::Except::throw_exception(_CPTC_("invalid argument"),
			           _CPTC_("unexpected NULL argument"),
			           _CPTC_("LvDeviceProxyRepository::device_proxy"));
  }

  //- convert to lower case
  std::string dev_name(_dp->dev_name());
  std::transform(dev_name.begin(), dev_name.end(), dev_name.begin(), ::tolower);

  //- search for <device_name> in the repository
  LvDeviceProxyMapIterator it = dev_map_.find(dev_name);
  if (it != dev_map_.end() && it->second) 
    return *(it->second);

  //- search failed, create the descriptor
  LvDeviceProxy* dvp = new LvDeviceProxy(_dp, _ownership);
  if ( ! dvp ) 
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			           _CPTC_("memory allocation failed"),
			           _CPTC_("LvDeviceProxyRepository::device_proxy"));
  }

  //- insert the device proxy into the repository
  dev_map_[dev_name] = dvp;

  //- return the device proxy
  return *dvp;
}

//=============================================================================
// LvDeviceProxyRepository::remove_device
//=============================================================================
void LvDeviceProxyRepository::remove_device (const std::string& _device_name)
{
  //- critical section
  yat::MutexLock guard(lock_);

  remove_device_i(_device_name);
}

//=============================================================================
// LvDeviceProxyRepository::remove_device_i
//=============================================================================
void LvDeviceProxyRepository::remove_device_i(const std::string& _device_name)
{
  if (_device_name == "*") 
  {
    LvDeviceProxyMapIterator cur = dev_map_.begin();
    LvDeviceProxyMapIterator end = dev_map_.end();
    for (; cur != end; cur++) 
    {
      if (cur->second)
	      delete cur->second;
    } 
    dev_map_.clear();
  }
  else 
  {
    std::string dev_name(_device_name);
    std::transform(dev_name.begin(), dev_name.end(), dev_name.begin(), ::tolower);
    LvDeviceProxyMapIterator it = dev_map_.find(dev_name);
    if (it != dev_map_.end()) 
    {
      if (it->second)
        delete it->second;
      dev_map_.erase(dev_name);
    }
  }
}
