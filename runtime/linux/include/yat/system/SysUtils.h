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


#ifndef __YAT_SYS_UTILS_H__
#define __YAT_SYS_UTILS_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <sys/types.h>

namespace yat
{

// ============================================================================
//! \class SysUtils 
//! \brief Generic system utilities.
//!
//! This class provides plateform abstracted interface for the following functions:
//! - command line execution,
//! - environment variable access.
//===========================================================================
class YAT_DECL SysUtils
{
public:
  //! \brief Gets the value of an environment variable.
  //!
  //! Returns true if the environment variable has been evaluated, false otherwise.
  //! \remark If the environment variable is not defined and default value is set,
  //! the function sets \<strVar\> with default value.
  //! \param strVar Environment variable name.
  //! \param pstrValue Environment variable value.
  //! \param pszDef Default value.
  static bool get_env(const std::string &strVar, std::string *pstrValue, const char *pszDef=NULL);

  //! \brief Executes a command line.
  //! \param pszCmdLine The command line to execute.
  //! \param pszDefDir Current directory for the command line execution.
  //! \param bBackground If set to true, the command line is executed in background.
  //! \param bThrow If set to true, function throws an exception when an error occurs.
  //! \param pulReturnCode Value returned by the system on command line execution (plateform dependent).
  //! \exception SYSTEM_ERROR Thrown when quotes missing in command line (on Windows plateform 
  //! implementation).
  //! \exception ERR_FILE Thrown if command execution fails (on Windows plateform implementation).
  static bool exec(const char* pszCmdLine, const char *pszDefDir = NULL,
           int bBackground = true, bool bThrow = true, int* pulReturnCode = NULL);
           
  //! \brief Executes a command line using the identity of another account.
  //!
  //! \remark Does the same as SysUtils::exec() function on a Windows plateform.
  //! \param pszCmdLine The command line to execute.
  //! \param pszDefDir Current directory for the command line execution.
  //! \param bBackground If set to true, the command line is executed in background.
  //! \param bThrow If set to true, function throws an exception when an error occurs.
  //! \param pulReturnCode Value returned by the system on command line execution (plateform dependent).
  //! \param uid User id.
  //! \param gid Group id.
  //! \exception SYSTEM_ERROR Thrown when :
  //! - quotes miss in command line (on Windows plateform implementation),
  //! - shell execution fails (on Posix plateform implementation).
  //! \exception ERR_FILE Thrown if command execution fails (on Windows plateform implementation). 
  static bool exec_as(const char* pszCmdLine, const char *pszDefDir = NULL,
           int bBackground = true, bool bThrow = true, int* pulReturnCode = NULL, uid_t uid = -1, gid_t gid = -1);
};

} // namespace

#endif