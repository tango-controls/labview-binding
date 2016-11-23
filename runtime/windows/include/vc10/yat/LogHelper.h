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

#ifndef _YAT_LOGHELPER_H_
#define _YAT_LOGHELPER_H_

#if defined (YAT_TRACE_THREADSAFE)
  #include <yat/threading/Mutex.h>
  #define DECLARE_YAT_TRACE_MUTEX static yat::Mutex s_mtx
  #define INSTANTIATE_YAT_TRACE_MUTEX yat::Mutex yat::TraceHelper::s_mtx
  #define YAT_TRACE_LOCK  yat::AutoMutex<yat::Mutex> __yat_lock_trace_log(yat::TraceHelper::s_mtx)
#else
  #define YAT_TRACE_LOCK
  #define DECLARE_YAT_TRACE_MUTEX
  #define INSTANTIATE_YAT_TRACE_MUTEX
#endif

/*!
 *  Define the YAT_LOG & YAT_TRACE helper macro depending on the YAT_ENABLE_LOG macro
 *  These macros can be used by yat users to log in their applications
 *  
 *  When the YAT_TRACE_THREADSAFE switch is set, then a global mutex is used to avoid 
 *  trace overlaps when several threads use YAT_TRACE & YAT_LOG at the same time
 *  The global mutex must be initialized by the client code using INSTANTIATE_YAT_TRACE_MUTEX 
 *  macro somewhere in a cpp source file
 */
#if defined (YAT_ENABLE_LOG)

#  include <iostream>


# if defined (YAT_ENABLE_TRACE)
    namespace yat
    {
      class YAT_DECL TraceHelper
      {
      public:
        TraceHelper(const char* _func_name, const void * _this = 0 )
          :  instance(_this), func_name(_func_name)
        { 
          YAT_TRACE_LOCK;
          std::cout << func_name 
                    << " [this::" 
                    << std::hex 
                    << instance 
                    << std::dec 
                    << "] <-" 
                    << std::endl; 
        };

        ~TraceHelper()
        { 
          YAT_TRACE_LOCK;
          std::cout << func_name 
                    << " [this::" 
                    << std::hex 
                    << instance
                    << std::dec 
                    << "] ->" 
                    << std::endl; 
        };

      private:
        const void * instance;
        const char* func_name;
      public:
        DECLARE_YAT_TRACE_MUTEX;
      };
    }
#   define YAT_TRACE(func_name) \
      yat::TraceHelper yat_trace_helper( (func_name), this )
#   define YAT_TRACE_STATIC(func_name) \
      yat::TraceHelper yat_trace_helper( (func_name) )
# else
#   define YAT_TRACE(func_name)
#   define YAT_TRACE_STATIC(func_name)
# endif

#  define YAT_LOG(s) \
    do \
    { \
      YAT_TRACE_LOCK; \
      std::cout << "[this:" \
                 << std::hex \
                 << (void *)this \
                 << std::dec \
                 << "]::" \
                 << __FUNCTION__ \
                 << "::" \
                 << s \
                 << std::endl; \
    } while (0)
    
#  define YAT_LOG_STATIC(s) \
    do \
    { \
      YAT_TRACE_LOCK; \
      std::cout << s << std::endl; \
    } while(0)
    
#else

# define YAT_LOG(x)
# define YAT_LOG_STATIC(x)
# define YAT_TRACE(x)
# define YAT_TRACE_STATIC(x)

#endif


#endif
