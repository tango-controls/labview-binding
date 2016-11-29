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

#ifndef _YAT4TANGO_LOGHELPER_H_
#define _YAT4TANGO_LOGHELPER_H_

// ============================================================================
// WIN32 SPECIFIC
// ============================================================================
#if defined (WIN32)
# pragma warning(disable:4786)
#endif

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "yat/time/Timer.h"
#include "yat4tango/CommonHeader.h"

namespace yat4tango
{

// ============================================================================
//! \class TangoLogAdapter 
//! \brief The Yat4Tango log adapter.
//!
/*! 
 This class provides an interface to associate a log request to a device name.
 This is useful for a function that is not implemented in the Device's main 
 implementation class. Unlike the Tango::LogAdapter class, the associated
 Device can be set after construction time (with the *host()* function).

\b Usage: \n
Inherit from TangoLogAdapter and initialize the Device pointer with *this*. For instance:
\verbatim 	
class MyDevice: public yat4tango::TangoLogAdapter
{
   public :
      MyDevice(...,Tango::DeviceImpl *device,...)
				:yat4tango::TangoLogAdapter(device)
      {
         ....
         //
         // The following log is associated to the device passed to the constructor
         //
         DEBUG_STREAM << "In MyDevice constructor" << endl;
         ....
      }
};
\endverbatim
*/
// ============================================================================
class YAT4TANGO_DECL TangoLogAdapter
{
public:

  //! \brief Constructor.
  //! \param _host The associated device.
  TangoLogAdapter (Tango::DeviceImpl * _host)
    : logger_ (0)
  {
    this->host(_host);
  }

  //! \brief Destructor.
  virtual ~TangoLogAdapter () 
  {
    //- noop dtor
  } ;

  //! \brief Gets the associated logger.
  inline log4tango::Logger * get_logger () 
  {
    return logger_;
  }

  //! \brief Sets the associated device.
  //! \param _host The associated device.
  inline void host (Tango::DeviceImpl * _host) 
  {
    if (_host)
      logger_ = _host->get_logger();
    else
      logger_ = 0;
  }

private:
  log4tango::Logger* logger_;
};

// ============================================================================
//! \class TraceHelper 
//! \brief Trace helper for a Tango Device's internal activity. -- DEBUG purpose --
//!
/*! 
This class supplies an interface to log the input and output activities of a function,
for DEBUG purpose.

\b Usage: \n
Construct a TraceHelper object to log the activity of a function at input time. 
For instance:
\verbatim 
void myFunction(...)
{
   yat4tango::TraceHelper t("MyDevice::init_device", this); 
   ...;
} \endverbatim
*/
//! At the end of the function, the TraceHelper object will be destroyed and then will
//! log the function activity.
//!
// ============================================================================
class YAT4TANGO_DECL TraceHelper
{
public:

  //! \brief Constructor.
  //!
  //! Logs the Device's object address.
  //! \param _func_name Function name.
  //! \param _this The device to log.
  TraceHelper(const char* _func_name, Tango::DeviceImpl * _this = 0)
    : m_this(_this), 
      m_func(_func_name),
      m_logger (_this ? _this->get_logger() : 0)
  { 
#if defined(WIN32)
    DEBUG_STREAM << "[this::0x" 
#else
    DEBUG_STREAM << "[this::" 
#endif
                 << std::hex 
                 << m_this 
                 << std::dec 
                 << "] "
                 << m_func 
                 << " <<"
                 << std::endl;
  };

  //! \brief Constructor.
  //!
  //! Logs the Device's object address.
  //! \param _func_name Function name.
  //! \param _this The object to log.
  TraceHelper(const char* _func_name, Tango::LogAdapter * _this = 0)
    : m_this(_this), 
      m_func(_func_name),
      m_logger (_this ? _this->get_logger() : 0)
  { 
#if defined(WIN32)
    DEBUG_STREAM << "[this::0x" 
#else
    DEBUG_STREAM << "[this::" 
#endif
                 << std::hex 
                 << m_this 
                 << std::dec 
                 << "] "
                 << m_func 
                 << " <<"
                 << std::endl;
  };

  //! \brief Destructor.
  //!
  //! Logs the Device's object address at destruction time.
  virtual ~TraceHelper()
  { 
#if defined(WIN32)
    DEBUG_STREAM << "[this::0x" 
#else
    DEBUG_STREAM << "[this::" 
#endif
                 << std::hex 
                 << m_this 
                 << std::dec 
                 << "] "
                 << m_func 
                 << " >> ["
                 << m_timer.elapsed_msec()
                 << " ms]"
                 << std::endl; 
  };

  //! \brief Gets the associated logger.
  inline log4tango::Logger * get_logger () 
  {
    return m_logger;
  }

private:
  const void * m_this;
  const char * m_func;
  log4tango::Logger * m_logger;
  yat::Timer m_timer;
};

} //- namespace yat4tango 

#endif //- _YAT4TANGO_LOGHELPER_H_
