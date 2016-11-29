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

// ============================================================================
/*! \page DynAttrPage Dynamic Attribute Example

	\verbatim 

   class MyClass : public Tango::LogAdapter
   {
     public:
       //------------------------------------------------------------------ 
       MyClass (Tango::DeviceImpl * d)
         : Tango::LogAdapter(d), m_dam (d)
       {
          this->init_dynamic_attributes();
       }
       //------------------------------------------------------------------ 
       ~MyClass ()
       {
          this->remove_dynamic_attributes();
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
          //- add the dynamic attribute to the device interface
          this->m_dam.add_attribute(dai);
        }
       //------------------------------------------------------------------ 
       void remove_dynamic_attributes ()
        {
          //- this method must be executed by a Tango thread!
          //-------------------------------------------------
          this->m_dam.remove_attributes();
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
        yat4tango::DynamicAttributeManager m_dam;
   }; 

	\endverbatim
*/

// ============================================================================

#ifndef _DYNAMIC_ATTRIBUTE_H_
#define _DYNAMIC_ATTRIBUTE_H_

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

//! Used as default value for 'local' attribute names.
extern const std::string KeepOriginalAttributeName;

// ============================================================================
// class: DynamicAttribute (forward declaration)
// ============================================================================
class DynamicAttribute;

// ============================================================================
//! \struct DynamicAttributeCallbackData 
//! \brief Read & write callback argument.
//!
// ============================================================================
struct YAT4TANGO_DECL DynamicAttributeCallbackData
{
  //! \brief The Tango device to which the dynamic attribute is attached.
  Tango::DeviceImpl * dev;

  //! \brief The dynamic attribute for which the callback is called. 
  DynamicAttribute * dya;

  //! \brief Constructor.
  DynamicAttributeCallbackData ()
    : dev (0), dya(0)
  {}

  //! \brief Copy constructor.
  DynamicAttributeCallbackData (const DynamicAttributeCallbackData& src)
    : dev(src.dev), dya(src.dya)
  {}

  //! \brief Destructor.
  ~DynamicAttributeCallbackData ()
  {}
};

// ============================================================================
//! \struct DynamicAttributeReadCallbackData 
//! \brief Read callback argument.
//!
//! Inherits from DynamicAttributeCallbackData class.
// ============================================================================
struct YAT4TANGO_DECL DynamicAttributeReadCallbackData : public DynamicAttributeCallbackData
{
  //! \brief The Tango::Attribute on which a \<set_value\> is supposed to be done.
  Tango::Attribute * tga;

  //! \brief Constructor.
  DynamicAttributeReadCallbackData ()
    : DynamicAttributeCallbackData(), tga(0)
  {}

  //! \brief Copy constructor.
  DynamicAttributeReadCallbackData (const DynamicAttributeReadCallbackData& src)
    : DynamicAttributeCallbackData(src), tga(src.tga)
  {}

  //! \brief Destructor.
  ~DynamicAttributeReadCallbackData ()
  {}
};

// ============================================================================
//! \struct DynamicAttributeWriteCallbackData 
//! \brief Write callback argument.
//!
//! Inherits from DynamicAttributeCallbackData class.
// ============================================================================
struct YAT4TANGO_DECL DynamicAttributeWriteCallbackData : public DynamicAttributeCallbackData
{
  //! \brief The Tango::WAttribute on which \<get_write_value\> is supposed to be called.
  Tango::WAttribute * tga;

  //! \brief Constructor.
  DynamicAttributeWriteCallbackData ()
    : DynamicAttributeCallbackData(), tga(0)
  {}

  //! \brief Copy constructor.
  DynamicAttributeWriteCallbackData (const DynamicAttributeWriteCallbackData& src)
    : DynamicAttributeCallbackData(src), tga(src.tga)
  {}

  //! \brief Destructor.
  ~DynamicAttributeWriteCallbackData ()
  {}
};

// ============================================================================
//! Defines DynamicAttributeReadCallback callback type.
// ============================================================================
YAT_DEFINE_CALLBACK(DynamicAttributeReadCallback, 
                    DynamicAttributeReadCallbackData &);

// ============================================================================
//! Defines DynamicAttributeWriteCallback callback type.
// ============================================================================
YAT_DEFINE_CALLBACK(DynamicAttributeWriteCallback, 
                    DynamicAttributeWriteCallbackData &);

