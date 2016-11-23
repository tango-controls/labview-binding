// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   Types.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//-----------------------------------------------------------------------------
// DEPENDENCIES
//-----------------------------------------------------------------------------
#include "Config.h"
#include "CommonHeader.h"

//-----------------------------------------------------------------------------
// ABOUT LABVIEW MEMORY STORAGE
//-----------------------------------------------------------------------------
//- http://zone.ni.com/reference/en-XX/help/371361K-01/lvconcepts/how_labview_stores_data_in_memory/

//-----------------------------------------------------------------------------
// DATA ALIGNMENT
//-----------------------------------------------------------------------------
#include <lv_prolog.h>

//-----------------------------------------------------------------------------
// LABVIEW ARRAYS SIZE
//-----------------------------------------------------------------------------
namespace tbfl {
  typedef tbfl::uint32 size_t;
}

//-----------------------------------------------------------------------------
// LABVIEW ARRAYS
//-----------------------------------------------------------------------------
template <typename _T> struct LabviewArray 
{
  tbfl::size_t length;
  _T data[1];
};

//-----------------------------------------------------------------------------
// DECLARE MACRO
//-----------------------------------------------------------------------------
#define _DECLARE_LABVIEW_1D_ARRAY_(_TYPE, _LV_TYPE_NAME) \
  typedef LabviewArray<_TYPE> _LV_TYPE_NAME; \
  typedef LabviewArray<_TYPE> **_LV_TYPE_NAME##Hdl;

//-----------------------------------------------------------------------------
// INTEGER NUM ARRAYS
//-----------------------------------------------------------------------------
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::int8, LvCharArray);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::uint8, LvUCharArray);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::int16, LvShortArray);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::uint16, LvUShortArray);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::int32, LvLongArray);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::uint32, LvULongArray);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::int64, LvLong64Array);
_DECLARE_LABVIEW_1D_ARRAY_(tbfl::uint64, LvULong64Array);

//-----------------------------------------------------------------------------
// FLOATING-POINT NUM ARRAYS
//-----------------------------------------------------------------------------
_DECLARE_LABVIEW_1D_ARRAY_(float, LvFloatArray);
_DECLARE_LABVIEW_1D_ARRAY_(double, LvDoubleArray);

//-----------------------------------------------------------------------------
// STRING ARRAY
//-----------------------------------------------------------------------------
_DECLARE_LABVIEW_1D_ARRAY_(unsigned char, LvString);
_DECLARE_LABVIEW_1D_ARRAY_(LvStringHdl, LvStringArray);

//-----------------------------------------------------------------------------
typedef LabviewArray<unsigned char> LvGenericNumArray;
typedef LvGenericNumArray *LvGenericNumArrayPtr;
typedef LvGenericNumArrayPtr *LvGenericNumArrayHdl;

//-----------------------------------------------------------------------------
// TANGO DATA TYPE ENUM - MUST MATCH <_AttributeDataType.ctl> ENUM (same order required) 
//-----------------------------------------------------------------------------
typedef enum {
  SCA_STATE =  0,
  SPE_STATE,
  IMG_STATE,
  SCA_STRING,
  SPE_STRING,
  IMG_STRING,
  SCA_BOOLEAN,
  SPE_BOOLEAN,
  IMG_BOOLEAN,
  SCA_UCHAR,
  SPE_UCHAR,
  IMG_UCHAR,
  SCA_SHORT,
  SPE_SHORT,
  IMG_SHORT,
  SCA_USHORT,
  SPE_USHORT,
  IMG_USHORT,
  SCA_LONG,
  SPE_LONG,
  IMG_LONG,
  SCA_ULONG,
  SPE_ULONG,
  IMG_ULONG,
  SCA_LONG64,
  SPE_LONG64,
  IMG_LONG64,
  SCA_ULONG64,
  SPE_ULONG64,
  IMG_ULONG64,
  SCA_FLOAT,
  SPE_FLOAT,
  IMG_FLOAT,
  SCA_DOUBLE,
  SPE_DOUBLE,
  IMG_DOUBLE,
  SCA_ENUM,
  SPE_ENUM,
  IMG_ENUM,
  SCA_ENCODED,
  SPE_ENCODED,
  IMG_ENCODED,
  UNKNOWN_ATTR_DATA_TYPE
} LvAttributeDataType;

