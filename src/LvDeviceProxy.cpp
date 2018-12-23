// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceProxy.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "LvDeviceProxy.h"

#if !defined (_LV_INLINE_)
# include "LvDeviceProxy.i"
#endif 

//=============================================================================
// LvDeviceProxy::LvDeviceProxy
//=============================================================================
LvDeviceProxy::LvDeviceProxy (const std::string& _dev_name)
  : dev_name_(_dev_name),
    dev_commands_(),
    dev_attributes_(),
    dev_proxy_ (0)
{
  //- connect
  connect();
 
  //- enable transparent reconnection
  dev_proxy_->unsafe_proxy().set_transparency_reconnection(true);
  
  //- get device attributes and commands info
  get_device_interface();
}

//=============================================================================
// LvDeviceProxy::LvDeviceProxy
//=============================================================================
LvDeviceProxy::LvDeviceProxy (Tango::DeviceProxy * _dp, bool _ownership)
  : dev_name_(_dp->name()),
    dev_commands_(),
    dev_attributes_(),
    dev_proxy_ (0)
{
  //- instanciate the device proxy
  try
  {
    dev_proxy_ = new ThreadSafeDeviceProxy(_dp, _ownership);
    if ( ! dev_proxy_ )
    {
      Tango::Except::throw_exception(_CPTC_("out of memory"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("LvDeviceProxy::connect"));
    }
  }
  catch (Tango::DevFailed &e)
  {
    std::string d = "failed to create proxy for device " + _dp->dev_name();
    Tango::Except::re_throw_exception(e,
                                      _CPTC_("Tango::DeviceProxy instanciation failed"),
                                      _CPTC_(d.c_str()),
                                      _CPTC_("LvDeviceProxy::connect"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("LvDeviceProxy::connect"));
  }
  
  //- get device attributes and commands info
  get_device_interface();
}

//=============================================================================
// LvDeviceProxy::~LvDeviceProxy
//=============================================================================
LvDeviceProxy::~LvDeviceProxy ()
{
  dev_commands_.clear();
  dev_attributes_.clear();
  disconnect();
}

//=============================================================================
// LvDeviceProxy::check_tango_host
//=============================================================================
void LvDeviceProxy::check_tango_host()
    throw (Tango::DevFailed)
{  
  char* th = getenv ("TANGO_HOST");
  if ( ! th)  {
    Tango::Except::throw_exception(_CPTC_("configuration error"),
                                   _CPTC_("TANGO_HOST environment variable is not set [a TANGO_HOST=<host>:<port> env. var. should point to your Tango database]"),
                                   _CPTC_("LvDeviceProxy::check_tango_host"));
  } 
}
    

//=============================================================================
// LvDeviceProxy::connect
//=============================================================================
void LvDeviceProxy::connect()
{
  check_tango_host();
  
  if ( dev_proxy_ )
    return;

  //- instanciate the device proxy
  try
  {
    dev_proxy_ = new ThreadSafeDeviceProxy(dev_name_);
    if ( ! dev_proxy_ )
    {
      Tango::Except::throw_exception(_CPTC_("out of memory"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("LvDeviceProxy::connect"));
    }
  }
  catch (Tango::DevFailed &e)
  {
    std::string d = "failed to create proxy for device " + dev_name_;
    Tango::Except::re_throw_exception(e,
                                      _CPTC_("Tango::DeviceProxy instanciation failed"),
                                      _CPTC_(d.c_str()),
                                      _CPTC_("LvDeviceProxy::connect"));
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("LvDeviceProxy::connect"));
  }
} 

//=============================================================================
// LvDeviceProxy::disconnect
//=============================================================================
void LvDeviceProxy::disconnect ()
{
  delete dev_proxy_;
  dev_proxy_ = 0;
}

//=============================================================================
// LvDeviceProxy::get_device_interface
//=============================================================================
void LvDeviceProxy::get_device_interface ()
  throw (Tango::DevFailed) 
{
  if ( ! dev_proxy_ )
    connect();

  //- get device commands list & info
  try 
  {
    command_list(dev_proxy_->command_list_query());
  }
  catch (Tango::DevFailed &e) 
  {
    Tango::Except::re_throw_exception(e,
                                      _CPTC_("command_list_query failed"),
			                                _CPTC_("failed to get device commands list"),
			                                _CPTC_("LvDeviceProxy::get_device_interface"));
  }
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
			                             _CPTC_("unknown exception caught"),
			                             _CPTC_("LvDeviceProxy::get_device_interface"));
  }

  //- get device attributes list 
  try 
  {
    attribute_list(dev_proxy_->attribute_list_query());  
  }
  catch (Tango::DevFailed &e) 
  {
    //--C++ and Java servers have different behavior!!! 
    //--a C++ server return an empty list if it has no attributes
    //--whereas a Java server throw an exception. Both should 
    //--return an empty list. A quick and dirty workaround...
    std::string reason("API_AttrNotFound");
    if (reason != std::string(e.errors[0].reason)) 
    {  
      Tango::Except::re_throw_exception(e,
                                        _CPTC_("attribute_list_query failed"),
			                                  _CPTC_("failed to get device attributes list"),
			                                  _CPTC_("LvDeviceProxy::get_device_interface"));
    }
    else 
    {
      attribute_list(0);
    }
  }
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
			                             _CPTC_("failed to get device attributes list"),
			                             _CPTC_("LvDeviceProxy::get_device_interface"));
  }
}

