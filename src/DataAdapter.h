// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DataAdapter.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//=============================================================================
// FORWARD DECL
//=============================================================================
class LvDeviceProxy;
namespace yat4tango {
  class DynamicCommand;
}

//=============================================================================
// CLASS : DataAdapter 
//=============================================================================
class DataAdapter 
{
  friend class ObjectManager;

public:

  static DataAdapter* instance ();
  //-----------------------------------------------------------------
  void encode_argin (const CommandInfo& cmd_info, LvArgIOPtr lv_data, Tango::DeviceData& tg_data)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void decode_argout (const CommandInfo& cmd_info, Tango::DeviceData& tg_data, LvArgIOPtr lv_data)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void encode_attr (const AttributeInfo& attr_info, LvAttributeValuePtr lv_attr, Tango::DeviceAttribute& tg_attr)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void decode_attr (const AttributeInfo& attr_info, Tango::DeviceAttribute& tg_attr, LvAttributeValuePtr lv_attr)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void copy_lv_argout (const CommandInfo& cmd_info, LvArgIOPtr dyn_cmd_argout, LvClientRequestReplyDataBuffer& cmd_data)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void copy_tg_argin (const CommandInfo& cmd_info, yat4tango::DynamicCommand* dyn_cmd, LvArgIOPtr dyn_cmd_argin)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void copy_lv_attr_value (const AttributeInfo& attr_info, LvAttributeValuePtr lv_attr, LvClientRequestReplyDataBuffer& attr_data)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void copy_tg_attr_value (Tango::WAttribute& tg_attr, LvClientRequest& crn)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void tg_to_lv_attribute_value (const AttributeInfo& attr_info, Tango::DeviceAttribute& tg_attr, LvAttributeValuePtr lv_attr)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void tg_to_lv_attribute_info (const std::string& dev_name, const Tango::AttributeInfo& tg_attr_info, LvAttributeInfo& lv_attr_info)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void lv_to_tg_attribute_info (const std::string& dev_name, const LvAttributeInfo& lv_attr_info, Tango::AttributeInfo& tg_attr_info)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void tg_to_lv_attribute_config (const std::string& dev_name, const Tango::AttributeConfig& tg_attr_config, LvAttributeInfo& lv_attr_info)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void lv_to_tg_attribute_config (const std::string& dev_name, const LvAttributeInfo& lv_attr_info, Tango::AttributeConfig& tg_attr_config)
    throw (Tango::DevFailed);
  //-----------------------------------------------------------------
  void tg_to_lv_device_property (const PropertyInfo& pi, std::vector<std::string>& tg_pv, LvDevicePropertyPtr lv_pv)
    throw(Tango::DevFailed);
  //-----------------------------------------------------------------
  void to_lv_str (const std::string& in, LvStringHdl& out)
    throw(Tango::DevFailed);
  //-----------------------------------------------------------------
  void to_std_str (const LvStringHdl& in, std::string& out)
    throw(Tango::DevFailed);
  //-----------------------------------------------------------------
    
private:

  static int init ();
  //- Instanciates the singleton.
  
  static void cleanup ();
  //- Releases the singleton.

  static DataAdapter* instance_;
  //- The unique instance of <DataAdapter>.

  // = Disallow these operations (except for ObjectManager).
  //-------------------------------------------------------
  DataAdapter();
  DataAdapter (const DataAdapter&);
  virtual ~DataAdapter();
  void operator= (const DataAdapter&); 
};

//=============================================================================
// #DEFINEs
//=============================================================================
#define DATA_ADAPTER (DataAdapter::instance())

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "DataAdapter.i"
#endif 


   
   
