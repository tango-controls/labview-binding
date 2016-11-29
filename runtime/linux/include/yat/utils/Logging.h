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

#ifndef __YAT_LOGGING_H__
#define __YAT_LOGGING_H__

#include <stack>
#include <set>
#include <sstream>
#include <yat/CommonHeader.h>
#include <yat/utils/String.h>

namespace yat
{

// =============================================================================
//! \brief Defines severity types using "syslog" levels definitions.
// =============================================================================
enum ELogLevel
{
  //! This is not an information message but a real result that must not be filtered.
  LOG_RESULT = 0,  
  //! To know every executed actions at low level.
  LOG_VERBOSE,   
  //! This is the default message level, it reports normal information about the system or the application.
  LOG_INFO,    
  //! Describes a (perhaps unusual) event that is important to report.
  LOG_NOTICE,   
  //! Reports a warning.
  LOG_WARNING,     
  //! An error occurred but the system is still functional.
  LOG_ERROR,       
  //! Critical error.
  LOG_CRITICAL,    
  //! Immediate fixing is needed.
  LOG_ALERT,       
  //! The system will shutdown now because of an unrecoverable failure.
  LOG_EMERGENCY,   
  //! Not a log level, just the total number of levels.
  LOG_LEVEL_COUNT  
};


// ============================================================================
//! \class ILogTarget 
//! \brief Log target interface.
//!
//! This is the base class for logging management.
//! This abstract class can not be used as this and must be derived.
//!
//! \par %Message logging usage: 
//! Just instantiate a LogManager and log messages using the yat::log_xxx() functions 
//! to log into the clog %stream.
//! \par
//! If a special log storage/presentation is necessary, then the things to do are:
//!  - implement the ILogTarget interface,
//!  - instantiate a LogCatcher object with a pointer to the ILogTarget object.
// ============================================================================
class YAT_DECL ILogTarget
{
public:
  //! \brief Logs message with specified level.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  virtual void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg)=0;
};

//! \brief Log target stack, for logging redirection.
typedef std::stack<class ILogTarget *> LogTargetStack;

// ============================================================================
//! \class DefaultLogHandler 
//! \brief Default log handler: prints log on console using clog %stream.
//!
//! Inherits from ILogTarget class.
// ============================================================================
class YAT_DECL DefaultLogHandler: public ILogTarget
{
public:
  //! \brief Logs a message on the clog %stream.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg);
};

// ============================================================================
//! \class LogManager 
//! \brief Log manager class.
//!
//! This class implements a log manager that provides logging functions with filter
//! capabilities (filter on message type, on message level). \n
//! This class is a singleton.
// ============================================================================
class YAT_DECL LogManager
{
friend class LogCatcher;
private:
  static LogManager *ms_pTheInstance;
  static LogManager *Instance();
  DefaultLogHandler  m_defLogHandler;

  //- Log target
  LogTargetStack     m_stkCatchLogTarget;

  //- Min severity level of logged messages
  int         m_iMinLevel;

  //- Logged messages types
  std::set<std::string>  m_setTypes;

  //- Add a new log target to the stack
  static void push_log_target(ILogTarget *pLogTarget);

  //- Remove top log target
  static void pop_log_target();

  //- Constructor
  LogManager();

public:

  //! \brief Initializes the log handler.
  //!
  //! The logged messages will be filtered with severity level and types.
  //! \param iMinLevel Minimum severity level.
  //! \param strFilter List (separator = '|') of message types used for message filtering.
  static void init(int iMinLevel, const std::string &strFilter= yat::StringUtil::empty);

  //! \brief Logs a message.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  static void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg);

  //! \brief Gets the minimum log level taken into account.
  static int min_level() { return Instance()->m_iMinLevel; }

  //! Returns the current log target.
  static ILogTarget *current_log_target();
};

//! \brief Log forwarding function type declaration.
typedef void (*pfn_log_fwd)(int iLevel, const char *pszType, const char *pszMsg);

// ============================================================================
//! \class LogForward 
//! \brief Helper class for log forwarding.
//!
//! This class provide an interface to forward messages logged towards another function.
//! This function is defined with the yat::pfn_log_fwd type.
//!
//! Inherits from ILogTarget class.
// ============================================================================
class YAT_DECL LogForward: public ILogTarget
{
private:
  //- Function to forward log to
  pfn_log_fwd m_pfn_log_fwd;

public:
  //! \brief Constructor.
  //! \param pfn_log_fwd Pointer to function to forward log to.
  LogForward(pfn_log_fwd pfn_log_fwd);

