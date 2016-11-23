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

#ifndef _YAT4TANGO_DYNAMIC_INTERFACE_MANAGER_H_
#define _YAT4TANGO_DYNAMIC_INTERFACE_MANAGER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/ExceptionHelper.h>
#include <yat4tango/DynamicCommandManager.h>
#include <yat4tango/DynamicAttributeManager.h>

// ============================================================================
/*! \page DynInterfacePage Dynamic Interface Example

\verbatim

  class MyClass : public Tango::LogAdapter
  {
    public:
      //------------------------------------------------------------------
      MyClass (Tango::DeviceImpl * d)
        : Tango::LogAdapter(d), m_dim(d)
      {
        this->init_dynamic_commands();
        this->init_dynamic_attributes();
      }
      //------------------------------------------------------------------
      ~MyClass ()
      {
        //- remove the dynamic part of the device interface (cmds & attrs) 
        this->dim.remove();
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
        this->m_dim.dynamic_commands_manager_manager().add_command(dci);
      }
      //------------------------------------------------------------------
      void init_dynamic_attributes ()
      {
        //- this method must be executed by a Tango thread!
        //-------------------------------------------------
        yat4tango::DynamicAttributeInfo dai;
        dai.dev = a pointer to the Tango::DeviceImpl to which the dyn. attr belongs;
        //- describe the dyn attr we want...
        dai.tai.data_type = Tango::DEV_SHORT;
        dai.tai.data_format = Tango::SCALAR;
        dai.tai.writable = Tango::READ_WRITE;
        dai.tai.disp_level = Tango::OPERATOR;

        //- cleanup tango db option: cleanup tango db when removing this dyn. attr. (i.e. erase its properties fom db)
        dai.cdb = true;

        //- read callback
        dai.rcb = yat4tango::DynamicAttributeReadCallback::instanciate(*this,
                                                                        &MyClass::read_callback);
        //- write callback
        dai.wcb = yat4tango::DynamicAttributeWriteCallback::instanciate(*this,
                                                                        &MyClass::write_callback);
        //- add the dynamic attribue to the device interface
        this->m_dim.dynamic_attributes_manager().add_attribute(dai);
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
      //------------------------------------------------------------------
      void read_callback (DynamicAttributeReadCallbackData& cbd)
      {
        DEBUG_STREAM << "read request for dynamic attribute: "
                      << cbd.dya->get_name()
                      << std::endl;
                      
        cbd.tga->set_value(...);
      }
      //------------------------------------------------------------------
      void write_callback (DynamicAttributeWriteCallbackData& cbd)
      {
        DEBUG_STREAM << "write request for dynamic attribute: "
                      << cbd.dya->get_name()
                      << std::endl;
                      
        cbd.tga->get_write_value(...);
      }
    private:
      //- our dynamic interface manager
      yat4tango::DynamicInterfaceManager m_dim;
  };
\endverbatim
*/

namespace yat4tango
{

// ============================================================================
//! \class DynamicInterfaceManager
//! \brief The dynamic interface manager.
//!
//! This class provides a manager of device dynamic interface (cmds & attrs).
//!
//! Adding or removing a dynamic attribute or command from the repository is
//! equivalent to adding or removing a dynamic attribute or command from the
//! Tango Device interface.
//!
//! Inherits from Tango::LogAdapter class (for logging management).
//!
// ===========================================================================

class YAT4TANGO_DECL DynamicInterfaceManager
{
public:
  //! \brief Constructor. 
  //! \param _dev The Tango Device handled by the instance.
  DynamicInterfaceManager (Tango::DeviceImpl * _dev = 0);
  
  //! \brief Destructor.
  //!
  //! Removes all registered dynamic attributes from the device.
  virtual ~DynamicInterfaceManager();

  //! \brief Removes any existing dynamic attribute or command.
  inline void remove ()
  {
    this->dcm_.remove_commands();
    this->dam_.remove_attributes();
  }

  //! \brief Sets host device.
  //!
  //! \param h The Tango device instance to which the dynamic commands are linked.
  //! \exception CONFIGURATION_ERROR Thrown if the manager is already linked to a device.
  inline void set_host_device (Tango::DeviceImpl * h)
  {
    this->dcm_.set_host_device(h);
    this->dam_.set_host_device(h);
  }
    
  //! \brief Provides access to the DynamicCommandManager
  inline DynamicCommandManager &  dynamic_commands_manager ()
  {
    return this->dcm_;
  }
  
  //! \brief Provides access to the DynamicAttributeManager
  inline DynamicAttributeManager &  dynamic_attributes_manager () 
  {
    return this->dam_;
  }
  
private:
  DynamicCommandManager dcm_;
  DynamicAttributeManager dam_;

  DynamicInterfaceManager(const DynamicInterfaceManager&);
  DynamicInterfaceManager& operator = (const DynamicInterfaceManager&);
};

} // namespace

#endif
