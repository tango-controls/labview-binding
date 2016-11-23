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

#ifndef _MONITORED_ATTRIBUTE_H_
#define _MONITORED_ATTRIBUTE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat4tango/ExceptionHelper.h>
#include <yat/threading/Mutex.h>

namespace yat4tango
{

// ============================================================================
//! \class MonitoredAttribute 
//! \brief The monitored Device attribute class.
//!
//! This class provides an interface to access a Device attribute's value 
//! in a "thread safe" way. This class is used by the MonitoredDevice class to 
//! implement the "polling Device" capability, i.e. the monitoring of a Device by
//! a remote client (which can also be a Device).
//!
// ============================================================================
class MonitoredAttribute
{
  friend class MonitoredDevice;
   
public :
  //! \brief Default constructor.
  MonitoredAttribute ();

  //! \brief Destructor.
  virtual ~MonitoredAttribute ();

  //! \brief Checks if an error occurred during attribute's reading.
  //!
  //! Returns true if any error has occurred, false otherwise.
  inline bool has_error () 
  { 
    yat::AutoMutex<yat::Mutex> guard(_lock);
    return _val.has_failed();
  }
	
  //! \brief Gets the value of the Device attribute as a generic type \<T\>.
  //!
  //! \param[out] v The attribute's value.
  //! \param[in,out] dim_x X dimension of the attribute's value. If pointer set to 
  //! Null value, the dimension is not returned.
  //! \param[in,out] dim_y Y dimension of the attribute's value. If pointer set to 
  //! Null value, the dimension is not returned.
  //! \exception DATA_ERROR/UNKNOWN_ERROR Thrown if an error occurs during data extraction.
  template <typename T> void get_value (T& v, size_t * dim_x = 0, size_t * dim_y = 0)
  {
    yat::AutoMutex<yat::Mutex> guard(_lock);

    //- did we get a local error? (no response from device)    
    if (this->_has_error)
    	throw this->_error;

    //- enable exceptions
    _val.set_exceptions(Tango::DeviceAttribute::isempty_flag);
    _val.set_exceptions(Tango::DeviceAttribute::wrongtype_flag);
    _val.set_exceptions(Tango::DeviceAttribute::failed_flag);
    
    //- backup dims
    Tango::AttributeDimension ad = _val.get_r_dimension();

    //- try to extract data (might throw an Tango::DevFailed) then...
    //- reinject data into the DeviceAttribute for (potential) reuse
    //- this is required cause the clients might read the value 
    //- "several times" between two updates. that's the way the Tango
    //- DeviceAttribute works!
    try
    {
      //- extract value
      _val >> v;
      //- reinject value 
      _val << v;
      //- restore dims
      _val.dim_x = ad.dim_x;
      _val.dim_y = ad.dim_y;
    }
    catch (Tango::DevFailed& df)
    {
    	RETHROW_DEVFAILED(df, 
      				"DATA_ERROR", 
           			"could not extract attribute value from DeviceAttribute [Tango exception caught]", 
           			"MonitoredAttribute::get_value");
    }
    catch (...)
    {
  		THROW_DEVFAILED("UNKNOWN_ERROR", 
            			"could not extract attribute value from DeviceAttribute [unknown exception caught]", 
            			"MonitoredAttribute::get_value");
    }
    
    //- return dims (if requested)
    if (dim_x) *dim_x =	static_cast<size_t>(ad.dim_x);
    if (dim_y) *dim_y =	static_cast<size_t>(ad.dim_y);
  }
  
  //! \brief Shortcut for the get_value() function.
  //!
  //! \param v The attribute's value.
  //! \exception DATA_ERROR/UNKNOWN_ERROR Thrown if an error occurs during data extraction.  
  template <typename T> void operator>> (T& v)
  {
    this->get_value(v);
  }
  
protected:
  //! \brief Sets the new attribute's value.
  //! 
  //! \param value The new value.
  void set_value (const Tango::DeviceAttribute& value);

  //! \brief Sets a new attribute's error.
  //! 
  //! \param error The new error.
  void set_error (const Tango::DevFailed& error);
  
private:
  //- thread safety
  yat::Mutex _lock;
  
  //- the attribute value
  Tango::DeviceAttribute _val;
  
  //- true if any error (e.g. during last value update), false otherwise
  bool _has_error;
  
  //- tango error storage 
  Tango::DevFailed _error;
};

}	//- namespace

#endif //-	_MONITORED_ATTRIBUTE_H_