//-----------------------------------------------------------------------------
// TANGO CMD ARG DATA TYPE ENUM - MUST MATCH <_CmdArgDataType.ctl> (same order required) 
//-----------------------------------------------------------------------------
typedef enum {
  DEV_STRING = 0,
  DEV_ENUM,
  DEV_BOOLEAN,
  DEV_SHORT,
  DEV_USHORT,
  DEV_LONG,
  DEV_ULONG,
  DEV_FLOAT,
  DEV_DOUBLE,
  DEV_STRING_ARRAY,
  DEV_CHAR_ARRAY,
  DEV_SHORT_ARRAY,
  DEV_USHORT_ARRAY,
  DEV_LONG_ARRAY,
  DEV_ULONG_ARRAY,
  DEV_FLOAT_ARRAY,
  DEV_DOUBLE_ARRAY,
  DEV_LONG_STRING_ARRAY,
  DEV_DOUBLE_STRING_ARRAY,
  UNKNOWN_CMD_DATA_TYPE
} LvCommandDataType; 

//-----------------------------------------------------------------------------
// TANGO ATTRIBUTE VALUE CLUSTER
//-----------------------------------------------------------------------------
struct LvAttributeValue
{
  LvAttributeValue()
   : dev_name(0),
     attr_name(0),
     quality_val(0),
     quality_str(0),
     timestamp(0),
     dim_x(0),
     dim_y(0),
     data(0),
     error_code(0),
     error_stack(0),
     data_type(0)
  {}
  ~LvAttributeValue()
  {
    ::DSDisposeHandle(dev_name);
    ::DSDisposeHandle(attr_name);
    ::DSDisposeHandle(quality_str);
    ::DSDisposeHandle(data);
    ::DSDisposeHandle(error_stack);
  }
  //- device name
  LvStringHdl dev_name;
  //- attribute name
  LvStringHdl attr_name;
  //- attribute quality value
  tbfl::uint16 quality_val;
  //- attribute quality string or encoded format for DEV_ENCODED (trick)
  LvStringHdl quality_str;
  //- timestamp
  double timestamp;
  //- x dim.size
  tbfl::int32 dim_x;
  //- y dim.size
  tbfl::int32 dim_y;
  //- actual attribute value (generic/polymorphic representation: LvGenericNumArrayHdl)
  void* data;
  //- error code (-1 if exception thrown, 0 otherwise)
  tbfl::int32 error_code;
  //- error stack 
  LvStringArrayHdl error_stack;
  //- data type (see LvAttributeDataType)
  tbfl::uint16 data_type; 
};
//-----------------------------------------------------------------------------
typedef LvAttributeValue  *LvAttributeValuePtr;
typedef LvAttributeValuePtr *LvAttributeValueHdl;

//-----------------------------------------------------------------------------
// ERROR STACK VECTOR
//-----------------------------------------------------------------------------
_DECLARE_VECTOR_(std::string, ErrorStack);
  
