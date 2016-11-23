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

// ============================================================================
/*! \page DynCmdPage Dynamic Command Example

  \verbatim

  class MyClass : public Tango::LogAdapter
  {
    public:
      //------------------------------------------------------------------
      MyClass (Tango::DeviceImpl * d)
        : Tango::LogAdapter(d), m_dcm (d)
      {
        this->init_dynamic_commands();
      }
      
      //------------------------------------------------------------------
      ~MyClass ()
      {
        this->remove_dynamic_commands();
      }
      
      //------------------------------------------------------------------
      void init_dynamic_commands ()
      {
        //- this method must be executed by a Tango thread!
        //-------------------------------------------------
        yat4tango::DynamicCommandInfo dci;
        dci.dev = a pointer to the Tango::DeviceImpl to which the dyn. attr belongs;
        //- cmd name
        dci.tci.cmd_name = "DynCmdVoid";
        //- cmd input & output arguments data type
        dci.tci.in_type  = Tango::DEV_ULONG;
        dci.tci.out_type = Tango::DEV_VARDOUBLEARRAY;
        //- cmd diplay level
        dai.tai.disp_level = Tango::OPERATOR;
        //- execute callback
        dci.ecb = yat4tango::DynamicCommandExecuteCallback::instanciate(*this,
                                                                        &MyClass::execute_callback);
        //- add the dynamic command to the device interface
        this->m_dcm.add_command(dci);
      }
      
      //------------------------------------------------------------------
      void remove_dynamic_commands ()
      {
        //- this method must be executed by a Tango thread!
        //-------------------------------------------------
        this->m_dcm.remove_commands();
      }
      
      //------------------------------------------------------------------
      void execute_callback (yat4tango::DynamicCommandExecuteCallbackData& cbd)
      {
        DEBUG_STREAM << "execute request for dynamic command: "
                      << cbd.dyc->get_name()
                      << std::endl;
                      
        //- extract argin
        Tango::DevULong argin;
        cbd.dyc->get_argin(argin);
        
        //- do some job (let's suppose it also allocate argout)
        Tango::DevVarDoubleArray * argout = this->do_some_job(argin);
        
        //- insert argout
        cbd.dyc->get_argout(argout);
      }

    private:
      yat4tango::DynamicCommandManager m_dcm;
   };
  \endverbatim
*/

// ============================================================================

#ifndef _DYNAMIC_COMMAND_H_
#define _DYNAMIC_COMMAND_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/any/Any.h>
#include <yat/utils/Singleton.h>
#include <yat/utils/Callback.h>

#include <yat4tango/Export.h>
#include <yat4tango/Types.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>

namespace yat4tango
{

// ============================================================================
// class: DynamicCommand (forward declaration)
// ============================================================================
class DynamicCommand;

// ============================================================================
//! \struct ArgIO
//! \brief Dynamic arguments identifiers
// ============================================================================
enum YAT4TANGO_DECL ArgIO
{
  ARGIN  = 0,
  ARGOUT = 1
};

// ============================================================================
//! \struct DynamicCommandExecuteCallbackData
//! \brief Read & write callback argument.
// ============================================================================
struct YAT4TANGO_DECL DynamicCommandExecuteCallbackData
{
  //! \brief The Tango device to which the dynamic command is attached.
  Tango::DeviceImpl * dev;

  //! \brief The dynamic command for which the callback is called.
  DynamicCommand * dyc;

  //! \brief Constructor.
  DynamicCommandExecuteCallbackData ()
    : dev (0), dyc(0)
  {}

  //! \brief Copy constructor.
  DynamicCommandExecuteCallbackData (const DynamicCommandExecuteCallbackData& src)
    : dev(src.dev), dyc(src.dyc)
  {}

  //! \brief Destructor.
  ~DynamicCommandExecuteCallbackData ()
  {}
};

// ============================================================================
//! Defines DynamicCommandExecuteCallback callback type.
// ============================================================================
YAT_DEFINE_CALLBACK(DynamicCommandExecuteCallback,
                    DynamicCommandExecuteCallbackData &);

// ============================================================================
//! \struct DynamicCommandInfo
//! \brief Dynamic command information.
//!
//! This class defines some command's information and a way to access them.
//!
// ============================================================================
struct YAT4TANGO_DECL DynamicCommandInfo
{
  //! \brief Default constructor.
  DynamicCommandInfo ();

  //! \brief Copy constructor.
  //! \param s The source information.
  DynamicCommandInfo (const DynamicCommandInfo& s);

  //! \brief Assignment operator.
  //! \param s The source information.
  const DynamicCommandInfo& operator= (const DynamicCommandInfo& s);

  //! \brief Sets the associated user data.
  //! 
  //! The user data are defined with a generic type \<T\>.
  //! \param d The user data.
  template <typename T> void set_user_data (T* d);
  
  //! \brief Sets the associated user data.
  //!
  //! The user data are defined with a generic type \<T\>.
  //! \param d The user data.
  template <typename T> void set_user_data (const T & d);
  