// ============================================================================
//! \struct DynamicAttributeInfo 
//! \brief Dynamic attribute information.
//!
//! This class defines some attribute's information and a way to access them.
//!
// ============================================================================
struct YAT4TANGO_DECL DynamicAttributeInfo
{
  //! \brief Default constructor.
  DynamicAttributeInfo ();

  //! \brief Copy constructor.
  //! \param s The source information.
  DynamicAttributeInfo (const DynamicAttributeInfo& s);

  //! \brief Assignment operator.
  //! \param s The source information.
  const DynamicAttributeInfo& operator= (const DynamicAttributeInfo& s);

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

  //! \brief Gets Tango data type as a humanly readable string.
  const char * get_tango_data_type_str () const;

  //! \brief Gets Tango access (i.e. read/write) as a humanly readable string.
  const char * get_tango_data_access_str () const;

  //! \brief Gets Tango data format as a humanly readable string.
  const char * get_tango_data_format_str () const;

  //! \brief Dumps *this* instance to std::cout.
  void dump () /*const*/;

  //! \brief The Tango device to which the dynamic attribute belongs to.
  //!
  //! Default value is to NULL.
  Tango::DeviceImpl * dev;
  
  //! \brief The 'local' attribute name.
  //!
  //! Default value is yat4tango::KeepOriginalAttributeName.
  //! - For a simple dynamic attribute: always equals the name specified by 
  //! 'tai.name' (Tango::AttributeInfo member).
  //! - For a forwarded attribute: provides a way to specify the local name of 
  //! the forwarded (or remote) attribute.
  std::string lan;
  
  //! \brief The Tango attribute description
  Tango::AttributeInfoEx tai;

  //! \brief Read callback.
  DynamicAttributeReadCallback rcb;
  
  //! \brief Write callback .
  DynamicAttributeWriteCallback wcb;

  //! \brief Is this attribute memorized?
  //!
  //! Default value is false.
  bool memorized;

  //! \brief Should we write the memorized attribute at init?
  //!
  //! Default value is false.
  bool write_memorized_value_at_init;

  //! \brief Polling period in ms
  //!
  //! Default value is 0 (disable).
  size_t polling_period_in_ms;

  //! \brief Should we cleanup the attribute properties from Tango 
  //! database when removing this dynamic attribute?
  //!
  //! Default value is true.
  bool cdb;
  
  //! \brief Does the user code fire data ready events for this attribute?
  //!
  //! Default value is false.
  bool data_ready_pushed_by_user_code;

  //! \brief Does the user code fire change events for this attribute?
  //!
  //! Default value is false.
  bool change_evt_pushed_by_user_code;

  //! \brief Does the user code manage the change event for this attribute or let the Tango kernel do the job?
  //!
  //! Default value is false.
  bool change_evt_managed_by_user_code;

  //! \brief Does the user code fire archive events for this attribute?
  //!
  //! Default value is false.
  bool archive_evt_pushed_by_user_code;

  //! \brief Does the user code manage the archive event for this attribute or let the Tango kernel do the job?
  //!
  //! Default value is false.
  bool archive_evt_managed_by_user_code;

private:
  friend class DynamicAttributeManager;
  friend class DynamicAttributeFactory;
  
  //- user 'abstract' data
  yat::Any uad;
}; 

// ============================================================================
//! \struct ForwardedAttributeInfo 
//! \brief Forwarded attribute information.
//!
// ============================================================================
struct YAT4TANGO_DECL ForwardedAttributeInfo
{
  //! \brief Default constructor.
  ForwardedAttributeInfo ();
  
  //! \brief Copy constructor.
  ForwardedAttributeInfo (const ForwardedAttributeInfo& s);

  //! \brief Assignment operator.
  //! \param s The source information.
  const ForwardedAttributeInfo& operator= (const ForwardedAttributeInfo& s);
  
  // \brief The Tango device to which the dynamic attribute belongs to.
  //!
  //! Default value is NULL.
  Tango::DeviceImpl * dev;
  
  //! \brief The fully qualified name or alias of the attribute to be forwarded.
  //!
  //! Default value is empty string.
  std::string url;
  
  //! \brief The 'local' attribute name used to forward \<url\>.
  //!
  //! Default value is yat4tango::KeepOriginalAttributeName.
  std::string lan;

  //! \brief Should the attribute access be restricted to read-only?
  //!
  //! Default value is false.
  bool rdo;
};

