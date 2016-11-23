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
//    Nicolas Leclercq
//    Synchrotron SOLEIL 
//----------------------------------------------------------------------------
/*!
 * \authors See AUTHORS file
 */

// ============================================================================
// DEPENDENCIES
// ============================================================================

namespace yat4tango
{

//-------------------------------------------------------------------------
// PropertyHelper::set_property
//-------------------------------------------------------------------------
template <class T>
void PropertyHelper::set_property(Tango::DeviceImpl* dev_p, const std::string& property_name, T value)
{
  if (!Tango::Util::instance()->_UseDb)
  {
    //- rethrow exception
    Tango::Except::throw_exception(static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                   static_cast<const char*>("NO DB"),
                                   static_cast<const char*>("PropertyHelper::set_property"));
  }  
  
  Tango::DbDatum current_value(property_name);
  current_value << value;
  Tango::DbData db_data;
  db_data.push_back(current_value);
  try
  {
    dev_p->get_db_device()->put_property(db_data);
  }
  catch (Tango::DevFailed &df)
  {
    //- rethrow exception
    Tango::Except::re_throw_exception(df,
                                      static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                      static_cast<const char*>(std::string(df.errors[0].desc).c_str()),
                                      static_cast<const char*>("PropertyHelper::set_property"));
  }
}

//-------------------------------------------------------------------------
// PropertyHelper::get_property
//-------------------------------------------------------------------------
template <class T>
T PropertyHelper::get_property(Tango::DeviceImpl* dev_p, const std::string& property_name)
{
  if (!Tango::Util::instance()->_UseDb)
  {
    //- rethrow exception
    Tango::Except::throw_exception(static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                   static_cast<const char*>("NO DB"),
                                   static_cast<const char*>("PropertyHelper::get_property"));
  }   
  
  T value;
  Tango::DbDatum current_value(property_name);  
  Tango::DbData db_data;
  db_data.push_back(current_value);
  try
  {
    dev_p->get_db_device()->get_property(db_data);
  }
  catch (Tango::DevFailed &df)
  {
    //- rethrow exception
    Tango::Except::re_throw_exception(df,
                                      static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                      static_cast<const char*>(std::string(df.errors[0].desc).c_str()),
                                      static_cast<const char*>("PropertyHelper::get_property"));
  }
  db_data[0] >> value;
  return value;
}

//-------------------------------------------------------------------------
// PropertyHelper::create_property_if_empty
//-------------------------------------------------------------------------
template <class T>
void PropertyHelper::create_property_if_empty(Tango::DeviceImpl* dev_p,
                                              Tango::DbData& dev_prop,
                                              T value, string property_name)
{
  std::size_t iNbProperties = dev_prop.size();
  std::size_t i;
  for (i = 0; i < iNbProperties; i++)
  {
    std::string sPropertyName(dev_prop[i].name);
    if (sPropertyName == property_name)
      break;
  }
  if (i == iNbProperties)
    //# TODO: throwing a DevFailed Exception
    return;
  
  int iPropertyIndex = i;
  
  if (dev_prop[iPropertyIndex].is_empty())
  {
    Tango::DbDatum current_value(dev_prop[iPropertyIndex].name);
    current_value << value;
    Tango::DbData db_data;
    db_data.push_back(current_value);

    try
    {
      dev_p->get_db_device()->put_property(db_data);
    }
    catch (Tango::DevFailed &df)
    {
      //- rethrow exception
      Tango::Except::re_throw_exception(df,
                                        static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                        static_cast<const char*>(std::string(df.errors[0].desc).c_str()),
                                        static_cast<const char*>("PropertyHelper::create_property_if_empty"));
    }
  }
}

#define PROP_VALUE_NAME "__value"

//-------------------------------------------------------------------------
// PropertyHelper::set_memorized_attribute
//-------------------------------------------------------------------------
template <class T>
void PropertyHelper::set_memorized_attribute(Tango::DeviceImpl* dev_p,
                                             const std::string& attribute_name, T val)
{
  if (!Tango::Util::instance()->_UseDb)
  {
    //- rethrow exception
    Tango::Except::throw_exception(static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                    static_cast<const char*>("NO DB"),
                                    static_cast<const char*>("PropertyHelper::set_memorized_attribute"));
  }

  Tango::DbData properties;

  Tango::DbDatum prop_attr_name(attribute_name);
  prop_attr_name << 1L;
  properties.push_back(prop_attr_name);

  Tango::DbDatum prop_name( PROP_VALUE_NAME );
  prop_name << val;

  properties.push_back(prop_name);

  try
  {
    dev_p->get_db_device()->put_attribute_property(properties);
  }
  catch (Tango::DevFailed &df)
  {
    //- rethrow exception
    Tango::Except::re_throw_exception(df,
                                      static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                      static_cast<const char*>(std::string(df.errors[0].desc).c_str()),
                                      static_cast<const char*>("PropertyHelper::create_property_if_empty"));
  }
}

//-------------------------------------------------------------------------
// PropertyHelper::get_memorized_attribute
//-------------------------------------------------------------------------
template <class T>
T PropertyHelper::get_memorized_attribute(Tango::DeviceImpl* dev_p,
                                          const std::string& attribute_name)
{
  if (!Tango::Util::instance()->_UseDb)
  {
    //- rethrow exception
    Tango::Except::throw_exception(static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                   static_cast<const char*>("NO DB"),
                                   static_cast<const char*>("PropertyHelper::get_memorized_attribute"));
  }

  T value;
  Tango::DbData dev_prop;
  dev_prop.push_back(Tango::DbDatum(attribute_name));

  try
  {
    dev_p->get_db_device()->get_attribute_property(dev_prop);
    for( std::size_t i=0; i < dev_prop.size(); ++i )
    {
      dev_prop[i] >> value;
      if( dev_prop[i].name == PROP_VALUE_NAME )
      {
        return value;
      }
    }
    Tango::Except::throw_exception( "NO_DATA", "No value", "PropertyHelper::get_property" );
  }
  catch (Tango::DevFailed &df)
  {
    //- rethrow exception
    Tango::Except::re_throw_exception(df,
                                      static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                      static_cast<const char*>(std::string(df.errors[0].desc).c_str()),
                                      static_cast<const char*>("PropertyHelper::get_property"));
  }
  catch( const std::exception& e )
  {
    Tango::Except::throw_exception( "SYSTEM_ERROR", e.what(), "PropertyHelper::get_property" );
  }
  catch( ... )
  {
    Tango::Except::throw_exception( "UNKNOWN_ERROR", "Unknown error", "PropertyHelper::get_property" );
  }
}

//-------------------------------------------------------------------------
// PropertyHelper::get_memorized_attribute
//-------------------------------------------------------------------------
template <class T>
T PropertyHelper::get_memorized_attribute(Tango::DeviceImpl* dev_p,
                                          const std::string& attribute_name, T default_value)
{
  if (!Tango::Util::instance()->_UseDb)
  {
    //- rethrow exception
    Tango::Except::throw_exception(static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                   static_cast<const char*>("NO DB"),
                                   static_cast<const char*>("PropertyHelper::get_memorized_attribute"));
  }

  T value;
  Tango::DbData dev_prop;
  dev_prop.push_back(Tango::DbDatum(attribute_name));

  try
  {
    dev_p->get_db_device()->get_attribute_property(dev_prop);

    for( std::size_t i=0; i < dev_prop.size(); ++i )
    {
      dev_prop[i] >> value;
      if( dev_prop[i].name == PROP_VALUE_NAME )
      {
        return value;
      }
    }
    return default_value;
  }
  catch (Tango::DevFailed &df)
  {
    //- rethrow exception
    Tango::Except::re_throw_exception(df,
                                      static_cast<const char*>("TANGO_DEVICE_ERROR"),
                                      static_cast<const char*>(std::string(df.errors[0].desc).c_str()),
                                      static_cast<const char*>("PropertyHelper::get_property"));
  }
  catch( const std::exception& e )
  {
    Tango::Except::throw_exception( "SYSTEM_ERROR", e.what(), "PropertyHelper::get_property" );
  }
  catch( ... )
  {
    Tango::Except::throw_exception( "UNKNOWN_ERROR", "Unknown error", "PropertyHelper::get_property" );
  }
}

} // namespace
