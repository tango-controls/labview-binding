//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 The Tango Community
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (c) 2004-2016 The Tango Community
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

#ifndef _YAT4TANGO_EXCEPTION_HELPER_H_
#define _YAT4TANGO_EXCEPTION_HELPER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>
#include <yat/Exception.h>

//-----------------------------------------------------------------------------
// WINDOWS PRAGMA
//-----------------------------------------------------------------------------
#if defined (WIN32)
# pragma warning (disable : 4286)
#endif

#define _CPTC(X) static_cast<const char*>(X)

//=============================================================================
//! \brief THROW_DEVFAILED MACRO
//! 
//! \param p Exception's reason.
//! \param q Exception's description.
//! \param r Exception's origin.
//=============================================================================
#define THROW_DEVFAILED(p, q, r) \
  Tango::Except::throw_exception(_CPTC(p), _CPTC(q), _CPTC(r))

//=============================================================================
//! \brief RETHROW_DEVFAILED
//!
//! \param ex Exception to re-throw.
//! \param p Exception's reason.
//! \param q Exception's description.
//! \param r Exception's origin.
//=============================================================================
#define RETHROW_DEVFAILED(ex, p, q, r) \
  Tango::Except::re_throw_exception(ex, _CPTC(p), _CPTC(q), _CPTC(r))

//=============================================================================
//! \brief _YAT_TO_TANGO_EXCEPTION MACRO
//!
//! \param _yat_ex The yat exception.
//! \param _tango_ex The corresponding Tango exception.
//=============================================================================
#define _YAT_TO_TANGO_EXCEPTION(_yat_ex, _tango_ex) \
  const yat::Exception::ErrorList & ael = _yat_ex.errors; \
  Tango::DevErrorList tel(ael.size()); \
  tel.length(ael.size()); \
  for (size_t _ii = 0; _ii < ael.size(); _ii++) \
  { \
    tel[_ii].reason = CORBA::string_dup(ael[_ii].reason.c_str()); \
    tel[_ii].desc   = CORBA::string_dup(ael[_ii].desc.c_str()); \
    tel[_ii].origin = CORBA::string_dup(ael[_ii].origin.c_str()); \
    switch (ael[_ii].severity) \
    { \
      case yat::WARN: \
        tel[_ii].severity = Tango::WARN; \
        break; \
      case yat::PANIC: \
        tel[_ii].severity = Tango::PANIC; \
        break; \
      case yat::ERR: \
      default: \
        tel[_ii].severity = Tango::ERR; \
        break; \
    } \
  } \
  Tango::DevFailed _tango_ex(tel)

//=============================================================================
//! \brief THROW_YAT_TO_TANGO_EXCEPTION MACRO
//! 
//! \param _yat_ex The yat exception to throw in Tango format.
//=============================================================================
#define THROW_YAT_TO_TANGO_EXCEPTION(_yat_ex) \
  _YAT_TO_TANGO_EXCEPTION(_yat_ex, _tango_ex); \
  throw _tango_ex

//=============================================================================
//! \brief _HANDLE_YAT_EXCEPTION MACRO
//! 
//! \param _cmd The command which failed.
//! \param _origin The function where the command failed.
//=============================================================================
#define _HANDLE_YAT_EXCEPTION(_cmd, _origin) \
  catch (const yat::Exception& _yat_ex) \
  { \
    _YAT_TO_TANGO_EXCEPTION(_yat_ex, _tango_ex); \
    TangoSys_OMemStream r; \
    r << _cmd \
      << " failed"; \
    TangoSys_OMemStream d; \
    d << "YAT Exception caught while trying to execute " \
      << _cmd; \
    TangoSys_OMemStream o; \
    o << _origin \
      << " [" \
      << __FILE__ \
      << "::" \
      << __LINE__ \
      << "]"; \
    Tango::Except::re_throw_exception(_tango_ex, \
                                      _CPTC(r.str().c_str()), \
				                              _CPTC(d.str().c_str()), \
				                              _CPTC(o.str().c_str())); \
  } \
  catch (...) \
  { \
    TangoSys_OMemStream r; \
    r << _cmd \
      << " failed [UNKNOWN_ERROR]"; \
    TangoSys_OMemStream d; \
    d << "unknown exception caught while trying to execute " \
      << _cmd; \
    TangoSys_OMemStream o; \
    o << _origin \
      << " [" \
      << __FILE__ \
      << "::" \
      << __LINE__ \
      << "]"; \
    Tango::DevErrorList errors(1); \
    errors.length(1); \
    errors[0].severity = Tango::ERR; \
    errors[0].reason = CORBA::string_dup(r.str().c_str()); \
    errors[0].desc = CORBA::string_dup(d.str().c_str()); \
    errors[0].origin = CORBA::string_dup(o.str().c_str()); \
    Tango::DevFailed _tango_ex(errors); \
    throw _tango_ex; \
  }

