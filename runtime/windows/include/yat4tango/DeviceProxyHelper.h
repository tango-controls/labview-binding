//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (c) 2004-2016 Synchrotron SOLEIL
//
// The YAT4Tango library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation; either version 2 of the License, or (at 
// your option) any later version.
//
// The YAT4Tango library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// See COPYING file for license details  
//
// Contact:
//      Nicolas Leclercq
//      Synchrotron SOLEIL
//------------------------------------------------------------------------------
/*!
 * \authors See AUTHORS file
 */

#pragma once

//=============================================================================
// DEPENDENCIES
//=============================================================================
#if defined(WIN32)
  #include <windows.h>
#else
  #include <unistd.h>
#endif
#include <tango.h>
#include <yat/memory/SharedPtr.h>
#include <yat/threading/Thread.h>
#include <yat/threading/Pulser.h>
#include <yat4tango/ExceptionHelper.h>

//-----------------------------------------------------------------------------
// WINDOWS PRAGMA
//-----------------------------------------------------------------------------
#if defined (WIN32)
# pragma warning (disable : 4286)
#endif

//=============================================================================
// MISC MACROS
//=============================================================================
#define FULL_CMD_NAME(C) \
  device_proxy_->dev_name() + "::" + C
//---------------------------------------------------------------------------
#define FULL_ATTR_NAME(A) \
  device_proxy_->dev_name() + "/" + A

//-----------------------------------------------------------------------------
// DEFINE A SPECIAL VERSION OF TANGO ERROR_STREAM MACRO SO THAT FOLLOWING
// MACROS CAN BE SAFELY USED FROM C++ (PURE) CLIENT WHERE THERE IS NO
// INSTANCIATED LOGGER
//-----------------------------------------------------------------------------
#define _ERROR_STREAM \
  if (get_logger() && get_logger()->is_error_enabled()) \
    get_logger()->error_stream() << log4tango::LogInitiator::_begin_log

