// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   LvDeviceProxy.i
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// LvDeviceProxy::proxy
//=============================================================================
LV_INLINE ThreadSafeDeviceProxy * 
LvDeviceProxy::proxy () const
{
  return dev_proxy_;
}

//=============================================================================
// LvDeviceProxy::operator->
//=============================================================================
LV_INLINE ThreadSafeDeviceProxy * 
LvDeviceProxy::operator-> () const
{
  return dev_proxy_;
}

//=============================================================================
// LvDeviceProxy::name
//=============================================================================
LV_INLINE const std::string& 
LvDeviceProxy::name () const
{
  return dev_proxy_->dev_name();
}

//=============================================================================
// LvDeviceProxy::command_info
//=============================================================================
LV_INLINE const CommandInfo&
LvDeviceProxy::command_info (const char* _cmd_name)
    throw (Tango::DevFailed)
{
  return command_info(std::string(_cmd_name));
}

//=============================================================================
// LvDeviceProxy::attribute_info
//=============================================================================
LV_INLINE const AttributeInfo&
LvDeviceProxy::attribute_info (const char* _attr_name)
    throw (Tango::DevFailed)
{
  return attribute_info(std::string(_attr_name));
}

