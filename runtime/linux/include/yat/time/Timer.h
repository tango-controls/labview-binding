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

#ifndef _YAT_TIMER_H_
#define _YAT_TIMER_H_

#include <yat/CommonHeader.h>


// ============================================================================
// TIME MACROS
// ============================================================================
#include <ctime>
#include <limits>
#if defined (YAT_WIN32)
# include <sys/timeb.h>
# include <time.h>
#else
# include <sys/time.h>
#endif

typedef struct _timeval
{
  int tv_sec;
  int tv_usec;
  time_t tv_utc;
  _timeval ()
    : tv_sec(0), tv_usec(0), tv_utc(0) {}
} _timeval;
# define _TIMESTAMP _timeval

#if defined (YAT_WIN32)
  typedef struct
  {
    int tv_sec;
    int tv_nsec;
  } _timespec;
# define _TIMESPEC _timespec
#else
# define _TIMESPEC timespec
#endif

#if defined (YAT_WIN32)  
# define _GET_TIME(T) \
  do \
  { \
    struct _timeb _timeb_now; \
    ::_ftime(&_timeb_now); \
    T.tv_sec = static_cast<int>(_timeb_now.time); \
    T.tv_usec = static_cast<int>(1000 * _timeb_now.millitm); \
    ::time(&T.tv_utc); \
  } while (0)
#else
# define _GET_TIME(T) \
  do \
  { \
    struct timeval _now; \
    ::gettimeofday(&_now, 0); \
    T.tv_sec = _now.tv_sec; \
    T.tv_usec = _now.tv_usec; \
    ::time(&T.tv_utc); \
  } while (0)
#endif

# define _MAX_DATE_LEN 256
# define _TIMESTAMP_TO_DATE(T,S) \
  do \
  { \
    struct tm * tmv = ::localtime(&T.tv_utc); \
    char b[_MAX_DATE_LEN]; \
    ::memset(b, 0, _MAX_DATE_LEN); \
    ::strftime(b, _MAX_DATE_LEN, "%a, %d %b %Y %H:%M:%S", tmv); \
    S = std::string(b); \
  } while (0)
# define  _RESET_TIMESTAMP(T) \
  do \
  { \
    T.tv_sec = 0; \
    T.tv_usec = 0; \
    T.tv_utc = 0; \
  } while (0)
# define  _COPY_TIMESTAMP(S, D) \
  do \
  { \
    D.tv_sec = S.tv_sec; \
    D.tv_usec = S.tv_usec; \
    D.tv_utc = S.tv_utc; \
  } while (0)
  
  
#define  _ELAPSED_SEC(B, A) \
  static_cast<double>((A.tv_sec - B.tv_sec) + (1.E-6 * (A.tv_usec - B.tv_usec)))

#define  _ELAPSED_MSEC(B, A) _ELAPSED_SEC(B, A) * 1.E3

#define  _ELAPSED_USEC(B, A) _ELAPSED_SEC(B, A) * 1.E6

#define _IS_VALID_TIMESTAMP(T) T.tv_sec != 0 || T.tv_usec != 0

#define  _TMO_EXPIRED(B, A, TMO) _ELAPSED_SEC (B, A) > TMO


#if defined (YAT_WIN32)
  namespace std 
  { 
    using ::clock_t; 
    using ::clock; 
  }
#endif

namespace yat
{
  typedef _TIMESTAMP Timestamp;
  typedef _TIMESPEC  Timespec;

#if ! defined (YAT_WIN32) 
 
// ============================================================================
//! \class Timer 
//! \brief The YAT timer class.
//!
//! This class implements a basic timer object, with microsecond precision.
// ============================================================================
class YAT_DECL Timer
{
public:
  //! \brief Creates/resets the timer.
  Timer () 
  { 
    this->restart();
  } 
  
  //! \brief Resets the timer.
  inline void restart() 
  {
    ::gettimeofday(&_start_time, NULL); 
  }

  //! \brief Returns the elapsed time in seconds.
  inline double elapsed_sec ()             
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return (now.tv_sec - _start_time.tv_sec) + 1e-6 * (now.tv_usec - _start_time.tv_usec);
  }

  //! \brief Returns the elapsed time in milliseconds.
  inline double elapsed_msec ()             
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return 1e3 * (now.tv_sec - _start_time.tv_sec) + 1e-3 * (now.tv_usec - _start_time.tv_usec);
  }

  //! \brief Returns the elapsed time in microseconds.
  inline double elapsed_usec ()              
  { 
    struct timeval now;
    ::gettimeofday(&now, NULL);
    return 1e6 * (now.tv_sec - _start_time.tv_sec) + (now.tv_usec - _start_time.tv_usec);
  }

private:
  struct timeval _start_time;
};

#else // ! YAT_WIN32

// ============================================================================
//! \class Timer 
//! \brief The YAT timer class.
//!
//! This class implements a basic timer (chronometer) object, with microsecond precision.
// ============================================================================
class YAT_DECL Timer 
{
public:
  //! \brief Default constructor.
  //!
  //! Creates/resets the timer.
  Timer () 
  {
    _start.QuadPart = 0;
    _stop.QuadPart = 0;
    ::QueryPerformanceFrequency(&_frequency);
    ::QueryPerformanceCounter(&_start);
  }
  
  //! \brief Resets the timer.
  inline void restart() 
  {
    ::QueryPerformanceCounter(&_start) ;
  }
  
