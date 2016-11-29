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

#ifndef _YAT_CALLBACK_H_
#define _YAT_CALLBACK_H_

#include <yat/CommonHeader.h>

namespace yat 
{

// ============================================================================
//! \class CallbackContainer 
//! \brief Generic callback container class.
//!
//! This is the base class for generic callback management.
//! This abstract class can not be used as this and must be derived.
// ============================================================================
template <typename P>
class CallbackContainer
{
public:
  //! \brief Destructor.
  virtual ~CallbackContainer()
  {};

  //! \brief Executes operation with \<P\> as argument.
  //! \param p Template type \<P\> argument.
  virtual void operator() (P) const = 0;

  //! \brief Deep copy.
  virtual CallbackContainer<P>* clone () const = 0;

  //! \brief Comparison operator with another CallbackContainer\<P\> object.
  virtual bool is_equal (const CallbackContainer<P> *) const = 0;
};

// ============================================================================
//! \class Callback 
//! \brief Generic callback class.
//!
//! This template class has a CallbackContainer member.
// ============================================================================
template <typename P> 
class Callback
{
public:
  //! \brief Constructor from a CallbackContainer object.
  //! \param c CallbackContainer object.
  Callback (CallbackContainer<P>* c)
    : container(c)
  {};

  //! \brief Copy constructor.
  //! \param c The source object.
  Callback (const Callback<P>& c)
    : container(c.container ? c.container->clone() : 0)
  {};

  //! \brief Destructor.
  ~Callback ()
  {
    delete this->container;
    this->container = 0;
  };

  //! \brief operator=.
  //! \param callback The source object.
  Callback<P>& operator= (const Callback<P>& callback)
  {
    if (this != &callback)
    {
      delete this->container;
      this->container =  callback.container ? callback.container->clone() : 0;
    }
    return *this;
  };

  //! \brief Executes operation defined in body.
  //! \param p Generic type \<P\> argument.
  //! \exception NULL_POINTER Thrown when the callback is null.
  void operator() (P p) 
  {
    if (this->container)
      (*this->container)(p);
    else
      THROW_YAT_ERROR("NULL_POINTER", "null/empty callback called", "Callback::operator()");
  }
  
  //! \brief Is the callback empty?  
  //!
  //! Returns true if the callback is null, false otherwise.
  bool is_empty () const
  {
    return this->container == 0;
  };

  //! \brief operator==.
  //!
  //! Compares the CallbackContainer\<P\> objects.
  //! \param s The source object.
  bool operator== (const Callback<P>& s) const
  {
    if (this->container == 0 && s.container == 0)
      return true;

    if (this->container == 0 || s.container == 0)
      return false;

    return this->container->is_equal(s.container);
  }

private:
  CallbackContainer<P>* container;
};

// ============================================================================
//! \class FunctionCallbackContainer 
//! \brief Generic callback class for a 'free' function.
//!
//! This template class provide an interface for a generic callback that uses a
//! specified 'free' function.
//! Inherits from CallbackContainer class.
//!
//! The 'free' function is defined by:
//! - a generic type \<P\> for the 'function argument type,
//! - a generic type \<F\> for the function type.
//! 
//! \b USAGE:
//! - %Callback definition:\n
//! For the callback definition, use the YAT MACRO, which defines the three types of 
//! callbacks derived from FunctionCallbackContainer, MemberCallbackContainer and Callback 
//! classes.\n
//! This MACRO also defines *instanciate()* functions providing a way to connect the callback to 
//! a 'free' function or a member function of an object.\n
//! For instance:
//! \verbatim YAT_DEFINE_CALLBACK(myCallbackType, myCallbackDataType &); \endverbatim
//! The previous line defines the following callback prototype: 
//! \verbatim myCallbackType CBk; \endverbatim
//! that will refer to the following function prototype:
//! \verbatim void myFunction(myCallbackDataType& param) \endverbatim
//! The callback data type must alos be defined. For instance:
//! \verbatim struct myCallbackDataType { ... }; \endverbatim
//!
//! - %Callback instantiation: \n
//! Example for a 'free' function:
//! \verbatim CBk = yat::myCallbackType::instanciate(&myFunction); \endverbatim
//! where myFunction has the following prototype:
//! \verbatim void myFunction(myCallbackDataType& param) \endverbatim
//! Example for a member function:
//! \verbatim CBk = yat::myCallbackType::instanciate(*this, &myClass::myFunction); \endverbatim
//! where myFunction has the following prototype:
//! \verbatim void myClass::myFunction(myCallbackDataType& param) \endverbatim
//!
// ============================================================================
template <typename P, class F>
class FunctionCallbackContainer : public CallbackContainer<P>
{
public:
  //! \brief Constructor taking a function pointer.
  //! \param function Pointer on callback function (generic type \<F\>).
  FunctionCallbackContainer (const F & function)
    : function_(function)
  {}

  //! \brief Executes operation: calls the function.
  //! \param p Generic type \<P\> argument of the function.
  void operator() (P p) const 
  { 
    if (function_) 
      function_(p);
  }

  //! \brief Comparison with another FunctionCallbackContainer object.
  //!
  //! Compares the pointers to callback functions.
  //! \param s The source object.
  bool _is_equal (const FunctionCallbackContainer* s) const
  {
    return this->function_ == s->function_;
  }

  //! \brief Comparison with a CallbackContainer object.
  //!
  //! Compares the callback functions if \<s\> type is compatible
  //! with FunctionCallbackContainer.
  //! \param s The source object.
  bool is_equal (const yat::CallbackContainer<P>* s) const
  {
    const FunctionCallbackContainer* fcc = dynamic_cast<const FunctionCallbackContainer*>(s);
    
    return fcc ? this->_is_equal(fcc) : false;
  }

private:
  //- the callback function
  const F function_;

