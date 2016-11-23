// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ObjectManager.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#if defined(_TBFL_LINUX_) || defined(_TBFL_MACOSX_)
# include <dlfcn.h>
#endif
#include "ErrorStack.h"
#include "LvDeviceProxyRepository.h"
#include "DeviceGroupRepository.h"
#include "DataAdapter.h"
#include "EventManager.h"
#include "LvDeviceRepository.h"
#include "LvDeviceServer.h"
#include "ObjectManager.h"

//=============================================================================
// STATIC MEMBERS
//=============================================================================
#if (_TBFL_HAS_STATIC_OBJ_MANAGER_ == 1)
  ObjectManager ObjectManager::instance;
#else
  ObjectManager * ObjectManager::instance = 0;
#endif

//=============================================================================
// WINDOWS DLLMain ROUTINE
//=============================================================================
#if (0 == _TBFL_HAS_STATIC_OBJ_MANAGER_)

#if defined (_TBFL_WINDOWS_)

LVB_EXPORT BOOL APIENTRY DllMain(HANDLE h, DWORD reason_for_call, LPVOID r);
BOOL APIENTRY DllMain (HANDLE h, DWORD reason_for_call, LPVOID r)
{
  static bool __dll_locked__ = false;
  static HMODULE __dll_hdl__ = NULL;

  switch (reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
      try 
      {
        TBFL_STDOUT("***** DllMain::DLL_PROCESS_ATTACH *****");
        if ( ! __dll_locked__ )
        {
          TCHAR dll_path[MAX_PATH];
          ::memset(dll_path, 0, MAX_PATH);
          DWORD path_len = ::GetModuleFileName(static_cast<HMODULE>(h), dll_path, MAX_PATH);
          TBFL_STDOUT("GetModuleFileName returned: " << dll_path);
          __dll_hdl__ = ::LoadLibraryEx(dll_path, NULL, DONT_RESOLVE_DLL_REFERENCES);
          if ( ! __dll_hdl__ )
          {
            //- ok...no choice, have to deal with it!
            TBFL_STDOUT("LoadLibraryEx failed!" << dll_path);
          }
          __dll_locked__ = true;
        }
       
        ObjectManager::init();
      }
      catch (...) 
      {
        return FALSE;
      }
      break;
    case DLL_PROCESS_DETACH:
      try 
      {
        TBFL_STDOUT("***** DllMain::DLL_PROCESS_DETACH *****");
        ObjectManager::fini();

        if ( __dll_locked__ )
        {
          BOOL success = ::FreeLibrary(__dll_hdl__); 
          __dll_locked__ = false;
        }
      }
      catch (...) 
      {
        //- ignore any error
      }
    default:
      break;
  }
  return TRUE;
}

#elif defined(_TBFL_LINUX_) || defined(_TBFL_MACOSX_)

//=============================================================================
// GNU-LINUX SHARED LIB INITIALIZATION ROUTINE
//=============================================================================
void init_shared_lib () __attribute__((constructor));
void init_shared_lib ()
{
  TBFL_STDOUT("***** init_shared_lib *****");

  try
  {
    static bool __dll_locked__ = false;

    if ( ! __dll_locked__ )
    {
      void* dll_hdl = dlopen("tango_binding.so", RTLD_LAZY | RTLD_NODELETE);
      if ( ! dll_hdl )
      {
        //- ok...no choice, have to deal with it!
        TBFL_STDOUT("dlopen failed!");
        TBFL_STDOUT("dlopen error:" << dlerror());
      }
      __dll_locked__ = true;
    }

    ObjectManager::init();
  }
  catch (...)
  {
    //- ignore any exception
  }
}

//=============================================================================
// GNU-LINUX SHARED LIB CLEANUP ROUTINE
//=============================================================================
void cleanup_shared_lib () __attribute__((destructor)); 
void cleanup_shared_lib ()
{
  TBFL_STDOUT("***** cleanup_shared_lib *****");
  try 
  {
    ObjectManager::fini();
  }
  catch (...) 
  {
    //- ignore any exception
  }
}

#else 

# error sorry, no support for your platform

#endif 

//=============================================================================
// ObjectManager::init
//=============================================================================
void ObjectManager::init () throw (Tango::DevFailed)
{
  if ( ObjectManager::instance ) 
    return;

  try
  {
    ObjectManager::instance = new ObjectManager();
    if ( ! ObjectManager::instance )
      throw std::bad_alloc();
  }
  catch (Tango::DevFailed& df)
  {
    Tango::Except::re_throw_exception(df,
                                      _CPTC_("internal error"),
                                      _CPTC_("TANGO binding initialization failed"),
                                      _CPTC_("ObjectManager::init"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("TANGO binding initialization failed"),
                                   _CPTC_("ObjectManager::init"));
  }
}

//=============================================================================
// ObjectManager::fini
//=============================================================================
void ObjectManager::fini () throw(Tango::DevFailed)
{
  try
  {
    delete ObjectManager::instance;
    ObjectManager::instance = 0;
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("TANGO binding cleanup failed"),
                                   _CPTC_("ObjectManager::fini"));
  }
}
#endif //- _TBFL_HAS_STATIC_OBJ_MANAGER_

//=============================================================================
// ObjectManager::ObjectManager 
//=============================================================================
ObjectManager::ObjectManager ()
{
  TBFL_TRACE(this);

  //- force TANGO client API initialization
  Tango::ApiUtil * u = Tango::ApiUtil::instance();
  if ( ! u ) throw std::bad_alloc();

  //- init the exception converter singleton
  ErrorStack::init();

  //- init the data adapter singleton
  DataAdapter::init();

  //- init the device repository singleton
  LvDeviceProxyRepository::init();

  //- init the group repository singleton
  DeviceGroupRepository::init();

  //- init the event manager singleton
  EventManager::init();
}

//=============================================================================
// ObjectManager::~ObjectManager 
//=============================================================================
ObjectManager::~ObjectManager ()
{
  TBFL_TRACE(this);
  
  try 
  { 
    //- release the event manager singleton;
    EventManager::cleanup();

    //- release the group repository singleton;
    DeviceGroupRepository::cleanup();

    //- release the device repository singleton
    LvDeviceProxyRepository::cleanup();

    //- release the data adapter singleton
    DataAdapter::cleanup();

    //- release the exception converter singleton
    ErrorStack::cleanup();

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)
    LvDeviceServer::cleanup();
#endif
  }
  catch (...)
  {
    //- ignore any exception
  }
}