//-----------------------------------------------------------------------------
// ATTRIBUTE DATA BUFFER
//-----------------------------------------------------------------------------
struct LvClientRequestReplyDataBuffer
{
  LvClientRequestReplyDataBuffer()
  {
    reset();
  }
  LvClientRequestReplyDataBuffer(const LvClientRequestReplyDataBuffer& src)
  {
    *this = src;
  }
  ~LvClientRequestReplyDataBuffer()
  {
    release();
  }
  LvClientRequestReplyDataBuffer& operator= (const LvClientRequestReplyDataBuffer& src)
  {
    if ( &src == this)
      *this;
    dim_x = src.dim_x;
    dim_y = src.dim_y;
    data_type = src.data_type;
    num_data = src.num_data;
    str_data = src.str_data;
    error_code = src.error_code;
    error_stack = src.error_stack;
    const_cast<LvClientRequestReplyDataBuffer&>(src).reset();
    return *this;
  }
  void release ()
  {
    delete[] num_data;
    if (str_data)
    {
      tbfl::size_t ns = ! dim_y ? dim_x : dim_x * dim_y;
      for (tbfl::size_t s = 0; s < ns ; s++)
        CORBA::string_free(str_data[s]);
      delete[] str_data;
    }
    reset();
  }
  void detach_data ()
  {
    reset();
  }
  void reset ()
  {
    dim_x = 0;
    dim_y = 0;
    num_data  = 0;
    str_data  = 0;
    data_type = static_cast<tbfl::uint16>(UNKNOWN_ATTR_DATA_TYPE);
    error_code = 0;
    error_stack.clear();
  }
  template <typename T> 
  void attach_tango_data (T* data, tbfl::int32 dimx = 0, tbfl::int32 dimy = 0)
  {
    dim_x = dimx;
    dim_y = dimy;
    num_data = reinterpret_cast<char*>(data);
  }
  Tango::DevString& get_tango_string ()
  {
    return reinterpret_cast<Tango::DevString&>(num_data);
  }
  template <typename T> 
  T& get_tango_scalar ()
  {
    return *reinterpret_cast<T*>(num_data);
  }
  template <typename T>
  T* get_tango_array ()
  {
    return reinterpret_cast<T*>(num_data);
  }
  void dump () const
  {
    TBFL_STDOUT("LvClientRequestReplyDataBuffer::dim_x........" << dim_x);
    TBFL_STDOUT("LvClientRequestReplyDataBuffer::dim_y........" << dim_y);
    TBFL_STDOUT("LvClientRequestReplyDataBuffer::num_data....." << std::hex << (unsigned long)num_data << std::dec);
    TBFL_STDOUT("LvClientRequestReplyDataBuffer::str_data....." << std::hex << (unsigned long)str_data << std::dec);
    TBFL_STDOUT("LvClientRequestReplyDataBuffer::data_type...." << data_type);
    TBFL_STDOUT("LvClientRequestReplyDataBuffer::error_code..." << error_code);
  }
  //- attribute name
  //- std::string attr_name;
  //- x dim.size
  tbfl::int32 dim_x;
  //- y dim.size
  tbfl::int32 dim_y;
  //- tango data type 
  tbfl::uint16 data_type;
  //- numeric part of the attribute value (generic/polymorphic representation)
  char* num_data;
  //- string part of the attribute value (generic/polymorphic representation)
  char** str_data;
  //- error code (anything != 0 means error)
  tbfl::int32 error_code;
  //- error stack
  ErrorStackVector error_stack;
};

//-----------------------------------------------------------------------------
// ARRAY OF ATTRIBUTE VALUES (i.e ARRAY OF CLUSTERS)
//-----------------------------------------------------------------------------
struct LvAttributeValueArray
{ 
  tbfl::size_t length;
	LvAttributeValue attr_values[1];
};
//-----------------------------------------------------------------------------
typedef LvAttributeValueArray *LvAttributeValueArrayPtr;
typedef LvAttributeValueArrayPtr *LvAttributeValueArrayHdl;

//-----------------------------------------------------------------------------
// TANGO CMD ARGIN/ARGOUT CLUSTER
//-----------------------------------------------------------------------------
struct LvArgIO
{
  LvArgIO ()
    : num_data(0),
      str_data(0)
  {
    //- noop
  }
  ~LvArgIO ()
  {
    ::DSDisposeHandle(num_data);
    if ( str_data && (*str_data) ) {
      for ( tbfl::size_t i = 0; i < (*str_data)->length; i++ )
        ::DSDisposeHandle((*str_data)->data[i]);
    }
    ::DSDisposeHandle(str_data);
  }
  //- numeric data (generic/polymorphic representation: LvGenericNumArrayHdl)
  void* num_data;
  //- string data
  LvStringArrayHdl str_data;
};
//-----------------------------------------------------------------------------
typedef LvArgIO *LvArgIOPtr;
typedef LvArgIOPtr *LvArgIOHdl;

