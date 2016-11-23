//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2014 The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
// The YAT library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
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
 * \author See AUTHORS file
 */

#ifndef _YAT_EXCEPTION_H_
#define _YAT_EXCEPTION_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <string>
#include <vector>
#include <yat/CommonHeader.h>

namespace yat 
{

#define THROW_YAT_ERROR( reason, desc, origin ) \
  throw yat::Exception( (reason), (desc), (origin) )

#define RETHROW_YAT_ERROR( exception, reason, desc, origin ) \
  { \
    exception.push_error( (reason), (desc), (origin) ); \
    throw exception; \
  }

//! \brief Macros set used to ensure all kind of exceptions will be catched and
//! \brief when one want to not stop at the 1st exception when one have to execute a set of
//! \brief statements whatever the exceptions those statements may throw
//! \verbatim
//!
//! YAT_INIT_TRY_CATCH
//! YAT_TRY( statement 1 )
//! YAT_TRY( statement 2 )
//! ...
//! YAT_TRY_WITH_MSG( statement i, "error message", "error origin" )
//! ...
//! YAT_TRY( statement n )
//! YAT_THROW
//!
//! \endverbatim
//!

//---------------------------------
//! YAT_INIT_TRY_CATCH
//! \brief Initialize try..catch..throw sequence
//---------------------------------
#define YAT_INIT_TRY_CATCH \
  yat::Exception _the_yat_exception_;

#define __YAT_CATCH_YAT__ \
    catch( yat::Exception& e ) \
    { _the_yat_exception_.errors.insert( _the_yat_exception_.errors.end(), e.errors.begin(), e.errors.end() ); }

#define __YAT_CATCH_STD__ \
    catch( std::exception& e ) \
    { _the_yat_exception_.push_error("ERROR", PSZ_FMT("Standard system error occured: %s", e.what()), "stdlib"); } \

#define __YAT_CATCH_OTHER__ \
    catch( ... ) \
    { _the_yat_exception_.push_error("ERROR", "!!!Unknown error occured!!!", "Unknown"); }

//---------------------------------
//! YAT_TRY
//! \brief try a statement and catch exceptions
//---------------------------------
#define YAT_TRY( statement ) \
  do \
  { \
    try  { statement; } \
    __YAT_CATCH_YAT__ \
    __YAT_CATCH_STD__ \
    __YAT_CATCH_OTHER__ \
  } while(0)

#define __YAT_CATCH_YAT_WITH_MSG__( reason, msg, org ) \
    catch( yat::Exception& e ) \
    { \
      _the_yat_exception_.errors.insert( _the_yat_exception_.errors.end(), e.errors.begin(), e.errors.end() ); \
      _the_yat_exception_.push_error(reason, msg, org); \
    }

#define __YAT_CATCH_STD_WITH_MSG__( reason, msg, org ) \
    catch( std::exception& e ) \
    { \
      _the_yat_exception_.push_error("ERROR", PSZ_FMT("Standard system error occured: %s", e.what()), "stdlib"); \
      _the_yat_exception_.push_error(reason, msg, org); \
    }
    
#define __YAT_CATCH_OTHER_WITH_MSG__( reason, msg, org ) \
    catch( ... ) \
    { \
      _the_yat_exception_.push_error("ERROR", "!!!Unknown error occured!!!", "Unknown"); \
      _the_yat_exception_.push_error(reason, msg, org); \
    }

//---------------------------------
//! YAT_TRY_WITH_MSG
//! \brief try a statement, catch exceptions and add an error message if needed
//---------------------------------
#define YAT_TRY_WITH_MSG( statement, reason, msg, org ) \
  do \
  { \
    try  { statement; } \
    __YAT_CATCH_YAT_WITH_MSG__( reason, msg, org ) \
    __YAT_CATCH_STD_WITH_MSG__( reason, msg, org ) \
    __YAT_CATCH_OTHER_WITH_MSG__( reason, msg, org ) \
  } while(0)

//---------------------------------
//! YAT_THROW
//! \brief throw the yat::Exception to finish the try..catch..throw sequence
//---------------------------------
#define YAT_THROW \
  do \
  { \
    if( !_the_yat_exception_.errors.empty() ) { throw _the_yat_exception_; } \
  } while (0)

//---------------------------------
//! YAT_THROW_WITH_MSG
//! \brief throw the yat::Exception with a new message to finish the try..catch..throw sequence
//---------------------------------
#define YAT_THROW_WITH_MSG( reason, msg, org ) \
  do \
  { \
    if( !_the_yat_exception_.errors.empty() ) \
    { \
       _the_yat_exception_.push_error(reason, msg, org); \
      throw _the_yat_exception_; \
    } \
  } while (0)

//---------------------------------
//! YAT_TRY_CATCH_THROW
//! \brief Macro that try a single statement and transform all kind of exceptions
//! \brief into a yat::Exception then throw it
//---------------------------------
#define __YAT_CATCH_THROW__  catch( yat::Exception& e ) { throw e; }

#define __YAT_CATCH_THROW_STD__ \
    catch( std::exception& e ) \
    { THROW_YAT_ERROR("ERROR", PSZ_FMT("Standard system error occured: %s", e.what()), "stdlib"); } \

#define __YAT_CATCH_THROW_OTHER__ \
    catch( ... ) \
    { THROW_YAT_ERROR("ERROR", "!!!Unknown error occured!!!", "Unknown origin"); }

#define YAT_TRY_CATCH_THROW( statement ) \
  do \
  { \
    try  { statement; } \
    __YAT_CATCH_THROW__ \
    __YAT_CATCH_THROW_STD__ \
    __YAT_CATCH_THROW_STD__ \
  } while(0)

//---------------------------------
//! YAT_TRY_CATCH_THROW_WITH_MSG
//! \brief Macro that try a single statement and transform all kind of exceptions 
//! \brief into a yat::Exception then throw it with a new error message on top level
//---------------------------------
#define __YAT_CATCH_THROW_WITH_MSG__(reason, msg, org) \
  catch( yat::Exception& e ) { RETHROW_YAT_ERROR( e, reason, msg, org ); }

#define __YAT_CATCH_THROW_STD_WITH_MSG__( reason, msg, org ) \
  catch( std::exception& e ) \
  { \
    yat::Exception ex; \
    ex.push_error( "ERROR", PSZ_FMT("Standard system error occured: %s", e.what()), "stdlib" ); \
    ex.push_error( reason, msg, org ); \
    throw ex; \
  } \

#define __YAT_CATCH_THROW_OTHER_WITH_MSG__( reason, msg, org ) \
  catch( ... ) \
  { \
    yat::Exception ex; \
    ex.push_error( "ERROR", "!!!Unknown error occured!!!", "Unknown origin" ); \
    ex.push_error( reason, msg, org ); \
    throw ex; \
  }
  
#define YAT_TRY_CATCH_THROW_WITH_MSG( statement, reason, msg, org ) \
  do \
  { \
    try  { statement; } \
    __YAT_CATCH_THROW_WITH_MSG__( reason, msg, org ) \
    __YAT_CATCH_THROW_STD_WITH_MSG__( reason, msg, org ) \
    __YAT_CATCH_THROW_OTHER_WITH_MSG__( reason, msg, org ) \
  } while(0)