//=============================================================================
// _HANDLE_TANGO_DEVICE_EXCEPTION
//=============================================================================
#define _HANDLE_TANGO_DEVICE_EXCEPTION(_cmd, _origin) \
  catch (Tango::ConnectionFailed &e)  { \
     TangoSys_OMemStream d; \
    d << "Tango::ConnectionFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_CONNECTION_FAILED"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::CommunicationFailed &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::CommunicationFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_COMMUNICATION_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::WrongNameSyntax &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::WrongNameSyntax exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_WRONG_NAME_SYNTAX_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::NonDbDevice &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::NonDbDevice exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_NON_DB_DEVICE_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::WrongData &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::WrongData exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::NonSupportedFeature &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::NonSupportedFeature exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_NON_SUPPORTED_FEATURE_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::AsynCall &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::AsynCall exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_ASYNC_CALL_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::AsynReplyNotArrived &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::EventSystemFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_ASYNC_REPLY_NOT_ARRIVED_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::EventSystemFailed &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::EventSystemFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    Tango::Except::re_throw_exception(static_cast<Tango::DevFailed&>(e), \
            static_cast<const char*>("TANGO_EVENT_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (Tango::NamedDevFailedList &e) \
  { \
    size_t nb_faulty = static_cast<size_t>(e.get_faulty_attr_nb()); \
    for (size_t i = 0; i < nb_faulty; i++) \
    { \
      _ERROR_STREAM  << "failed to write attribute " << e.err_list[i].name << ":" << std::endl; \
      for (size_t j = 0; j < e.err_list[i].err_stack.length(); j++) \
      { \
        _ERROR_STREAM << "err[" << j << "].reason..." << e.err_list[i].err_stack[j].reason << std::endl; \
        _ERROR_STREAM << "err[" << j << "].desc....." << e.err_list[i].err_stack[j].desc << std::endl; \
        _ERROR_STREAM << "err[" << j << "].origin..." << e.err_list[i].err_stack[j].origin << std::endl; \
      } \
    } \
    throw; \
  } \
  catch (Tango::DevFailed &e) \
  { \
    TangoSys_OMemStream d; \
    d << "Tango::DevFailed exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    _ERROR_STREAM << e << std::endl; \
    Tango::Except::re_throw_exception(e, \
            static_cast<const char*>("TANGO_DEVICE_ERROR"), \
            static_cast<const char*>(d.str().c_str()), \
            static_cast<const char*>(o.str().c_str())); \
  } \
  catch (const std::bad_alloc &) \
  { \
    TangoSys_OMemStream d; \
    d << "std::bad_alloc exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    Tango::DevErrorList errors(1); \
    errors.length(1); \
    errors[0].severity = Tango::ERR; \
    errors[0].reason = CORBA::string_dup("OUT_OF_MEMORY"); \
    errors[0].desc = CORBA::string_dup(d.str().c_str()); \
    errors[0].origin = CORBA::string_dup(o.str().c_str()); \
    Tango::DevFailed df(errors); \
    _ERROR_STREAM << df << std::endl; \
    throw df; \
  } \
  catch (...) \
  { \
    TangoSys_OMemStream d; \
    d << "unknown exception caught while trying to execute " \
      << _cmd \
      << std::ends; \
    TangoSys_OMemStream o; \
    o << _origin << std::ends; \
    Tango::DevErrorList errors(1); \
    errors.length(1); \
    errors[0].severity = Tango::ERR; \
    errors[0].reason = CORBA::string_dup("UNKNOWN_ERROR"); \
    errors[0].desc = CORBA::string_dup(d.str().c_str()); \
    errors[0].origin = CORBA::string_dup(o.str().c_str()); \
    Tango::DevFailed df(errors); \
    _ERROR_STREAM << df << std::endl; \
    throw df; \
  }

//=============================================================================
// _MSEC_SLEEP MACRO
//============================================================================= 
#if defined(WIN32)
# define _MSEC_SLEEP(T) ::Sleep(T)
#else
# define _MSEC_SLEEP(T) ::usleep(1000 * T);
#endif

//=============================================================================
// _HANDLE_TANGO_DEVICE_RETRY_EXCEPTION
//=============================================================================  
#define _HANDLE_TANGO_DEVICE_RETRY_EXCEPTION(_cmd, _num_retries) \
  catch (Tango::ConnectionFailed &e) \
  { \
    if ( --_num_retries <= 0 ) \
      throw; \
    TangoSys_OMemStream d; \
    d << "the following Tango exception has been caught while trying to execute " \
      << _cmd \
      << ":" \
      << std::ends; \
    _ERROR_STREAM << d.str() << std::endl; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    _ERROR_STREAM << "retrying... [remaining attempts " << _num_retries << "]" << std::endl; \
    _MSEC_SLEEP(250); \
  } \
  catch (Tango::CommunicationFailed &e) \
  { \
    if ( --_num_retries <= 0 ) \
      throw; \
    TangoSys_OMemStream d; \
    d << "the following Tango exception has been caught while trying to execute " \
      << _cmd \
      << ":" \
      << std::ends; \
    _ERROR_STREAM << d.str() << std::endl; \
    _ERROR_STREAM << static_cast<const Tango::DevFailed&>(e) << std::endl; \
    _ERROR_STREAM << "retrying... [remaining attempts " << _num_retries << "]" << std::endl; \
    _MSEC_SLEEP(250); \
  } \
  catch (...) \
  { \
    throw; \
  }
  
//=============================================================================
// _TANGO_DEVICE_RETRY MACRO
//=============================================================================
#define _TANGO_DEVICE_RETRY(_invoke, _cmd, _origin, _num_retries) \
  try \
  { \
    size_t nr = _num_retries >= 0 ? static_cast<size_t>(_num_retries) : 1; \
    while ( nr ) \
    { \
      try  \
      {  \
        _invoke; \
        break; \
      }  \
      _HANDLE_TANGO_DEVICE_RETRY_EXCEPTION(_cmd, nr) \
    } \
  } \
  _HANDLE_TANGO_DEVICE_EXCEPTION(_cmd, _origin)
  
namespace yat4tango
{

//=============================================================================
// CLASS: HelperBase
//=============================================================================
class HelperBase : public Tango::LogAdapter
{
public:
  //---------------------------------------------------------------------------
  //  HelperBase::tango_proxy
  //  returns the underlying Tango device proxy
  //---------------------------------------------------------------------------
  inline Tango::DeviceProxy* tango_proxy () const
  {
    if ( ! device_proxy_ )
    {
      THROW_DEVFAILED("NULL_POINTER", 
                      "unexpected null pointer to Tango::DeviceProxy",
                      "DeviceProxyHelper::get_device_proxy");
    }
    
    return device_proxy_;
  }
  //---------------------------------------------------------------------------
  //  HelperBase::get_device_proxy
  //  returns the underlying Tango device proxy
  //  Deprecated, here for backward compatibility...
  //---------------------------------------------------------------------------
  inline Tango::DeviceProxy* get_device_proxy () const
  {
    if ( ! device_proxy_ )
    {
      THROW_DEVFAILED("NULL_POINTER", 
                      "unexpected null pointer to Tango::DeviceProxy",
                      "DeviceProxyHelper::get_device_proxy");
    }
    
    return device_proxy_;
  }
  //---------------------------------------------------------------------------
  //  HelperBase::operator->
  //  returns the underlying Tango device proxy
  //---------------------------------------------------------------------------
  inline Tango::DeviceProxy* operator-> ()
  {
    return device_proxy_;
  }

protected:
  //---------------------------------------------------------------------------
  //  HelperBase::HelperBase  (ctor)
  //  device_name : The name of the target device.
  //  client_device : Reference to the client device (for logging purpose).
  //---------------------------------------------------------------------------
  HelperBase (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
    : Tango::LogAdapter(client_device), 
      device_proxy_(0)
  {
    try
    {
      device_proxy_ = new Tango::DeviceProxy(const_cast<std::string&>(device_name));
    }
    catch (...)
    {
      delete device_proxy_; 
      device_proxy_ = 0;
      throw;
    }
  }
  //---------------------------------------------------------------------------
  // HelperBase::~HelperBase (dtor)
  //---------------------------------------------------------------------------
  virtual ~HelperBase ()
  {
    if ( device_proxy_ )
    {
      delete device_proxy_;
      device_proxy_ = 0;
    }
  }
  //---------------------------------------------------------------------------
  //- the underlying device
  //---------------------------------------------------------------------------
  Tango::DeviceProxy* device_proxy_;
};

//=============================================================================
// CLASS: CommandInOutHelper
//=============================================================================
class CommandInOutHelper : public virtual HelperBase
{
public:
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::CommandInOutHelper  (ctor)
  //  device_name : The name of the target device.
  //  client_device : Reference to the client device (for logging purpose).
  //---------------------------------------------------------------------------
  CommandInOutHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
    : HelperBase(device_name, client_device)
  {
    dd_out_.exceptions(Tango::DeviceData::isempty_flag | Tango::DeviceData::wrongtype_flag);
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::~CommandInOutHelper  (dtor)
  //---------------------------------------------------------------------------
  virtual ~CommandInOutHelper ()
  {
    //- noop dtor
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command
  //  exec a DEV_VOID/DEV_VOID TANGO command on the underlying device
  //  cmd_name : The name of the TANGO command
  //---------------------------------------------------------------------------
  void command (const std::string& cmd_name, int num_retries)
  {
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        device_proxy_->command_inout(const_cast<std::string&>(cmd_name)),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command",
        num_retries
      );
    }
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //---------------------------------------------------------------------------
  Tango::DeviceData command_inout (const std::string& cmd_name, Tango::DeviceData& argin, int num_retries)
  {
    Tango::DeviceData argout;
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        argout = device_proxy_->command_inout(const_cast<std::string&>(cmd_name), argin),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_inout",
        num_retries
      );
    }
    return argout;
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //  exec a ARG_OUT/ARG_IN TANGO command on the underlying device
  //  cmd_name : the name of the TANGO command
  //  argin : input argument
  //  argout : output argument
  //---------------------------------------------------------------------------
  //  template arg _IN must be supported by DeviceData::operator<<
  //  template arg _OUT must be supported by DeviceData::operator>>
  //---------------------------------------------------------------------------
  template <class _IN, class _OUT>
  void command_inout (const std::string& cmd_name, 
                      const _IN& argin, 
                      _OUT& argout,
                      int num_retries)
  {
    if ( device_proxy_ )
    {
      Tango::DeviceData dd_in;
      dd_in << const_cast<_IN&>(argin);
      _TANGO_DEVICE_RETRY
      (
        dd_out_ = device_proxy_->command_inout(const_cast<std::string&>(cmd_name), dd_in),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_inout",
        num_retries
      );
      _TANGO_DEVICE_RETRY
      (
        dd_out_ >> argout,
        std::string("DeviceData::operator>> on data returned by ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_inout",
        1
      );
    }
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //  exec a DEVVAR<X>STRINGARRAY/DEVVAR<X>STRINGARRAY command on the underlying device
  //  cmd_name : the name of the TANGO command
  //  _nv_in : numerical part of the input DEVVAR<X>STRINGARRAY
  //  _sv_in : string part of the input DEVVAR<X>STRINGARRAY
  //  _nv_out : numerical part of the ouput DEVVAR<X>STRINGARRAY
  //  _sv_out : string part of the ouput DEVVAR<X>STRINGARRAY
  //---------------------------------------------------------------------------
  //  template arg _IN must be supported by DeviceData::.insert
  //  template arg _OUT must be supported by DeviceData::.insert
  //---------------------------------------------------------------------------
  template <class _IN, class _OUT>
  void command_inout (const std::string& cmd_name,
                      const std::vector<_IN>& _nv_in,
                      const std::vector<std::string>& _sv_in,
                      std::vector<_OUT>& _nv_out,
                      std::vector<std::string>& _sv_out,
                      int num_retries)
  {
    if ( device_proxy_ )
    {
      Tango::DeviceData dd_in;
      dd_in.insert(const_cast<std::vector<_IN>&>(_nv_in), const_cast<std::vector<std::string>&>(_sv_in));
      _TANGO_DEVICE_RETRY
      (
        dd_out_ = device_proxy_->command_inout(const_cast<std::string&>(cmd_name), dd_in),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_inout",
        num_retries
      );
      _TANGO_DEVICE_RETRY
      (
        dd_out_.extract(_nv_out, _sv_out),
        std::string("DeviceData::extract on data returned by ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_inout",
        1
      );
    }
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //  Overloaded commands to  avoid usage of Tango::DevVarXX ARRAY for argout
  //---------------------------------------------------------------------------
  template <class _IN>
  void command_inout (const std::string&, 
                      const _IN&, 
                      Tango::DevVarDoubleStringArray*,
                      int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_inout"));
  }
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //  Overloaded commands to  avoid usage of Tango::DevVarXX ARRAY for argout
  //---------------------------------------------------------------------------
  template <class _IN>
  void command_inout (const std::string&,
                      const _IN&, 
                      Tango::DevVarLongStringArray*,
                      int num_retries) 
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_inout"));
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //  Overloaded commands to  avoid usage of Tango::DevVarXX ARRAY for argout
  //---------------------------------------------------------------------------
  template <class _IN>
  void command_inout (const std::string&, 
                      const _IN&, 
                      Tango::DevVarDoubleStringArray&,
                      int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_inout"));
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_inout
  //  Overloaded commands to  avoid usage of Tango::DevVarXX ARRAY for argout
  //---------------------------------------------------------------------------
  template <class _IN>
  void command_inout (const std::string&, 
                      const _IN&, 
                      Tango::DevVarLongStringArray&,
                      int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_inout"));
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_in
  //---------------------------------------------------------------------------
  void command_in (const std::string& cmd_name, const Tango::DeviceData& argin, int num_retries)
  {
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        device_proxy_->command_inout(const_cast<std::string&>(cmd_name), const_cast<Tango::DeviceData&>(argin)),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_in",
        num_retries
      );
    }
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_in
  //  exec a DEV_VOID/ARG_IN TANGO command on the underlying device
  //  cmd_name : the name of the TANGO command
  //  argin : input argument
  // dummy : used to have same number of parameter for the 2 command_in methods
  //---------------------------------------------------------------------------
  //  template arg _IN must be supported by DeviceData::operator<<
  //---------------------------------------------------------------------------
  template <class _IN>
  void command_in (const std::string& cmd_name, 
                   const _IN& argin,
                   int num_retries)
  {
    if ( device_proxy_ )
    {
      Tango::DeviceData dd_in;
      dd_in << const_cast<_IN&>(argin);
      _TANGO_DEVICE_RETRY
      (
        device_proxy_->command_inout(const_cast<std::string&>(cmd_name), dd_in),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_in",
        num_retries
      );
    }
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_in
  //  exec a DEV_VOID/DEVVAR<X>STRINGARRAY command on the underlying device
  //  cmd_name : the name of the TANGO command
  //  _nv_in   : numerical part of the input DEVVAR<X>STRINGARRAY
  //  _sv_in   : string part of the input DEVVAR<X>STRINGARRAY
  //---------------------------------------------------------------------------
  //  template arg _IN must be supported by DeviceData::.insert
  //---------------------------------------------------------------------------
  template <class _IN>
  void command_in (const std::string& cmd_name,
                   const std::vector<_IN>& _nv_in,
                   const std::vector<std::string>& _sv_in,
                   int num_retries)
  {
    if ( device_proxy_ )
    {
      Tango::DeviceData dd_in;
      dd_in.insert(const_cast<std::vector<_IN>&>(_nv_in), const_cast<std::vector<std::string>&>(_sv_in));
      _TANGO_DEVICE_RETRY
      (
        device_proxy_->command_inout(const_cast<std::string&>(cmd_name), dd_in),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_in",
        num_retries
      );
    }
  }

  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_out
  //---------------------------------------------------------------------------
  Tango::DeviceData command_out (const std::string& cmd_name,  int num_retries)
  {
    Tango::DeviceData argout;
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        argout = device_proxy_->command_inout(const_cast<std::string&>(cmd_name)),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_out",
        num_retries
      );
    }
    return argout;
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_out
  //  exec a ARG_OUT/DEV_VOID TANGO command on the underlying device
  //  cmd_name : the name of the TANGO command
  //  argout : output argument
  //---------------------------------------------------------------------------
  //  template arg _OUT must be supported by DeviceData::operator>>
  //---------------------------------------------------------------------------
  template <class _OUT>
  void command_out (const std::string& cmd_name, 
                    _OUT& argout,
                    int num_retries)
  {
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        dd_out_ = device_proxy_->command_inout(const_cast<std::string&>(cmd_name)),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_out",
        num_retries
      );
      _TANGO_DEVICE_RETRY
      (
        dd_out_ >> argout,
        std::string("DeviceData::operator>> on data returned by ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_out",
        1
      );
    }
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_out
  //  Overloaded commands to  avoid usage of Tango::DevVarDoubleStringArray ARRAY
  //---------------------------------------------------------------------------
  template <class _OUT>
  void command_out(_OUT, 
                   Tango::DevVarDoubleStringArray*,
                   int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_out"));
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_out
  //  Overloaded commands to  avoid usage of Tango::DevVarLongStringArray ARRAY
  //---------------------------------------------------------------------------
  template <class _OUT>
  void command_out (_OUT, 
                    Tango::DevVarLongStringArray*,
                    int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_out"));
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_out
  //  Overloaded commands to  avoid usage of Tango::DevVarDoubleStringArray ARRAY
  //---------------------------------------------------------------------------
  template <class _OUT>
  void command_out(_OUT, 
                   Tango::DevVarDoubleStringArray&,
                   int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_out"));
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_out
  //  Overloaded commands to  avoid usage of Tango::DevVarLongStringArray ARRAY
  //---------------------------------------------------------------------------
  template <class _OUT>
  void command_out (_OUT, 
                    Tango::DevVarLongStringArray&,
                    int num_retries)
  {
    Tango::Except::throw_exception(static_cast<const char*>("WRONG_DATA_TYPE_ERROR"),
                                   static_cast<const char*>("use only std::vector instead of Tango::DevVarXXStringArray"),
                                   static_cast<const char*>("bad usage of yat4tango::CommandInOutHelper::command_out"));
  }
  
  //---------------------------------------------------------------------------
  //  CommandInOutHelper::command_in
  //  exec a DEV_VOID/DEVVAR<X>STRINGARRAY command on the underlying device
  //  cmd_name : the name of the TANGO command
  //  _nv_out : numerical part of the output DEVVAR<X>STRINGARRAY
  //  _sv_out : string part of the output DEVVAR<X>STRINGARRAY
  //---------------------------------------------------------------------------
  //  template arg _OUT must be supported by DeviceData::.extract
  //---------------------------------------------------------------------------
  template <class _OUT>
  void command_out (const std::string& cmd_name,
                    std::vector<_OUT>& _nv_out,
                    std::vector<std::string>& _sv_out,
                    int num_retries)
  {
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        dd_out_ = device_proxy_->command_inout(const_cast<std::string&>(cmd_name)),
        std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_out",
        num_retries
      );
      _TANGO_DEVICE_RETRY
      (
        dd_out_.extract(_nv_out, _sv_out),
        std::string("DeviceData::extract on data returned by ") + FULL_CMD_NAME(cmd_name),
        "yat4tango::DeviceProxyHelper::command_out",
        1
      );
    }
  }

private:
  //- placed here as a workaround due to CORBA::any_var limitations
  Tango::DeviceData dd_out_;
};

//=============================================================================
// CLASS: AttributeHelper
//=============================================================================
class AttributeHelper : public virtual HelperBase
{
public:
  //---------------------------------------------------------------------------
  // snapshot of device writable attributes
  //---------------------------------------------------------------------------
  typedef std::vector<Tango::DeviceAttribute> Snapshot;
  typedef std::vector<Snapshot*> Snapshots;
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::AttributeHelper  (ctor)
  //  device_name : name of the target device
  //  client_device : reference to the client device (for logging purpose)
  //---------------------------------------------------------------------------
  AttributeHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
    : HelperBase(device_name, client_device)
  {
    m_da_out.exceptions(Tango::DeviceAttribute::isempty_flag | Tango::DeviceAttribute::wrongtype_flag);
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::~AttributeHelper (dtor)
  //---------------------------------------------------------------------------
  virtual ~AttributeHelper ()
  {
    cleanup();
  }

  //---------------------------------------------------------------------------
  //  AttributeHelper::get_device_attribute
  //---------------------------------------------------------------------------
  Tango::DeviceAttribute get_device_attribute ()
  {
    return m_da_out;
  }

  //---------------------------------------------------------------------------
  //  AttributeHelper::write_attribute
  //  writes the specified attribute
  //  attr_name : name of the TANGO attribute to be written
  //  attr_value : the attribute value
  //---------------------------------------------------------------------------
  //  template arg _VAL must be supported by DeviceAttribute::operator<<
  //---------------------------------------------------------------------------
  template <class _VAL>
  void write_attribute (const std::string& attr_name, 
                        const _VAL& attr_value, 
                        int num_retries)
  {
    if ( device_proxy_ )
    {
      Tango::DeviceAttribute da;
      da.set_name(const_cast<std::string&>(attr_name));
      da << const_cast<_VAL&>(attr_value);
      _TANGO_DEVICE_RETRY
      (
        device_proxy_->write_attribute(da),
        std::string("write_attribute on ") + FULL_ATTR_NAME(attr_name),
        "yat4tango::DeviceProxyHelper::write_attribute",
        num_retries
      );
    }
  }
  
	//---------------------------------------------------------------------------
	//  AttributeHelper::read_attribute
	//  reads the specified attribute
	//  attr_name : the name of the TANGO attribute to be read
	//  attr_value : the attribute value
	//---------------------------------------------------------------------------
	//  template arg _VAL must be supported by DeviceAttribute::operator>>
	//---------------------------------------------------------------------------
  template <class _VAL>
  void read_attribute (const std::string& attr_name, 
                       _VAL& attr_value, 
                       int num_retries)
  {
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        m_da_out = device_proxy_->read_attribute(const_cast<std::string&>(attr_name)),
        std::string("read_attribute on ") + FULL_ATTR_NAME(attr_name),
        "yat4tango::DeviceProxyHelper::read_attribute",
        num_retries
      );
      _TANGO_DEVICE_RETRY
      (
        m_da_out >> attr_value,
        std::string("DeviceAttribute::operator>> on data returned by ") + FULL_ATTR_NAME(attr_name),
        "yat4tango::DeviceProxyHelper::read_attribute",
        1
      );
    }
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::read_attribute
  //---------------------------------------------------------------------------
  Tango::DeviceAttribute read_attribute (const std::string& attr_name, int num_retries)
  {
    Tango::DeviceAttribute da;
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        da = device_proxy_->read_attribute(const_cast<std::string&>(attr_name)),
        std::string("read_attribute on ") + FULL_ATTR_NAME(attr_name),
        "yat4tango::DeviceProxyHelper::read_attribute",
        num_retries
      );
    }
    return da;
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::write_attribute
  //---------------------------------------------------------------------------
  void write_attribute (const Tango::DeviceAttribute& da, int num_retries)
  {
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        device_proxy_->write_attribute(const_cast<Tango::DeviceAttribute&>(da)),
        std::string("write_attribute on ") + FULL_ATTR_NAME(const_cast<Tango::DeviceAttribute&>(da).get_name()),
        "yat4tango::DeviceProxyHelper::read_attribute",
        num_retries
      );
    }
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::snapshot
  //---------------------------------------------------------------------------
  void snapshot (const std::string& attribute_name, bool throw_exception_on_error = true)
  {
    std::vector<std::string> al(1, attribute_name);
    snapshot(al, throw_exception_on_error);
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::snapshot
  //---------------------------------------------------------------------------
  void snapshot (const std::vector<std::string>& attributes_list, bool throw_exception_on_error = true)
  {
    if ( attributes_list.empty() )
      return;

    #define HANDLE_SNAPSHOT_CASE(T) \
      { \
        T* p; \
        if ( (*i).has_failed() || (*i).is_empty() ) \
        { \
          supported = false; \
        } \
        else \
        { \
          (*i) >> p; \
          p->length((*i).get_nb_read()); \
          (*i) << p; \
        } \
      }
      
    try
    {
      try
      {
        std::vector<Tango::DeviceAttribute> * vda = 
            tango_proxy()->read_attributes(const_cast<std::vector<std::string>&>(attributes_list));
       
        for ( std::vector<Tango::DeviceAttribute>::iterator i = vda->begin(); i != vda->end(); )
        {
          bool supported = true;
          switch ( (*i).get_type() )
          {
            case Tango::DEV_BOOLEAN:
            case Tango::DEVVAR_BOOLEANARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarBooleanArray);
                break;
            case Tango::DEV_UCHAR:
            case Tango::DEVVAR_CHARARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarCharArray);
                break;
            case Tango::DEV_SHORT:
            case Tango::DEVVAR_SHORTARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarShortArray);
                break;
            case Tango::DEV_USHORT:
            case Tango::DEVVAR_USHORTARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarUShortArray);
                break;
            case Tango::DEV_LONG:
            case Tango::DEVVAR_LONGARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarLongArray);
                break;
            case Tango::DEV_ULONG:
            case Tango::DEVVAR_ULONGARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarULongArray);
                break;
            case Tango::DEV_LONG64:
            case Tango::DEVVAR_LONG64ARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarLong64Array);
                break;
            case Tango::DEV_ULONG64:
            case Tango::DEVVAR_ULONG64ARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarULong64Array);
                break;
            case Tango::DEV_FLOAT:
            case Tango::DEVVAR_FLOATARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarFloatArray);
                break;
            case Tango::DEV_DOUBLE:
            case Tango::DEVVAR_DOUBLEARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarDoubleArray);
                break;
            case Tango::DEV_STRING:
            case Tango::DEVVAR_STRINGARRAY:
                HANDLE_SNAPSHOT_CASE(Tango::DevVarStringArray);
                break;
            default:
              supported = false;
              break;
          }
          if ( supported ) 
          {
            ++i;
          }
          else
          {
            i = vda->erase(i);
          }
        }
                     
        m_snapshots.push_back(vda);
      }
      _HANDLE_TANGO_DEVICE_EXCEPTION("snapshot", "yat4tango::DeviceProxyHelper::snapshot");
    }
    catch (...)
    {
      if ( throw_exception_on_error ) 
        throw;
    }
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::snapshot all writable attributes!
  //---------------------------------------------------------------------------
  void snapshot (bool throw_exception_on_error = true)
  {
    try
    { 
      try
      {
        //- get read all writable attribute
        std::vector<std::string> writables;
        yat::SharedPtr<Tango::AttributeInfoList> al = tango_proxy()->attribute_list_query();
        Tango::AttributeInfoList& v = *(al.get());
        for ( size_t i = 0; i < v.size(); i++ )
        {
          std::vector<std::string> writables;
          if ( v[i].writable != Tango::READ )
            writables.push_back(v[i].name);
        }
        snapshot(writables);
      }
      _HANDLE_TANGO_DEVICE_EXCEPTION("snapshot-all", "yat4tango::DeviceProxyHelper::snapshot");
    }
    catch (...)
    {
      if ( throw_exception_on_error ) 
        throw;
    }
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::return snapshoted value of the specified attribute
  //---------------------------------------------------------------------------
  Tango::DeviceAttribute& snapshoted_value (const std::string& attribute_name)
  {
    for ( size_t i = 0; i < m_snapshots.size(); i++ )
    {
      Snapshot& s = *m_snapshots[i];
      for  ( size_t j = 0; j < s.size(); j++ )
      {
        if ( s[j].get_name() == attribute_name ) 
          return s[j];
      }
    }
    yat::OSStream oss;
    oss << "could not found any snapshot for attribute '" 
        << attribute_name
        << "'";
    THROW_DEVFAILED("ATTRIBUTE_NOT_FOUND", 
                    oss.str().c_str(), 
                    "DeviceProxyHelper::snapshoted_value");
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::restore previously snapshoted attributes
  //---------------------------------------------------------------------------
  void restore (bool throw_exception_on_error = true)
  {
    if ( m_snapshots.empty() )
      return;
    
    try
    {
      try
      {
        for ( size_t i = 0; i < m_snapshots.size(); i++ )
          tango_proxy()->write_attributes(*m_snapshots[i]);
      }
      _HANDLE_TANGO_DEVICE_EXCEPTION("restore-snapshot", "yat4tango::DeviceProxyHelper::restore");
    }
    catch (...)
    {
      if ( throw_exception_on_error ) 
        throw;
    }
  }
  
  //---------------------------------------------------------------------------
  //  AttributeHelper::reset_snapshot
  //---------------------------------------------------------------------------
  void reset_snapshot ()
  {
    cleanup();
  }

private:
  //- avoid memory leaks...
  void cleanup ()
  {
    for ( size_t i = 0; i < m_snapshots.size(); i++ )
      delete m_snapshots[i];
    m_snapshots.clear();
  }
  
  //- placed here as a workaround due to CORBA::any_var limitations
  Tango::DeviceAttribute m_da_out;
  
  //- device snapshot  
  Snapshots m_snapshots;
};

//=============================================================================
// CLASS: PolledStateHelper
//=============================================================================
struct DeviceProxyHelperStateNotification
{
  std::string     device_name;
  Tango::DevState device_state;
};

// Callback definition
YAT_DEFINE_CALLBACK(DeviceProxyHelperStateCallback, DeviceProxyHelperStateNotification);

class PolledStateHelper : public virtual HelperBase
{
public:
  PolledStateHelper( const std::string& device_name, Tango::DeviceImpl *client_device = 0 )
    : HelperBase(device_name, client_device)
  {
  }

  //! @brief Asynchronously wait for state (or any other state)
  //! @param state wait state 
  //! @param cb notification callback
  void async_wait_state( Tango::DevState state, DeviceProxyHelperStateCallback cb, std::size_t period_ms = 100 )
  {
    config_ptr_ = new Config;
    config_ptr_->state = state;
    config_ptr_->cb = cb;
    config_ptr_->period_ms = period_ms;
    start_pulser();
  }

  //! @brief Asynchronously wait for state (or any other state)
  //! @param state wait state 
  //! @param task_p task to notify
  //! @param msg_type message type to post to the task
  void async_wait_state( Tango::DevState state, yat::Task* task_p, std::size_t msg_type, std::size_t period_ms = 100 )
  {
    config_ptr_ = new Config;
    config_ptr_->state = state;
    config_ptr_->task_p = task_p;
    config_ptr_->msg_type = msg_type;
    config_ptr_->period_ms = period_ms;
    start_pulser();
  }

  //! Synchronously wait for state (or any other state)
  //! @param state wait state 
  void sync_wait_state( Tango::DevState state, std::size_t tmo_sec, std::size_t period_ms = 100 )
  {
    priv_sync_wait_state(state, tmo_sec, period_ms, false);
  }

  //! @brief Asynchronously wait for any state different than the given state
  //! @param state wait state 
  //! @param cb notification callback
  void async_wait_end_state( Tango::DevState state, DeviceProxyHelperStateCallback cb, std::size_t period_ms = 100 )
  {
    config_ptr_ = new Config;
    config_ptr_->state = state;
    config_ptr_->cb = cb;
    config_ptr_->wait_end = true;
    config_ptr_->period_ms = period_ms;
    start_pulser();
  }

  //! @brief Asynchronously wait for any state different than the given state
  //! @param state wait state 
  //! @param task_p task to notify
  //! @param msg_type message type to post to the task
  void async_wait_end_state( Tango::DevState state, yat::Task* task_p, std::size_t msg_type, std::size_t period_ms = 100 )
  {
    config_ptr_ = new Config;
    config_ptr_->state = state;
    config_ptr_->task_p = task_p;
    config_ptr_->msg_type = msg_type;
    config_ptr_->wait_end = true;
    config_ptr_->period_ms = period_ms;
    start_pulser();
  }

  //! Synchronously wait for any state different than the given state
  //! @param state wait state 
  void sync_wait_end_state( Tango::DevState state, std::size_t tmo_sec, std::size_t period_ms = 100 )
  {
    priv_sync_wait_state(state, tmo_sec, period_ms, true);
  }

private:
  struct Config
  {
    Tango::DevState                state;
    bool                           wait_end;
    DeviceProxyHelperStateCallback cb;
    yat::Task*                     task_p;
    std::size_t                    msg_type;
    std::size_t                    period_ms;

    Config() : state(Tango::UNKNOWN), wait_end(false), task_p(0), msg_type(0) {}
  };
  
  //---------------------------------------------------------------------------
  //  PolledStateHelper::priv_sync_wait_state
  //---------------------------------------------------------------------------
  void priv_sync_wait_state( Tango::DevState state, std::size_t tmo_sec, std::size_t period_ms, bool wait_end )
  {
    Tango::DevState ds = Tango::UNKNOWN;
    yat::Timeout tmo(tmo_sec, yat::Timeout::TMO_UNIT_SEC, true);

    while( true )
    {
      if ( tmo.expired() )
      {
        yat::OSStream oss;
        oss << "timeout expired while waiting for device "
            << device_proxy_->dev_name()
            << " to switch to the expected Tango device state";
        THROW_YAT_ERROR("DEVICE_ERROR", oss.str().c_str(), "PolledStateHelper::sync_wait_state");
      }      
      _TANGO_DEVICE_RETRY
      (
        //- try
        ds = device_proxy_->state(),
        FULL_CMD_NAME("State"),
        "PolledStateHelper::sync_wait_state",
        2 // retry 2 times in case of failure
      );

      if( (state == ds && !wait_end) || (state != ds && wait_end) )
        break;

      yat::Thread::sleep(period_ms);
    }
  }

  //---------------------------------------------------------------------------
  //  PolledStateHelper::pulser_cb
  //---------------------------------------------------------------------------
  void pulser_cb(yat::Thread::IOArg)
  {
    Tango::DevState ds = Tango::UNKNOWN;
    if ( device_proxy_ )
    {
      _TANGO_DEVICE_RETRY
      (
        //- try
        ds = device_proxy_->state(),
        FULL_CMD_NAME("State"),
        "DeviceProxyHelper::state",
        2
      );
    }

    if( config_ptr_->state == ds && !config_ptr_->wait_end )
    {
      if( config_ptr_->task_p && config_ptr_->msg_type >= yat::FIRST_USER_MSG )
      {
        yat::Message* msg_p = new yat::Message(config_ptr_->msg_type);
        msg_p->attach_data( notification(ds) );
        config_ptr_->task_p->post(msg_p);
      }
      else if( !config_ptr_->cb.is_empty() )
        config_ptr_->cb( notification(ds) );

      pulser_ptr_->stop();
      config_ptr_.reset();
    }
  }
  
  //---------------------------------------------------------------------------
  //  PolledStateHelper::notification
  //---------------------------------------------------------------------------
  DeviceProxyHelperStateNotification notification(Tango::DevState ds)
  {
    DeviceProxyHelperStateNotification notif;
    notif.device_name = device_proxy_->dev_name();
    notif.device_state = ds;
    return notif;
  }

  //---------------------------------------------------------------------------
  //  PolledStateHelper::start_pulser
  //---------------------------------------------------------------------------
  void start_pulser()
  {
    yat::Pulser::Config cfg;
    cfg.period_in_msecs = config_ptr_->period_ms;
    cfg.callback = yat::PulserCallback::instanciate(*this, &PolledStateHelper::pulser_cb);
    pulser_ptr_ = new yat::Pulser(cfg);
    pulser_ptr_->start();
  }

  yat::SharedPtr<yat::Pulser> pulser_ptr_;
  yat::SharedPtr<Config>      config_ptr_;
};

//=============================================================================
// CLASS: DeviceProxyHelper
//=============================================================================
class DeviceProxyHelper : public CommandInOutHelper, public AttributeHelper, public PolledStateHelper
{
public:
    //---------------------------------------------------------------------------
    //  DeviceProxyHelper::DeviceProxyHelper  (ctor)
    //  device_name : name of the target device
    //  client_device : reference to the client device (for logging purpose)
    //---------------------------------------------------------------------------
    DeviceProxyHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
      : HelperBase(device_name, client_device),
        CommandInOutHelper(device_name, client_device),
        AttributeHelper(device_name, client_device),
        PolledStateHelper(device_name, client_device)
    {
      //- noop ctor
    }
    
    //---------------------------------------------------------------------------
    //  DeviceProxyHelper::~DeviceProxyHelper (dtor)
    //---------------------------------------------------------------------------
    virtual ~DeviceProxyHelper ()
    {
      //- noop dtor
    }
    
    //---------------------------------------------------------------------------
    //  DeviceProxyHelper::ping
    //---------------------------------------------------------------------------
    int ping (int num_retries)
    {
      int rtt = 0; 
      if ( device_proxy_ )
      {
        _TANGO_DEVICE_RETRY
        (
          //- try
          rtt = device_proxy_->ping(),
          FULL_CMD_NAME("ping"),
          "DeviceProxyHelper::ping",
          num_retries
        );
      }
      return rtt;
    }
    
    //---------------------------------------------------------------------------
    //  DeviceProxyHelper::state
    //---------------------------------------------------------------------------
    Tango::DevState state (int num_retries)
    {
      Tango::DevState ds = Tango::UNKNOWN;
      if ( device_proxy_ )
      {
        _TANGO_DEVICE_RETRY
        (
          //- try
          ds = device_proxy_->state(),
          FULL_CMD_NAME("State"),
          "DeviceProxyHelper::state",
          num_retries
        );
      }
      return ds;
    }
    
    //---------------------------------------------------------------------------
    //  DeviceProxyHelper::status
    //---------------------------------------------------------------------------
    std::string status (int num_retries)
    {
      std::string  ds = "UNKNOWN";
      if ( device_proxy_ )
      {
        _TANGO_DEVICE_RETRY
        (
          ds = device_proxy_->status(),
          FULL_CMD_NAME("Status"),
          "DeviceProxyHelper::status",
          num_retries
        );
      }
      return ds;
    }
    
  };
  
} //- namespace tango

