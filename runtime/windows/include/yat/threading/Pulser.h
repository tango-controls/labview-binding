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
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
//
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

//============================================================================
/*! \page PulserExample Pulser Example:
    \section P1 Quick & dirty Pulser attribute example of usage:
    
	\verbatim 

  #include "yat/threading/Pulser.h"

  class MyClass
  {
  public:
    MyClass() : calls(0) {}

    void my_callback (yat::Thread::IOArg arg)
    {
      std::cout << "MyClass::my_callback::call #" << ++calls << std::endl;
    }

    size_t calls;
  };

  int main (int, char**)
  {
    try
    {
      //- MyClass instance
      MyClass mc;

      //- pulser's config
      yat::Pulser::Config cfg;
      cfg.period_in_msecs = 250;
      cfg.num_pulses = 10;
      cfg.callback = yat::PulserCallback::instanciate(mc, &MyClass::my_callback);
      cfg.user_data = 0;

      yat::Pulser p(cfg);
      p.start();

      yat::Thread::sleep(5000);

      std::cout << "done!" << std::endl;
    }
    catch (...)
    {
      std::cout << "Unknown exception caught" << std::endl;
    }

    return 0;  
  }

	\endverbatim
*/


#ifndef _PULSER_H_
#define _PULSER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/threading/Task.h>
#include <yat/utils/Callback.h>

namespace yat
{

// ============================================================================
//! Defines DynamicAttributeReadCallback callback type.
// ============================================================================
YAT_DEFINE_CALLBACK(PulserCallback, Thread::IOArg);

// ============================================================================
//! Forward declaration of the opaque implemention
// ============================================================================
class PulserCoreImpl;

// ============================================================================
//! \class Pulser
//! \brief Periodically call a callback accepting a Thread::IOArg as input arg.
// ============================================================================
class YAT_DECL Pulser
{
public:

  //! Pulser configuration structure.
  struct YAT_DECL Config
  {
    //! Pulser's period in msecs. Defaults to 1000 (i.e. one 1Hz).
    size_t period_in_msecs;
    //! Number of pulses to generate (0 means infinite). Defaults to 0.
    size_t num_pulses;
    //! Pulsed callback. No default value!
    PulserCallback callback;
    //! User data (will be passed callback). Defaults to NULL.
    Thread::IOArg user_data;
    //! Default constructor.
    Config ();
  };
  
  //! \brief Config constructor.
  //! \param cfg Task configuration.
  Pulser (const Config& cfg);

  //! \brief Destructor.
  virtual ~Pulser ();

  //! \brief Starts the pulser.
  //!
  //! \exception An exception is thrown in case the pulser initialization failed.
  //! \param sync if set then use wait_msg_handled to ensure the start action is done before return
  virtual void start ();
  
  //! \brief Stops the pulser.
  //!
  //! \exception An exception is thrown in case the pulser initialization failed.
  //! \param sync if set then use wait_msg_handled to ensure the stop action is done before return
  virtual void stop ();

  //! \brief Starts the pulser synchronously.
  //!
  //! \exception An exception is thrown in case the pulser initialization failed.
  //! \notice Ensure the start action is done before return
  virtual void start_sync ();
  
  //! \brief Stops the pulser synchronously.
  //!
  //! \exception An exception is thrown in case the pulser initialization failed.
  //! \notice Ensure the stop action is done before return
  virtual void stop_sync ();

  //! \brief Changes the pulser's period.
  //! \param p_msecs The new period in msecs.
  void set_period (size_t p_msecs);
  
  //! \brief Returns the pulser's in msecs.
  size_t get_period () const;

  //! \brief Changes the number of pulses to be generated.
  //! \param num_pulses The new number of pulses.
  void set_num_pulses (size_t num_pulses);
  
  //! \brief Returns the number of pulses to be generated.
  size_t get_num_pulses () const;
  
  //! \brief Suspends the Pulser activity
  void suspend ();

  //! \brief Suspends the Pulser activity synchronously
  //! \notice Ensure the suspend action is done before return
  void suspend_sync ();

  //! \brief Check the Pulser activity
  bool is_done ();

  //! \brief Resumes the Pulser activity 
  void resume ();

  //! \brief Resumes the Pulser activity synchronously
  //! \notice Ensure the resume action is done before return
  void resume_sync ();

private:
  //_ pulser's config
  Config cfg_;

  //- underlying implemantation
  PulserCoreImpl * impl_;
};

} // namespace

#endif // _PULSER_H_
