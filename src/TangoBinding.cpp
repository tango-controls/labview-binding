// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   TangoBinding.cpp
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "ErrorStack.h"
#include "LvDeviceProxyRepository.h"
#include "DeviceGroupRepository.h"
#include "DataAdapter.h"
#include "EventManager.h"
#include "LvDeviceServer.h"
#include "ObjectManager.h" 

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW
//=============================================================================
//- Tango ping support --------------------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV ping (const char*, tbfl::uint32*, void*);
//- Tango device state/status support  ----------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV state (const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV status (const char*, void*, void*);
//- Tango commands support  ---------------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV command_inout (const char*, const char*, void*, void*, void*);
//- Tango read/write attributes support ---------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV read_attributes (const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV write_attributes (const char*, void*, void*);
//- Tango events support ------------------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV events_group_create (LVUserEventRef*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV events_group_kill (LVUserEventRef*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV subscribe_event (const char*, const char*, tbfl::uint32, LVUserEventRef*, tbfl::uint32*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV unsubscribe_event (tbfl::uint32, void*);
//- Tango groups support ------------------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV group_create (const char*, void*, void*, tbfl::int32*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV group_kill (tbfl::int32, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV group_size (tbfl::int32, tbfl::uint32*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV group_command_inout (tbfl::int32, const char*, void*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV group_read_attributes (tbfl::int32, void*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV group_write_attribute (tbfl::int32, const char*, void*, void*);
//- Tango archiving support ---------------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV extract_archived_data (const char*, const char*, tbfl::uint32, void*, void*); 
//- Tango device timeout support ----------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV set_timeout (const char*, tbfl::uint32, void*);
//- Tango device interface ----------------------------------------------------
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV reload_device_interface (const char*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV device_commands_list (const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV device_commands_info (const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV device_attributes_list (const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV device_attributes_info(const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV attribute_data_type (const char*, const char*, tbfl::uint32*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV get_attributes_info(const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV set_attributes_info(const char*, void*, void*);
//- Tango device server interface ----------------------------------------------
#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_start_device_server (const char*, const char*, void*,  void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_stop_device_server (void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_set_client_req_tmo_by_class (const char*, double, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_set_client_req_tmo_by_device (const char*, double, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_get_device_property (const char*, const char*, void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_enable_state_machine (const char*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_disable_state_machine (const char*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_set_device_timeout (const char*, tbfl::uint32, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_set_execute_cmd_argout (void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_set_read_attr_value (void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_ack_client_request (void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_push_data_ready_event (const char*, const char*, tbfl::int32, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_get_attribute_info (void*, void*);
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_set_attribute_info (void*, void*);
LVB_EXPORT void        LVB_CALLING_CONV dsi_log(const char*, unsigned char, const char*);
#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_

//=============================================================================
// CONSTS
//=============================================================================
const int kNoError =  0;
const int kError   = -1;

//=============================================================================
// MACROS
//=============================================================================
#define _TO_LV_ERR_STACK(X) reinterpret_cast<LvStringArrayHdl&>(X)

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: command_inout
//=============================================================================
tbfl::int32 LVB_CALLING_CONV command_inout (const char* _device_name,
                                            const char* _cmd_name,
                                            void* _argin, 
                                            void* argout_,
                                            void* error_stack_)
{
  try
  {
    //- validate input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::command_inout"));
    }
    if ( ! _cmd_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                    _CPTC_("unexpected null pointer unexpected null pointer to command name  [please check your VIs inputs]"),
                                    _CPTC_("TangoBinding::command_inout"));
    }
    LvArgIOPtr ai = reinterpret_cast<LvArgIOPtr>(_argin);
    if ( ! ai )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvArgIO cluster [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::command_inout"));
    }
    LvArgIOPtr ao = reinterpret_cast<LvArgIOPtr>(argout_);
    if ( ! ao )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvArgIO cluster [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::command_inout"));
    }

    //- get local descriptor
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- get "_cmd_name" info
    const CommandInfo& cmd_info = dp.command_info(_cmd_name);

    //- <cmd_name> result
    Tango::DeviceData dd_out;

    //- encode <argin> then exec the command 
    if ( Tango::DEV_VOID != cmd_info.in_type )
    {
      Tango::DeviceData dd_in;
      DATA_ADAPTER->encode_argin(cmd_info, ai, dd_in);
      dd_out = dp.proxy()->command_inout(_cmd_name, dd_in);
    }
    //- just exec the command 
    else 
    {
      dd_out = dp.proxy()->command_inout(_cmd_name);
    }

    //- decode <argout>
    if ( Tango::DEV_VOID != cmd_info.out_type )
    {
      DATA_ADAPTER->decode_argout(cmd_info, dd_out, ao);
    }
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("command_inout", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("command_inout", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: read_attributes
//=============================================================================
tbfl::int32 LVB_CALLING_CONV read_attributes (const char* _device_name,
                                              void* values_inout_,
                                              void* error_stack_)
{
  std::vector<Tango::DeviceAttribute>* attr_values = 0;

  try 
  {
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::read_attributes"));
    }

    //- get local descriptor
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- values_inout_ is a handle to an LvAttributeValueArray (i.e. an array of clusters)
    LvAttributeValueArrayHdl avah = reinterpret_cast<LvAttributeValueArrayHdl>(values_inout_);
    if ( ! avah || ! (*avah) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeValue array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::read_attributes"));
    }
    if ( ! (*avah)->attr_values || ! (*avah)->length )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("got an invalid LvAttributeValue array [not properly initialized, please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::read_attributes"));
    }
    
    //- how many entries do we have in this array?
    tbfl::size_t n = (*avah)->length;

    std::vector<std::string> attr_names;
    attr_names.resize(n);

    //- for each entry in the LvAttributeValueArray...
    for (tbfl::size_t i = 0; i < n; i++)
    {
      //- point to the i-th LvAttributeValue in the array
      LvAttributeValuePtr av = (*avah)->attr_values + i;
      //- get attribute name 
      DATA_ADAPTER->to_std_str(reinterpret_cast<LvStringHdl>(av->attr_name), attr_names[i]);
    }

    //- read attributes
    attr_values = dp.proxy()->read_attributes(attr_names);

    //- populate each LvAttributeValue
    for (tbfl::size_t i = 0; i < n; i++)
    {
      //- point to the i-th LvAttributeValue in the array
      LvAttributeValuePtr av = (*avah)->attr_values + i;

      //- get attribute info
      const AttributeInfo& ai = dp.attribute_info(attr_names[i]);
      
      //- decode attribute value
      DATA_ADAPTER->tg_to_lv_attribute_value(ai, (*attr_values)[i], av);
    }

    //- release memory 
    if (attr_values) delete attr_values;
  }
  catch (const Tango::DevFailed& e)
  {
    if (attr_values) delete attr_values;
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    if (attr_values) delete attr_values;
    ERROR_STACK->unknown("read_attributes", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("read_attributes", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: attribute_data_type
//=============================================================================
tbfl::int32 LVB_CALLING_CONV attribute_data_type (const char* _device_name,
                                                  const char* _attr_name,
                                                  tbfl::uint32* attr_data_type_,
                                                  void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::attribute_data_type"));
    }
    if ( ! _attr_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to attribute name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::attribute_data_type"));
    }

    //- in case of error...
    *attr_data_type_ = UNKNOWN_ATTR_DATA_TYPE;

    //- get local descriptor
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- be sure attr_data_type_ is valid
    if ( ! attr_data_type_ ) 
    {
      Tango::Except::throw_exception(_CPTC_("internal error"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("attribute_data_type"));
    }

    //- actual data type
    *attr_data_type_ = dp.attribute_data_type(_attr_name);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("attribute_data_type", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("attribute_data_type", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: write_attributes
//=============================================================================
tbfl::int32 LVB_CALLING_CONV write_attributes (const char* _device_name,
                                               void* values_inout_,
                                               void* error_stack_)
{
  try 
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::write_attributes"));
    }

    //- get local descriptor
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- values_inout_ is a handle to an LvAttributeValueArray (i.e. an array of clusters)
    LvAttributeValueArrayHdl avah = reinterpret_cast<LvAttributeValueArrayHdl>(values_inout_);
    if (! avah || ! (*avah))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeValue array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::write_attributes"));
    }

    //- how many entries do we have in this array?
    tbfl::size_t n = (*avah)->length;

    //- some locals
    std::string attr_name;

    //- create and resize a vector of TANGO::DeviceAttribute 
    std::vector<Tango::DeviceAttribute> da_list;
    da_list.resize(n);

    //- Get attr/val for each entry in the array
    for (tbfl::size_t i = 0; i < n; i++) 
    {
      //- point to the i-th LvAttributeValue in the array
      LvAttributeValuePtr av = (*avah)->attr_values + i;
      
      //- get attr name
      DATA_ADAPTER->to_std_str(reinterpret_cast<LvStringHdl>(av->attr_name), attr_name);
      
      //- get attr info
      const AttributeInfo& ai = dp.attribute_info(attr_name);
      
      //- encode data (may throw an exception)
      try 
      {
        DATA_ADAPTER->encode_attr(ai, av, da_list[i]);
      }
      catch (const Tango::DevFailed& e) 
      {
        ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
        return kError;
      }
      catch (...) 
      {
        ERROR_STACK->unknown("write_attributes", _TO_LV_ERR_STACK(error_stack_));
        return kError;
      }
    }

    //- write attributes 
    dp.proxy()->write_attributes(da_list);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("write_attributes", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("write_attributes", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: ping
//=============================================================================
tbfl::int32 LVB_CALLING_CONV ping (const char* _device_name,
                                   tbfl::uint32* rtt_,
                                   void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::ping"));
    }
    if ( ! rtt_ )
    {
      Tango::Except::throw_exception(_CPTC_("internal error"),
                                     _CPTC_("unexpected null pointer to input parameter [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::ping"));
    }

    //- get local descriptor
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- ping the device
    *rtt_ = dp.proxy()->ping();
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("ping", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("ping", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: state
//=============================================================================
tbfl::int32 LVB_CALLING_CONV state (const char* _device_name,
                                    void* argout_,
                                    void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::state"));
    }

    //- get local descriptor
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- push result into argout_
    *(reinterpret_cast<tbfl::uint16*>(argout_)) = static_cast<tbfl::uint16>(dp.proxy()->state());
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("state", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("state", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: status
//=============================================================================
tbfl::int32 LVB_CALLING_CONV status (const char* _device_name,
                                     void* argout_,
                                     void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::status"));
    }

    Tango::DevFailed df;
    bool has_error = false;
    bool has_tango_error = false;

    Tango::DevState state = Tango::UNKNOWN;
    std::string lv_status = "unknown (failed to get state/status from device)";

    try
    {
      //- get local descriptor
      LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

      //- get both state and status from device
      state = dp.proxy()->state();
      lv_status = dp.proxy()->status();
    }
    catch (const Tango::DevFailed& e)
    {
      df = e;
      has_error = true;
      has_tango_error = true;
    }
    catch (...)
    {
      has_error = true;
      has_tango_error = false;
    }

    //- convert from DevState to i16 (Labview calls to DLL don't like enum values)
    std::string lv_state_val;
    std::string lv_state_str;
    switch (state)
    {
    case Tango::ON:
      lv_state_val = "0";
      lv_state_str = "ON";
      break;
    case Tango::OFF:
      lv_state_val = "1";
      lv_state_str = "OFF";
      break;
    case Tango::CLOSE:
      lv_state_val = "2";
      lv_state_str = "CLOSE";
      break;
    case Tango::OPEN:
      lv_state_val = "3";
      lv_state_str = "OPEN";
      break;
    case Tango::INSERT:
      lv_state_val = "4";
      lv_state_str = "INSERT";
      break;
    case Tango::EXTRACT:
      lv_state_val = "5";
      lv_state_str = "EXTRACT";
      break;
    case Tango::MOVING:
      lv_state_val = "6";
      lv_state_str = "MOVING";
      break;
    case Tango::STANDBY:
      lv_state_val = "7";
      lv_state_str = "STANDBY";
      break;
    case Tango::FAULT:
      lv_state_val = "8";
      lv_state_str = "FAULT";
      break;
    case Tango::INIT:
      lv_state_val = "9";
      lv_state_str = "INIT";
      break;
    case Tango::RUNNING:
      lv_state_val = "10";
      lv_state_str = "RUNNING";
      break;
    case Tango::ALARM:
      lv_state_val = "11";
      lv_state_str = "ALARM";
      break;
    case Tango::DISABLE:
      lv_state_val = "12";
      lv_state_str = "DISABLE";
      break;
    default:
    case Tango::UNKNOWN:
      lv_state_val = "13";
      lv_state_str = "UNKNOWN";
      break;
    }

    MgErr err = ::NumericArrayResize(LvMemoryAddress, 1L, reinterpret_cast<UHandle*>(&argout_), 3);
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("out of memory"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("TangoBinding::status"));
    }

    LvStringArrayHdl lvh = _TO_LV_ERR_STACK(argout_);
    (*lvh)->length = 3;
    ::memset((*lvh)->data, 0, (*lvh)->length * sizeof(void*));

    LvStringHdl sh = 0;
    tbfl::size_t l = static_cast<tbfl::size_t>(lv_state_val.size());
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("out of memory"),
                                  _CPTC_("memory allocation failed"),
                                  _CPTC_("TangoBinding::status"));
    }
    (*sh)->length = l;
    ::memcpy((*sh)->data, lv_state_val.c_str(), l);
    (*lvh)->data[0] = sh;

    sh = 0;
    l = static_cast<tbfl::size_t>(lv_state_str.size());
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("out of memory"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("TangoBinding::status"));
    }
    (*sh)->length = l;
    ::memcpy((*sh)->data, lv_state_str.c_str(), l);
    (*lvh)->data[1] = sh;

    sh = 0;
    l = static_cast<tbfl::size_t>(lv_status.size());
    err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("out of memory"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("TangoBinding::status"));
    }
    (*sh)->length = l;
    ::memcpy((*sh)->data, lv_status.c_str(), l);
    (*lvh)->data[2] = sh;

    if (has_error)
    {
      if (has_tango_error)
        throw df;
      else
        throw;
    }
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("status", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("status", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: events_group_create
//=============================================================================
tbfl::int32 LVB_CALLING_CONV events_group_create (LVUserEventRef *_lv_evt_refnum,
                                                  void *error_stack_)
{
  try
  {
    EVT_MANAGER->events_group_create(*_lv_evt_refnum);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("events_group_create", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("events_group_create", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: events_group_kill
//=============================================================================
tbfl::int32 LVB_CALLING_CONV events_group_kill (LVUserEventRef *_lv_evt_refnum,
                                                void *error_stack_)
{
  try
  {
    EVT_MANAGER->events_group_kill(*_lv_evt_refnum);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("events_group_kill", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("events_group_kill", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: subscribe_event
//=============================================================================
tbfl::int32 LVB_CALLING_CONV subscribe_event (const char* _device_name,
                                              const char* _attr_name,
                                              tbfl::uint32 evt_type,
                                              LVUserEventRef *_lv_evt_refnum,
                                              tbfl::uint32* evt_identifier_,
                                              void *error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::subscribe_event"));
    }
    
    if ( ! _attr_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to attribute name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::subscribe_event"));
    }
    
    if ( ! evt_identifier_ )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("TangoBinding::subscribe_event"));
    }

    *evt_identifier_ = EVT_MANAGER->subscribe_event(_device_name,
                                                    _attr_name, 
                                                    static_cast<Tango::EventType>(evt_type),
                                                    *_lv_evt_refnum);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("subscribe_event", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("subscribe_event", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: unsubscribe_event
//=============================================================================
tbfl::int32 LVB_CALLING_CONV unsubscribe_event (tbfl::uint32 evt_identifier,
                                                void *error_stack_)
{
  try
  {
    EVT_MANAGER->unsubscribe_event(evt_identifier);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("unsubscribe_event", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("unsubscribe_event", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: group_create
//=============================================================================
tbfl::int32 LVB_CALLING_CONV group_create (const char* _group_name,
                                           void* _managed_devices,
                                           void* _ignored_devices,
                                           tbfl::int32* group_id_,
                                           void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _group_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to group name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::group_create"));
    }

    //- build vector of <managed device> names
    LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(_managed_devices);
    if (! h || ! (*h))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvString array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::group_create"));
    }

    DeviceList managed_devices((*h)->length);
    for (tbfl::size_t i = 0; i < (*h)->length; i++) 
    {
      DATA_ADAPTER->to_std_str((*h)->data[i], managed_devices[i]);
    }

    //- build vector of <ignored device> names
    DeviceList ignored_devices(0);
    h = reinterpret_cast<LvStringArrayHdl>(_ignored_devices);
    if ( h && *h )
    {
      ignored_devices.resize((*h)->length);
    }
    
    for (tbfl::size_t i = 0; i < (*h)->length; i++) 
    {
      DATA_ADAPTER->to_std_str((*h)->data[i], ignored_devices[i]);
    }

    //- create the group and return its id to the caller
    *group_id_ = GRP_REP->add(_group_name, managed_devices, ignored_devices); 
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("group_create", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("group_create", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: group_kill
//=============================================================================
tbfl::int32 LVB_CALLING_CONV group_kill (tbfl::int32 _group_id, 
                                         void* error_stack_)
{
  try
  {
    //- remove group from repository
    GRP_REP->remove(_group_id);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("group_kill", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("group_kill", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: group_size
//=============================================================================
tbfl::int32 LVB_CALLING_CONV group_size (tbfl::int32 _group_id,
                                         tbfl::uint32* group_size_,
                                         void* error_stack_)
{
  try
  {
    //- get group from repository
    DeviceGroup* dg = GRP_REP->get(_group_id);
    
    //- check returned reference
    if (! dg || ! dg->group())
    {
      Tango::Except::throw_exception(_CPTC_("invalid group identifier"),
                                     _CPTC_("invalid group identifier specified [no such group]"),
                                     _CPTC_("group_size"));
    } 

    //- return group size to caller
    *group_size_ = static_cast<tbfl::uint32>(dg->group()->get_size(false));
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("group_size", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("group_size", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: group_command_inout
//=============================================================================
tbfl::int32 LVB_CALLING_CONV group_command_inout (tbfl::int32 _group_id,
                                                  const char* _command_name,
                                                  void* _args_in,
                                                  void* args_out_,
                                                  void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _command_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to command name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::group_command_inout"));
    }

    //- get group from repository
    DeviceGroup* dg = GRP_REP->get(_group_id);
  
    //- check returned reference
    if (! dg || ! dg->group())
    {
      Tango::Except::throw_exception(_CPTC_("invalid group identifier"),
                                     _CPTC_("invalid group identifier specified [no such group]"),
                                     _CPTC_("group_command_inout"));
    } 

    //- get group size
    tbfl::uint32 dg_size = dg->group()->get_size(false);

    //- check input array size
    if (! dg_size)
    {
      Tango::Except::throw_exception(_CPTC_("empty group"),
                                     _CPTC_("can't execute a command on an empty group"),
                                     _CPTC_("group_command_inout"));
    } 

    //- get device descriptor of the first device in the group (arbitrary choice)
    LvDeviceProxy& dp = DEV_REP->device_proxy(dg->device_list()[0]);

    //- get command info
    const CommandInfo& cmd_info = dp.command_info(_command_name);

    bool in_type_is_not_void = Tango::DEV_VOID != cmd_info.in_type;
    bool ao_type_is_not_void = Tango::DEV_VOID != cmd_info.out_type;

    //- _args_in is a handle to an LvArgIOArray (i.e. an array of clusters)
    LvArgIOArrayHdl args_in = reinterpret_cast<LvArgIOArrayHdl>(_args_in);
    if ( in_type_is_not_void && (! args_in || ! (*args_in)) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvArgIO array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::group_command_inout"));
    }

    //- args_out_ is a handle to an LvArgIOArray (i.e. an array of clusters)
    LvArgIOArrayHdl args_out = reinterpret_cast<LvArgIOArrayHdl>(args_out_);
    if ( ao_type_is_not_void && (! args_out || ! (*args_out)) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvArgIO array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::group_command_inout"));
    }

    //- how many entries do we have in args_in array?
    tbfl::size_t n_in = in_type_is_not_void ? (*args_in)->length : 0;

    //- how many entries do we have in args_out array?
    tbfl::size_t n_out = ao_type_is_not_void ? (*args_out)->length : 0;

    //- check input array size
    if ( in_type_is_not_void && (n_in != dg_size) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid array"),
                                     _CPTC_("the size of the arg-in array must equal the number of devices in the group"),
                                     _CPTC_("group_command_inout"));
    } 

    //- check output array size
    if ( ao_type_is_not_void && (n_out != dg_size) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid array"),
                                     _CPTC_("the size of the arg-out array must equal the number of devices in the group"),
                                     _CPTC_("group_command_inout"));
    } 

    //- cmd replies
    Tango::GroupCmdReplyList cmd_args_out;

    if ( in_type_is_not_void )
    {
      //- create and resize a vector of TANGO::DeviceAttribute 
      std::vector<Tango::DeviceData> dd_list;

      //- Get attr/val for each entry in the array
      for (tbfl::size_t i = 0; i < n_in; i++) 
      {
        //- point to associated LvAttributeValue 
        LvArgIOPtr arg_io = (*args_in)->arg_io + i;

        //- encode <argin> 
        Tango::DeviceData dd;
        DATA_ADAPTER->encode_argin(cmd_info, arg_io, dd);
        dd_list.push_back(dd);
      }
      //- exec ncmd 
      cmd_args_out = dg->group()->command_inout(_command_name, dd_list, false);
    }
    else
    {
      //- exec cmd 
      cmd_args_out = dg->group()->command_inout(_command_name, false);
    }

    //- decode <argout>
    if ( ao_type_is_not_void ) 
    {
      for (tbfl::uint32 i = 0; i < dg_size; i++)
      {
        //- point to the i-th LvAttributeValue in the array
        LvArgIOPtr argout = (*args_out)->arg_io + i;
        //- decode <argout> 
        DATA_ADAPTER->decode_argout(cmd_info, cmd_args_out[i].get_data(), argout);
      }
    }
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("group_command_inout", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("group_command_inout", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: group_read_attributes
//=============================================================================
tbfl::int32 LVB_CALLING_CONV group_read_attributes (tbfl::int32 _group_id,
                                                    void* _attributes_names,
                                                    void* values_inout_,
                                                    void* error_stack_)
{
  try
  {
    //- get group from repository
    DeviceGroup* dg = GRP_REP->get(_group_id);
  
    //- check returned reference
    if (! dg || ! dg->group())
    {
      Tango::Except::throw_exception(_CPTC_("invalid group identifier"),
                                     _CPTC_("invalid group identifier specified [no such group]"),
                                     _CPTC_("group_read_attributes"));
    } 

    //- get attribute names from 2nd argument
    LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(_attributes_names);
    if ( ! h ) 
    {
      Tango::Except::throw_exception(_CPTC_("invalid argument"),
                                     _CPTC_("binding internal error: unexpected null argument"),
                                     _CPTC_("group_read_attributes"));
    }

    //- vector of attribute names: the attributes to be read
    std::vector<std::string> attributes_names((*h)->length);
    for (size_t i = 0; i < attributes_names.size(); i++)
    {
      DATA_ADAPTER->to_std_str((*h)->data[i], attributes_names[i]);
    }

    //- read attributes on each device in the group (may throw an exception)
    Tango::GroupAttrReplyList arl = dg->group()->read_attributes(attributes_names, false);

    //- the number of device in the group
    tbfl::size_t dg_size = dg->group()->get_size(false);

    //- check input array size
    if (! dg_size)
    {
      Tango::Except::throw_exception(_CPTC_("empty group"),
                                     _CPTC_("can't read attributes on an empty group"),
                                     _CPTC_("group_read_attributes"));
    } 

    //- the number of read attributes
    tbfl::size_t nattrs = static_cast<tbfl::size_t>(attributes_names.size());

    //- values_inout_ is a handle to an LvAttributeValueArray (i.e. an array of clusters)
    LvAttributeValueArrayHdl avah = reinterpret_cast<LvAttributeValueArrayHdl>(values_inout_);

    //- how many entries do we have in this array?
    tbfl::size_t avah_size = (*avah)->length;

    //- be sure we have enough room to store results
    tbfl::size_t required_avah_size = dg_size * nattrs;
    if (avah_size != required_avah_size)
    {
      Tango::Except::throw_exception(_CPTC_("invalid argument"),
                                     _CPTC_("wrong size for input array of AttributeValues"),
                                     _CPTC_("group_read_attributes"));
    }
    
    //- for each device in the group...
    for (tbfl::size_t d = 0; d < dg_size; d++)
    {
      //- get local device descriptor
      LvDeviceProxy& dp = DEV_REP->device_proxy(dg->device_list()[d]);

      //- for each read attribute 
      for (tbfl::size_t a = 0; a < nattrs; a++)
      {
        //- compute current index in arl (GroupAttrReplyList)
        tbfl::size_t current_index = d * nattrs + a;
        
        //- point to associated LvAttributeValue 
        LvAttributeValuePtr av = (*avah)->attr_values + current_index;

        //- get attribute info
        const AttributeInfo& attr_info = dp.attribute_info(attributes_names[a]);
              
        //- decode attribute value
        DATA_ADAPTER->tg_to_lv_attribute_value(attr_info,
                                                  arl[current_index].get_data(), 
                                                  av);
      }
    }
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("group_read_attributes", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("group_read_attributes", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: group_write_attribute
//=============================================================================
tbfl::int32 LVB_CALLING_CONV group_write_attribute (tbfl::int32 _group_id,
                                                    const char* _attribute_name,
                                                    void* _values_in,
                                                    void* error_stack_)
{
  try
  {
    //- get group from repository
    DeviceGroup* dg = GRP_REP->get(_group_id);
  
    //- check returned reference
    if (! dg || ! dg->group())
    {
      Tango::Except::throw_exception(_CPTC_("invalid group identifier"),
                                     _CPTC_("invalid group identifier specified [no such group]"),
                                     _CPTC_("group_write_attribute"));
    } 

    //- get group size
    size_t dg_size = dg->group()->get_size(false);

    //- check input array size
    if ( ! dg_size )
    {
      Tango::Except::throw_exception(_CPTC_("empty group"),
                                     _CPTC_("can't write attribute on an empty group"),
                                     _CPTC_("group_write_attribute"));
    } 

    //- _values_in is a handle to an LvAttributeValueArray (i.e. an array of clusters)
    LvAttributeValueArrayHdl avah = reinterpret_cast<LvAttributeValueArrayHdl>(_values_in);
    
    //- how many entries do we have in this array?
    tbfl::size_t n = (*avah)->length;

    //- check returned reference
    if (n != dg_size || n != dg->device_list().size())
    {
      Tango::Except::throw_exception(_CPTC_("invalid array"),
                                     _CPTC_("the size of the AttributeValue array must equal the number of devices in the group"),
                                     _CPTC_("group_write_attribute"));
    } 

    //- some locals
    std::string attr_name;

    //- create and resize a vector of TANGO::DeviceAttribute 
    std::vector<Tango::DeviceAttribute> da_list;
    da_list.resize(n);

    //- Get attr/val for each entry in the array
    for (tbfl::size_t i = 0; i < n; i++) 
    {
      //- get local device descriptor
      LvDeviceProxy& dp = DEV_REP->device_proxy(dg->device_list()[i]);

        //- get attribute info
      const AttributeInfo& attr_info = dp.attribute_info(_attribute_name);

      //- point to the i-th LvAttributeValue in the array
      LvAttributeValuePtr av = (*avah)->attr_values + i;
      
      //- encode data (may throw an exception)
      DATA_ADAPTER->encode_attr(attr_info, av, da_list[i]);
    }

    //- write attribute
    dg->group()->write_attribute(da_list, false);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("group_write_attribute", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("group_write_attribute", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: set_timeout
//=============================================================================
tbfl::int32 LVB_CALLING_CONV set_timeout (const char* _device_name,
                                          tbfl::uint32 _tmo, 
                                          void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::set_timeout"));
    }

    //- avoid too short tmo (min = 1 second)
    if (_tmo < 1000) _tmo = 1000;

    //- get local descriptor for <_device_name>
    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    //- change timeout on <_device_name>
    dp.proxy()->set_timeout(_tmo);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("set_timeout", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("set_timeout", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: extract_archived_data
//=============================================================================
tbfl::int32 LVB_CALLING_CONV extract_archived_data (const char* _db_extractor_device_name,
                                                    const char* _dynamic_attr_name,
                                                    tbfl::uint32 _dynamic_attr_history_len,
                                                    void* values_inout_,
                                                    void* error_stack_)
{
  std::vector<Tango::DeviceAttributeHistory>* archived_attr_values = 0;

  try
  {
    //- check input parameters
    if ( ! _db_extractor_device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to extractor device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::extract_archived_data"));
    }
    if ( ! _dynamic_attr_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to attribute name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::extract_archived_data"));
    }

    { 
      //- get local descriptor for <_db_extractor_device_name>
      LvDeviceProxy& dp = DEV_REP->device_proxy(_db_extractor_device_name);

      //- we now need to read <dynamic_attr_name> history on <db_extractor_device_name> to get the archived data!
      archived_attr_values = dp.proxy()->attribute_history(_dynamic_attr_name, _dynamic_attr_history_len);
      if ( ! archived_attr_values )
      {
        Tango::Except::throw_exception(_CPTC_("internal error"),
                                       _CPTC_("got unexpected null pointer from DeviceProxy::attribute_history"),
                                       _CPTC_("TangoBinding::extract_archived_data"));
      }

      //- dyn. attr. workaround: close/(re)open device connection to get the <dynamic_attr_name> data type
      DEV_REP->remove_device(_db_extractor_device_name);
    }
    
    LvDeviceProxy& dp = DEV_REP->device_proxy(_db_extractor_device_name);
    
    //- reinterpret <values_inout_> as a <LvAttributeValueArrayHdl>
    LvAttributeValueArrayHdl avah = reinterpret_cast<LvAttributeValueArrayHdl>(values_inout_);
    if (! avah || ! (*avah))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeValue array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::extract_archived_data"));
    }

    //- be sure the input array as correct size (can't resize it from a DLL!)
    if ( (*avah)->length != _dynamic_attr_history_len )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected <destination array> size"),
                                     _CPTC_("TangoBinding::extract_archived_data"));
    }

    //- populate each LvAttributeValue
    for ( size_t i = 0; i < archived_attr_values->size(); i++ )
    {
      try
      {
        //- point to the i-th LvAttributeValue in the array
        const AttributeInfo& attr_info = dp.attribute_info(_dynamic_attr_name);
   
        //- point to the i-th LvAttributeValue in the array
        LvAttributeValuePtr av = (*avah)->attr_values + i;
        
        //- decode attribute value
        DATA_ADAPTER->tg_to_lv_attribute_value(attr_info, (*archived_attr_values)[i], av);
      }
      catch ( ... )
      {
        //- ignore error (best effort to get the valid data) 
      }
    }
    
    //- avoid memory leak...
    delete archived_attr_values;
  }
  catch (const Tango::DevFailed& e)
  {
    if (archived_attr_values) delete archived_attr_values;
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    if (archived_attr_values) delete archived_attr_values;
    ERROR_STACK->unknown("extract_archived_data", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("extract_archived_data", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: reload_device_interface
//=============================================================================
tbfl::int32 LVB_CALLING_CONV reload_device_interface (const char* _device_name,
                                                      void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::reload_device_interface"));
    }

    //- close/(re)open device connection to reload the device interface
    DEV_REP->remove_device(_device_name);
    DEV_REP->device_proxy(_device_name);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    ERROR_STACK->unknown("reload_device_interface", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("reload_device_interface", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: device_commands_list
//=============================================================================
tbfl::int32 LVB_CALLING_CONV device_commands_list (const char* _device_name,
                                                   void* commands_list_, 
                                                   void* error_stack_)
{
  Tango::CommandInfoList * cil = 0;

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_commands_list"));
    }

    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    cil = dp.proxy()->command_list_query();

    LvStringArrayHdl lvh = reinterpret_cast<LvStringArrayHdl&>(commands_list_);
    if (! lvh || ! (*lvh))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvStringArray array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_commands_list"));
    }

    tbfl::size_t l = static_cast<tbfl::size_t>(cil->size());

    MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                     1L, 
                                     reinterpret_cast<UHandle*>(&lvh), 
                                     l);
    if (err != noErr)
      Tango::Except::throw_exception(_CPTC_("internal error"),
                                     _CPTC_("failed to resize a LabVIEW array"),
                                     _CPTC_("TangoBinding::device_commands_list"));

    (*lvh)->length = l;

    ::memset((*lvh)->data, 0, l * sizeof(void*));

    for (size_t i = 0; i < cil->size(); i++)
    {
      LvStringHdl sh = 0;
      l = static_cast<tbfl::size_t>((*cil)[i].cmd_name.length());
      err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
      if (err != noErr)
      {
      	Tango::Except::throw_exception(_CPTC_("internal error"),
                                       _CPTC_("failed to resize a LabVIEW array"),
                                       _CPTC_("TangoBinding::device_commands_list"));
      }
      (*sh)->length = l;
      ::memcpy((*sh)->data, (*cil)[i].cmd_name.c_str(), l);
      (*lvh)->data[i] = sh;
    }

    delete cil;
  }
  catch (const Tango::DevFailed& e)
  {
    delete cil;
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    delete cil;
    ERROR_STACK->unknown("device_commands_list", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("device_commands_list", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: device_commands_info
//=============================================================================
tbfl::int32 LVB_CALLING_CONV device_commands_info (const char* _device_name,
                                                   void* commands_info_,
                                                   void* error_stack_)
{
  Tango::CommandInfoList * cil = 0;

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_commands_info"));
    }

    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    cil = dp.proxy()->command_list_query();

    LvCommandInfoArrayHdl lvh = reinterpret_cast<LvCommandInfoArrayHdl&>(commands_info_);
    if (! lvh || ! (*lvh))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvCommandInfo array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_commands_info"));
    }

    //- be sure the input array as correct size (can't resize it from a DLL!)
    if ((*lvh)->length != static_cast<tbfl::size_t>(cil->size()))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected <destination array> size"),
                                     _CPTC_("TangoBinding::device_commands_info"));
    }

    //- fill in each CommandInfo
    for (size_t i = 0; i < cil->size(); i++)
    {
      //- point to the i-th LvCommandInfo in the LvCommandInfoArray
      LvCommandInfo & lci = (*lvh)->cmd_infos[i];
      //- point to the i-th Tango::CommandInfo in the Tango::CommandInfoList 
      Tango::CommandInfo & tci = (*cil)[i];
      //- CommandInfo.name
      LvStringHdl sh = lci.name;
      if (! sh || ! (*sh))
      {
        Tango::Except::throw_exception(_CPTC_("unexpected null handle"),
                                       _CPTC_("invalid LvCommandInfo [seems to be uninitialized]"),
                                       _CPTC_("DataAdapter::device_commands_info"));
      }
      tbfl::size_t l = static_cast<tbfl::size_t>(tci.cmd_name.size());
      if ((*sh)->length != l)
      {
        MgErr err = ::NumericArrayResize(uB, 1L, (UHandle*)&sh, l);
        if (err != noErr) 
        {
          Tango::Except::throw_exception(_CPTC_("out of memory error"),
                                         _CPTC_("memory allocation failed"),
                                         _CPTC_("DataAdapter::device_commands_info"));
        }
        sh = lci.name;
        (*sh)->length = l;
        ::memcpy((*sh)->data, tci.cmd_name.c_str(), l);
      }
      //- CommandInfo.in_type
      lci.in_type = static_cast<short int>(tci.in_type);
      //- CommandInfo.out_type
      lci.out_type = static_cast<short int>(tci.out_type);
      //- CommandInfo.level
      lci.level = static_cast<short int>(tci.disp_level);
    }

    delete cil;
  }
  catch (const Tango::DevFailed& e)
  {
    delete cil;
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    delete cil;
    ERROR_STACK->unknown("device_commands_info", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("device_commands_info", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: device_attributes_list
//=============================================================================
tbfl::int32 LVB_CALLING_CONV device_attributes_list (const char* _device_name,
                                                     void * attributes_list_,
                                                     void* error_stack_)
{
  std::vector<std::string> * al = 0;

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_attributes_list"));
    }

    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    al = dp.proxy()->get_attribute_list();

    LvStringArrayHdl lvh = reinterpret_cast<LvStringArrayHdl&>(attributes_list_);
    if (! lvh || ! (*lvh))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvString array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_attributes_list"));
    }

    tbfl::size_t al_len = static_cast<tbfl::size_t>(al->size());

    MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                     1L, 
                                     reinterpret_cast<UHandle*>(&lvh), 
                                     al_len);
    if (err != noErr)
      Tango::Except::throw_exception(_CPTC_("internal error"),
                                     _CPTC_("failed to resize a LabVIEW array"),
                                     _CPTC_("TangoBinding::device_attributes_list"));

    (*lvh)->length = al_len;

    ::memset((*lvh)->data, 0, al_len * sizeof(void*));

    for (tbfl::size_t i = 0; i < al_len; i++)
    {
      LvStringHdl sh = 0;
      tbfl::size_t l = static_cast<tbfl::size_t>((*al)[i].length());
      err = ::NumericArrayResize(iB, 1L, reinterpret_cast<UHandle*>(&sh), l);
      if (err != noErr)
      {
      	Tango::Except::throw_exception(_CPTC_("internal error"),
                                       _CPTC_("failed to resize a LabVIEW array"),
                                       _CPTC_("TangoBinding::device_attributes_list"));
	  }
	  (*sh)->length = l;
      ::memcpy((*sh)->data, (*al)[i].c_str(), l);
      (*lvh)->data[i] = sh;
    }

    delete al;
  }
  catch (const Tango::DevFailed& e)
  {
    delete al;
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    delete al;
    ERROR_STACK->unknown("device_attributes_list", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("device_attributes_list", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: device_attributes_info
//=============================================================================
tbfl::int32 LVB_CALLING_CONV device_attributes_info (const char* _device_name, 
                                                     void* attributes_info_, 
                                                     void* error_stack_)
{
  Tango::AttributeInfoList * ail = 0;

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_attributes_info"));
    }

    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    ail = dp.proxy()->attribute_list_query();

    LvAttributeInfoArrayHdl lvh = reinterpret_cast<LvAttributeInfoArrayHdl&>(attributes_info_);
    if (! lvh || ! (*lvh))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeInfo array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::device_attributes_list"));
    }

    //- be sure the input array as correct size (can't resize it from a DLL!)
    if ((*lvh)->length != static_cast<tbfl::size_t>(ail->size()))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected <destination array> size"),
                                     _CPTC_("TangoBinding::device_attributes_info"));
    }

    //- fill in each AttributeInfo
    for (size_t i = 0; i < ail->size(); i++)
    {
      //- point to the i-th LvAttributeInfo in the LvAttributeInfoArray
      LvAttributeInfo & lai = (*lvh)->attr_infos[i];

      //- point to the i-th Tango::AttributeInfo in the Tango::AttributeInfoList 
      const Tango::AttributeInfo & tai = (*ail)[i];

      //- convert tango to labview attribute info
      DATA_ADAPTER->tg_to_lv_attribute_info(std::string(_device_name), tai, lai);
    }
    
    delete ail;
  }
  catch (const Tango::DevFailed& e)
  {
    delete ail;
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...) 
  {
    delete ail;
    ERROR_STACK->unknown("device_attributes_info", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("device_attributes_info", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
} 

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: get_attributes_info
//=============================================================================
tbfl::int32 LVB_CALLING_CONV get_attributes_info(const char* _device_name,
                                                 void* attributes_info_, 
                                                 void* error_stack_)
{
  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::get_attributes_info"));
    }

    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    LvAttributeInfoArrayHdl lvh = reinterpret_cast<LvAttributeInfoArrayHdl&>(attributes_info_);
    if ( ! lvh || ! (*lvh) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeInfo array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::get_attributes_info"));
    }

    if ( 0 == (*lvh)->length )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected empty LvAttributeInfo array"),
                                     _CPTC_("TangoBinding::get_attributes_info"));
    }

    //- fill in each AttributeInfo
    for (size_t i = 0; i < (*lvh)->length; i++)
    {
      //- point to the i-th LvAttributeInfo in the LvAttributeInfoArray
      LvAttributeInfo & lai = (*lvh)->attr_infos[i];

      //- extract attribute name
      std::string attr_name;
      DATA_ADAPTER->to_std_str(reinterpret_cast<LvStringHdl>(lai.attr_name), attr_name);

      //- get attribute config
      const Tango::AttributeInfoEx & tai = dp.proxy()->unsafe_proxy().get_attribute_config(attr_name);

      //- convert tango to labview attribute info
      DATA_ADAPTER->tg_to_lv_attribute_info(std::string(_device_name), tai, lai);
    }
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("get_attributes_info", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("get_attributes_info", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: set_attributes_info
//=============================================================================
tbfl::int32 LVB_CALLING_CONV set_attributes_info(const char* _device_name,
                                                 void* attributes_info_,
                                                 void* error_stack_)
{
  try
  {
    //- check input parameters
    if (!_device_name)
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::set_attributes_info"));
    }

    LvDeviceProxy& dp = DEV_REP->device_proxy(_device_name);

    LvAttributeInfoArrayHdl lvh = reinterpret_cast<LvAttributeInfoArrayHdl&>(attributes_info_);
    if (!lvh || !(*lvh))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeInfo array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::set_attributes_info"));
    }

    //- the list of tango attribute config to be written on device
    std::vector<Tango::AttributeInfo> tai((*lvh)->length);
    
    //- fill in each AttributeInfo
    for (size_t i = 0; i < (*lvh)->length; i++)
    {
      //- point to the i-th LvAttributeInfo in the LvAttributeInfoArray
      LvAttributeInfo & lai = (*lvh)->attr_infos[i];

      //- extract attribute name
      std::string attr_name;
      DATA_ADAPTER->to_std_str(reinterpret_cast<LvStringHdl>(lai.attr_name), attr_name);

      //- don't trust user and get the read only part of the attribute info from the device
      tai[i] = dp.proxy()->unsafe_proxy().get_attribute_config(attr_name);

      //- convert labview to tango attribute info (this doesn't change the read only part of the attribute info) 
      DATA_ADAPTER->lv_to_tg_attribute_info(std::string(_device_name), lai, tai[i]);
    }

    //- apply attributes config on device
    dp.proxy()->unsafe_proxy().set_attribute_config(tai);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("set_attributes_info", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("set_attributes_info", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)
//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_intialize_device_server
//=============================================================================
tbfl::int32 dsi_start_device_server (const char* _executable,
                                     const char* _instance,
                                     void* _ds_evt_ref,
                                     void* device_list_,
                                     void* error_stack_)
{
  TBFL_TRACE("tbfl");
  
  try
  {
    if ( ! _executable || ! _instance )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device server executable or instance name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_intialize_device_server"));
    }

    LVUserEventRef* de = reinterpret_cast<LVUserEventRef*>(_ds_evt_ref);
    if ( ! de )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LVUserEventRef [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_intialize_device_server"));
    }
    
    LvStringArrayHdl dl = reinterpret_cast<LvStringArrayHdl>(device_list_);
    if ( ! dl )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvStringArray array [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_intialize_device_server"));
    }

    std::string ex(_executable);
    std::string in(_instance);
    LvDeviceServer::init(ex, in, *de, dl);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_intialize_device_server", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_intialize_device_server", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_stop_device_server
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_stop_device_server (void* error_stack_)
{
  TBFL_TRACE("tbfl");
  
  try
  {
    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_stop_device_server"));
    }

    LvDeviceServer::instance()->stop_devices();
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_stop_devices", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_stop_devices", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_set_client_req_tmo_by_class
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_set_client_req_tmo_by_class (const char* _class_name, 
                                                              double _tmo_in_sec, 
                                                              void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    //- check input parameters
    if ( ! _class_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to class name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_set_client_req_tmo_by_class"));
    }

    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_set_client_req_tmo_by_class"));
    }

    LvDeviceServer::instance()->set_client_request_timeout_by_class(_class_name, _tmo_in_sec);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_set_client_req_tmo_by_class", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_set_client_req_tmo_by_class", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_set_client_req_tmo_by_device
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_set_client_req_tmo_by_device (const char* _device_name, 
                                                               double _tmo_in_sec, 
                                                               void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_set_client_req_tmo_by_device"));
    }

    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_set_client_req_tmo_by_device"));
    }

    LvDeviceServer::instance()->set_client_request_timeout_by_device(_device_name, _tmo_in_sec);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_set_device_timeout", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_set_device_timeout", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_enable_state_machine
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_enable_state_machine (const char* _device_name,
                                                       void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_enable_state_machine"));
    }

    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_enable_state_machine"));
    }

    LvDeviceServer::instance()->enable_state_machine(_device_name, true);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_enable_state_machine", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_enable_state_machine", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_disable_state_machine
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_disable_state_machine (const char* _device_name,
                                                        void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_disable_state_machine"));
    }

    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_disable_state_machine"));
    }

    LvDeviceServer::instance()->enable_state_machine(_device_name, false);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_disable_state_machine", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_disable_state_machine", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_set_execute_cmd_argout
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_set_execute_cmd_argout (void* _reply_to_request, 
                                                         void* error_stack_)
{
  //- TBFL_TRACE("tbfl");
  
  try
  {
    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_set_execute_cmd_argout"));
    }

    LvExecuteCommandReplyPtr ecr = reinterpret_cast<LvExecuteCommandReplyPtr>(_reply_to_request);
    if ( ! ecr )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeValue [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_set_execute_cmd_argout"));
    }

    LvDeviceServer::instance()->set_execute_command_argout(ecr);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_set_read_attr_value", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_set_read_attr_value", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}


//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_set_read_attr_value
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_set_read_attr_value (void* _reply_to_request, 
                                                      void* error_stack_)
{
  //- TBFL_TRACE("tbfl");
  
  try
  {
    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_set_read_attr_value"));
    }

    LvReadAttributeReplyPtr rar = reinterpret_cast<LvReadAttributeReplyPtr>(_reply_to_request);
    if ( ! rar )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvAttributeValue [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_set_read_attr_value"));
    }

    LvDeviceServer::instance()->set_read_attribute_value(rar);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_set_read_attr_value", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_set_read_attr_value", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_ack_client_request
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_ack_client_request (void* _cl_ack, 
                                                     void* error_stack_)
{
  //- TBFL_TRACE("tbfl");
  
  try
  {
    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_ack_client_request"));
    }

    LvClientRequestReplyPtr cra = reinterpret_cast<LvClientRequestReplyPtr>(_cl_ack);
    if ( ! cra )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvClientRequestReply [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_ack_client_request"));
    }
    
    LvDeviceServer::instance()->ack_client_request(cra);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_ack_client_request", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_ack_client_request", _TO_LV_ERR_STACK(error_stack_));
  
  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_push_data_ready_event
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_push_data_ready_event (const char* _device_name,
                                                        const char* _attribute_name,
                                                        tbfl::int32 _user_counter,
                                                        void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_push_data_ready_event"));
    }
    if ( ! _attribute_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to attribute name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_push_data_ready_event"));
    }
    
    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_push_data_ready_event"));
    }

    std::string dn(_device_name);
    std::string an(_attribute_name);
    
    LvDeviceServer::instance()->push_data_ready_event(dn, an, _user_counter);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_push_data_ready_event", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_push_data_ready_event", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_get_device_property
//=============================================================================
LVB_EXPORT tbfl::int32 LVB_CALLING_CONV dsi_get_device_property (const char* _device_name,
                                                                 const char* _property_name,
                                                                 void* device_property_,
                                                                 void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    //- check input parameters
    if ( ! _device_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to device name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_get_device_property"));
    }
    if ( ! _property_name )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to property name [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_get_device_property"));
    }
    
    LvDevicePropertyPtr dp = reinterpret_cast<LvDevicePropertyPtr>(device_property_);
    if ( ! dp )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvDeviceProperty [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_get_device_property"));
    }
    
    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_get_device_property"));
    }

    std::string dn(_device_name);
    std::string pn(_property_name);

    LvDeviceServer::instance()->get_device_property(dn, pn, dp);
  }
  catch (const Tango::DevFailed& e)
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch (...)
  {
    ERROR_STACK->unknown("dsi_get_device_property", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_get_device_property", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_get_attribute_info
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_get_attribute_info(void* attribute_info_,
                                                    void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    LvAttributeInfoPtr ai = reinterpret_cast<LvAttributeInfoPtr>(attribute_info_);
    if ( ! ai )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvDeviceProperty [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_get_attribute_info"));
    }

    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_get_attribute_info"));
    }

    std::string dn;
    DATA_ADAPTER->to_std_str(ai->dev_name, dn);

    std::string an;
    DATA_ADAPTER->to_std_str(ai->attr_name, an);

    LvDeviceServer::instance()->get_attribute_info(dn, an, ai);
  }
  catch ( const Tango::DevFailed& e )
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch ( ... )
  {
    ERROR_STACK->unknown("dsi_get_attribute_info", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_get_attribute_info", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_set_attribute_info
//=============================================================================
tbfl::int32 LVB_CALLING_CONV dsi_set_attribute_info (void* _attribute_info,
                                                     void* error_stack_)
{
  TBFL_TRACE("tbfl");

  try
  {
    LvAttributeInfoPtr ai = reinterpret_cast<LvAttributeInfoPtr>(_attribute_info);
    if ( ! ai )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer to LvDeviceProperty [please check your VIs inputs]"),
                                     _CPTC_("TangoBinding::dsi_set_attributes_info"));
    }

    if ( ! LvDeviceServer::instance() )
    {
      Tango::Except::throw_exception(_CPTC_("programming error"),
                                     _CPTC_("the Tango device-server instance is not [or not properly] initialized"),
                                     _CPTC_("TangoBinding::dsi_set_attributes_info"));
    }

    std::string dn;
    DATA_ADAPTER->to_std_str(ai->dev_name, dn);

    std::string an;
    DATA_ADAPTER->to_std_str(ai->attr_name, an);

    LvDeviceServer::instance()->set_attribute_info(dn, an, ai);
  }
  catch ( const Tango::DevFailed& e )
  {
    ERROR_STACK->devfailed(e, _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }
  catch ( ... )
  {
    ERROR_STACK->unknown("dsi_set_attribute_info", _TO_LV_ERR_STACK(error_stack_));
    return kError;
  }

  ERROR_STACK->no_error("dsi_set_attribute_info", _TO_LV_ERR_STACK(error_stack_));

  return kNoError;
}

//=============================================================================
// BINDING C-INTERFACE TO LABVIEW: dsi_log
//=============================================================================
LVB_EXPORT void LVB_CALLING_CONV dsi_log (const char* _device_name, 
                                          unsigned char _level, 
                                          const char* _text)
{
  //- TBFL_TRACE("tbfl");

  try
  {
    if ( ! LvDeviceServer::instance() || ! _device_name || ! _text )
    {
      return;
    }

    LoggingLevel ll = static_cast<LoggingLevel>(_level);
    
    LvDeviceServer::instance()->log(_device_name, ll, _text);
  }
  catch (...)
  {
    //- ignore any error
  }
}

#endif //-_TBFL_HAS_DEVICE_SERVER_SUPPORT_