  //! \brief Returns the associated user data.
  //! Returns null (i.e. 0) in case the user data is not of type T.
  template <typename T> T * get_user_data ();

  //! \brief Returns the user data.
  template <typename T> void get_user_data (T& ud);

  //! \brief Returns the Tango data type of the specified command argument
  //! as a humanly readable string.
  const char * get_tango_data_type_str (ArgIO arg) const;

  //! \brief Dumps *this* instance to std::cout.
  void dump () /*const*/;

  //! \brief The Tango device to which the dynamic command belongs to.
  //!
  //! Default value is to NULL.
  Tango::DeviceImpl * dev;
  
  //! \brief The Tango command description
  Tango::CommandInfo tci;
  
  //! \brief Execute callback.
  DynamicCommandExecuteCallback ecb;
  
private:
  friend class DynamicInterfaceManager;
  friend class DynamicCommandFactory;
  
  //- user 'abstract' data
  yat::Any uad;
}; 

// ============================================================================
//! \class DynamicCommand
//! \brief The Yat4Tango dynamic command.
//!
//! This class can not be used as this and must be derived.
//!
//! An example of dynamic command usage is given here: \ref DynAttrPage "Dynamic command example"
//!
//! Inherits from Tango::LogAdapter class (for logging management).
//!
// ============================================================================
class YAT4TANGO_DECL DynamicCommand : public Tango::LogAdapter
{
public:
  //! \brief Attaches the specified execute callback to the dynamic command.
  //! \param ecb Reference to the execute callback.
  void set_execute_callback (const DynamicCommandExecuteCallback & ecb);

  //! \brief Returns the underlying Tango command.
  //!
  //! This virtual function has to be implemented in derived class.
  virtual Tango::Command * get_tango_command () = 0;

  //! \brief Returns the command's name.
  inline const std::string & get_name () const
  {
    return this->dci_.tci.cmd_name;
  }

  //! \brief Returns the command's data type.
  //! 
  //! See \p "server/tango_const.h/CmdArgType" enumeration for details upon possible return values.
  inline int get_tango_data_type (ArgIO arg) const
  {
    return arg == ARGIN ? this->dci_.tci.in_type : this->dci_.tci.out_type;
  }

  //! \brief Returns the Tango data type of the specified command argument as a humanly readable string.
  const char * get_tango_data_type_str (ArgIO arg) const;

  //! \brief Returns the associated Tango::CommandInfo (i.e. command description).
  inline const Tango::CommandInfo & get_command_info () const
  {
    return this->dci_.tci;
  }

  //! \brief Sets the user data.
  //! \param d Pointer to the user data (generic type \<T\>).
  template <typename T> void set_user_data (T* d);
  
  //! \brief Sets the user data.
  //! \param d Pointer to the user data (generic type \<T\>).
  template <typename T> void set_user_data (const T & d);

  //! \brief Returns the user data (returns NULL is the user dat is not of type T) 
  template <typename T> T* get_user_data ();

  //! \brief Returns the user data.
  template <typename T> void get_user_data (T& ud);

  //! \brief Checks whether or not the data type of the specified argument is T.
  //! Returns true if the specified argument is of type T, returns false otherwise.
  template <typename T> bool check_argument_data_type (ArgIO arg) const;

  //! \brief Input argument extraction helper.
  template <typename T> void get_argin (T & argin);

   //! \brief Input argument extraction helper.
  template <typename T> void get_argin (T *& argin);

   //! \brief Output argument insertion helper.
  template <typename T> void set_argout (T* argout);

   //! \brief Output argument insertion helper.
  template <typename T> void set_argout (T& argout);
  
  //! \brief Dumps command information to std::cout.
  void dump () /*const*/;

protected:
  
  //! \brief Constructor.
  // \param dai Dynamic command information.
  DynamicCommand (const DynamicCommandInfo & dai);

  //! \brief Destructor.
  virtual ~DynamicCommand();


  //! \brief The associated dynamic command information.
  DynamicCommandInfo dci_;

  //- commands arguments
  CORBA::Any const * argin_;
  CORBA::Any * argout_;
  
private:
  friend class DynamicCommandManager;
  friend class DynamicCommandFactory;

