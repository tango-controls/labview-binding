// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceProxy.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//=============================================================================
// DEPENDENCIEs
//=============================================================================
#include "CommonHeader.h"
#include "ThreadSafeDeviceProxy.h"

//=============================================================================
// DEFINEs
//=============================================================================
#define NO_SUCH_ATTR (int)-1
#define NO_SUCH_CMD  (int)-1

//=============================================================================
// STRUCT : PropertyInfo
//=============================================================================
struct PropertyInfo
{
  std::string dev;
  std::string name;
  Tango::CmdArgType data_type;
};

//=============================================================================
// STRUCT : CommandInfo
//=============================================================================
struct CommandInfo
{
  std::string dev;
  std::string name;
  Tango::CmdArgType in_type;
  Tango::CmdArgType out_type;
};
typedef std::map<std::string, CommandInfo> CommandInfoMap;
  
//=============================================================================
// STRUCT : AttributeInfo
//=============================================================================
struct AttributeInfo
{
  std::string dev;
  std::string name;
  std::string alias;
  Tango::AttrWriteType writable;
  Tango::AttrDataFormat data_format;
  Tango::CmdArgType data_type;
  //---------------------------------
  bool is_writable () const
  {
    switch ( writable )
    {
      case Tango::READ:
      case Tango::READ_WITH_WRITE:
        return false;
      default:
        break;
    }
    return true;
  }
};
typedef std::map<std::string, AttributeInfo> AttributeInfoMap;
   
//=============================================================================
// CLASS : LvDeviceProxy
//=============================================================================
class LvDeviceProxy
{
  friend class LvDeviceProxyRepository;

public:
  LvDeviceProxy (const std::string& _dev_name);
  // Ctor

  virtual ~LvDeviceProxy();
  // Dtor
  
  const std::string& name () const;
  // Returns the device name.

  const CommandInfo& command_info (const std::string& cmd_name)
    throw (Tango::DevFailed);
  // If <cmd_name> is supported by the device, returns its 
  // indice in <command_list>,NO_SUCH_CMD otherwise.

  const CommandInfo& command_info (const char* cmd_name)
    throw (Tango::DevFailed);
  // If <cmd_name> is supported by the device, returns its 
  // indice in <command_list>, NO_SUCH_CMD otherwise.

  const AttributeInfo& attribute_info (const std::string& attr_name)
    throw (Tango::DevFailed);
  // If <attr_name> is an attribute of the device, returns its 
  // indice in <attribute_list>, NO_SUCH_ATTR otherwise.

  const AttributeInfo& attribute_info (const char* attr_name)
    throw (Tango::DevFailed);
  // If <attr_name> is an attribute of the device, returns its 
  // indice in <attribute_list>, NO_SUCH_ATTR otherwise.

  LvAttributeDataType attribute_data_type (const std::string& _attr_name);
  // Returns the data type of the specified attribute
  // Returns if no such attribute, returns AttributeDataType::UNKNOWN_ATTR_DATA_TYPE

  ThreadSafeDeviceProxy* operator-> () const;
  // Returns the underlying device proxy

  ThreadSafeDeviceProxy* proxy () const;
  // Returns the underlying device proxy

  void disconnect ();
  // Release the underlying proxy keeping the attrs and cmds info
  
private:
  LvDeviceProxy (Tango::DeviceProxy * dp, bool ownership);
  // Ctor

  void connect ();
  // Instanciate the underlying proxy then get attrs and cmds info
  
  void get_device_interface ()
    throw (Tango::DevFailed);
  // Request then store localy the device interface 

  void command_list (Tango::CommandInfoList* command_list);
  // Set the device commands list

  void attribute_list (Tango::AttributeInfoList* attribute_list);
  // Set the device attributes list

  void check_tango_host()
    throw (Tango::DevFailed);
  // Check that TANGO_HOST is defined (throw DevFailed otherwise)
  
  std::string dev_name_;
  // The device name (for reconnection)
  
  CommandInfoMap dev_commands_;
  // The device command list

  AttributeInfoMap dev_attributes_;
  // The device attribute list

  ThreadSafeDeviceProxy * dev_proxy_;
  //- The underlying device proxy

  // = Disallow these operations (except for LvDeviceProxyRepository).
  //-----------------------------------------------------------
  void operator= (const LvDeviceProxy&);
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "LvDeviceProxy.i"
#endif 

   
   