//-----------------------------------------------------------------------------
// ARRAY OF CMD ARGIN/ARGOUT (i.e ARRAY OF CLUSTERS)
//-----------------------------------------------------------------------------
struct LvArgIOArray
{ 
  tbfl::size_t length;
  LvArgIO arg_io[1];
};
//-----------------------------------------------------------------------------
typedef LvArgIOArray *LvArgIOArrayPtr;
typedef LvArgIOArrayPtr *LvArgIOArrayHdl;

//-----------------------------------------------------------------------------
// TANGO COMMAND INFO CLUSTER
//-----------------------------------------------------------------------------
struct LvCommandInfo
{
  //- command name
  LvStringHdl name;
  //- command arg-in type
  tbfl::int16 in_type;
  //- command arg-out type
  tbfl::int16 out_type;
  //- command level (operator or expert)
  tbfl::int16 level;
};
//-----------------------------------------------------------------------------
typedef LvCommandInfo *LvCommandInfoPtr;
typedef LvCommandInfoPtr *LvCommandInfoHdl;

//-----------------------------------------------------------------------------
// ARRAY OF COMMAND INFOS (i.e ARRAY OF CLUSTERS)
//-----------------------------------------------------------------------------
struct LvCommandInfoArray
{ 
  tbfl::size_t length;
  LvCommandInfo cmd_infos[1];
};
//-----------------------------------------------------------------------------
typedef LvCommandInfoArray *LvCommandInfoArrayPtr;
typedef LvCommandInfoArrayPtr *LvCommandInfoArrayHdl;

//-----------------------------------------------------------------------------
// TANGO ATTRIBUTE INFO CLUSTER
//-----------------------------------------------------------------------------
struct LvAttributeInfo
{
  LvAttributeInfo()
    : dev_name(0),
      attr_name(0),
      access(0),
      format(0),
      type(0),
      level(0),
      disp_label(0),
      disp_unit(0),
      disp_format(0),
      min_value(0),
      max_value(0),
      min_alarm(0),
      max_alarm(0)
  {}
  ~LvAttributeInfo()
  {
    ::DSDisposeHandle(dev_name);
    ::DSDisposeHandle(attr_name);
    ::DSDisposeHandle(disp_label);
    ::DSDisposeHandle(disp_unit);
    ::DSDisposeHandle(disp_format);
    ::DSDisposeHandle(min_value);
    ::DSDisposeHandle(max_value);
    ::DSDisposeHandle(min_alarm);
    ::DSDisposeHandle(min_alarm);
  }
  //- device name
  LvStringHdl dev_name;
  //- attribute name
  LvStringHdl attr_name;
  //- attribute R/W access 
  tbfl::int16 access;
  //- attribute format 
  tbfl::int16 format;
  //- attribute data type 
  tbfl::int16 type;
  //- attribute level (operator or expert)
  tbfl::int16 level;
  //- attribute label
  LvStringHdl disp_label;
  //- attribute unit
  LvStringHdl disp_unit;
  //- a la "C" display format
  LvStringHdl disp_format;
  //- min/max values (as strings - for display only)
	LvStringHdl min_value;
	LvStringHdl max_value;
  //- min/max alarm values (as strings - for display only)
	LvStringHdl min_alarm;
	LvStringHdl max_alarm;
};
//-----------------------------------------------------------------------------
typedef LvAttributeInfo *LvAttributeInfoPtr;
typedef LvAttributeInfoPtr *LvAttributeInfoHdl;
//-----------------------------------------------------------------------------
// ARRAY OF ATTRIBUTE INFOS (i.e ARRAY OF CLUSTERS)
//-----------------------------------------------------------------------------
struct LvAttributeInfoArray
{ 
  tbfl::size_t length;
  LvAttributeInfo attr_infos[1];
};
//-----------------------------------------------------------------------------
typedef LvAttributeInfoArray *LvAttributeInfoArrayPtr;
typedef LvAttributeInfoArrayPtr *LvAttributeInfoArrayHdl;