// ============================================================================
//! \class DynamicAttribute 
//! \brief The Yat4Tango dynamic attribute.
//!
//! This class provides an interface to manage a dynamic attribute through the
//! following functions:
//! - attachment of read and write callbacks,
//! - access to Tango attribute's features,
//! - access to forwarded attribute's features (if set).
//!
//! This class can not be used as this and must be derived.
//!
//! An example of dynamic attribute usage is given here: \ref DynAttrPage "Dynamic attribute example"
//!
//! Inherits from Tango::LogAdapter class (for logging management).
//!
// ============================================================================
class YAT4TANGO_DECL DynamicAttribute : public Tango::LogAdapter
{
public:

  //! \brief Attaches the specified read callback to the dynamic attribute.
  //! \param rcb Reference to the read callback.
  void set_read_callback (const DynamicAttributeReadCallback & rcb);

  //! \brief Attaches the specified write callback to the dynamic attribute.
  //! \param wcb Reference to the write callback.
  void set_write_callback (const DynamicAttributeWriteCallback & wcb);

  //! \brief Returns the underlying Tango attribute.
  //!
  //! This virtual function has to be implemented in derived class.
  virtual Tango::Attr * get_tango_attribute () = 0;

  //! \brief Returns the attribute's name.
  inline const std::string & get_name () const
  {
    return this->dai_.tai.name;
  }

  //! \brief Returns the attribute's data format.
  //!
  //! Can be one of the following:
  //! - Tango::SCALAR,
  //! - Tango::SPECTRUM,
  //! - Tango::IMAGE,
  //! - Tango::FMT_UNKNOWN.
  inline int get_tango_data_format () const
  {
    return static_cast<int>(this->dai_.tai.data_format);
  }

  //! \brief Returns the attribute's data format as a humanly readable string.
  //! 
  //! Can be one of the following:
  //! - "SCALAR",
  //! - "SPECTRUM",
  //! - "IMAGE",
  //! - "UNKNOWN_TANGO_DATA_FORMAT".
  const char * get_tango_data_format_str () const;

  //! \brief Returns the attribute's data type.
  //! 
  //! See \p "server/tango_const.h/CmdArgType" enumeration for details upon possible return values.
  inline int get_tango_data_type () const
  {
    return this->dai_.tai.data_type;
  }

  //! \brief Returns the attribute's data type as a humanly readable string.
  //! 
  //! See server/tango_const.h/CmdArgType enumeration for details upon possible return values
  //! (the name of the enumeration value is returned, for instance: "DEV_SHORT" for a short data
  //! type).
  const char * get_tango_data_type_str () const;

  //! \brief Returns the attribute's data access.
  //!
  //! Can be one of the following:
  //! - Tango::READ,
  //! - Tango::READ_WITH_WRITE,
  //! - Tango::WRITE,
  //! - Tango::READ_WRITE.
  inline int get_tango_data_access () const
  {
    return static_cast<int>(this->dai_.tai.writable);
  }

  //! \brief Returns the attribute's data access as a humanly readable string.
  //! 
  //! Can be one of the following:
  //! - "READ",
  //! - "READ_WITH_WRITE",
  //! - "WRITE",
  //! - "READ_WRITE",
  //! - "UNKNOWN_TANGO_RW_ACCESS_TYPE".
  const char * get_tango_data_access_str () const;

  //! \brief Returns the associated Tango::AttributeInfo (i.e. attribute description).
  inline const Tango::AttributeInfo & get_attribute_info () const
  {
    return this->dai_.tai;
  }

  //! \brief Checks if attribute is writable.
  //! Returns true if the dynamic attribute is writable, false otherwise.
  inline bool is_writable () const
  {
    return this->dai_.tai.writable == Tango::WRITE
        || this->dai_.tai.writable == Tango::READ_WRITE
        || this->dai_.tai.writable == Tango::READ_WITH_WRITE;
  }

