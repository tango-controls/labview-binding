// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ThreadSafeDeviceProxy.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "ThreadSafeDeviceProxy.h"

#if !defined (_LV_INLINE_)
  #include "ThreadSafeDeviceProxy.i"
#endif 

//=============================================================================
// ThreadSafeDeviceProxy::ThreadSafeDeviceProxy 
//=============================================================================
ThreadSafeDeviceProxy::ThreadSafeDeviceProxy (const std::string& dev_name)
    throw (Tango::DevFailed)
 : dp_(0),
   ownership_(true),
   name_(dev_name)  
{
  dp_ = new Tango::DeviceProxy(const_cast<std::string&>(dev_name));
  
  if ( ! dp_ ) 
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			                             _CPTC_("memory allocation failed"),
			                             _CPTC_("ThreadSafeDeviceProxy::ThreadSafeDeviceProxy"));
  } 
}

//=============================================================================
// ThreadSafeDeviceProxy::ThreadSafeDeviceProxy 
//=============================================================================
ThreadSafeDeviceProxy::ThreadSafeDeviceProxy (const char * dev_name)
    throw (Tango::DevFailed)
 : dp_(0),
   ownership_(true),
   name_(dev_name)
{
  std::string dev(dev_name);
  
  dp_ = new Tango::DeviceProxy(dev);
  
  if ( ! dp_ ) 
  {
    Tango::Except::throw_exception(_CPTC_("out of memory"),
			                             _CPTC_("memory allocation failed"),
			                             _CPTC_("ThreadSafeDeviceProxy::ThreadSafeDeviceProxy"));
  }
}

//=============================================================================
// ThreadSafeDeviceProxy::ThreadSafeDeviceProxy 
//=============================================================================
ThreadSafeDeviceProxy::ThreadSafeDeviceProxy (Tango::DeviceProxy * _dp, bool _ownership)
    throw (Tango::DevFailed)
 : dp_(_dp),
   ownership_(_ownership)
{
  if ( ! dp_ ) 
  {
    Tango::Except::throw_exception(_CPTC_("invalid argument"),
			                             _CPTC_("unexpected NULL DeviceProxy"),
			                             _CPTC_("ThreadSafeDeviceProxy::ThreadSafeDeviceProxy"));
  }
  name_ = dp_->dev_name();
}

//=============================================================================
// ThreadSafeDeviceProxy::~ThreadSafeDeviceProxy 
//=============================================================================
ThreadSafeDeviceProxy::~ThreadSafeDeviceProxy ()
{
#if (TANGO_VERSION_MAJOR < 7)
  yat::MutexLock guard(lock_);
#endif

  if ( dp_ && ownership_ )
    delete dp_;
}