//-----------------------------------------------------------------------------
// TANGO ATTRIBUTE DATA READY EVENT (TANGO EVENT DATA MAPPED TO LV CLUSTER)
//-----------------------------------------------------------------------------
struct LvAttributeDataReady
{
  LvAttributeDataReady()
    : dev_name(0),
      attr_name(0),
      attr_data_type(0),
      ctr(0),
      error_code(0),
      error_stack(0)
  {}
  ~LvAttributeDataReady()
  {
    ::DSDisposeHandle(dev_name);
    ::DSDisposeHandle(attr_name);
    ::DSDisposeHandle(error_stack);
  }
  //- device name
  LvStringHdl dev_name;
  //- attribute name
  LvStringHdl attr_name;
  //- attribute data type
  tbfl::int32 attr_data_type;
  //- attribute data counter (meaning is device specific)
  tbfl::int32 ctr;
  //- error code (-1 if exception thrown, 0 otherwise)
  tbfl::int32 error_code;
  //- error stack
  LvStringArrayHdl error_stack;
};
//-----------------------------------------------------------------------------
typedef LvAttributeDataReady *LvAttributeDataReadyPtr;
typedef LvAttributeDataReadyPtr *LvAttributeDataReadyHdl;
//-----------------------------------------------------------------------------

#if defined (_TBFL_HAS_DEVICE_SERVER_SUPPORT_)
//-----------------------------------------------------------------------------
// DEVICE LOGGING LEVEL
//-----------------------------------------------------------------------------
typedef enum
{
  LV_DEBUG_STREAM = 0,
  LV_INFO_STREAM,
  LV_WARN_STREAM,
  LV_ERROR_STREAM,
  LV_FATAL_STREAM,
} LoggingLevel;

//-----------------------------------------------------------------------------
// DEVICE REG. FORM CLUSTER
//-----------------------------------------------------------------------------
struct LvDeviceRegistrationForm
{
  LvStringHdl device_name;
  LVUserEventRef lv_evt_refnum;
};
//-----------------------------------------------------------------------------
typedef LvDeviceRegistrationForm *LvDeviceRegistrationFormPtr;
typedef LvDeviceRegistrationFormPtr *LvDeviceRegistrationFormHdl;

//-----------------------------------------------------------------------------
// ARRAY OF DEVICE REG. FORM (i.e ARRAY OF CLUSTERS)
//-----------------------------------------------------------------------------
struct LvDeviceRegistrationFormArray
{
  tbfl::size_t length;
  LvDeviceRegistrationForm lv_devices[1];
};
//-----------------------------------------------------------------------------
typedef LvDeviceRegistrationFormArray *LvDeviceRegistrationFormArrayPtr;
typedef LvDeviceRegistrationFormArrayPtr *LvDeviceRegistrationFormArrayHdl;

//-----------------------------------------------------------------------------
// DEVICE PROPERTY
//-----------------------------------------------------------------------------
struct LvDeviceProperty
{
  //- property name
  LvStringHdl name;
  //- property value (generic/polymorphic representation: LvGenericNumArrayHdl)
  void* value;
  //- property data type (see LvAttributeDataType)
  tbfl::uint16 type;
};
//-----------------------------------------------------------------------------
typedef LvDeviceProperty *LvDevicePropertyPtr;
typedef LvDevicePropertyPtr *LvDevicePropertyHdl;


//=============================================================================
// DEFAULT CLIENT REQUEST TIMEOUT IN SEC
//=============================================================================
#define MIN_CLIENT_REQ_TMO_IN_SEC      0.1
#define DEFAULT_CLIENT_REQ_TMO_IN_SEC  3.0
#define MAX_CLIENT_REQ_TMO_IN_SEC     60.0

