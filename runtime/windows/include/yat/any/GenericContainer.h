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

#ifndef _YAT_GENERIC_CONTAINER_H_
#define _YAT_GENERIC_CONTAINER_H_

#include <typeinfo>
#include <yat/CommonHeader.h> 

namespace yat 
{

// ============================================================================
//! \class Container 
//! \brief Basic container class only used for genericity constraints.
// ============================================================================
class YAT_DECL Container
{
public:
  //! \brief Constructor.
  Container ()
  {
    //- noop
  };

  //! \brief Destructor.
  virtual ~Container () 
  {
    //- noop
  };
};

// ============================================================================
//! \class GenericContainer 
//! \brief Generic container implementation.
//!
//! This template class provides a generic container implementation , with elementary
//! content manipulation functions (eg set/get, contructors, operators...) and cast functions.
//!
//! Implementation constraint: class \<T\> must have a copy constructor.
// ============================================================================
template <typename T> 
class GenericContainer : public Container
{
public:

  //! \brief Default constructor.
  GenericContainer () 
    : ptr_(0), own_(false)
  {
    //- noop
  }

  //! \brief Constructor with parameters.
  //! 
  //! No copy is made. Points to specified data and optionnally gets 
  //! specified ownership.
  //! \param _data The source data.
  //! \param _transfer_ownership If set to true, ownership of source data is transfered
  //! to *this*.
  GenericContainer (T* _data, bool _transfer_ownership = true) 
    : ptr_(0), own_(false)
  {
    this->set_content(_data, _transfer_ownership);
  }

  //! \brief Constructor with parameters.
  //! 
  //! Makes a copy of source data.
  //! \param _data The source data.
  GenericContainer (const T& _data) 
    : ptr_(0), own_(false)
  {
    this->set_content(_data);
  }

  //! \brief Copy constructor.
  //! \param _src The source container.
  GenericContainer (const GenericContainer<T>& _src) 
    : ptr_(0), own_(false)
  {
    *this = _src;
  }

  //! \brief Destructor.
  //!
  //! Deletes data if the ownership flag is set to true.
  virtual ~GenericContainer ()
  {
    if (own_) delete ptr_;
  }

  //! \brief Operator=.
  //!
  //! Changes content and makes a copy of the source content.
  //! \param _src The source container.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  const GenericContainer& operator= (const GenericContainer<T>& _src)
  {  
    if (&_src == this)
      return *this;
    if (! ptr_ || ! own_)
    {
      try
      {
        ptr_ = new (std::nothrow) T(_src.content());
        if (! ptr_)
          throw std::bad_alloc();
      }
      catch (const std::bad_alloc&)
      {
        ptr_ = 0;
        own_ = false;
        THROW_YAT_ERROR("OUT_OF_MEMORY",
                        "memory allocation failed",
                        "GenericContainer:operator=");
      }
    }
    else 
    {
      *ptr_ = _src.content();
    }
    own_ = true;
    return *this;
  }

  //! \brief Operator=.
  //!
  //! Changes content and makes a copy of the source data.
  //! \param _src The source data.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  const GenericContainer& operator= (T& _src)
  {
    return this->operator=(static_cast<const T&>(_src));
  }

  //! \brief Operator=.
  //!
  //! Changes content and makes a copy of the source data.
  //! \param _src The const source data.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  const GenericContainer& operator= (const T& _src)
  {
    if (! ptr_ || ! own_)
    {
      try
      {
        ptr_ = new (std::nothrow) T(_src);
        if (! ptr_)
          throw std::bad_alloc();
      }
      catch (const std::bad_alloc&)
      {
        ptr_ = 0;
        own_ = false;
        THROW_YAT_ERROR("OUT_OF_MEMORY",
                        "memory allocation failed",
                        "GenericContainer:operator=");
      }
    }
    else 
    {
      *ptr_ = _src;
    }
    own_ = true;
    return *this;
  }

