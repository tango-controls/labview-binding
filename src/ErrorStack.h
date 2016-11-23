// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ErrorStack.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//=============================================================================
// Class ErrorStack [SINGLETON]
//=============================================================================
class ErrorStack
{
  friend class ObjectManager;

public:

  static ErrorStack* instance ();
  // Returns the unique instance of <ErrorStack>.

  int devfailed (const Tango::DevFailed& _tango_error, LvStringArrayHdl& lv_error_);
  // Tango to LabVIEW exception

  void devfailed(const Tango::DevFailed& _tango_error, std::string& err_str_);
  // Tango exception to string

  Tango::DevFailed devfailed (const std::string& _lv_error, bool raise = false)
    throw (Tango::DevFailed);
  // LabVIEW to Tango exception (throw or return)
    
  Tango::DevFailed devfailed (const ErrorStackVector& _lv_error, bool raise = false)
    throw (Tango::DevFailed);
  // LabVIEW to Tango exception (throw or return)

  Tango::DevFailed devfailed (LvStringArrayHdl& _lv_error, bool raise = false)
    throw (Tango::DevFailed);
  // LabVIEW to Tango exception (throw or return)

  int unknown (const char* o, LvStringArrayHdl& lv_error_);
  // Tango to LabVIEW exception

  void unknown (const char* o = NULL)
    throw (Tango::DevFailed);
  // LabVIEW to Tango exception
   
  int no_error (const char* o, LvStringArrayHdl& lv_error_);
  // Tango to LabVIEW exception
    
private:
  static int init ();
  //- Instanciates the singleton.
  
  static void cleanup ();
  //- Releases the singleton.

  static ErrorStack* instance_;
  //- The unique instance of <ErrorStack>.

  ErrorStack ();
  //- Ctor

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  ErrorStack (const ErrorStack&);
  virtual ~ErrorStack ();
  void operator= (const ErrorStack&); 
};

//=============================================================================
// MACROS 
//=============================================================================
#define ERROR_STACK ErrorStack::instance()

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "ErrorStack.i"
#endif 