//-----------------------------------------------------------------------------
// DEVICE SERVER CLIENT REQUEST TYPES 
//-----------------------------------------------------------------------------
typedef enum {
  //- execute command
  CLIENT_REQ_EXEC_CMD = 0,
  //- read attribute
  CLIENT_REQ_READ_ATTR,
  //- write attribute
  CLIENT_REQ_WRITE_ATTR,
  //- unknown request
  CLIENT_REQ_UNKNOWN
} LvClientRequestType;

//-----------------------------------------------------------------------------
// DEVICE SERVER CLIENT REQUEST STATE
//-----------------------------------------------------------------------------
typedef enum {
  //- client request is being processed synchronously (emitter waiting for reply)
  CLIENT_REQ_STATE_PENDING_SYNC = 0,
  //- client request is being processed asynchronously (no waiter)
  CLIENT_REQ_STATE_PENDING_ASYNC,
  //- client request successfully handled
  CLIENT_REQ_STATE_HANDLED,
  //- client request failed
  CLIENT_REQ_STATE_FAILED,
  //- client request expired (timeout)
  CLIENT_REQ_STATE_EXPIRED,
  //- client request aborted
  CLIENT_REQ_STATE_ABORTED,
  //- unknown request state
  CLIENT_REQ_STATE_UNKNOWN
} LvClientRequestState;

//-----------------------------------------------------------------------------
// CLIENT REQUEST  HELPERS
//-----------------------------------------------------------------------------
inline bool is_command_request (LvClientRequestType r)
{
  bool b = false;
  switch ( r )
  {
    case CLIENT_REQ_EXEC_CMD:
      b = true;
      break;
    default:
      break;
  }
  return b;
}
//-----------------------------------------------------------------------------
inline bool is_attribute_request (LvClientRequestType r)
{
  bool b = false;
  switch ( r )
  {
    case CLIENT_REQ_READ_ATTR:
    case CLIENT_REQ_WRITE_ATTR:
      b = true;
      break;
    default:
      break;
  }
  return b;
}
//-----------------------------------------------------------------------------
inline const std::string request_type_str (LvClientRequestType r)
{
  std::string s = "unknown client request";
  switch ( r )
  {
    case CLIENT_REQ_EXEC_CMD:
      s = "execute command";
      break;
    case CLIENT_REQ_READ_ATTR:
      s = "read attribute";
      break;
    case CLIENT_REQ_WRITE_ATTR:
      s = "write attribute";
      break;
    default:
      break;
  }
  return s;
}
//-----------------------------------------------------------------------------
inline const std::string request_type_label (LvClientRequestType r)
{
  std::string s = "UNKNOWN_REQ";
  switch ( r )
  {
    case CLIENT_REQ_EXEC_CMD:
      s = "EXEC_CMD";
      break;
    case CLIENT_REQ_READ_ATTR:
      s = "READ_ATTR";
      break;
    case CLIENT_REQ_WRITE_ATTR:
      s = "WRITE_ATTR";
      break;
    default:
      break;
  }
  return s;
}

//-----------------------------------------------------------------------------
// TANGO DEVICE SERVER: CLIENT REQUEST DATA BUFFER (FOR WRITE ATTR & EXEC CMD)
//-----------------------------------------------------------------------------
struct LvGenericDataBuffer
{
  LvGenericDataBuffer()
    : dimx(0),
      dimy(0),
      buffer(0),
      type(UNKNOWN_ATTR_DATA_TYPE)
  {
    //- noop
  }
  ~LvGenericDataBuffer()
  {
    ::DSDisposeHandle(buffer);
  }
  //- data size 
  tbfl::uint32 dimx;
  tbfl::uint32 dimy;
  //- data (generic/polymorphic representation)
  void* buffer;
  //- data type (see LvAttributeDataType)
  tbfl::uint16 type;
};
//-----------------------------------------------------------------------------
// TANGO DEVICE SERVER: CLIENT REQUEST DATA (FOR WRITE ATTR & EXEC CMD)
//-----------------------------------------------------------------------------
struct LvClientRequestData
{
  //- write attribute value 
  LvGenericDataBuffer wav;
  //- execute command (input) argument
  LvArgIO eca;
};