  //! \brief Returns the elapsed time in seconds.
  inline double elapsed_sec ()             
  { 
    ::QueryPerformanceCounter(&_stop);
    LARGE_INTEGER dt;
    dt.QuadPart = _stop.QuadPart - _start.QuadPart;
    return li_to_secs(dt);
  }

  //! \brief Returns the elapsed time in milliseconds.
  inline double elapsed_msec ()           
  { 
    return  1.E3 * elapsed_sec (); 
  }

  //! \brief Returns the elapsed time in microseconds.
  inline double elapsed_usec ()            
  { 
    return  1.E6 * elapsed_sec (); 
  }
  
private:
  LARGE_INTEGER _start;
  LARGE_INTEGER _stop;
  LARGE_INTEGER _frequency;
  inline double li_to_secs ( LARGE_INTEGER & li) {
    return (double)li.QuadPart / (double)_frequency.QuadPart;
  }
};

#endif // ! YAT_WIN32

// ============================================================================
//! \class Timeout 
//! \brief The YAT timeout class.
//!
//! This class implements a basic timeout object, with customizable unit (from 
//! second to microsecond precision).\n 
//! The yat::Timeout class uses a yat::Timer timer.
// ============================================================================
class YAT_DECL Timeout 
{
public:
  //! \brief %Timeout value type.
  typedef double TimeoutValue;
  
  //! \brief %Timeout units.
  typedef enum  
  {
    //! Second.
    TMO_UNIT_SEC,
    //! Millisecond.
    TMO_UNIT_MSEC,
    //! Microsecond.
    TMO_UNIT_USEC, 
  } TimeoutUnit;
  
  //! \brief Default constructor. 
  //!
  //! Creates/resets the timeout. The default unit is millisecond and
  //! the timeout is set to 0 and disabled.
  Timeout () 
    : _unit(TMO_UNIT_MSEC), _tmo(0), _enabled(false)
  {
    //- noop 
  }
  
  //! \brief Constructor.
  //!
  //! Creates a timeout.
  //! \param tmo_in_unit %Timeout value in specified unit.
  //! \param unit %Unit.
  //! \param enabled If set to true, timeout is started.
  Timeout (TimeoutValue tmo_in_unit, TimeoutUnit unit = TMO_UNIT_MSEC, bool enabled = false) 
    : _unit(unit), _tmo(tmo_in_unit), _enabled(enabled)
  {
    //- noop 
  }
  
  //! \brief Restarts/(re)enables the timeout.
  //!
  //! Enables the timeout.
  inline void restart () 
  {
    enable();
  }
  
  //! \brief Enables the timeout.
  //!
  //! \param restart_timer If set to true, restart the timeout.
  inline void enable (bool restart_timer = true) 
  {
    if (restart_timer) _t.restart();
    _enabled = true;
  }
  
  //! \brief Disables the timeout.
  //!
  //! In the disabled state, Timeout::expired() will always return false.
  inline void disable () 
  {
    _enabled = false;
  }
    
  //! \brief Is the timeout enabled?
  //!
  //! Returns true if the timeout is enabled, false otherwise.
  inline bool enabled () const
  {
    return _enabled;
  }
  
  //! Did the timeout expire?
  //! 
  //! Return true if timeout is enabled and has expired.
  //! In the disabled state, always returns false.
  inline bool expired ()
  {
    //- a disabled Timeout never expire
    if (! _enabled)
      return false;
    //- however, an enabled Timeout might expire :-)
    double dt = 0.;
    switch (_unit) 
    {
      case TMO_UNIT_SEC:
        dt = _t.elapsed_sec();
        break;
      case TMO_UNIT_MSEC:
        dt = _t.elapsed_msec();
        break;
      case TMO_UNIT_USEC:
        dt = _t.elapsed_usec();
        break;
      default:
        break;
    }
    return dt >= _tmo;
  }
  
  //! \brief Gets the time to expiration in timeout unit.
  //!
  //! A negative value means: expired ... \<timeout unit\> ago.\n
  //! A positive value means: will expired in ... \<timeout unit\>.\n
  //! For a disabled timeout, returns the infinity value.
  inline TimeoutValue time_to_expiration ()
  {
    //- undefined if disabled...
    if (! _enabled)
      return std::numeric_limits<TimeoutValue>::infinity();
    double dt = 0.;
    switch (_unit) 
    {
      case TMO_UNIT_SEC:
        dt = _t.elapsed_sec();
        break;
      case TMO_UNIT_MSEC:
        dt = _t.elapsed_msec();
        break;
      case TMO_UNIT_USEC:
        dt = _t.elapsed_usec();
        break;
      default:
        break;
    }
    return _tmo - dt;
  }
  
  //! \brief Changes the timeout value.
  //! \param tmo New timeout value (in the current timeout unit).
  inline void set_value (TimeoutValue tmo) 
  {
    _tmo = tmo;
  }
  
  //! \brief Returns the timeout value.
  inline TimeoutValue get_value () const
  {
    return _tmo;
  }
  
  //! \brief Changes the timeout unit.
  //! \param unit New timeout unit.
  inline void set_unit (TimeoutUnit unit) 
  {
    _unit = unit;
  }
  
  //! \brief Returns the timeout unit.
  inline TimeoutUnit get_unit () const
  {
    return _unit;
  }
  
private:
  Timer _t;
  TimeoutUnit _unit;
  double _tmo;
  bool _enabled;
};

} //- namespace yat

#endif