  // ============================================================================
  //! \brief Error severity.
  // ============================================================================
  typedef enum {
    //! Warning.
    WARN, 
    //! Functional error.
    ERR, 
    //! Fatal error.
    PANIC
  } ErrorSeverity;

  // ============================================================================
  //! \class Error 
  //! \brief Yat error class.
  //!
  //! The Yat error class provides an implementation of an application error. 
  //! This application error is defined with the following features:
  //! - reason: gives the error type. The goal of this text is to guide the 
  //! \b operator towards the functional cause of the problem.
  //!
  //! - description: describes the error. The goal of this text is to guide the
  //! <b>system expert</b> towards the cause of the problem.
  //!
  //! - origin: gives the function name where the error occurred. The goal of this
  //! text is to guide the \b developper towards the programming error.
  //!
  //! - code: gives the error code. The goal of this number is to identify the error.
  //! - severity: gives the error severity (from yat::ErrorSeverity).
  // ============================================================================
  struct YAT_DECL Error
  {
    //! \brief Default constructor.
    //!
    //! The default values are :
    //! - reason = "unknown"
    //! - desc = "unknown error"
    //! - origin = "unknown"
    //! - code = -1
    //! - severity = yat::ERR
    Error ();

    //! \brief Constructor with parameters.
    //! \param reason %Error reason.
    //! \param desc %Error description.
    //! \param origin %Error origin.
    //! \param err_code %Error id.
    //! \param severity %Error severity.
    Error ( const char *reason,
            const char *desc,
            const char *origin,
            int err_code = -1, 
            int severity = yat::ERR);