  //! \brief Checks if attribute is forwarded.
  //! Returns true if the dynamic attribute is a forwarded attribute, false otherwise.
  inline bool is_forwarded () const
  {
    return this->dp_ ? true : false;
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

  //! \brief Checks whether or not the attribute data type is T.
  //! Returns true if the attribute data type is T, returns false otherwise.
  template <typename T> bool check_attribute_data_type () const;

  //! \brief Dumps attribute information to std::cout.
  void dump () /*const*/ ;

protected:
  //! \brief Constructor.
  // \param dai Dynamic attribute information.
  DynamicAttribute (const DynamicAttributeInfo & dai);

  //! \brief Destructor.
  virtual ~DynamicAttribute();

  //! \brief Links a forwarded attribute to its "remote" Tango device.
  //! \param dp The Tango device proxy.
  void link_to_proxy (Tango::DeviceProxy * dp);

  //! \brief The associated dynamic attribute information.
  DynamicAttributeInfo dai_;

  //! \brief The associated device proxy.
  //!
  //! For forwarded attribute only - NULL otherwise.
  Tango::DeviceProxy * dp_;
  
  //! \brief The associated attribute value.
  //!
  //! For forwarded attribute only - NULL otherwise.
  Tango::DeviceAttribute * da_;

  //! \brief Attach user defined properties to the dynamic attribute.
  //! \param dai The attribute info
  
private:
  friend class DynamicAttributeManager;
  friend class DynamicAttributeFactory;
  
  //- default forwarded attr read callback
  void fwd_attr_read (DynamicAttributeReadCallbackData& cbd);
   
  //- forwards attribute data for reading
  void fwd_attr_read_data (Tango::Attribute& ta, Tango::DeviceAttribute& da);
    
  //- default forwarded attr write callback
  void fwd_attr_write (DynamicAttributeWriteCallbackData& cbd);

  //- forwards attribute data for writting
  void fwd_attr_write_data (Tango::WAttribute & wa, Tango::DeviceAttribute& da);
 
  //- removes the attribute properties from the Tango db
  void cleanup_properties ();

  //- not allowed methods
  DynamicAttribute (DynamicAttribute&);
  DynamicAttribute&  operator= (DynamicAttribute&);
};

// ============================================================================
//! \class DynamicAttributeFactory 
//! \brief The dynamic attribute factory.
//!
//! This class provides an interface to create generic dynamic attributes.
//! It is used by friend class yat4tango::DynamicAttributeManager implementation.
//!
//! Inherits from yat::Singleton class (for the "singleton" pattern).
//!
// ============================================================================
class YAT4TANGO_DECL DynamicAttributeFactory : public yat::Singleton<DynamicAttributeFactory>
{
  friend class DynamicAttributeManager;

private:
  //- dynamic attribute factory
  //- i: the Tango attribute info (name, format, type, ...) 
  DynamicAttribute * create_attribute (const yat4tango::DynamicAttributeInfo& i);

  //- dynamic attribute factory
  //- i: the forwarded attribute info (host, url, ...) 
  DynamicAttribute * create_attribute (const yat4tango::ForwardedAttributeInfo& i);
};

// ============================================================================
// DynamicAttributeInfo::set_user_data
// ============================================================================
template <typename T>
void DynamicAttributeInfo::set_user_data (T* d)
{
  uad = d;
}

// ============================================================================
// DynamicAttributeInfo::set_user_data
// ============================================================================
template <typename T>
void DynamicAttributeInfo::set_user_data (const T & d)
{
  uad = d;
}

// ============================================================================
// DynamicAttributeInfo::get_user_data
// ============================================================================
template <typename T>
T * DynamicAttributeInfo::get_user_data ()
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
// DynamicAttributeInfo::get_user_data
// ============================================================================
template <typename T>
void DynamicAttributeInfo::get_user_data (T& ud)
{
  ud = yat::any_cast<T>(uad);
}

// ============================================================================
// DynamicAttribute::check_attribute_data_type
// ============================================================================
template <typename T> bool DynamicAttribute::check_attribute_data_type () const
{
  return this->get_tango_data_type() == TangoTraits<T>::type_id;
}
  
// ============================================================================
// DynamicAttribute::set_user_data
// ============================================================================
template <typename T>
void DynamicAttribute::set_user_data (T* d)
{
  this->dai_.set_user_data<T>(d);
}

// ============================================================================
// DynamicAttribute::set_user_data
// ============================================================================
template <typename T>
void DynamicAttribute::set_user_data (const T & d)
{
  this->dai_.set_user_data<T>(d);
}

// ============================================================================
// DynamicAttribute::get_user_data
// ============================================================================
template <typename T>
T* DynamicAttribute::get_user_data ()
{
  return this->dai_.get_user_data<T>();
}

// ============================================================================
// DynamicAttribute::get_argin
// ============================================================================
template <typename T>
void DynamicAttribute::get_user_data (T& ud)
{
  this->dai_.get_user_data<T>(ud);
}
} //- namespace 

#endif //- _DYNAMIC_ATTRIBUTE_H_
