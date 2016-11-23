// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   EventManager.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#include "Config.h"

#pragma once

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <yat/time/Timer.h>
#include "TargetPlatform.h"

#if defined(_TBFL_WINDOWS_)
//=============================================================================
// WindowsOutputDebugString
//=============================================================================
struct WindowsOutputDebugString
{    
  //--------------------------------------------------------------------------
  //- this is a simplified version of a piece of code found on the following
  //- URL while searching for a windows 'OutputDebugStringA' example:
  //- http://coliru.stacked-crooked.com/a/481b444c09361f1d
  //--------------------------------------------------------------------------
  //- usage examples
  //--------------------------------------------------------------------------
  //- TBFL_STDOUT(L"HI");
  //- TBFL_STDOUT("my name is %s", name.c_str());
  //- TBFL_STDOUT << name;
  //--------------------------------------------------------------------------
  WindowsOutputDebugString ()
  {
    //- noop
  }
  ~WindowsOutputDebugString ()  
  {
    //- oss << '\n'; 
    OutputDebugStringA(oss.str().c_str());
  }
  std::ostringstream& operator() () 
  { 
    return oss;
  }
  void operator() (const char* msg) 
  { 
    oss << msg;
  }
  void operator() (const std::string& msg) 
  { 
    oss << msg;
  }
  template<class char_type> void operator() (const char_type* format, ...) 
  {
    char_type buf[4096];
    va_list ap;
    va_start(ap,format);
    vsnprintf_s(buf, 4096, format, ap);
    va_end(ap);
    operator()(buf);
  }
  void operator()(const Tango::DevFailed& e) 
  {
    static tbfl::uint32 exception_tag = 0;
    tbfl::uint32 num_errors = e.errors.length();
    for ( tbfl::uint32 i = 0; i < num_errors; i++ ) 
    {
      TangoSys_OMemStream msg;
      msg << "[Ex:" 
          << exception_tag 
          << "-Err:" 
          << i 
          << "] "
          << "Rsn: " 
          << e.errors[i].reason.in() 
          << " "
          << "Dsc: " 
          << e.errors[i].desc.in()   
          << " "
          << "Org: " 
          << e.errors[i].origin.in();
      operator()(msg.str().c_str());
    }
    exception_tag++;
  }
  static int vsnprintf_s (char* buffer, int bufsize, const char* format, va_list ap) 
  { 
    return ::vsnprintf_s (buffer, bufsize, _TRUNCATE, format, ap);
  }
  std::ostringstream oss;
};
//=============================================================================
// allow to push a Tango::DevFailed into a WindowsOutputDebugString
//=============================================================================
std::ostringstream& operator<< (std::ostringstream& oss, const Tango::DevFailed& e) 
{
  static tbfl::uint32 exception_tag = 0;
  tbfl::uint32 num_errors = e.errors.length();
  for ( tbfl::uint32 i = 0; i < num_errors; i++ ) 
  {
    oss << "[Ex:" 
        << exception_tag 
        << "-Err:" 
        << i 
        << "] "
        << "Rsn: " 
        << e.errors[i].reason.in() 
        << " "
        << "Dsc: " 
        << e.errors[i].desc.in()   
        << " "
        << "Org: " 
        << e.errors[i].origin.in();
  }
  exception_tag++;
  return oss;
}
#endif

//-----------------------------------------------------------------------------
// TBFL_STDOUT
//-----------------------------------------------------------------------------
#if defined(_TBFL_VERBOSE_)
# if defined(_TBFL_WINDOWS_)
#   define TBFL_STDOUT(A) WindowsOutputDebugString()() << A << std::endl
# else
#   define TBFL_STDOUT(A) std::cout << A << std::endl
# endif
#else
# define TBFL_STDOUT(A)
#endif
  
//-----------------------------------------------------------------------------
// LOGGING
//-----------------------------------------------------------------------------
struct LbflTrace
{
  LbflTrace(const std::string& _loc, const char* _cid)
    :  cid(_cid), loc(_loc)
  {
    TBFL_STDOUT(loc << "[" << cid << "] <-");
  };
  LbflTrace(const std::string& _loc, const std::string& _cid)
    :  cid(_cid), loc(_loc)
  {
    TBFL_STDOUT(loc << "[" << cid << "] <-");;
  };
  LbflTrace(const std::string& _loc, const void* _cid)
    :  cid(), loc(_loc)
  {
    std::ostringstream oss;
    oss << std::hex << _cid << std::dec;
    cid = oss.str();
    TBFL_STDOUT(loc << "[" << cid << "] <-");
  };
  ~LbflTrace()
  {
    TBFL_STDOUT(loc << "[" << cid << "][took: " << tmr.elapsed_msec() << "ms] ->");
  };
  yat::Timer  tmr;
  std::string cid;
  std::string loc;
};
//-----------------------------------------------------------------------------
# if defined(_TBFL_WINDOWS_)
#   define TBFL_FUNC  __FUNCTION__ 
# else
#   define TBFL_FUNC  __func__
#endif
//-----------------------------------------------------------------------------
#if defined(_TBFL_VERBOSE_)
# define TBFL_TRACE(I) LbflTrace t(TBFL_FUNC, I)
#else
# define TBFL_TRACE(I)
#endif
//-----------------------------------------------------------------------------

   