  //! \brief Logs a message.
  //! \param eLevel Log level.
  //! \param pszType %Message type.
  //! \param strMsg %Message to log.
  virtual void log(ELogLevel eLevel, pcsz pszType, const std::string &strMsg);
};

// ============================================================================
//! \class LogCatcher 
//! \brief Log catcher class.
//!
//! This class defines an object that catches log, during the LogCatcher life time.
//!
//! LogCatcher object are managed as following:
//! - when a LogCatcher object die, then the previous object still alive will receive messages;
//! - when the first created LogCatcher is deleted, messages are catched by the DefaultLogHandler.
//!
//! \par %Message logging usage: 
//! Just instantiate a LogManager and log messages using the yat::log_xxx() functions 
//! to log into the clog %stream.
//! \par
//! If a special log storage/presentation is necessary, then the things to do are:
//!  - implement the ILogTarget interface,
//!  - instantiate a LogCatcher object with a pointer to the ILogTarget object.
//!
// ============================================================================
class YAT_DECL LogCatcher
{
public:
  //! \brief Constructor.
  //! 
  //! Pushes the log target that catches log in ClogHandler stack.
  //! \param pLogTarget Log target.
  LogCatcher(ILogTarget *pLogTarget);

  //! \brief Destructor.
  //!
  //! Removes top log target from the stack.
  ~LogCatcher();
};

// =============================================================================
//! \name Log functions
//@{
// =============================================================================

//! \brief Logs a result message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_result(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs a verbose message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_verbose(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs an information message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_info(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs a notice message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_notice(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs a warning message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_warning(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs an error message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_error(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs a critical message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_critical(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs an alert message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_alert(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs an emergency message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_emergency(pcsz pszType, pcsz pszFormat, ...);

//! \brief Logs a result message.
//! \param msg Message
YAT_DECL void log_result(const std::string& msg);

//! \brief Logs a verbose message.
//! \param msg Message
YAT_DECL void log_verbose(const std::string& msg);

//! \brief Logs an information message.
//! \param msg Message
YAT_DECL void log_info(const std::string& msg);

//! \brief Logs a notice message.
//! \param msg Message
YAT_DECL void log_notice(const std::string& msg);

//! \brief Logs a warning message.
//! \param msg Message
YAT_DECL void log_warning(const std::string& msg);

//! \brief Logs an error message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_error(const std::string& msg);

//! \brief Logs a critical message.
//! \param pszType %Message type.
//! \param pszFormat %Message format (printf like).
//! \param ... %Message text.
YAT_DECL void log_critical(const std::string& msg);

//! \brief Logs an alert message.
//! \param msg Message
YAT_DECL void log_alert(const std::string& msg);

//! \brief Logs an emergency message.
//! \param msg Message
YAT_DECL void log_emergency(const std::string& msg);

//@} Log functions

} // namespace

#define LOG_EXCEPTION(domain, e) \
  do \
  { \
    for (size_t i = 0; i < e.errors.size(); i++) \
      yat::log_error(domain, "%s. %s. From %s.", PSZ(e.errors[i].reason), PSZ(e.errors[i].desc), PSZ(e.errors[i].origin)); \
  } while(0)

// Useful macros when not using messages type information
#define YAT_LOG_EXCEPTION(e)        LOG_EXCEPTION("exc", e)
#define YAT_LOG_VERBOSE(...)     yat::log_verbose("vbs", __VA_ARGS__)
#define YAT_LOG_RESULT(...)       yat::log_result("res", __VA_ARGS__)
#define YAT_LOG_INFO(...)           yat::log_info("inf", __VA_ARGS__)
#define YAT_LOG_NOTICE(...)       yat::log_notice("not", __VA_ARGS__)
#define YAT_LOG_WARNING(...)     yat::log_warning("wrn", __VA_ARGS__)
#define YAT_LOG_ERROR(...)         yat::log_error("err", __VA_ARGS__)
#define YAT_LOG_CRITICAL(...)   yat::log_critical("crt", __VA_ARGS__)
#define YAT_LOG_ALERT(...)         yat::log_alert("alt", __VA_ARGS__)
#define YAT_LOG_EMERGENCY(...) yat::log_emergency("emg", __VA_ARGS__)

//=============================================================================
// Macro for stream log functions
//=============================================================================
#define YAT_MSG_STREAM(level, type, s)                        \
do {                                                          \
  std::ostringstream oss;                                     \
  oss << s;                                                   \
  yat::LogManager::log(level, type, oss.str());               \
} while(0)

#define YAT_VERBOSE_STREAM(s)   YAT_MSG_STREAM(yat::LOG_VERBOSE, "vbs", s)
#define YAT_RESULT_STREAM(s)    YAT_MSG_STREAM(yat::LOG_RESULT, "res", s)
#define YAT_INFO_STREAM(s)      YAT_MSG_STREAM(yat::LOG_INFO, "inf", s)
#define YAT_WARNING_STREAM(s)   YAT_MSG_STREAM(yat::LOG_WARNING, "wrn", s)
#define YAT_NOTICE_STREAM(s)    YAT_MSG_STREAM(yat::LOG_NOTICE, "not", s)
#define YAT_ERROR_STREAM(s)     YAT_MSG_STREAM(yat::LOG_ERROR, "err", s)
#define YAT_CRITICAL_STREAM(s)  YAT_MSG_STREAM(yat::LOG_CRITICAL, "crt", s)
#define YAT_ALERT_STREAM(s)     YAT_MSG_STREAM(yat::LOG_ALERT, "alt", s)
#define YAT_EMERGENCY_STREAM(s) YAT_MSG_STREAM(yat::LOG_EMERGENCY, "emg", s)

#endif