//-----------------------------------------------------------------------------
// TANGO DEVICE SERVER: CLIENT REQUEST
//-----------------------------------------------------------------------------
struct LvClientRequest
{
  LvClientRequest()
    : request_id(0),
      request_type(CLIENT_REQ_UNKNOWN),
      class_name(0),
      device_name(0),
      cmd_or_attr_name(0)
  {
    //- noop
  }
  ~LvClientRequest()
  {
    ::DSDisposeHandle(class_name);
    ::DSDisposeHandle(device_name);
    ::DSDisposeHandle(cmd_or_attr_name);
  }
  //- request identifier 
  tbfl::uint32 request_id;
  //- source identifier
  tbfl::uint32 source_id;
  //- request type 
  tbfl::uint16 request_type;
  //- name tango the tango device class on which the request applies
  LvStringHdl class_name;
  //- name tango device on which the request applies
  LvStringHdl device_name;
  //- name of command or attribute on which the request applies
  LvStringHdl cmd_or_attr_name;
  //- incoming data (for write attr. & exec. cmd request)
  LvClientRequestData data;
};

//-----------------------------------------------------------------------------
// TANGO DEVICE SERVER: CLIENT REQUEST ACK
//-----------------------------------------------------------------------------
struct LvClientRequestReply
{
  LvClientRequestReply()
    : request_id(0),
      source_id(0),
      error_code(0),
      error_stack(0)
  {}
  //- request identifier 
  tbfl::uint32 request_id;
  //- request identifier
  tbfl::uint32 source_id;
  //- error code (-1 if exception thrown, 0 otherwise)
  tbfl::int32 error_code;
  //- error stack
  LvStringArrayHdl error_stack;
};
//-----------------------------------------------------------------------------
typedef LvClientRequestReply *LvClientRequestReplyPtr;
typedef LvClientRequestReplyPtr LvClientRequestReplyHdl;

//-----------------------------------------------------------------------------
// TANGO DEVICE SERVER: REPLY TO "READ ATTRIBUTE" REQUEST
//-----------------------------------------------------------------------------
struct LvReadAttributeReply
{
  //- the request acknowledgement 
  LvClientRequestReply ack;
  //- the attribute value (valid if ack.error_code == 0)
  LvAttributeValue val;
};
//-----------------------------------------------------------------------------
typedef LvReadAttributeReply *LvReadAttributeReplyPtr;
typedef LvReadAttributeReplyPtr LvReadAttributeReplyHdl;

//-----------------------------------------------------------------------------
// TANGO DEVICE SERVER: REPLY TO "EXECUTE COMMAND" REQUEST
//-----------------------------------------------------------------------------
struct LvExecuteCommandReply
{
  //- the request acknowledgement 
  LvClientRequestReply ack;
  //- the command argout (valid if ack.error_code == 0)
  LvArgIO argout;
};
//-----------------------------------------------------------------------------
typedef LvExecuteCommandReply *LvExecuteCommandReplyPtr;
typedef LvExecuteCommandReplyPtr LvExecuteCommandReplyHdl;
#endif //- _TBFL_HAS_DEVICE_SERVER_SUPPORT_

//-----------------------------------------------------------------------------
// REQUIRED SPACE FOR MEMORY ADDRESS STORAGE [32 vs 64 bits platforms]
//-----------------------------------------------------------------------------
#if (_TBFL_HAS_64_BITS_PLATFORM_)
//- uQ: lv type code for 64-bits unsigned integer
# define LvMemoryAddress uQ 
#else
//- uL: lv type code for 32-bits unsigned integer
# define LvMemoryAddress uL 
#endif

//-----------------------------------------------------------------------------
// DATA ALIGNMENT
//-----------------------------------------------------------------------------
#include <lv_epilog.h>

