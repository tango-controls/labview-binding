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

#ifndef __YAT_STRING_TEMPLATE_H__
#define __YAT_STRING_TEMPLATE_H__

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <list>
#include <set>

namespace yat
{
// ============================================================================
//! \class ISymbolInterpreter 
//! \brief Interface class providing template symbols interpreter.
//!
//! This abstract class can not be used as this and must be derived.
//! The goal of the value() function is to replace a symbol by its real value.
//! See yat::StringTemplate class.
// ============================================================================
class YAT_DECL ISymbolInterpreter
{
protected:
  //! \brief Constructor.
  ISymbolInterpreter() { }

public:
  //! \brief Attempts to evaluate a variable. 
  //!
  //! Returns true if template has been evaluated, false otherwise.
  //! \param[in,out] pstrSymbol Variable to evaluate, will contain the result.
  virtual bool value(std::string *pstrSymbol)=0;
};

// ============================================================================
//! \class StringTemplate 
//! \brief %String template processor.
//!
//! A StringTemplate object is a string that contains items which will be replaced by
//! their real value.
//! For instance: in the string 'date is \$\(date\)', '\$\(date\)' will be replaced
//! by the current date when processed. \n
//! The substitution will be realized by each symbol interpreter added in the template 
//! processor.
//! \remark The substitution function only looks for "$(xxx)" templates.
// ============================================================================
class YAT_DECL StringTemplate
{
public:
  //! \brief Substitution behavior in case the string is not found.
  enum NotFoundReplacement
  {
    //! Replace by an empty string.
    EMPTY_STRING,
    //! Keep symbol name.
    SYMBOL_NAME,
    //! Keep template.
    UNCHANGE_STRING
  };

private:
  std::list<ISymbolInterpreter *> m_lstInterpreter;
  NotFoundReplacement       m_eNotFoundReplacement;
  
  bool PrivProcess(std::string *pstrTemplate, bool bRecurse, std::set<std::string> &setEvaluatedSymbols);
  bool PrivProcessVar(std::string *pstrVar, bool bRecurse, bool bDeepEvaluation, std::set<std::string> &setEvaluatedSymbols);

public:
  //! \brief Constructor.
  //! \param eNotFoundReplacement %Template not found strategy.
  StringTemplate(NotFoundReplacement eNotFoundReplacement = SYMBOL_NAME) : m_eNotFoundReplacement(eNotFoundReplacement) {}

  //! \brief Adds a symbol interpreter.
  //! \param pInterpreter Symbol interpreter.
  void add_symbol_interpreter(ISymbolInterpreter *pInterpreter);

  //! \brief Removes a symbol interpreter.
  //! \param pInterpreter Symbol interpreter.
  void remove_symbol_interpreter(ISymbolInterpreter *pInterpreter);

  //! \brief Replaces a symbol by its real value.
  //!
  //! Returns true if evaluation is done, false otherwise.
  //! \param[in,out] pstrSymbol Symbol to evaluate, will contain the result.
  bool value(std::string *symbol);

  //! \brief Processes a template string.
  //!
  //! Evaluates the whole string. Every single template substitution is
  //! done by symbol interpreters.
  //! Returns true if evaluation is done, false otherwise.
  //! \param[in,out] pstrTemplate %String to evaluate, will contain the result.
  bool substitute(std::string *pstrTemplate);

  //! \deprecated
  bool value(String *pstrSymbol);

  //! \deprecated
  bool substitute(String *pstrTemplate);
};

// ============================================================================
//! \class EnvVariableInterpreter 
//! \brief A template symbol interpreter for environment variables.
//!
//! This class provides an evaluation function for system environment variables.
//! Inherits from ISymbolInterpreter class.
// ============================================================================
class YAT_DECL EnvVariableInterpreter : public ISymbolInterpreter
{
public:
  //! \brief Attempts to evaluate a variable. 
  //!
  //! Returns true if template has been evaluated, false otherwise.
  //! \param[in,out] pstrVar Variable to evaluate, will contain the result.
  virtual bool value(std::string *pstrVar);
};

} // namespace

#endif