//=============================================================================
//! \brief _TANGO_TO_YAT_EXCEPTION MACRO
//! 
//! \param _tango_ex The Tango exception.
//! \param _yat_ex The corresponding YAT exception.
//=============================================================================
#define _TANGO_TO_YAT_EXCEPTION(_tango_ex, _yat_ex) \
  const Tango::DevErrorList & tel = _tango_ex.errors; \
  yat::Exception::ErrorList yel(tel.length()); \
  for (size_t _ii = 0; _ii < yel.size(); _ii++) \
  { \
    yel[_ii].reason = std::string(tel[_ii].reason); \
    yel[_ii].desc   = std::string(tel[_ii].desc); \
    yel[_ii].origin = std::string(tel[_ii].origin); \
    switch (tel[_ii].severity) \
    { \
      case Tango::WARN: \
        yel[_ii].severity = yat::WARN; \
        break; \
      case Tango::PANIC: \
       yel[_ii].severity = yat::PANIC; \
        break; \
      case Tango::ERR: \
      default: \
        yel[_ii].severity = yat::ERR; \
        break; \
    } \
  } \
  yat::Exception _yat_ex; \
  _yat_ex.errors = yel;

//=============================================================================
//! \brief THROW_TANGO_TO_YAT_EXCEPTION MACRO
//! 
//! \param _dev_failed The Tango::DevFailed exception to throw as yat::Exception.
//=============================================================================
#define THROW_TANGO_TO_YAT_EXCEPTION(_dev_failed) \
  _TANGO_TO_YAT_EXCEPTION(_dev_failed, _yat_ex); \
  throw _yat_ex

namespace yat4tango
{
#ifdef YAT_WIN32
#  pragma warning( push )
#  pragma warning( disable: 4275 )
#endif

  // ============================================================================
  //! \class YATDevFailed 
  //! \brief A YAT to Tango exception class.
  //!
  //! This class provides a constructor that creates a Tango exception from a YAT
  //! exception.
  //!
  //! Inherits from Tango::DevFailed class.
  //!
  // ============================================================================
  class YAT4TANGO_DECL YATDevFailed : public Tango::DevFailed
  {
  public:
    //! \brief Creates a Tango exception from a YAT exception.
    //! \param ex The YAT exception.
    YATDevFailed( const yat::Exception& ex );
  };

  // ============================================================================
  //! \class TangoYATException 
  //! \brief A Tango to YAT exception class.
  //!
  //! This class provides a constructor that creates a YAT exception from a Tango
  //! exception.
  //!
  //! Inherits from Tango::DevFailed class.
  //!
  //! @notice DEPRECATED
  // ============================================================================
  class YAT4TANGO_DECL TangoYATException : public yat::Exception
  {
  public:
	  //! \brief Creates a YAT exception from a Tango exception.
	  //! \param df The Tango exception.
	  TangoYATException( const Tango::DevFailed& );
  };

#ifdef YAT_WIN32
#  pragma warning( pop )
#endif

} // namespace


#endif // _EXCEPTION_HELPER_H_



