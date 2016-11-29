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

#ifndef _YAT_ANY_H_
#define _YAT_ANY_H_

// ============================================================================
//! \page genericPage Generic containers documentation
//! \tableofcontents
//! These utilities provide a set of container classes. \n
//!
//! \section secA1 Container utilities
//! The container utilities implement a generic container class named Any.
//! The GenericContainer is the first implementation of a generic container and is
//! now only used by Message class.
//!
//! \section secA2 Generic container classes
//! Links to container classes : \n
//!   - yat::Any
//!   - yat::Container
//!   - yat::GenericContainer
//! 
// ============================================================================

#include <typeinfo>
#include <yat/CommonHeader.h>

namespace yat
{
// ============================================================================
//! \class Any 
//! \brief Basic container implementation.
//!
//! This class provides a basic container implementation, with elementary  
//! manipulation functions (eg swap, clear, copy, operators...) and cast functions.
// ============================================================================
    class Any
    {
    public:
        //! \brief Default constructor.
        //!
        //! Content initialized to NULL value.
        Any ()
          : m_content(0)
        {
          //- noop ctor
        }

        //! \brief Constructor from \<ValueType\> type.
        //! \param value \<ValueType\> type value.
        template<typename ValueType> 
        Any (const ValueType & value)
          : m_content (new Holder<ValueType>(value))
        {
          //- noop ctor
        }

        //! \brief Copy constructor.
        //! \param src The source object.
        Any (const Any & src)
          : m_content (src.m_content ? src.m_content->clone() : 0)
        {
          //- noop ctor
        }

        //! \brief Destructor.
        //!
        //! Deletes content.
        ~Any()
        {
          delete m_content;
        }

        //! \brief Swaps *this* content with specified one.
        //! \param src The source content.
        Any & swap (Any & src)
        {
            std::swap(m_content, src.m_content);
            return *this;
        }

        //! \brief Operator= template function.
        //! \param src The source value to affect to *this* content.
        template<typename ValueType> 
        Any & operator= (const ValueType & src)
        {
            Any(src).swap(*this);
            return *this;
        }

        //! \brief Operator=.
        //! \param src The source content.
        Any & operator= (const Any & src)
        {
            Any(src).swap(*this);
            return *this;
        }

        //! \brief Tells if content is NULL value or not.
        //! 
        //! Returns true if *this* content is NULL value, false otherwise.
        bool empty() const
        {
            return ! m_content;
        }

        //! \brief Clears content.
        //!
        //! Sets content to NULL value.
        void clear ()
        {
          Any().swap(*this);
        }

        //! \brief Gets the real type of content.
        //!
        //! Returns the type of *this* content (or void if content is NULL).
        const std::type_info & type() const
        {
            return m_content ? m_content->type() : typeid(void);
        }

        // ============================================================================
        //! \class Placeholder 
        //! \brief Pure virtual container class : has to be implemented.
        // ============================================================================
        class Placeholder
        {
        public:
            //! \brief Destructor.
            virtual ~Placeholder() 
            {
              //- noop ctor
            }
        public:
            //! \brief Gets the real type of content. 
            //!
            //! Virtual function : has to be implemented.
            virtual const std::type_info & type() const = 0;

            //! \brief Returns a copy of content.
            //!
            //! Virtual function : has to be implemented.
            virtual Placeholder * clone() const = 0;
        };

        // ============================================================================
        //! \class Holder 
        //! \brief Template class implementing a PlaceHoleder content of \<ValueType\> type data.
        // ============================================================================
        template<typename ValueType>
        class Holder : public Placeholder
        {
        public:
            //! \brief Constructor from \<ValueType\> type.
            //! \param value \<ValueType\> type value.
            Holder (const ValueType & value)
              : m_held (value)
            {
              //- noop ctor
            }
      
            //! \brief Gets the real type of content. 
            virtual const std::type_info & type() const
            {
              return typeid(ValueType);
            }

            //! \brief Returns a copy of content.
            virtual Placeholder * clone() const
            {
                return new Holder(m_held);
            }

            //! \brief Container content.
            ValueType m_held;
        };
    
        //! \brief Pointer to content.
        Placeholder * m_content;
    };

    //! \brief Cast function from Any type to \<ValueType\> type.
    //! 
    //! If \<operand\> is NULL pointer or \<operand\> type is not compatible
    //! with \<ValueType\>, returns 0.
    //! \param operand Pointer to value to cast.
    template<typename ValueType>
    ValueType * any_cast (Any * operand)
    {
        return operand && operand->type() == typeid(ValueType)
                  ? &static_cast<Any::Holder<ValueType> *>(operand->m_content)->m_held
                  : 0;
    }

