// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ErrorStack.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include <cstdarg>
#include "CommonHeader.h"
#include "LvDeviceProxy.h"
#include "DataAdapter.h"
#include "ErrorStack.h"

#if !defined (_LV_INLINE_)
  #include "ErrorStack.i"
#endif 

//=============================================================================
// #DEFINEs                                                 
//=============================================================================
#define kNUM_STR_PER_ERROR 4

//=============================================================================
// STATIC MEMBERS
//=============================================================================
ErrorStack* ErrorStack::instance_ = 0;  

//=============================================================================
// ErrorStack::init
//=============================================================================
int ErrorStack::init ()
{ 
  if (ErrorStack::instance_ != 0)
    return 0;

  ErrorStack::instance_ = new ErrorStack;

  return (ErrorStack::instance_) ? 0 : -1;
}

//=============================================================================
// ErrorStack::cleanup
//=============================================================================
void ErrorStack::cleanup ()
{

}

//=============================================================================
// ErrorStack::ErrorStack                                                
//=============================================================================
ErrorStack::ErrorStack ()
{

}

//=============================================================================
// ErrorStack::~ErrorStack                                                
//=============================================================================
ErrorStack::~ErrorStack ()
{

}

//=============================================================================
// ErrorStack::no_error                                   
//=============================================================================
int ErrorStack::no_error (const char* o, LvStringArrayHdl& lv_error_)
{
  if ( ! lv_error_ || ! (*lv_error_) )
    return kError;

  unsigned int nerr = kNUM_STR_PER_ERROR;

  MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                   1L, 
                                   reinterpret_cast<UHandle*>(&lv_error_), 
                                   nerr);
  if (err != noErr) return kError;

  (*lv_error_)->length = nerr;
  ::memset((*lv_error_)->data, 0, (*lv_error_)->length * sizeof(void*));
  tbfl::size_t l;
  LvStringHdl sh;
  std::string str;
  for (unsigned int i = 0; i < nerr; i++)
  {
    switch (i)
    {
      case 3:
        str = o;
        break;
      default:
        str = "no error";
        break;
    }
    sh = 0;
	  l = static_cast<tbfl::size_t>(str.size());
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr) return kError;
	  (*sh)->length = l;
    ::memcpy((*sh)->data, str.c_str(), l);
    (*lv_error_)->data[i] = sh;
  }
  
  return kNoError;
}

//=============================================================================
// ErrorStack::unknown                                   
//=============================================================================
int ErrorStack::unknown (const char* o, LvStringArrayHdl& lv_error_)
{
  if ( ! lv_error_ || ! (*lv_error_) )
    return kError;

  unsigned int nerr = kNUM_STR_PER_ERROR;

  MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                   1L, 
                                   reinterpret_cast<UHandle*>(&lv_error_), 
                                   nerr);
  if (err != noErr) return kError;

  (*lv_error_)->length = nerr;
  ::memset((*lv_error_)->data, 0, (*lv_error_)->length * sizeof(void*));
  tbfl::size_t l;
  LvStringHdl sh;
  std::string str;
  for (unsigned int i = 0; i < nerr; i++)
  {
    switch (i)
    {
      case 0:
        str = "ERROR";
        break;
      case 1:
        str = "unknown error (may be due to a bug in the TANGO binding for Labview)";
        break;
      case 2:
        str = "unknown exception caught";
        break;
      case 3:
        str = o;
        break;
      default:
        str = "-";
        break;
    }
    sh = 0;
	  l = static_cast<tbfl::size_t>(str.size());
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr) return kError;
	  (*sh)->length = l;
    ::memcpy((*sh)->data, str.c_str(), l);
    (*lv_error_)->data[i] = sh;
  }
  return kNoError;
}

//=============================================================================
// ErrorStack::devfailed
//=============================================================================
Tango::DevFailed ErrorStack::devfailed (LvStringArrayHdl& _lv_error, bool raise)
  throw(Tango::DevFailed)
{
  if ( ! _lv_error || ! (*_lv_error) )
  {  
    try
    {
      Tango::Except::throw_exception(_CPTC_("LABVIEW_DEVICE_ERROR"),
                                     _CPTC_("sorry, no error details available!"),
                                     _CPTC_("LabVIEW Device Impl."));
    }
    catch (const Tango::DevFailed& df)
    {
      if ( raise )
      {
        throw df;
      }
      return df;
    }
  }

  Tango::DevFailed df;
  tbfl::size_t l = (*_lv_error)->length;
  for ( tbfl::size_t i = 0; i < l; i++ )
  {
    std::string s;
    DATA_ADAPTER->to_std_str(reinterpret_cast<LvStringHdl&>((*_lv_error)->data[i]), s);
    try
    {
      Tango::Except::re_throw_exception(df,
                                        _CPTC_("LABVIEW_DEVICE_ERROR"),
                                        _CPTC_(s.c_str()),
                                        _CPTC_("LabVIEW Device Impl."));
    }   
    catch (...) {}
  }

  if ( raise )
  {
    throw df;
  }

  return df;
}