  //- not allowed methods
  DynamicCommand (DynamicCommand&);
  DynamicCommand&  operator= (DynamicCommand&);
};

// ============================================================================
//! \class DynamicCommandFactory
//! \brief The dynamic command factory.
//!
//! This class provides an interface to create generic dynamic commands.
//! It is used by friend class yat4tango::DynamicCommandManager implementation.
//!
//! Inherits from yat::Singleton class (for the "singleton" pattern).
//!
// ============================================================================
class YAT4TANGO_DECL DynamicCommandFactory : public yat::Singleton<DynamicCommandFactory>
{
  friend class DynamicCommandManager;

private:
  //- dynamic command factory
  //- i: the Tango command info (name, format, type, ...)
  DynamicCommand * create_command (const yat4tango::DynamicCommandInfo& i);
};

// ============================================================================
// DynamicCommandInfo::set_user_data
// ============================================================================
template <typename T>
void DynamicCommandInfo::set_user_data (T* d)
{
  uad = d;
}

// ============================================================================
// DynamicCommandInfo::set_user_data
// ============================================================================
template <typename T>
void DynamicCommandInfo::set_user_data (const T & d)
{
  uad = d;
}

// ============================================================================
// DynamicCommandInfo::get_user_data
// ============================================================================
template <typename T>
T * DynamicCommandInfo::get_user_data ()
{
  try
  {
    return yat::any_cast<T*>(uad);
  }
  catch (const yat::Exception&)
  {
    return 0;
  }
}

// ============================================================================
// DynamicCommandInfo::get_user_data
// ============================================================================
template <typename T>
void DynamicCommandInfo::get_user_data (T& ud)
{
  ud = yat::any_cast<T>(uad);
}
  
// ============================================================================
// DynamicCommand::check_argument_data_type
// ============================================================================
template <typename T>
bool DynamicCommand::check_argument_data_type (ArgIO arg) const
{
  return this->get_tango_data_type(arg) == TangoTraits<T>::type_id;
}

// ============================================================================
// DynamicCommand::set_user_data
// ============================================================================
template <typename T>
void DynamicCommand::set_user_data (T* d)
{
  this->dci_.set_user_data<T>(d);
}

// ============================================================================
// DynamicCommand::set_user_data
// ============================================================================
template <typename T>
void DynamicCommand::set_user_data (const T & d)
{
  this->dci_.set_user_data<T>(d);
}

// ============================================================================
// DynamicCommand::get_user_data
// ============================================================================
template <typename T>
T* DynamicCommand::get_user_data ()
{
  return this->dci_.get_user_data<T>();
}

// ============================================================================
// DynamicCommand::get_argin
// ============================================================================
template <typename T>
void DynamicCommand::get_user_data (T& ud)
{
  this->dci_.get_user_data<T>(ud);
}
  
// ============================================================================
// DynamicCommand::get_argin
// ============================================================================
template <typename T>
void DynamicCommand::get_argin (T & argin)
{
  if ( ! this->argin_ )
  {
    THROW_DEVFAILED("DATA_ERROR",
                    "can't extract data from a void input argument",
                    "yat4tango::DynamicCommand::extract_argin");
  }

  try
  {
    this->get_tango_command()->extract(*(this->argin_), argin);
  }
  catch (Tango::DevFailed& e)
  {
    RETHROW_DEVFAILED(e,
                      "DATA_ERROR",
                      "failed to extract the requested data type from the command input argument",
                      "yat4tango::DynamicCommand::extract_argin");
  }
  catch ( ... )
  {
    THROW_DEVFAILED("DATA_ERROR",
                    "failed to extract the requested data type from the command input argument",
                    "yat4tango::DynamicCommand::extract_argin");
  }
}

// ============================================================================
// DynamicCommand::get_argin
// ============================================================================
template <typename T>
void DynamicCommand::get_argin (T *& argin)
{
  if ( ! this->argin_ )
  {
    THROW_DEVFAILED("DATA_ERROR",
                    "can't extract data from a void input argument",
                    "yat4tango::DynamicCommand::extract_argin");
  }
  try
  {
    this->get_tango_command()->extract(*(this->argin_), argin);
  }
  catch (Tango::DevFailed& e)
  {
    RETHROW_DEVFAILED(e,
                      "DATA_ERROR",
                      "failed to extract the requested data type from the command input argument",
                      "yat4tango::DynamicCommand::extract_argin");
  }
  catch ( ... )
  {
    THROW_DEVFAILED("DATA_ERROR",
                    "failed to extract the requested data type from the command input argument",
                    "yat4tango::DynamicCommand::extract_argin");
  }
}

// ============================================================================
// DynamicCommand::set_argout
// ============================================================================
template <typename T>
void DynamicCommand::set_argout (T* argout)
{
  if ( this->argout_ )
  {
    THROW_DEVFAILED("PROGRAMING_ERROR",
                    "<insert_argout> has been called more than for the same command execution request",
                    "yat4tango::DynamicCommand::insert_argout");
  }
  this->argout_ = this->get_tango_command()->insert(argout);
}

// ============================================================================
// DynamicCommand::set_argout
// ============================================================================
template <typename T>
void DynamicCommand::set_argout (T& argout)
{
  if ( this->argout_ )
  {
    THROW_DEVFAILED("PROGRAMING_ERROR",
                    "<insert_argout> has been called more than for the same command execution request",
                    "yat4tango::DynamicCommand::insert_argout");
  }
  this->argout_ = this->get_tango_command()->insert(argout);
}
  
} //- namespace 

#endif //- _DYNAMIC_COMMAND_H_