    //! \brief Constructor with parameters.
    //! \param reason %Error reason.
    //! \param desc %Error description.
    //! \param origin %Error origin.
    //! \param err_code %Error id.
    //! \param severity %Error severity.    
    Error ( const std::string& reason,
            const std::string& desc,
            const std::string& origin, 
            int err_code = -1, 
            int severity = yat::ERR);

    //! \brief Copy constructor. 
    //! \param src The source error.
    Error (const Error& src);

    //!\brief Destructor.
    virtual ~Error ();

    //! \brief operator=.
    //! \param _src The source error.
    Error& operator= (const Error& _src);

    //! \brief %Error details: reason.
    std::string reason;

    //! \brief %Error details: description.
    std::string desc;

    //! \brief %Error details: origin.
    std::string origin;

    //! \brief %Error details: code.
    int code;

    //! \brief %Error details: severity.
    int severity;
  };

  // ============================================================================
  //! \class Exception 
  //! \brief Yat exception class.
  //!
  //! The Yat exception class provides an implementation of an application exception. 
  //! This application exception contains a list of yat::Error errors.
  // ============================================================================
  class YAT_DECL Exception
  {
  public:

    //! \brief List of errors.
    typedef std::vector<Error> ErrorList;

    //! \brief Default constructor.
    Exception ();

    //! \brief Constructor from an application error.
    //! \param reason %Error reason.
    //! \param desc %Error description.
    //! \param origin %Error origin.
    //! \param err_code %Error code.
    //! \param severity %Error severity.
    Exception ( const char *reason,
                const char *desc,
                const char *origin,
                int err_code = -1, 
                int severity = yat::ERR);

    //! \brief Constructor from an application error.
    //! \param reason %Error reason.
    //! \param desc %Error description.
    //! \param origin %Error origin.
    //! \param err_code %Error code.
    //! \param severity %Error severity.
    Exception ( const std::string& reason,
                const std::string& desc,
                const std::string& origin, 
                int err_code = -1, 
                int severity = yat::ERR);

    //! \brief Constructor from an application error.
    //! \param error Error.
    Exception (const Error& error);

    //! \brief Copy constructor.
    //! \param src The source exception.
    Exception (const Exception& src);

    //! \brief operator=.
    //! \param _src The source exception.
    Exception& operator= (const Exception& _src); 

    //! \brief Destructor.
    virtual ~Exception ();

    //! \brief Pushes the specified error into the error list.
    //! \param reason %Error reason.
    //! \param desc %Error description.
    //! \param origin %Error origin.
    //! \param err_code %Error code.
    //! \param severity %Error severity.    
    void push_error ( const char *reason,
                      const char *desc,
                      const char *origin, 
                      int err_code = -1, 
                      int severity = yat::ERR);

    //! \brief Pushes the specified error into the error list.
    //! \param reason %Error reason.
    //! \param desc %Error description.
    //! \param origin %Error origin.
    //! \param err_code %Error code.
    //! \param severity %Error severity.    
    void push_error ( const std::string& reason,
                      const std::string& desc,
                      const std::string& origin, 
                      int err_code = -1, 
                      int severity = yat::ERR);

    //! \brief Pushes the specified error into the error list.
    //! \param error Error.
    void push_error (const Error& error);

    //! \brief Concatenates the whole exception stack as a single string then return it
    std::string to_string () const;
 
    //! \brief Dumps the content of the exception's error list towards standard output.
    virtual void dump () const;

    //! \brief The error list.
    ErrorList errors;
  };

} // namespace

/*
#if defined (YAT_INLINE_IMPL)
# include <yat/Exception.i>
#endif // YAT_INLINE_IMPL
*/

#endif // _EXCEPTION_H_