//=============================================================================
// ErrorStack::devfailed                                    
//=============================================================================
int ErrorStack::devfailed (const Tango::DevFailed& _tango_error, LvStringArrayHdl& lv_error_)
{
  if ( ! lv_error_ || ! (*lv_error_) )
    return kError;

  tbfl::uint32 nerr = _tango_error.errors.length();
 
  MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                   1L, 
                                   reinterpret_cast<UHandle*>(&lv_error_), 
                                   nerr * kNUM_STR_PER_ERROR);
  if (err != noErr) return kError;

  (*lv_error_)->length = nerr * kNUM_STR_PER_ERROR;
  ::memset((*lv_error_)->data, 0, (*lv_error_)->length * sizeof(void*));
  tbfl::size_t l = 0;
  LvStringHdl sh = 0;
  std::string severity;
  for (tbfl::size_t i = 0, j = 0; i < nerr; i++) 
  {
    //- severity ---------------------------------------------------------------
    switch(_tango_error.errors[i].severity)
    {
      case Tango::WARN:
        severity = "WARNING";
        break;
      case Tango::PANIC:
        severity = "PANIC";
        break;
      case Tango::ERR:
      default:
        severity = "ERROR";
        break;
    }
    sh = 0;
	  l = static_cast<tbfl::size_t>(severity.size());
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr) return kError;
	  (*sh)->length = l;
    ::memcpy((*sh)->data, severity.c_str(), l);
    (*lv_error_)->data[j++] = sh;
    //- reason ------------------------------------------------------------------
    sh = 0;
    l = static_cast<tbfl::size_t>(::strlen(_tango_error.errors[i].reason));
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr) return kError;
	  (*sh)->length = l;
    ::memcpy((*sh)->data, _tango_error.errors[i].reason, l);
    (*lv_error_)->data[j++] = sh;
    //- desc --------------------------------------------------------------------
    sh = 0;
    l = static_cast<tbfl::size_t>(::strlen(_tango_error.errors[i].desc));
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr) return kError;
	  (*sh)->length = l;
    ::memcpy((*sh)->data, _tango_error.errors[i].desc, l);
    (*lv_error_)->data[j++] = sh;
    //- origin --------------------------------------------------------------------
    sh = 0;
    l = static_cast<tbfl::size_t>(::strlen(_tango_error.errors[i].origin));
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr) return kError;
	  (*sh)->length = l;
    ::memcpy((*sh)->data, _tango_error.errors[i].origin, l);
    (*lv_error_)->data[j++] = sh;
  }
  return kNoError;
}

//=============================================================================
// ErrorStack::devfailed                                    
//=============================================================================
void ErrorStack::devfailed (const Tango::DevFailed& _tango_error, std::string& err_str_)
{
  std::ostringstream oss;

  tbfl::uint32 nerr = _tango_error.errors.length();

  for (tbfl::size_t i = 0; i < nerr; i++) 
  {
    //- reason ------------------------------------------------------------------
    oss << _tango_error.errors[i].reason << ": ";
    //- desc --------------------------------------------------------------------
    oss << _tango_error.errors[i].desc << " [";
    //- origin --------------------------------------------------------------------
    oss << _tango_error.errors[i].origin << "]";
    if (i != nerr - 1)
    {
      oss << _tango_error.errors[i].origin << " - ";
    }
  }

  err_str_ = oss.str();
}

//=============================================================================
// ErrorStack::devfailed
//=============================================================================
Tango::DevFailed ErrorStack::devfailed (const std::string& err_txt, bool raise)
    throw (Tango::DevFailed)
{
  try
  {
    Tango::Except::throw_exception(_CPTC_("LABVIEW_DEVICE_ERROR"),
                                   _CPTC_(err_txt.c_str()),
                                   _CPTC_("LabVIEW Device Impl."));
  }
  catch (const Tango::DevFailed& df)
  {
    if ( raise )
    {
      throw df;
    }
    return df;
  }

  return Tango::DevFailed();
}

//=============================================================================
// ErrorStack::devfailed
//=============================================================================
Tango::DevFailed ErrorStack::devfailed (const ErrorStackVector& _lv_error, bool raise)
    throw (Tango::DevFailed)
{
  if ( ! _lv_error.size() )
  {
    try
    {
      Tango::Except::throw_exception(_CPTC_("LABVIEW_DEVICE_ERROR"),
                                     _CPTC_("sorry, no error details available!"),
                                     _CPTC_("LabVIEW Device Impl."));
    }
    catch (const Tango::DevFailed& df)
    {
      if ( raise )
      {
        throw df;
      }
      return df;
    }
  }
  
  std::string err_txt;
  
  for (tbfl::size_t i = 0; i < _lv_error.size(); i++)
  {
	err_txt += _lv_error[i] + " - ";
  }

  try
  {
    Tango::Except::throw_exception(_CPTC_("LABVIEW_DEVICE_ERROR"),
                                   _CPTC_(err_txt.c_str()),
                                   _CPTC_("LabVIEW Device Impl."));
  }
  catch (const Tango::DevFailed& df)
  {
    if ( raise )
    {
      throw df;
    }
    return df;
  }

  return Tango::DevFailed();
}

//=============================================================================
// ErrorStack::unknown
//=============================================================================
void ErrorStack::unknown (const char* o)
    throw (Tango::DevFailed)
{
  if ( ! o )
  {
    Tango::Except::throw_exception(_CPTC_("LABVIEW_DEVICE_ERROR"),
                                   _CPTC_("an error occurred but the LabVIEW device implementation did not provide any error details"),
                                   _CPTC_("LabVIEW Device Impl."));
  }
  
  Tango::Except::throw_exception(_CPTC_("LABVIEW_DEVICE_ERROR"), o, _CPTC_("LabVIEW Device Impl."));
}