    //! \brief Cast function from Any type to \<ValueType\> type.
    //! \brief Supposed to be used when trying to cast something exchanged between to RTTI contexts (e.g. a process and a shared lib)
    //! 
    //! If \<operand\> is NULL pointer or \<operand\> type is not compatible
    //! with \<ValueType\>, returns 0.
    //! \param operand Pointer to value to cast.
    template<typename ValueType>
    ValueType * any_cast_ext (Any * operand)
    {
        return operand 
            && static_cast<std::string>(operand->type().name()) == static_cast<std::string>(typeid(ValueType).name())
             ? &static_cast<Any::Holder<ValueType> *>(operand->m_content)->m_held
             : 0;
    }

    //! \brief Cast function from *const* Any type to *const* \<ValueType\> type.
    //! \brief Supposed to be used when trying to cast something exchanged between to RTTI contexts (e.g. a process and a shared lib)
    //! 
    //! If \<operand\> is NULL pointer or \<operand\> type is not compatible
    //! with \<ValueType\>, returns 0.
    //! \param operand Pointer to value to cast.
    template<typename ValueType>
    ValueType * any_cast_ext (const Any * operand)
    {
        return any_cast_ext<ValueType>(const_cast<Any *>(operand));
    }

    //! \brief Cast function from *const* Any type to *const* \<ValueType\> type.
    //! 
    //! If \<operand\> is NULL pointer or \<operand\> type is not compatible
    //! with \<ValueType\>, returns 0.
    //! \param operand Pointer to const value to cast.
    template<typename ValueType>
    const ValueType * any_cast (const Any * operand)
    {
        return any_cast<ValueType>(const_cast<Any *>(operand));
    }

    //! \brief Cast function from *const* Any type to *const* \<ValueType\> type.
    //! \param operand Const value to cast.
    //! \exception yat::Exception Thrown if cast conversion fails.
    template<typename ValueType>
    const ValueType & any_cast (const Any & operand)
    {
        const ValueType * result = any_cast<ValueType>(&operand);
    
        if (! result)
          THROW_YAT_ERROR(_CPTC("yat::any_cast error"),
                          _CPTC("yat::any_cast conversion failed"),
                          _CPTC("yat::any_cast"));
      
        return *result;
    }

    //! \brief Cast function from *const* Any type to *const* \<ValueType\> type.
    //! \brief Supposed to be used when trying to cast something exchanged between to RTTI contexts (e.g. a process and a shared lib)
    //! 
    //! \param operand Const value to cast.
    //! \exception yat::Exception Thrown if cast conversion fails.
    template<typename ValueType>
    const ValueType & any_cast_ext (const Any & operand)
    {
        const ValueType * result = any_cast_ext<ValueType>(&operand);
    
        if (! result)
          THROW_YAT_ERROR(_CPTC("yat::any_cast error"),
                          _CPTC("yat::any_cast conversion failed"),
                          _CPTC("yat::any_cast"));
      
        return *result;
    }

    //! \brief Cast function from Any type to \<ValueType\> type.
    //! \param operand Value to cast.
    //! \exception yat::Exception Thrown if cast conversion fails.
    template<typename ValueType>
    ValueType & any_cast (Any & operand)
    {
        ValueType * result = any_cast<ValueType>(&operand);
    
        if (! result)
          THROW_YAT_ERROR(_CPTC("yat::any_cast error"),
                          _CPTC("yat::any_cast conversion failed"),
                          _CPTC("yat::any_cast"));
      
        return *result;
    }

    //! \brief Cast function from Any type to \<ValueType\> type. 
    //! \brief Supposed to be used when trying to cast something exchanged between to RTTI contexts (e.g. a process and a shared lib)
    //! \param operand Value to cast.
    //! \exception yat::Exception Thrown if cast conversion fails.
    template<typename ValueType>
    ValueType & any_cast_ext (Any & operand)
    {
        ValueType * result = any_cast_ext<ValueType>(&operand);
    
        if ( ! result )
          THROW_YAT_ERROR(_CPTC("yat::any_cast error"),
                          _CPTC("yat::any_cast conversion failed"),
                          _CPTC("yat::any_cast_ext"));
      
        return *result;
    }


    //! \brief Cast function from Any type to \<ValueType\> type.
    //!
    //! No test of type compatibility is made.
    //! \param operand Pointer to value to cast.
    template<typename ValueType>
    inline ValueType * unsafe_any_cast (Any * operand)
    {
        return &static_cast<Any::Holder<ValueType> *>(operand->m_content)->m_held;
    }

    //! \brief Cast function from const Any type to const \<ValueType\> type.
    //!
    //! No test of type compatibility is made.
    //! \param operand Pointer to const value to cast.
    template<typename ValueType>
    inline const ValueType * unsafe_any_cast (const Any * operand)
    {
        return unsafe_any_cast<ValueType>(const_cast<Any *>(operand));
    }
  
} //- namespace

#endif //- _YAT_ANY_H_