  //! \brief Operator=.
  //!
  //! Changes content but does NOT get ownership of the data.
  //! \param _data The source data.
  const GenericContainer& operator= (T* _data)
  { 
    if (_data == ptr_)
      return *this;
    if (own_)
      delete ptr_;
    ptr_ = _data;
    own_ = false;
    return *this;
  }

  //! \brief Sets the content of the container.
  //!
  //! Changes content and sets ownership to specified value.
  //! \param _data The source data.
  //! \param _transfer_ownership If set to true, ownership of source data is transfered
  //! to *this*.
  void set_content (T* _data, bool _transfer_ownership)
  {
    *this = _data;
    own_ = _transfer_ownership;
  }

  //! \brief Sets the content of the container (makes a copy).
  //! \param _data The const source data.
  void set_content (const T& _data)
  {
    *this = _data;
  }

  //! \brief Sets the content of the container (makes a copy).
  //! \param _data The source data.
  void set_content (T& _data)
  {
    *this = _data;
    own_ = true;
  }
  
  //! \brief Returns content of the container.
  //!
  //! Does NOT transfer data ownership to the caller.
  T * operator-> ()
  {
    return ptr_;
  }

  //! \brief Returns content of the container.
  //!
  //! Optionally transfers data ownership to the caller.
  //! \param transfer_ownership If set to true, ownership of content is transfered
  //! to the caller.
  T * get_content (bool transfer_ownership)
  {
    if (transfer_ownership)
      own_ = false;
    return ptr_;
  }

  //! \brief Returns content of the container.
  //! \exception RUNTIME_ERROR Thrown if could not extract data from GenericContainer
  //! [empty].
  T & get_content ()
  {
    if (! ptr_)
    {
      THROW_YAT_ERROR("RUNTIME_ERROR",
                      "could not extract data from GenericContainer [empty]",
                      "GenericContainer::get_content");
    }
    return *ptr_;
  }

  //! \brief Does this container have ownership of the underlying data?
  //!
  //! Returns true if container has ownership, false otherwise.
  bool get_ownership () const
  {
    return own_;
  }
  
  //! \brief Does the content of the container is empty ?
  //!
  //! Returns true if the container is empty, false otherwise.
  bool empty () const 
  {
    return ptr_ ? false : true;
  }

private:
  //- Actual container content.
  T * ptr_;

  //- Do we have data ownership?
  bool own_;
};


//! \brief Cast function from Container type to \<T\> type.
//!
//! Returns NULL pointer if bad cast conversion.
//! \param _c Container to cast.
//! \param _transfer_ownership If set to true, ownership of source content is transfered
//! to cast object.
template<typename T>
T * any_cast (Container * _c, bool _transfer_ownership = false)
{
  GenericContainer<T> * gc = dynamic_cast<GenericContainer<T>*>(_c);
  return gc 
       ? gc->get_content(_transfer_ownership)
       : 0;
}

//! \brief Cast function from const Container type to const \<T\> type.
//!
//! Returns NULL pointer if bad cast conversion.
//! \param c Const container to cast.
template<typename T>
const T * any_cast (const Container * c)
{
  return any_cast<T>(const_cast<Container*>(c), false);
}

//! \brief Cast function from const Container type to const \<T\> type.
//!
//! \param c Const container to cast.
//! \exception RUNTIME_ERROR Thrown if could not extract data from GenericContainer 
//! [attached data type is not of specified type].
template<typename T>
const T & any_cast (const Container & c)
{
  const T * t = any_cast<T>(&c);
  if (! t)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from GenericContainer [attached data type is not of specified type]",
                    "yat::any_cast");
  }
  return *t;
}

//! \brief Cast function from Container type to \<T\> type.
//!
//! \param c Container to cast.
//! \exception RUNTIME_ERROR Thrown if could not extract data from GenericContainer 
//! [attached data type is not of specified type].
template<typename T>
T & any_cast (Container & c)
{
  T * t = any_cast<T>(&c, false);
  if (! t)
  {
    THROW_YAT_ERROR("RUNTIME_ERROR",
                    "could not extract data from GenericContainer [attached data type is not of specified type]",
                    "yat::any_cast");
  }
  return *t;
}

} // namespace yat

#endif