  //- no assignment operator
  FunctionCallbackContainer& operator=(FunctionCallbackContainer&);
};

// ============================================================================
//! \class MemberCallbackContainer 
//! \brief Generic callback class for 'member' functions.
//!
//! This template class provide a callback interface that uses a member function.
//! Inherits from CallbackContainer class. \n
//! 
//! The class and member function are defined by:
//! - a generic type \<P\> for the member function argument type,
//! - a generic type \<C\> for the class type,
//! - a generic type \<M\> for the member function type.\n
//! 
//! \b USAGE:
//! - %Callback definition:\n
//! For the callback definition, use the YAT MACRO, which defines the three types of 
//! callbacks derived from FunctionCallbackContainer, MemberCallbackContainer and Callback 
//! classes. \n 
//! This MACRO also defines *instanciate()* functions providing a way to connect the callback to 
//! a 'free' function or a member function of an object.\n
//! For instance:
//! \verbatim YAT_DEFINE_CALLBACK(myCallbackType, myCallbackDataType &); \endverbatim
//! The previous line defines the following callback prototype: 
//! \verbatim myCallbackType CBk; \endverbatim
//! that will refer to the following function prototype:
//! \verbatim void myFunction(myCallbackDataType& param) \endverbatim
//! The callback data type must alos be defined. For instance:
//! \verbatim struct myCallbackDataType { ... }; \endverbatim
//!
//! - %Callback instantiation:\n
//! Example for a 'free' function:
//! \verbatim CBk = yat::myCallbackType::instanciate(&myFunction); \endverbatim
//! where myFunction has the following prototype:
//! \verbatim void myFunction(myCallbackDataType& param) \endverbatim
//! Example for a member function:
//! \verbatim CBk = yat::myCallbackType::instanciate(*this, &myClass::myFunction); \endverbatim
//! where myFunction has the following prototype:
//! \verbatim void myClass::myFunction(myCallbackDataType& param) \endverbatim
//!
// ============================================================================
template <typename P, class C, class M>
class MemberCallbackContainer : public CallbackContainer<P>
{
public:
  //! \brief Member function type.
  typedef void (C::*PMEMBERFUNC)(P);


  //! \brief Constructor taking a function pointer.
  //! \param client The class.
  //! \param member The function which belongs to the class.
  MemberCallbackContainer (C& client, M member)
    : client_(client), member_(member)
  {}

  //! \brief Executes operation: calls the function.
  //! \param p Generic type \<P\> argument of the class function.
  void operator() (P p) const 
  { 
    if (member_) 
      (client_.*member_)(p);
  }

  //! \brief Comparison with another MemberCallbackContainer object.
  //! 
  //! Compares member functions.
  //! \param s The source object.
  bool _is_equal (const MemberCallbackContainer* s) const
  {
    if (&this->client_ != &s->client_)
      return false;

    return member_ == s->member_;
  }

  //! \brief Comparison with a CallbackContainer object.
  //!
  //! Compares the member functions if \<s\> type is compatible
  //! with MemberCallbackContainer.
  //! \param s The source object.
  bool is_equal (const yat::CallbackContainer<P>* s) const
  {
    const MemberCallbackContainer* mcc = dynamic_cast<const MemberCallbackContainer*>(s);

    return mcc ? this->_is_equal(mcc) : false;
  }

private:
  //- The object the method function belongs to.
  C& client_;

  //- The method to call.
  PMEMBERFUNC member_;

  //- no assignment operator.
  MemberCallbackContainer& operator=(MemberCallbackContainer&);
};

// ============================================================================
//! YAT_DEFINE_CALLBACK: the magical macro.
// ============================================================================
#define YAT_DEFINE_CALLBACK(CallbackClass, CallbackArg) \
  template <typename Function> \
  class Function_##CallbackClass##Container : public yat::FunctionCallbackContainer<CallbackArg, Function> \
  { \
    typedef yat::FunctionCallbackContainer<CallbackArg, Function> InHerited; \
  public: \
    Function_##CallbackClass##Container (Function f) \
     : InHerited(f) \
    {} \
    yat::CallbackContainer<CallbackArg>* clone() const \
    { \
      return new Function_##CallbackClass##Container(*this); \
    } \
  }; \
  template <typename Client, class Member> \
  class Member_##CallbackClass##Container : public yat::MemberCallbackContainer<CallbackArg, Client, Member> \
  { \
    typedef yat::MemberCallbackContainer<CallbackArg, Client, Member> InHerited; \
  public: \
    Member_##CallbackClass##Container (Client& c, Member m) \
      : InHerited(c, m) \
    {}\
    yat::CallbackContainer<CallbackArg>* clone() const \
    { \
      return new Member_##CallbackClass##Container(*this); \
    } \
  }; \
  class CallbackClass : public yat::Callback<CallbackArg> \
  { \
    typedef yat::Callback<CallbackArg> InHerited; \
  public: \
    CallbackClass (yat::CallbackContainer<CallbackArg>* cc = 0) \
      : InHerited(cc) \
    {} \
    CallbackClass (const CallbackClass& cb) \
      : InHerited(cb) \
    {} \
    CallbackClass& operator= (const CallbackClass& cb) \
    { \
      InHerited::operator =(cb); \
      return *this; \
    } \
    template <typename Function> \
    static CallbackClass instanciate (Function f) \
    { \
      return new Function_##CallbackClass##Container<Function>(f); \
    } \
    template <typename Client, class Member> \
    static CallbackClass instanciate (Client& c, Member m) \
    { \
      return new Member_##CallbackClass##Container<Client, Member>(c, m); \
    } \
  }; 

} // namespace

#endif