//=============================================================================
// LvDeviceProxy::command_info
//=============================================================================
const CommandInfo&  LvDeviceProxy::command_info (const std::string& _cmd_name)
  throw (Tango::DevFailed)
{
  try
  {
    return dev_commands_.at(_cmd_name);
  }
  catch ( ... )
  {
    std::ostringstream oss;
    oss << "no such command: "
        << dev_name_
        << " has no command named "
        << _cmd_name;
    Tango::Except::throw_exception(_CPTC_("command not found"),
                                   oss.str().c_str(),
                                   _CPTC_("LvDeviceProxy::command_info"));
  }
}

//=============================================================================
// LvDeviceProxy::attribute_info
//=============================================================================
const AttributeInfo& LvDeviceProxy::attribute_info (const std::string& _attr_name)
  throw (Tango::DevFailed)
{
  try
  {
    return dev_attributes_.at(_attr_name);
  }
  catch ( ... )
  {
    std::ostringstream oss;
    oss << "no such attribute: "
        << dev_name_
        << " has no attribute named "
        << _attr_name;
    Tango::Except::throw_exception(_CPTC_("attribute not found"),
                                    oss.str().c_str(),
                                    _CPTC_("LvDeviceProxy::attribute_info"));
  }
}

//=============================================================================
// AttributeDataType::attribute_data_type
//=============================================================================
LvAttributeDataType LvDeviceProxy::attribute_data_type (const std::string& _attr_name)
{
  const AttributeInfo& attr_info = attribute_info(_attr_name); 
 
  switch ( attr_info.data_format )
  {
    case Tango::SCALAR:
      switch ( attr_info.data_type )
      {
        case Tango::DEV_STATE:
          return SCA_STATE;
          break;
        case Tango::DEV_ENUM:
          return SCA_ENUM;
          break;
        case Tango::DEV_BOOLEAN:
          return SCA_BOOLEAN;
          break;
        case Tango::DEV_UCHAR:
          return SCA_UCHAR;
          break;
        case Tango::DEV_SHORT:
          return SCA_SHORT;
          break;
        case Tango::DEV_USHORT:
          return SCA_USHORT;
          break;
        case Tango::DEV_LONG:
          return SCA_LONG;
          break;
        case Tango::DEV_ULONG:
          return SCA_ULONG;
          break;
        case Tango::DEV_LONG64:
          return SCA_LONG64;
          break;
        case Tango::DEV_ULONG64:
          return SCA_ULONG64;
          break;
        case Tango::DEV_FLOAT:
          return SCA_FLOAT;
          break;
        case Tango::DEV_DOUBLE:
          return SCA_DOUBLE;
          break;
        case Tango::DEV_STRING:
        case Tango::CONST_DEV_STRING:
          return SCA_STRING;
          break;
        default:
          break;
      }
      break;
    case Tango::SPECTRUM:
      switch ( attr_info.data_type )
      {
        case Tango::DEV_STATE:
          return SPE_STATE;
          break;
        case Tango::DEV_BOOLEAN:
          return SPE_BOOLEAN;
          break;
        case Tango::DEV_UCHAR:
          return SPE_UCHAR;
          break;
        case Tango::DEV_SHORT:
          return SPE_SHORT;
          break;
        case Tango::DEV_USHORT:
          return SPE_USHORT;
          break;
        case Tango::DEV_LONG:
          return SPE_LONG;
          break;
        case Tango::DEV_ULONG:
          return SPE_ULONG;
          break;
        case Tango::DEV_LONG64:
          return SPE_LONG64;
          break;
        case Tango::DEV_ULONG64:
          return SPE_ULONG64;
          break;
        case Tango::DEV_FLOAT:
          return SPE_FLOAT;
          break;
        case Tango::DEV_DOUBLE:
          return SPE_DOUBLE;
          break;
        case Tango::DEV_STRING:
        case Tango::CONST_DEV_STRING:
          return SPE_STRING;
          break;
        default:
          break;
      }
      break;
    case Tango::IMAGE:
      switch ( attr_info.data_type )
      {
        case Tango::DEV_STATE:
          return IMG_STATE;
          break;
        case Tango::DEV_BOOLEAN:
          return IMG_BOOLEAN;
          break;
        case Tango::DEV_UCHAR:
          return IMG_UCHAR;
          break;
        case Tango::DEV_SHORT:
          return IMG_SHORT;
          break;
        case Tango::DEV_USHORT:
          return IMG_USHORT;
          break;
        case Tango::DEV_LONG:
          return IMG_LONG;
          break;
        case Tango::DEV_ULONG:
          return IMG_ULONG;
          break;
        case Tango::DEV_LONG64:
          return IMG_LONG64;
          break;
        case Tango::DEV_ULONG64:
          return IMG_ULONG64;
          break;
        case Tango::DEV_FLOAT:
          return IMG_FLOAT;
          break;
        case Tango::DEV_DOUBLE:
          return IMG_DOUBLE;
          break;
        case Tango::DEV_STRING:
        case Tango::CONST_DEV_STRING:
          return IMG_STRING;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

  return UNKNOWN_ATTR_DATA_TYPE; 
}

//=============================================================================
// LvDeviceProxy::command_list
//=============================================================================
void LvDeviceProxy::command_list (Tango::CommandInfoList* _command_list)
{
  if ( ! _command_list)
    return;

  dev_commands_.clear();
  
  std::size_t s = _command_list->size();

  for ( std::size_t i = 0; i < s; i++ )
  {
    CommandInfo ci;
    ci.dev = dev_name_;
    ci.name = (*_command_list)[i].cmd_name;
    ci.in_type = static_cast<Tango::CmdArgType>((*_command_list)[i].in_type);
    ci.out_type = static_cast<Tango::CmdArgType>((*_command_list)[i].out_type);
    dev_commands_[ci.name] = ci;
  }

  delete _command_list;
}

//=============================================================================
// LvDeviceProxy::attribute_list
//=============================================================================
void LvDeviceProxy::attribute_list (Tango::AttributeInfoList* _attribute_list)
{
  if ( ! _attribute_list)
    return;
  
  dev_attributes_.clear();

  std::size_t s = _attribute_list->size();

  Tango::Database * db = 0;
  try
  {
    db = new Tango::Database();
  }
  catch (...)
  {
    delete db;
    db = 0;
  }
  
  for ( std::size_t i = 0; i < s; i++ )
  {
    AttributeInfo ai;
    ai.dev = dev_name_;
    ai.name = (*_attribute_list)[i].name;
    ai.alias = ai.name;
    ai.writable = (*_attribute_list)[i].writable;
    ai.data_format = (*_attribute_list)[i].data_format;
    ai.data_type = static_cast<Tango::CmdArgType>((*_attribute_list)[i].data_type);
    if ( db )
    {
      try
      {
        std::string fqn;
        fqn = name() + "/" + ai.name;
        Tango::DeviceData ddi;
        ddi << fqn;
        Tango::DeviceData ddo = db->command_inout("DbGetAttributeAlias2", ddi);
        ddo >> ai.alias;
      }
      catch (...) 
      { 
        /* ignore error and use attribute name */ 
      }
    }
    dev_attributes_[ai.name] = ai;
  }

  delete db;
  delete _attribute_list;
}