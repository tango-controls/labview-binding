// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   DataAdapter.cpp 
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// DEPENDENCIES
//=============================================================================
#ifndef WIN32
#  include <sys/time.h>
#endif 
#include "CommonHeader.h"
#include <yat/utils/XString.h>
#include <yat4tango/DynamicCommand.h>
#include "LvDeviceProxyRepository.h"
#include "Endianness.h"
#include "DataAdapter.h"
#include "ErrorStack.h"

#if !defined (_LV_INLINE_)
  #include "DataAdapter.i"
#endif 

//- TODO: TOO MUCH CODE HERE FACTORIZE OR SPLIT INTO SEVRAL UNITS - NOT A BIG DEAL HOWEVER...

//=============================================================================
// WORKAROUND FOR MSVC++ BUG
//=============================================================================
#define DUMMY_TMPL_ARG char 

//=============================================================================
// DECODE CLASS & MACROS
//=============================================================================
// ENCODE_BASIC_TYPE MACRO
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TEMPLATE CLASS : Encode
//-----------------------------------------------------------------------------
template <typename _A, typename _T> class Encode
{
public:
  //-----------------------------------------------------------------------------
  // Encode::cmd_scalar
  //-----------------------------------------------------------------------------
  static void cmd_scalar (void *lv_data, Tango::DeviceData& tg_data)
    throw (Tango::DevFailed)
  {  
    if ( ! lv_data )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::cmd_scalar"));
    }

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_data);

#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        break;
      case 2:
        Endianness::swap_2(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 4:
        Endianness::swap_4(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 8:
        Endianness::swap_8(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 16:
        Endianness::swap_16(reinterpret_cast<const char*>((*h)->data),
                            reinterpret_cast<char*>((*h)->data));
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::cmd_scalar"));
        break;
    }
#endif

    _A* a = reinterpret_cast<_A*>((*h)->data);
    tg_data << *a;
  }

  //-----------------------------------------------------------------------------
  // Encode::cmd_array
  //-----------------------------------------------------------------------------
  static void cmd_array (void *lv_data, Tango::DeviceData& tg_data)
    throw (Tango::DevFailed)
  {
    _A* tango_array = new _A;
    if ( ! tango_array ) 
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Encode::cmd_array"));
    }
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_data);
    tango_array->length((*h)->length / sizeof(_T));
    Encode::lv_char_array_to_tango_array(*h, tango_array);
    tg_data << tango_array;
  };

  //-----------------------------------------------------------------------------
  // Encode::attr_scalar
  //-----------------------------------------------------------------------------
  static void attr_scalar (LvAttributeValuePtr lv_attr, Tango::DeviceAttribute& tg_attr)
    throw (Tango::DevFailed)
  {  
    if ( ! lv_attr || ! lv_attr->data ) 
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::attr_scalar"));
    }
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr->data);
    if ( ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::attr_scalar"));
    }
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        //- cool, nothing to do!
        break;
      case 2:
        Endianness::swap_2(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 4:
        Endianness::swap_4(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 8:
        Endianness::swap_8(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 16:
        Endianness::swap_16(reinterpret_cast<const char*>((*h)->data),
                            reinterpret_cast<char*>((*h)->data));
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::attr_scalar"));
        break;
    }
#endif
    //- set dimensions              
    tg_attr.dim_x = 1;
    tg_attr.dim_y = 0;
    //- push LabVIEW data into the TANGO container
    _A* a = reinterpret_cast<_A*>((*h)->data);
    tg_attr << *a;
  }

  //-----------------------------------------------------------------------------
  // Encode::attr_array
  //-----------------------------------------------------------------------------
  static void attr_array (LvAttributeValuePtr lv_attr, Tango::DeviceAttribute& tg_attr)
    throw (Tango::DevFailed)
  {
    if ( ! lv_attr )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::attr_array"));
    }
    tbfl::size_t np = lv_attr->dim_x * ((lv_attr->dim_y) ? lv_attr->dim_y : 1);
    _A* tango_array = new _A(np);
    if ( ! tango_array )
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Encode::attr_array"));
    }
    tango_array->length(np);
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr->data);
    Encode::lv_char_array_to_tango_array(*h, tango_array);
    tg_attr << tango_array;
    tg_attr.dim_x = lv_attr->dim_x;
    tg_attr.dim_y = lv_attr->dim_y;
  }

  //-----------------------------------------------------------------------------
  // Encode::lv_char_array_to_tango_array
  //-----------------------------------------------------------------------------
  static void lv_char_array_to_tango_array (LvGenericNumArrayPtr lv_data, _A* tango_array)
    throw (Tango::DevFailed)
  {
    if ( ! lv_data || ! tango_array )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::lv_array_to_tango_array"));
    }
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_T))
    {
      case 1:
        ::memcpy(tango_array->get_buffer(), lv_data->data, tango_array->length());
        break;
      case 2:
        Endianness::swap_2_array(reinterpret_cast<const char*>(lv_data->data),
                                 reinterpret_cast<char*>(tango_array->get_buffer()),
                                 (size_t)tango_array->length());
        break;
      case 4:
        Endianness::swap_4_array(reinterpret_cast<const char*>(lv_data->data),
                                 reinterpret_cast<char*>(tango_array->get_buffer()),
                                 (size_t)tango_array->length());
        break;
      case 8:
        Endianness::swap_8_array(reinterpret_cast<const char*>(lv_data->data),
                                 reinterpret_cast<char*>(tango_array->get_buffer()),
                                 (size_t)tango_array->length());
        break;
      case 16:
        Endianness::swap_16_array(reinterpret_cast<const char*>(lv_data->data),
                                  reinterpret_cast<char*>(tango_array->get_buffer()),
                                  (size_t)tango_array->length());
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::lv_array_to_tango_array"));
        break;
    }
#else
    ::memcpy(tango_array->get_buffer(), lv_data->data, tango_array->length());
#endif
  }
  
  //-----------------------------------------------------------------------------
  // Encode::copy_lv_argout_scalar
  //-----------------------------------------------------------------------------
  static void copy_lv_argout_scalar (void *lv_data, LvClientRequestReplyDataBuffer& cdb)
    throw (Tango::DevFailed)
  {  
    if ( ! lv_data )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::copy_lv_argout_scalar"));
    }
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_data);

    _A a = *reinterpret_cast<_A*>((*h)->data);

#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        //- TBFL_STDOUT << "Encode::copy_lv_argout_scalar::Endianness::swap_00" << std::endl;
        break;
      case 2:
        //- TBFL_STDOUT << "Encode::copy_lv_argout_scalar::Endianness::swap_02" << std::endl;
        Endianness::swap_2(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>(&a));
        break;
      case 4:
        //- TBFL_STDOUT << "Encode::copy_lv_argout_scalar::Endianness::swap_04" << std::endl;
        Endianness::swap_4(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>(&a));
        break;
      case 8:
        //- TBFL_STDOUT << "Encode::copy_lv_argout_scalar::Endianness::swap_08" << std::endl;
        Endianness::swap_8(reinterpret_cast<const char*>((*h)->data),
                           reinterpret_cast<char*>(&a));
        break;
      case 16:
        //- TBFL_STDOUT << "Encode::copy_lv_argout_scalar::Endianness::swap_16" << std::endl;
        Endianness::swap_16(reinterpret_cast<const char*>((*h)->data),
                            reinterpret_cast<char*>(&a));
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::copy_lv_argout_scalar"));
        break;
    }
#endif
    _A* p = new _A[1];
    *p = a;
    cdb.attach_tango_data(p, 1);
  }

  //-----------------------------------------------------------------------------
  // Encode::copy_lv_argout_array
  //-----------------------------------------------------------------------------
  static void copy_lv_argout_array (void *lv_data, LvClientRequestReplyDataBuffer& cdb)
    throw (Tango::DevFailed)
  {
    _A* tango_array = new _A;
    if ( ! tango_array ) 
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Encode::cmd_array"));
    }
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_data);
    tango_array->length((*h)->length / sizeof(_T));
    Encode::lv_char_array_to_tango_array(*h, tango_array);
    cdb.attach_tango_data(tango_array, tango_array->length());
  };

  //-----------------------------------------------------------------------------
  // Encode::copy_lv_attr_scalar
  //-----------------------------------------------------------------------------
  static void copy_lv_attr_scalar (LvAttributeValuePtr lv_attr, LvClientRequestReplyDataBuffer& adb)
    throw (Tango::DevFailed)
  {
    copy_lv_attr_array(lv_attr, adb);
  }
  
  //-----------------------------------------------------------------------------
  // Encode::copy_lv_attr_array
  //-----------------------------------------------------------------------------
  static void copy_lv_attr_array (LvAttributeValuePtr lv_attr, LvClientRequestReplyDataBuffer& adb)
    throw (Tango::DevFailed)
  {
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr->data);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::copy_lv_attr_array"));
    }
    tbfl::size_t np = lv_attr->dim_x * ((lv_attr->dim_y) ? lv_attr->dim_y : 1);
    _A* a = new _A[np];
    if ( ! a )
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Encode::copy_lv_attr_array"));
    }
    Encode::copy_lv_array(*h, a, np);
    adb.attach_tango_data(a, lv_attr->dim_x, lv_attr->dim_y);
  }

  //-----------------------------------------------------------------------------
  // Encode::copy_lv_array
  //-----------------------------------------------------------------------------
  static void copy_lv_array (LvGenericNumArrayPtr lv_data, _A* adb, tbfl::size_t np)
    throw (Tango::DevFailed)
  {
    if ( ! lv_data || ! adb )
    {
      Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                     _CPTC_("unexpected null input argument"),
                                     _CPTC_("Encode::lv_array_to_tango_array"));
    }
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        ::memcpy(adb, lv_data->data, np);
        break;
      case 2:
        Endianness::swap_2_array(reinterpret_cast<const char*>(lv_data->data),
                                 reinterpret_cast<char*>(adb),
                                 np);
        break;
      case 4:
        Endianness::swap_4_array(reinterpret_cast<const char*>(lv_data->data),
                                 reinterpret_cast<char*>(adb),
                                 np);
        break;
      case 8:
        Endianness::swap_8_array(reinterpret_cast<const char*>(lv_data->data),
                                 reinterpret_cast<char*>(adb),
                                 np);
        break;
      case 16:
        Endianness::swap_16_array(reinterpret_cast<const char*>(lv_data->data),
                                  reinterpret_cast<char*>(adb),
                                  np);
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::lv_array_to_tango_array"));
        break;
    }
#else
    ::memcpy(adb, lv_data->data, np);
#endif
  }
};

//-----------------------------------------------------------------------------
#define ENCODE_CMD_SCALAR(_T,_ARG_IN, _TD) \
  Encode<_T, DUMMY_TMPL_ARG>::cmd_scalar(_ARG_IN, _TD);
//-----------------------------------------------------------------------------
#define ENCODE_CMD_ARRAY(_A, _T, _ARG_IN, _TD) \
  Encode<_A,_T>::cmd_array(_ARG_IN, _TD);
//-----------------------------------------------------------------------------
#define LV_CHAR_ARRAY_TO_tango_array(_A, _T, _ARG_IN, _TD) \
  Encode<_A,_T>::lv_char_array_to_tango_array(_ARG_IN, _TD);
//-----------------------------------------------------------------------------
#define ENCODE_ATTR_SCALAR(_T,_ATTR_VAL, _TD) \
  Encode<_T, DUMMY_TMPL_ARG>::attr_scalar(_ATTR_VAL, _TD);
//-----------------------------------------------------------------------------
#define ENCODE_ATTR_ARRAY(_A, _T, _ATTR_VAL, _TD) \
  Encode<_A,_T>::attr_array(_ATTR_VAL, _TD);
//-----------------------------------------------------------------------------
#define COPY_LV_ARGOUT_SCALAR(_T,_ARG_OUT, _CDB) \
  Encode<_T, DUMMY_TMPL_ARG>::copy_lv_argout_scalar(_ARG_OUT, _CDB);
//-----------------------------------------------------------------------------
#define COPY_LV_ARGOUT_ARRAY(_A, _T, _ARG_OUT, _CDB) \
  Encode<_A,_T>::copy_lv_argout_array(_ARG_OUT, _CDB);
//-----------------------------------------------------------------------------
#define COPY_LV_ATTR_SCALAR_VALUE(_A, _ATTR_VAL, _TD) \
  Encode<_A, DUMMY_TMPL_ARG>::copy_lv_attr_scalar(_ATTR_VAL, _TD);
//-----------------------------------------------------------------------------
#define COPY_LV_ATTR_ARRAY_VALUE(_A, _ATTR_VAL, _TD) \
  Encode<_A, DUMMY_TMPL_ARG>::copy_lv_attr_array(_ATTR_VAL, _TD);
//-----------------------------------------------------------------------------

//=============================================================================
// DECODE CLASS & MACROS
//=============================================================================
//-----------------------------------------------------------------------------
// TEMPLATE CLASS: Decode
//-----------------------------------------------------------------------------
template <typename _A, typename _T> class Decode 
{
public:
  //-----------------------------------------------------------------------------
  // Decode::cmd_scalar
  //-----------------------------------------------------------------------------
  static void cmd_scalar (Tango::DeviceData& tg_attr, void*& lv_attr)
    throw (Tango::DevFailed)
  {
    tg_attr.exceptions(Tango::DeviceData::isempty_flag
                             | Tango::DeviceData::wrongtype_flag);
    _A a;
    tg_attr >> a;

    //- TBFL_STDOUT << "Encode::cmd_scalar::value-from-device-data-bswap: " << a << std::endl;

    MgErr err = ::NumericArrayResize(uB,  
                                     1L, 
                                     reinterpret_cast<UHandle*>(&lv_attr),
                                     sizeof(_A));
    if (err != noErr) 
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode::cmd_scalar"));
    } 

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::cmd_scalar"));
    }
    
    (*h)->length = sizeof(_A);

    _A swaped_a;
    
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_00" << std::endl;
        swaped_a = a;
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_02" << std::endl;
        Endianness::swap_2(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_04" << std::endl;
        Endianness::swap_4(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_08" << std::endl;
        Endianness::swap_8(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_16" << std::endl;
        Endianness::swap_16(reinterpret_cast<const char*>(&a),
                            reinterpret_cast<char*>(&swaped_a));
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Decode::cmd_scalar"));
        break;
    }
#endif

    //- TBFL_STDOUT << "Encode::cmd_scalar::value-from-device-data-aswap: " << a << std::endl;

    ::memcpy((*h)->data, &swaped_a, sizeof(_A));

    //- TBFL_STDOUT << "Decode::cmd_scalar::decoded value: " << *reinterpret_cast<_A*>((*h)->data) << std::endl;
  }

  //-----------------------------------------------------------------------------
  // Decode::cmd_array
  //-----------------------------------------------------------------------------
  static void cmd_array (Tango::DeviceData& cmd_data, void*& lv_attr)
    throw (Tango::DevFailed)
  {
    cmd_data.exceptions(Tango::DeviceData::isempty_flag | Tango::DeviceData::wrongtype_flag);
    
    const _A* tango_array;
    if ( (cmd_data >> tango_array) == false )
    {
      Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                     _CPTC_("could not extract data from Tango::DeviceData"),
                                     _CPTC_("Decode::cmd_array"));

    }

    size_t n = static_cast<size_t>(tango_array->length());

    Decode::tango_array_to_lv_array(n, n, tango_array, lv_attr);
  }

  //-----------------------------------------------------------------------------
  // Decode::attr_scalar
  //-----------------------------------------------------------------------------
  static void attr_scalar (Tango::DeviceAttribute& tg_attr, LvAttributeValuePtr lv_attr)
    throw (Tango::DevFailed)
  {
    _A a;
    tg_attr.exceptions(Tango::DeviceData::isempty_flag | Tango::DeviceData::wrongtype_flag);
    tg_attr >> a;

    //- attr. value + setpoint
    tbfl::size_t data_size = 2 * sizeof(_T);
    
    MgErr err = ::NumericArrayResize(uB,  
                                     1L, 
                                     reinterpret_cast<UHandle*>(&lv_attr->data),
                                     data_size);
    if (err != noErr) 
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode:::attr_scalar"));
    }  

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr->data);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::attr_scalar"));
    }
    
    (*h)->length = data_size;

    _A swaped_a;

#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        //- TBFL_STDOUT << "Decode::attr_scalar::Endianness::swap_00" << std::endl;
        swaped_a = a;
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::attr_scalar::Endianness::swap_02" << std::endl;
        Endianness::swap_2(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::attr_scalar::Endianness::swap_04" << std::endl;
        Endianness::swap_4(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::attr_scalar::Endianness::swap_08" << std::endl;
        Endianness::swap_8(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::attr_scalar::Endianness::swap_16" << std::endl;
        Endianness::swap_16(reinterpret_cast<const char*>(&a),
                            reinterpret_cast<char*>(&swaped_a));
        break;
      default:
        //- TBFL_STDOUT << "Decode::attr_scalar::Endianness::swap_00" << std::endl;
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::attr_scalar"));
        break;
    }
#endif
    //- attr. value
    *(reinterpret_cast<_T*>((*h)->data) + 0) = static_cast<_T>(swaped_a);
    //- setpoint
    *(reinterpret_cast<_T*>((*h)->data) + 1) = static_cast<_T>(0);
  }

  //-----------------------------------------------------------------------------
  // Decode::attr_array
  //-----------------------------------------------------------------------------
  static void attr_array (const AttributeInfo& attr_info,
                          Tango::DeviceAttribute& tg_attr,
                          LvAttributeValuePtr lv_attr)
    throw (Tango::DevFailed)
  {
    tg_attr.exceptions(Tango::DeviceAttribute::isempty_flag | Tango::DeviceAttribute::wrongtype_flag);

    size_t src_n = 0;
    size_t dest_n = 0;
    switch  ( attr_info.data_format )
    {
      case Tango::SCALAR:
        {
          src_n = attr_info.is_writable() ? 2 : 1;
          //- actual value + setpoint (might be a dummy one - set to 0)
          dest_n = 2;
        }
        break;
      case Tango::SPECTRUM:
        {
          src_n = tg_attr.get_nb_read();
          //- no setpoint for spectrum
          dest_n = src_n;
        }
        break;
      case Tango::IMAGE:
        {
          Tango::AttributeDimension r_dim = tg_attr.get_r_dimension();
          src_n = r_dim.dim_x * r_dim.dim_y;
          //- no setpoint for image
          dest_n = src_n;
        }
        break;
    }
    
    _A* tango_array = 0;
    try 
    {
      tg_attr >> tango_array;
      Decode::tango_array_to_lv_array(src_n, dest_n, tango_array, lv_attr->data);
      delete tango_array;
    }
    catch (...)
    {
      delete tango_array;
      throw;
    }
  }

  //-----------------------------------------------------------------------------
  // Decode::tango_array_to_lv_array
  //-----------------------------------------------------------------------------
  static void tango_array_to_lv_array (size_t src_n,
                                     size_t dest_n,
                                     const _A* _tango_array, 
                                     void*& lv_array)
    throw (Tango::DevFailed)
  {
    if ( dest_n  < src_n )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("destination buffer is smaller than source buffer"),
                                     _CPTC_("Decode::tango_array_to_lv_array"));
    }
    
    tbfl::size_t l = static_cast<tbfl::size_t>(dest_n * sizeof(_T));

    MgErr err = ::NumericArrayResize(uB, 
                                     1L, 
                                     reinterpret_cast<UHandle*>(&lv_array),
                                     l);
    if (err != noErr) 
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode::tango_array_to_lv_array"));
    }  
    
    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_array);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::tango_array_to_lv_array"));
    }
    
    (*h)->length = l;
    
    tbfl::size_t dn = static_cast<tbfl::size_t>(dest_n - src_n);

    if ( dn > 0 )
    {
      ::memset((*h)->data + dn, 0, dn * sizeof(_T));
    }
    
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch( sizeof(_T) )
    {
      case 1:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_00" << std::endl;
        ::memcpy((*h)->data, _tango_array->get_buffer(), src_n * sizeof(_T));
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_02" << std::endl;
        Endianness::swap_2_array(reinterpret_cast<const char*>(_tango_array->get_buffer()),
                                 reinterpret_cast<char*>((*h)->data),
                                 src_n);
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_04" << std::endl;
        Endianness::swap_4_array(reinterpret_cast<const char*>(_tango_array->get_buffer()),
                                 reinterpret_cast<char*>((*h)->data),
                                 src_n);
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_08" << std::endl;
        Endianness::swap_8_array(reinterpret_cast<const char*>(_tango_array->get_buffer()),
                                 reinterpret_cast<char*>((*h)->data),
                                 src_n);
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_16" << std::endl;
        Endianness::swap_16_array(reinterpret_cast<const char*>(_tango_array->get_buffer()),
                                  reinterpret_cast<char*>((*h)->data),
                                  src_n);
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Decode::tango_array_to_lv_array"));
        break;
    }
#else
    ::memcpy((*h)->data, _tango_array->get_buffer(), src_n * sizeof(_T));
#endif
  } 

  //-----------------------------------------------------------------------------
  // Decode::device_property
  //-----------------------------------------------------------------------------
  static void device_property (std::vector<std::string>& tg_dp, void*& lv_dp)
    throw (Tango::DevFailed)
  {
    if ( tg_dp.empty() )
      return;

    tbfl::size_t l = tg_dp.size();

    MgErr err = ::NumericArrayResize(uB,
                                     1L,
                                     reinterpret_cast<UHandle*>(&lv_dp),
                                     l * sizeof(_T));
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode::device_property"));
    }

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_dp);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::device_property"));
    }

    (*h)->length = l * sizeof(_T);

    for ( tbfl::size_t i = 0; i < l; i++ )
    {
      try
      {
        TBFL_STDOUT("Decode::device_property::converting "
                    << tg_dp[i]
                    << " into a num of "
                    << sizeof(_T)
                    << " bytes");
        
        reinterpret_cast<_T*>((*h)->data)[i] = yat::XString<_T>::to_num(tg_dp[i]);

        TBFL_STDOUT("Decode::device_property::convertion result is "
                    << reinterpret_cast<_T*>((*h)->data)[i]);
      }
      catch (...)
      {
        std::ostringstream oss;
        oss << "invalid entry found in device property [failed to convert '"
            << tg_dp[i]
            << "' from string to num: invalid syntax or value greater than max value for property data type]";
        Tango::Except::throw_exception(_CPTC_("invalid property"),
                                       oss.str().c_str(),
                                       _CPTC_("Decode::device_property"));
      }
    }
      
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch( sizeof(_T) )
    {
      case 1:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_00" << std::endl;
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_02" << std::endl;
        Endianness::swap_2_array(reinterpret_cast<const char*>((*h)->data),
                                 reinterpret_cast<char*>((*h)->data),
                                 l);
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_04" << std::endl;
        Endianness::swap_4_array(reinterpret_cast<const char*>((*h)->data),
                                 reinterpret_cast<char*>((*h)->data),
                                 l);
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_08" << std::endl;
        Endianness::swap_8_array(reinterpret_cast<const char*>((*h)->data),
                                 reinterpret_cast<char*>((*h)->data),
                                 l);
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_16" << std::endl;
        Endianness::swap_16_array(reinterpret_cast<const char*>((*h)->data),
                                  reinterpret_cast<char*>((*h)->data),
                                  l);
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Decode::device_property"));
        break;
    }
#endif
  }
  
  //-----------------------------------------------------------------------------
  // Decode::copy_tg_cmd_scalar
  //-----------------------------------------------------------------------------
  static void copy_tg_cmd_scalar (yat4tango::DynamicCommand* _dyn_cmd, void*& lv_array)
    throw (Tango::DevFailed)
  {
    if ( ! _dyn_cmd->check_argument_data_type<_A>(yat4tango::ARGIN) )
    {
      Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                     _CPTC_("could not extract data from command input argument [unexpected data type]"),
                                     _CPTC_("Decode::copy_tg_cmd_scalar"));

    }

    _A a;
    _dyn_cmd->get_argin(a);

    MgErr err = ::NumericArrayResize(uB,  
                                     1L, 
                                     reinterpret_cast<UHandle*>(&lv_array),
                                     sizeof(_A));
    if (err != noErr) 
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode::copy_tg_cmd_scalar"));
    } 

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_array);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::copy_tg_cmd_scalar"));
    }
    
    (*h)->length = sizeof(_A);

    _A swaped_a;
    
#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_A))
    {
      case 1:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_00" << std::endl;
        swaped_a = a;
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_02" << std::endl;
        Endianness::swap_2(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_04" << std::endl;
        Endianness::swap_4(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_08" << std::endl;
        Endianness::swap_8(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>(&swaped_a));
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::cmd_scalar::Endianness::swap_16" << std::endl;
        Endianness::swap_16(reinterpret_cast<const char*>(&a),
                            reinterpret_cast<char*>(&swaped_a));
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Decode::cmd_scalar"));
        break;
    }
    ::memcpy((*h)->data, &swaped_a, sizeof(_A));
#else
    ::memcpy((*h)->data, &a, sizeof(_A));
#endif
  }

  //-----------------------------------------------------------------------------
  // Decode::copy_tg_cmd_array
  //-----------------------------------------------------------------------------
  static void copy_tg_cmd_array (yat4tango::DynamicCommand* _dyn_cmd, void*& lv_array)
    throw (Tango::DevFailed)
  {
/*  
    TODO: modify yat4tango::TangoTraits, not really a problem cause the Tango kernel already protect us against this situation 
    if ( ! _dyn_cmd->check_argument_data_type<_A>(yat4tango::ARGIN) )
    {
      Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                     _CPTC_("could not extract data from command input argument [unexpected data type]"),
                                     _CPTC_("Decode::copy_tg_cmd_argin"));

    }
*/
    const _A* tango_array;
    _dyn_cmd->get_argin(tango_array);

    size_t n = static_cast<size_t>(tango_array->length());

    Decode::tango_array_to_lv_array(n, n, tango_array, lv_array);
  }

  //-----------------------------------------------------------------------------
  // Decode::copy_tg_attr_scalar
  //-----------------------------------------------------------------------------
  static void copy_tg_attr_scalar (Tango::WAttribute& tg_attr, LvClientRequest& crn)
    throw (Tango::DevFailed)
  {
    //- allocate space to store attribute value
    tbfl::size_t l = static_cast<tbfl::size_t>(sizeof(_T));

    MgErr err = ::NumericArrayResize(uB,
                                     1L,
                                     reinterpret_cast<UHandle*>(&crn.data.wav.buffer),
                                     l);
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode:::copy_tg_attr_scalar"));
    }

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(crn.data.wav.buffer);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::copy_tg_attr_scalar"));
    }

    (*h)->length = l;

    crn.data.wav.dimx = 1;
    crn.data.wav.dimy = 0;
     
    _T a;
    tg_attr.get_write_value(a);

#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_T))
    {
      case 1:
        //- TBFL_STDOUT << "Decode::copy_tg_attr_scalar::Endianness::swap_00" << std::endl;
        ::memcpy((*h)->data, &a, sizeof(_T));
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::copy_tg_attr_scalar::Endianness::swap_02" << std::endl;
        Endianness::swap_2(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::copy_tg_attr_scalar::Endianness::swap_04" << std::endl;
        Endianness::swap_4(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::copy_tg_attr_scalar::Endianness::swap_08" << std::endl;
        Endianness::swap_8(reinterpret_cast<const char*>(&a),
                           reinterpret_cast<char*>((*h)->data));
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::copy_tg_attr_scalar::Endianness::swap_16" << std::endl;
        Endianness::swap_16(reinterpret_cast<const char*>(&a),
                            reinterpret_cast<char*>((*h)->data));
        break;
      default:
        //- TBFL_STDOUT << "Decode::copy_tg_attr_scalar::Endianness::swap_00" << std::endl;
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Encode::copy_tg_attr_scalar"));
        break;
    }
#else
    ::memcpy((*h)->data, &a, sizeof(_T));
#endif
  }

  //-----------------------------------------------------------------------------
  // Decode::copy_tg_attr_array
  //-----------------------------------------------------------------------------
  static void copy_tg_attr_array (Tango::WAttribute& tg_attr, LvClientRequest& crn)
    throw (Tango::DevFailed)
  {
    //- allocate space to store attribute value
    crn.data.wav.dimx = tg_attr.get_w_dim_x();
    crn.data.wav.dimy = tg_attr.get_w_dim_y();

    tbfl::size_t n = crn.data.wav.dimy ? crn.data.wav.dimx * crn.data.wav.dimy : crn.data.wav.dimx;

    tbfl::size_t l = static_cast<tbfl::size_t>(n * sizeof(_T));

    MgErr err = ::NumericArrayResize(uB,
                                     1L,
                                     reinterpret_cast<UHandle*>(&crn.data.wav.buffer),
                                     l);
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("Decode:::copy_tg_attr_array"));
    }

    LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(crn.data.wav.buffer);
    if ( ! h || ! (*h) )
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("unexpected null pointer"),
                                     _CPTC_("Decode::copy_tg_attr_array"));
    }

    (*h)->length = l;
    
    const _T* a;
    tg_attr.get_write_value(a);

#if (_TBFL_LITTLE_ENDIAN_PLATFORM_ == 1)
    switch(sizeof(_T))
    {
      case 1:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_00" << std::endl;
        ::memcpy((*h)->data, a, n);
        break;
      case 2:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_02" << std::endl;
        Endianness::swap_2_array(reinterpret_cast<const char*>(a),
                                 reinterpret_cast<char*>((*h)->data),
                                 n);
        break;
      case 4:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_04" << std::endl;
        Endianness::swap_4_array(reinterpret_cast<const char*>(a),
                                 reinterpret_cast<char*>((*h)->data),
                                 n);
        break;
      case 8:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_08" << std::endl;
        Endianness::swap_8_array(reinterpret_cast<const char*>(a),
                                 reinterpret_cast<char*>((*h)->data),
                                 n);
        break;
      case 16:
        //- TBFL_STDOUT << "Decode::tango_array_to_lv_array::Endianness::swap_16" << std::endl;
        Endianness::swap_16_array(reinterpret_cast<const char*>(a),
                                  reinterpret_cast<char*>((*h)->data),
                                  n);
        break;
      default:
        Tango::Except::throw_exception(_CPTC_("unexpected data type"),
                                       _CPTC_("no support for > 128-bits data types"),
                                       _CPTC_("Decode::tango_array_to_crn_buffer"));
        break;
    }
#else
    ::memcpy((*h)->data, a, n * sizeof(_T));
#endif
  } 
}; 
//-----------------------------------------------------------------------------
#define DECODE_CMD_SCALAR(_SRC_T, _TD, _LVD) \
  Decode<_SRC_T, _SRC_T>::cmd_scalar(_TD, _LVD);
//-----------------------------------------------------------------------------
#define DECODE_ATTR_SCALAR(_SRC_T, _TD, _LVD) \
  Decode<_SRC_T, _SRC_T>::attr_scalar(_TD, _LVD);
//-----------------------------------------------------------------------------
#define COPY_TG_CMD_SCALAR(_SRC_T, _TD, _ADB) \
  Decode<_SRC_T, _SRC_T>::copy_tg_cmd_scalar(_TD, _ADB);
//-----------------------------------------------------------------------------
#define COPY_TG_CMD_ARRAY(_SEQ_T, _ELEM_T, _TD, _ADB) \
  Decode<_SEQ_T, _ELEM_T>::copy_tg_cmd_array(_TD, _ADB);
//-----------------------------------------------------------------------------
#define COPY_TG_ATTR_SCALAR(_SRC_T, _TD, _ADB) \
  Decode<_SRC_T, _SRC_T>::copy_tg_attr_scalar(_TD, _ADB);
//-----------------------------------------------------------------------------
#define COPY_TG_ATTR_ARRAY(_SRC_T, _TD, _ADB) \
  Decode<_SRC_T, _SRC_T>::copy_tg_attr_array(_TD, _ADB);
//-----------------------------------------------------------------------------
#define tango_array_TO_LV_ARRAY(_A, _T, _TD, _LVD) \
  Decode<_A,_T>::tango_array_to_lv_array((_TD)->length(), (_TD)->length(), _TD, _LVD);
//-----------------------------------------------------------------------------
#define DECODE_CMD_ARRAY(_A, _T, _TD, _LVD) \
  Decode<_A,_T>::cmd_array(_TD, _LVD);
//-----------------------------------------------------------------------------
#define DECODE_ATTR_ARRAY(_AI, _A, _T, _TD, _LVD) \
  Decode<_A,_T>::attr_array(_AI, _TD, _LVD);
//-----------------------------------------------------------------------------
#define DECODE_DEV_PROPERTY(_PROP_T, _TD, _LVD) \
  Decode<_PROP_T, _PROP_T>::device_property(_TD, _LVD);
//-----------------------------------------------------------------------------

//=============================================================================
// STATIC MEMBERS
//=============================================================================
DataAdapter* DataAdapter::instance_ = 0;

//=============================================================================
// DataAdapter::init
//=============================================================================
int DataAdapter::init ()
{
  if (DataAdapter::instance_ != 0)
    return 0;

  DataAdapter::instance_ = new DataAdapter;

  return (DataAdapter::instance_) ? 0 : -1; 
}

//=============================================================================
// DataAdapter::cleanup
//=============================================================================
void DataAdapter::cleanup ()
{
  if (DataAdapter::instance_) 
  {
    delete DataAdapter::instance_;

    DataAdapter::instance_ = 0;
  }
}

//=============================================================================
// DataAdapter::DataAdapter 
//=============================================================================
DataAdapter::DataAdapter ()
{
 // no-op ctor
}

//=============================================================================
// DataAdapter::~DataAdapter 
//=============================================================================
DataAdapter::~DataAdapter ()
{
 // no-op dtor
}

//=============================================================================
// DataAdapter::encode_argin
//=============================================================================
void DataAdapter::encode_argin (const CommandInfo& cmd_info,
                                LvArgIOPtr _argin,
                                Tango::DeviceData& dd_in_)
    throw (Tango::DevFailed)
{
  try 
  {
    switch ( cmd_info.in_type ) 
    {
      //-- DEV_VOID ------------------------------------
      case Tango::DEV_VOID: 
        {
          return; 
        } break;
      //-- DEVVAR_LONGSTRINGARRAY ----------------------
      case Tango::DEVVAR_LONGSTRINGARRAY: 
        {
          //- create the DevVarLongStringArray
          Tango::DevVarLongStringArray* dvls_array = new Tango::DevVarLongStringArray;
          if ( ! dvls_array ) 
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::encode_argin"));
          }
          //- encode int32 data
          LvGenericNumArrayHdl lvgah = reinterpret_cast<LvGenericNumArrayHdl>(_argin->num_data);
          if ( ! lvgah || ! (*lvgah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));
          }
          dvls_array->lvalue.length((*lvgah)->length / sizeof(Tango::DevLong));
          LV_CHAR_ARRAY_TO_tango_array(Tango::DevVarLongArray, Tango::DevLong, *lvgah, &(dvls_array->lvalue));
          //- encode string data
          LvStringArrayHdl sah = reinterpret_cast<LvStringArrayHdl>(_argin->str_data);
          if ( ! sah || ! (*sah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));
          }
          dvls_array->svalue.length((*sah)->length);
          LvStringHdl h = 0;
	        for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = (*sah)->data[i];
            dvls_array->svalue[i] = CORBA::string_alloc((*h)->length + 1);
            if ( ! dvls_array->svalue[i] ) 
            {
              delete dvls_array;
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::encode_argin"));
            }
            ::memcpy(dvls_array->svalue[i], (*h)->data, (*h)->length);
            ::memset(dvls_array->svalue[i] + (*h)->length, 0 , 1);
	        }
          dd_in_ << dvls_array;
        } break;
      //-- DEVVAR_DOUBLESTRINGARRAY --------------------
      case Tango::DEVVAR_DOUBLESTRINGARRAY: 
        {
          //- create the DevVarDoubleStringArray
          Tango::DevVarDoubleStringArray* dvds_array = new Tango::DevVarDoubleStringArray;
          if ( ! dvds_array ) 
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::encode_argin"));
          }
          //- encode double data
          LvGenericNumArrayHdl lvgah = reinterpret_cast<LvGenericNumArrayHdl>(_argin->num_data);
          if ( ! lvgah || ! (*lvgah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));
          }
          dvds_array->dvalue.length((*lvgah)->length / sizeof(double));
          LV_CHAR_ARRAY_TO_tango_array(Tango::DevVarDoubleArray, double, *lvgah, &(dvds_array->dvalue));
          //- encode string data
          LvStringArrayHdl sah = reinterpret_cast<LvStringArrayHdl>(_argin->str_data);
          if ( ! sah || ! (*sah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));;
          }
          dvds_array->svalue.length((*sah)->length);
          LvStringHdl h = 0;
	        for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = (*sah)->data[i];
            dvds_array->svalue[i] = CORBA::string_alloc((*h)->length + 1);
            if ( ! dvds_array->svalue[i] ) 
            {
              delete dvds_array;
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::encode_argin"));
            }
            ::memcpy(dvds_array->svalue[i], (*h)->data, (*h)->length);
            ::memset(dvds_array->svalue[i] + (*h)->length, 0 , 1);
	        }
          dd_in_ << dvds_array;
        } break;
      //-- DEVVAR_STRINGARRAY --------------------------
      case Tango::DEVVAR_STRINGARRAY: 
        {
          LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(_argin->str_data);
          if ( ! h || ! (*h) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("DataAdapter::encode_argin"));;
          }
	        Tango::DevVarStringArray *str_array = new Tango::DevVarStringArray((*h)->length);
          if ( ! str_array ) 
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::encode_argin"));
          }
          str_array->length((*h)->length);
          LvStringHdl sh = 0;
	        for (tbfl::size_t i = 0; i < (*h)->length; i++) 
          {
            sh = (*h)->data[i];
            (*str_array)[i] = CORBA::string_alloc((*sh)->length + 1);
            if ( ! (*str_array)[i] ) 
            {
              delete str_array;
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::encode_argin"));
            }
            ::memcpy((*str_array)[i], (*sh)->data, (*sh)->length);
            ::memset((*str_array)[i] + (*sh)->length, 0 , 1);
	        }
          dd_in_ << str_array;
        } 
        break;
      //-- DEVVAR_DOUBLEARRAY --------------------------
      case Tango::DEVVAR_DOUBLEARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_FLOATARRAY ---------------------------
      case Tango::DEVVAR_FLOATARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarFloatArray, Tango::DevFloat, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_LONG64ARRAY --------------------------
      case Tango::DEVVAR_LONG64ARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarLong64Array, Tango::DevLong64, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_ULONG64ARRAY -------------------------
      case Tango::DEVVAR_ULONG64ARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarULong64Array, Tango::DevULong64, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_LONGARRAY ---------------------------
      case Tango::DEVVAR_LONGARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarLongArray, Tango::DevLong, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_ULONGARRAY ---------------------------
      case Tango::DEVVAR_ULONGARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarULongArray, Tango::DevULong, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_SHORTARRAY ---------------------------
      case Tango::DEVVAR_SHORTARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarShortArray, Tango::DevShort, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_USHORTARRAY --------------------------
      case Tango::DEVVAR_USHORTARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarUShortArray, Tango::DevUShort, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEVVAR_CHARARRAY
      case Tango::DEVVAR_CHARARRAY: 
        {
          ENCODE_CMD_ARRAY(Tango::DevVarCharArray, Tango::DevUChar, _argin->num_data, dd_in_);
        } 
        break;
      //-- DEV_STRING ----------------------------------
      case Tango::DEV_STRING:
      case Tango::CONST_DEV_STRING:
        {
          std::string s;
          to_std_str(reinterpret_cast<LvStringHdl&>(_argin->num_data), s);
		      dd_in_ << s;
        } 
        break;
      //-- DEV_BOOLEAN ---------------------------------
      case Tango::DEV_BOOLEAN: 
        {
 		      ENCODE_CMD_SCALAR(Tango::DevBoolean, _argin->num_data, dd_in_); 
        } 
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- DEV_ENUM ------------------------------------
      case Tango::DEV_ENUM:
#endif
      //-- DEV_SHORT -----------------------------------
      case Tango::DEV_SHORT: 
        {
 		      ENCODE_CMD_SCALAR(Tango::DevShort, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_SHORT -----------------------------------
      case Tango::DEV_USHORT: 
        {
          ENCODE_CMD_SCALAR(Tango::DevUShort, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_LONG ------------------------------------
      case Tango::DEV_LONG: 
        {
          ENCODE_CMD_SCALAR(Tango::DevLong, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_ULONG -----------------------------------
      case Tango::DEV_ULONG: 
        {
          ENCODE_CMD_SCALAR(Tango::DevULong, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_LONG64 ----------------------------------
      case Tango::DEV_LONG64: 
        {
          ENCODE_CMD_SCALAR(Tango::DevLong64, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_ULONG64 ---------------------------------
      case Tango::DEV_ULONG64: 
        {
			    ENCODE_CMD_SCALAR(Tango::DevULong64, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_FLOAT -----------------------------------
      case Tango::DEV_FLOAT: 
        {
 		      ENCODE_CMD_SCALAR(Tango::DevFloat, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_DOUBLE ----------------------------------
      case Tango::DEV_DOUBLE: 
        {
 		      ENCODE_CMD_SCALAR(Tango::DevDouble, _argin->num_data, dd_in_); 
        } 
        break;
      //-- DEV_STATE
      case Tango::DEV_STATE:
        {
          ENCODE_CMD_SCALAR(Tango::DevState, _argin->num_data, dd_in_);
        }
        break;
      //-- DEFAULT -------------------------------------
      default:
        Tango::Except::throw_exception(_CPTC_("unknown or unsupported TANGO data type"),
                                       _CPTC_("unsupported argin type for command"),
                                       _CPTC_("DataAdapter::encode_argin"));
        break;
    } // switch 
  } //try
  catch (const Tango::DevFailed&) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::encode_argin"));
  }
}

//=============================================================================
// DataAdapter::decode_argout
//=============================================================================
void DataAdapter::decode_argout (const CommandInfo& cmd_info,
                                 Tango::DeviceData& _dd_out, 
                                 LvArgIOPtr argout_)
    throw (Tango::DevFailed)
{
  _dd_out.exceptions(Tango::DeviceData::isempty_flag | Tango::DeviceData::wrongtype_flag);

  try 
  {
    switch ( cmd_info.out_type ) 
    {
      //-- DEV_VOID ------------------------------------
      case Tango::DEV_VOID: 
        {
          return; 
        } 
        break;
      //-- DEVVAR_LONGSTRINGARRAY ----------------------
      case Tango::DEVVAR_LONGSTRINGARRAY:
        {
          const Tango::DevVarLongStringArray *corba_struct;
          _dd_out >> corba_struct;
          //- decode int32 data
          tango_array_TO_LV_ARRAY(Tango::DevVarLongArray, Tango::DevLong, &(corba_struct->lvalue), argout_->num_data);
          //- decode string data
          LvStringArrayHdl sah = argout_->str_data;
          if ( ! sah ) 
          {
            Tango::Except::throw_exception(_CPTC_("invalid output argument"),
                                           _CPTC_("unexpected null output argument"),
                                           _CPTC_("DataAdapter::decode_argout"));
          }
          MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                           1L, 
                                           reinterpret_cast<UHandle*>(&sah), 
                                           corba_struct->svalue.length());
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::decode_argout"));
          }
		      (*sah)->length = corba_struct->svalue.length();
          ::memset((*sah)->data, 0, (*sah)->length * sizeof(LvMemoryAddress));
          tbfl::size_t l = 0;
          LvStringHdl h = 0;
		      for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = 0;
			      l = static_cast<tbfl::size_t>(::strlen(corba_struct->svalue[i]));
            err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&h), l);
            if (err != noErr)
            {
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::decode_argout"));
            }
			      (*h)->length = l;
            ::memcpy((*h)->data, corba_struct->svalue[i], l);
            (*sah)->data[i] = h;
          }
        } 
        break;
      //-- DEVVAR_DOUBLESTRINGARRAY --------------------
      case Tango::DEVVAR_DOUBLESTRINGARRAY: 
        {
          const Tango::DevVarDoubleStringArray *corba_struct;
          _dd_out >> corba_struct;
          //- decode double data
          tango_array_TO_LV_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, &(corba_struct->dvalue), argout_->num_data);
          //- decode string data
          LvStringArrayHdl sah = argout_->str_data;
          if ( ! sah ) {
            Tango::Except::throw_exception(_CPTC_("invalid output argument"),
                                           _CPTC_("unexpected null output argument"),
                                           _CPTC_("DataAdapter::decode_argout"));
          }
          MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                           1L, 
                                           reinterpret_cast<UHandle*>(&sah), 
                                           corba_struct->svalue.length());
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::decode_argout"));
          }
		      (*sah)->length = corba_struct->svalue.length();
          ::memset((*sah)->data, 0, (*sah)->length * sizeof(LvMemoryAddress));
          tbfl::size_t l = 0;
          LvStringHdl h = 0;
		      for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = 0;
			      l = static_cast<tbfl::size_t>(::strlen(corba_struct->svalue[i]));
            err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&h), l);
            if (err != noErr)
            {
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::decode_argout"));
            }
			      (*h)->length = l;
            ::memcpy((*h)->data, corba_struct->svalue[i], l);
            (*sah)->data[i] = h;
          }
        } 
        break;
      //-- DEVVAR_STRINGARRAY -------------------------
      case Tango::DEVVAR_STRINGARRAY: 
        {
          const Tango::DevVarStringArray *tango_array;
          _dd_out >> tango_array;
          MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                           1L, 
                                           reinterpret_cast<UHandle*>(&(argout_->str_data)), 
                                           tango_array->length());
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::decode_argout"));
          }
          LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(argout_->str_data);
          if ( ! h || ! (*h) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("DataAdapter::decode_argout"));;
          }
		      (*h)->length = tango_array->length();
          ::memset((*h)->data, 0, (*h)->length * sizeof(LvMemoryAddress));
          tbfl::size_t l = 0;
          LvStringHdl sh = 0;
		      for (tbfl::size_t i = 0; i < (*h)->length; i++)
          {
            sh = 0;
			      l = static_cast<tbfl::size_t>(::strlen((*tango_array)[i]));
            err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&sh), l);
            if (err != noErr)
            {
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::decode_argout"));
            }
			      (*sh)->length = l;
            ::memcpy((*sh)->data, (*tango_array)[i], l);
            (*h)->data[i] = sh;
          }
        } 
        break;
      //-- DEVVAR_DOUBLEARRAY ------------------------
      case Tango::DEVVAR_DOUBLEARRAY: 
        {
          DECODE_CMD_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_FLOATARRAY -------------------------
      case Tango::DEVVAR_FLOATARRAY: 
        {
          DECODE_CMD_ARRAY(Tango::DevVarFloatArray, Tango::DevFloat, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_LONGARRAY --------------------------
      case Tango::DEVVAR_LONGARRAY: 
        { 
          DECODE_CMD_ARRAY(Tango::DevVarLongArray, Tango::DevLong, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_ULONGARRAY -------------------------
      case Tango::DEVVAR_ULONGARRAY: 
        {   
          DECODE_CMD_ARRAY(Tango::DevVarULongArray, Tango::DevULong, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_LONG64 -----------------------------
      case Tango::DEVVAR_LONG64ARRAY:
        { 
          DECODE_CMD_ARRAY(Tango::DevVarLong64Array, Tango::DevLong64, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_ULONG64 ----------------------------
      case Tango::DEVVAR_ULONG64ARRAY:
        { 
          DECODE_CMD_ARRAY(Tango::DevVarULong64Array, Tango::DevULong64, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_SHORTARRAY -------------------------
      case Tango::DEVVAR_SHORTARRAY: 
        {
          DECODE_CMD_ARRAY(Tango::DevVarShortArray, Tango::DevShort, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_USHORTARRAY ------------------------
      case Tango::DEVVAR_USHORTARRAY: 
        {
          DECODE_CMD_ARRAY(Tango::DevVarUShortArray, Tango::DevUShort, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEVVAR_CHARARRAY --------------------------
      case Tango::DEVVAR_CHARARRAY: 
        {
          DECODE_CMD_ARRAY(Tango::DevVarCharArray, Tango::DevUChar, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEV_STRING --------------------------------
      case Tango::DEV_STRING:
      case Tango::CONST_DEV_STRING: 
        {
          std::string s;
          _dd_out >> s;
          to_lv_str(s, reinterpret_cast<LvStringHdl&>(argout_->num_data));
        } 
        break;
      //-- DEV_BOOLEAN -------------------------------
      case Tango::DEV_BOOLEAN: 
        {
          DECODE_CMD_SCALAR(Tango::DevBoolean, _dd_out, argout_->num_data);
        }  
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- DEV_ENUM ---------------------------------
      case Tango::DEV_ENUM:
#endif
      //-- DEV_SHORT ---------------------------------
      case Tango::DEV_SHORT: 
        {
          DECODE_CMD_SCALAR(Tango::DevShort, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEV_USHORT --------------------------------
      case Tango::DEV_USHORT: 
        {
          DECODE_CMD_SCALAR(Tango::DevUShort, _dd_out, argout_->num_data);
        }  
        break;
      //-- DEV_LONG ---------------------------------
      case Tango::DEV_LONG: 
        {
          DECODE_CMD_SCALAR(Tango::DevLong, _dd_out, argout_->num_data);
        }  
        break;      
      //-- DEV_ULONG --------------------------------
      case Tango::DEV_ULONG: 
        {
          DECODE_CMD_SCALAR(Tango::DevULong, _dd_out, argout_->num_data);
        }  
        break;
      //-- DEV_LONG64 -------------------------------
      case Tango::DEV_LONG64: 
        {
          DECODE_CMD_SCALAR(Tango::DevLong64, _dd_out, argout_->num_data);
        }  
        break;
      //-- DEV_ULONG64 ------------------------------
      case Tango::DEV_ULONG64: 
        {
          DECODE_CMD_SCALAR(Tango::DevULong64, _dd_out, argout_->num_data);
        }  
        break;
      //-- DEV_FLOAT --------------------------------
      case Tango::DEV_FLOAT: 
        {
          DECODE_CMD_SCALAR(Tango::DevFloat, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEV_DOUBLE -------------------------------
      case Tango::DEV_DOUBLE: 
        {
          DECODE_CMD_SCALAR(Tango::DevDouble, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEV_STATE --------------------------------
      case Tango::DEV_STATE: 
        {
          DECODE_CMD_SCALAR(Tango::DevState, _dd_out, argout_->num_data);
        } 
        break;
      //-- DEFAULT ----------------------------------
      default:
        Tango::Except::throw_exception(_CPTC_("unknown or unsupported TANGO data type"),
                                       _CPTC_("unexpected TANGO data type for command"),
                                       _CPTC_("DataAdapter::decode_argout"));
        break;
    } // switch
  } // try        
  catch (const Tango::DevFailed&) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::decode_argout"));
  }
}

//=============================================================================
// DataAdapter::copy_lv_argout
//=============================================================================
void DataAdapter::copy_lv_argout (const CommandInfo& cmd_info,
                                  LvArgIOPtr _dyn_cmd_argout,
                                  LvClientRequestReplyDataBuffer& cmd_data_)
    throw (Tango::DevFailed)
{
  try 
  {
    switch ( cmd_info.in_type ) 
    {
      //-- DEV_VOID ------------------------------------
      case Tango::DEV_VOID: 
        {
          return; 
        } break;
      //-- DEVVAR_LONGSTRINGARRAY ----------------------
      case Tango::DEVVAR_LONGSTRINGARRAY: 
        {
          //- create the DevVarLongStringArray
          Tango::DevVarLongStringArray* dvls_array = new Tango::DevVarLongStringArray;
          if ( ! dvls_array ) 
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::encode_argin"));
          }
          //- encode int32 data
          LvGenericNumArrayHdl lvgah = reinterpret_cast<LvGenericNumArrayHdl>(_dyn_cmd_argout->num_data);
          if ( ! lvgah || ! (*lvgah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));
          }
          dvls_array->lvalue.length((*lvgah)->length / sizeof(Tango::DevLong));
          LV_CHAR_ARRAY_TO_tango_array(Tango::DevVarLongArray, Tango::DevLong, *lvgah, &(dvls_array->lvalue));
          //- encode string data
          LvStringArrayHdl sah = reinterpret_cast<LvStringArrayHdl>(_dyn_cmd_argout->str_data);
          if ( ! sah || ! (*sah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));
          }
          dvls_array->svalue.length((*sah)->length);
          LvStringHdl h = 0;
	        for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = (*sah)->data[i];
            dvls_array->svalue[i] = CORBA::string_alloc((*h)->length + 1);
            if ( ! dvls_array->svalue[i] ) 
            {
              delete dvls_array;
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::encode_argin"));
            }
            ::memcpy(dvls_array->svalue[i], (*h)->data, (*h)->length);
            ::memset(dvls_array->svalue[i] + (*h)->length, 0 , 1);
	        }
          cmd_data_.attach_tango_data(dvls_array);
        } break;
      //-- DEVVAR_DOUBLESTRINGARRAY --------------------
      case Tango::DEVVAR_DOUBLESTRINGARRAY: 
        {
          //- create the DevVarDoubleStringArray
          Tango::DevVarDoubleStringArray* dvds_array = new Tango::DevVarDoubleStringArray;
          if ( ! dvds_array ) 
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::encode_argin"));
          }
          //- encode double data
          LvGenericNumArrayHdl lvgah = reinterpret_cast<LvGenericNumArrayHdl>(_dyn_cmd_argout->num_data);
          if ( ! lvgah || ! (*lvgah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));
          }
          dvds_array->dvalue.length((*lvgah)->length / sizeof(double));
          LV_CHAR_ARRAY_TO_tango_array(Tango::DevVarDoubleArray, double, *lvgah, &(dvds_array->dvalue));
          //- encode string data
          LvStringArrayHdl sah = reinterpret_cast<LvStringArrayHdl>(_dyn_cmd_argout->str_data);
          if ( ! sah || ! (*sah) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("Decode::encode_argin"));;
          }
          dvds_array->svalue.length((*sah)->length);
          LvStringHdl h = 0;
	        for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = (*sah)->data[i];
            dvds_array->svalue[i] = CORBA::string_alloc((*h)->length + 1);
            if ( ! dvds_array->svalue[i] ) 
            {
              delete dvds_array;
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::encode_argin"));
            }
            ::memcpy(dvds_array->svalue[i], (*h)->data, (*h)->length);
            ::memset(dvds_array->svalue[i] + (*h)->length, 0 , 1);
	        }
          cmd_data_.attach_tango_data(dvds_array);
        } break;
      //-- DEVVAR_STRINGARRAY --------------------------
      case Tango::DEVVAR_STRINGARRAY: 
        {
          LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(_dyn_cmd_argout->str_data);
          if ( ! h || ! (*h) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("DataAdapter::encode_argin"));;
          }
	        Tango::DevVarStringArray *str_array = new Tango::DevVarStringArray((*h)->length);
          if ( ! str_array ) 
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::encode_argin"));
          }
          str_array->length((*h)->length);
          LvStringHdl sh = 0;
	        for (tbfl::size_t i = 0; i < (*h)->length; i++) 
          {
            sh = (*h)->data[i];
            (*str_array)[i] = CORBA::string_alloc((*sh)->length + 1);
            if ( ! (*str_array)[i] ) 
            {
              delete str_array;
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::encode_argin"));
            }
            ::memcpy((*str_array)[i], (*sh)->data, (*sh)->length);
            ::memset((*str_array)[i] + (*sh)->length, 0 , 1);
	        }
          cmd_data_.attach_tango_data(str_array);
        } 
        break;
      //-- DEVVAR_DOUBLEARRAY --------------------------
      case Tango::DEVVAR_DOUBLEARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_FLOATARRAY ---------------------------
      case Tango::DEVVAR_FLOATARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarFloatArray, Tango::DevFloat, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_LONG64ARRAY --------------------------
      case Tango::DEVVAR_LONG64ARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarLong64Array, Tango::DevLong64, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_ULONG64ARRAY -------------------------
      case Tango::DEVVAR_ULONG64ARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarULong64Array, Tango::DevULong64, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_LONGARRAY ---------------------------
      case Tango::DEVVAR_LONGARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarLongArray, Tango::DevLong, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_ULONGARRAY ---------------------------
      case Tango::DEVVAR_ULONGARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarULongArray, Tango::DevULong, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_SHORTARRAY ---------------------------
      case Tango::DEVVAR_SHORTARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarShortArray, Tango::DevShort, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_USHORTARRAY --------------------------
      case Tango::DEVVAR_USHORTARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarUShortArray, Tango::DevUShort, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
      //-- DEVVAR_CHARARRAY
      case Tango::DEVVAR_CHARARRAY: 
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarCharArray, Tango::DevUChar, _dyn_cmd_argout->num_data, cmd_data_);
        } 
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- DEVVAR_STATEARRAY
      case Tango::DEVVAR_STATEARRAY:
        {
          COPY_LV_ARGOUT_ARRAY(Tango::DevVarStateArray, Tango::DevState, _dyn_cmd_argout->num_data, cmd_data_);
        }
        break;
#endif
      //-- DEV_STRING ----------------------------------
      case Tango::DEV_STRING:
      case Tango::CONST_DEV_STRING:
        {
          std::string s;
          to_std_str(reinterpret_cast<LvStringHdl&>(_dyn_cmd_argout->num_data), s);
          cmd_data_.attach_tango_data(CORBA::string_dup(s.c_str()));
        } 
        break;
      //-- DEV_BOOLEAN ---------------------------------
      case Tango::DEV_BOOLEAN: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevBoolean, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- DEV_ENUM -----------------------------------
      case Tango::DEV_ENUM:
#endif
      //-- DEV_SHORT -----------------------------------
      case Tango::DEV_SHORT: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevShort, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_SHORT -----------------------------------
      case Tango::DEV_USHORT: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevUShort, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_LONG ------------------------------------
      case Tango::DEV_LONG: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevLong, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_ULONG -----------------------------------
      case Tango::DEV_ULONG: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevULong, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_LONG64 ----------------------------------
      case Tango::DEV_LONG64: 
        {
          COPY_LV_ARGOUT_SCALAR(Tango::DevLong64, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_ULONG64 ---------------------------------
      case Tango::DEV_ULONG64: 
        {
          COPY_LV_ARGOUT_SCALAR(Tango::DevULong64, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_FLOAT -----------------------------------
      case Tango::DEV_FLOAT: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevFloat, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_DOUBLE ----------------------------------
      case Tango::DEV_DOUBLE: 
        {
 		      COPY_LV_ARGOUT_SCALAR(Tango::DevDouble, _dyn_cmd_argout->num_data, cmd_data_); 
        } 
        break;
      //-- DEV_STATE
      case Tango::DEV_STATE:
        {
          COPY_LV_ARGOUT_SCALAR(Tango::DevState, _dyn_cmd_argout->num_data, cmd_data_);
        }
        break;
      //-- DEFAULT -------------------------------------
      default:
        Tango::Except::throw_exception(_CPTC_("unknown or unsupported TANGO data type"),
                                       _CPTC_("unsupported argin type for command"),
                                       _CPTC_("DataAdapter::encode_argin"));
        break;
    } // switch 
  } //try
  catch (const Tango::DevFailed&) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::encode_argin"));
  }
}

//=============================================================================
// DataAdapter::copy_tg_argin
//=============================================================================
void DataAdapter::copy_tg_argin (const CommandInfo& cmd_info,
                                 yat4tango::DynamicCommand* _dyn_cmd, 
                                 LvArgIOPtr dyn_cmd_argin_)
    throw (Tango::DevFailed)
{
  if ( ! _dyn_cmd || ! dyn_cmd_argin_ )
  {
    Tango::Except::throw_exception(_CPTC_("internal error"),
                                   _CPTC_("unexpected null argument"),
                                   _CPTC_("DataAdapter::decode_argout"));
  }

  try 
  {
    switch ( cmd_info.in_type ) 
    {
      //-- DEV_VOID ------------------------------------
      case Tango::DEV_VOID: 
        {
          return; 
        } 
        break;
      //-- DEVVAR_LONGSTRINGARRAY ----------------------
      case Tango::DEVVAR_LONGSTRINGARRAY:
        {
          /*  
          TODO: modify yat4tango::TangoTraits, not really a problem cause the Tango kernel already protect us against this situation
          if ( ! _dyn_cmd->check_argument_data_type<Tango::DevVarLongStringArray>(yat4tango::ARGIN) )
          {
            Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                           _CPTC_("could not extract data from command input argument [unexpected data type]"),
                                           _CPTC_("Decode::copy_tg_argin"));

          }
          */
          const Tango::DevVarLongStringArray *corba_struct;
          _dyn_cmd->get_argin(corba_struct);
          //- decode int32 data
          tango_array_TO_LV_ARRAY(Tango::DevVarLongArray, Tango::DevLong, &(corba_struct->lvalue), dyn_cmd_argin_->num_data);
          //- decode string data
          MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                           1L, 
                                           reinterpret_cast<UHandle*>(&dyn_cmd_argin_->str_data), 
                                           corba_struct->svalue.length());
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::copy_tg_argin"));
          }
          LvStringArrayHdl sah = dyn_cmd_argin_->str_data;
		      (*sah)->length = corba_struct->svalue.length();
          ::memset((*sah)->data, 0, (*sah)->length * sizeof(LvMemoryAddress));
          tbfl::size_t l = 0;
          LvStringHdl h = 0;
		      for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = 0;
			      l = static_cast<tbfl::size_t>(::strlen(corba_struct->svalue[i]));
            err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&h), l);
            if (err != noErr)
            {
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::copy_tg_argin"));
            }
			      (*h)->length = l;
            ::memcpy((*h)->data, corba_struct->svalue[i], l);
            (*sah)->data[i] = h;
          }
        } 
        break;
      //-- DEVVAR_DOUBLESTRINGARRAY --------------------
      case Tango::DEVVAR_DOUBLESTRINGARRAY: 
        {
          /*  
          TODO: modify yat4tango::TangoTraits, not really a problem cause the Tango kernel already protect us against this situation 
          if ( ! _dyn_cmd->check_argument_data_type<Tango::DevVarDoubleStringArray>(yat4tango::ARGIN) )
          {
            Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                           _CPTC_("could not extract data from command input argument [unexpected data type]"),
                                           _CPTC_("Decode::copy_tg_argin"));

          }
          */
          const Tango::DevVarDoubleStringArray *corba_struct;
          _dyn_cmd->get_argin(corba_struct);
          //- decode double data
          tango_array_TO_LV_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, &(corba_struct->dvalue), dyn_cmd_argin_->num_data);
          //- decode string data
          MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                           1L, 
                                           reinterpret_cast<UHandle*>(&dyn_cmd_argin_->str_data), 
                                           corba_struct->svalue.length());
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::copy_tg_argin"));
          }
          LvStringArrayHdl sah = dyn_cmd_argin_->str_data;
		      (*sah)->length = corba_struct->svalue.length();
          ::memset((*sah)->data, 0, (*sah)->length * sizeof(LvMemoryAddress));
          tbfl::size_t l = 0;
          LvStringHdl h = 0;
		      for (tbfl::size_t i = 0; i < (*sah)->length; i++) 
          {
            h = 0;
			      l = static_cast<tbfl::size_t>(::strlen(corba_struct->svalue[i]));
            err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&h), l);
            if (err != noErr)
            {
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::copy_tg_argin"));
            }
			      (*h)->length = l;
            ::memcpy((*h)->data, corba_struct->svalue[i], l);
            (*sah)->data[i] = h;
          }
        } 
        break;
      //-- DEVVAR_STRINGARRAY -------------------------
      case Tango::DEVVAR_STRINGARRAY: 
        {
          /*  
          TODO: modify yat4tango::TangoTraits, not really a problem cause the Tango kernel already protect us against this situation 
          if ( ! _dyn_cmd->check_argument_data_type<Tango::DevVarStringArray>(yat4tango::ARGIN) )
          {
            Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                           _CPTC_("could not extract data from command input argument [unexpected data type]"),
                                           _CPTC_("Decode::copy_tg_argin"));

          }
          */
          const Tango::DevVarStringArray *tango_array;
          _dyn_cmd->get_argin(tango_array);
          MgErr err = ::NumericArrayResize(LvMemoryAddress, 
                                           1L, 
                                           reinterpret_cast<UHandle*>(&(dyn_cmd_argin_->str_data)), 
                                           tango_array->length());
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::copy_tg_argin"));
          }
          LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(dyn_cmd_argin_->str_data);
          if ( ! h || ! (*h) )
          {
            Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                           _CPTC_("unexpected null pointer"),
                                           _CPTC_("DataAdapter::copy_tg_argin"));;
          }
		      (*h)->length = tango_array->length();
          ::memset((*h)->data, 0, (*h)->length * sizeof(LvMemoryAddress));
          tbfl::size_t l = 0;
          LvStringHdl sh = 0;
		      for (tbfl::size_t i = 0; i < (*h)->length; i++)
          {
            sh = 0;
			      l = static_cast<tbfl::size_t>(::strlen((*tango_array)[i]));
            err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&sh), l);
            if (err != noErr)
            {
              Tango::Except::throw_exception(_CPTC_("memory error"),
                                             _CPTC_("memory allocation failed"),
                                             _CPTC_("DataAdapter::copy_tg_argin"));
            }
			      (*sh)->length = l;
            ::memcpy((*sh)->data, (*tango_array)[i], l);
            (*h)->data[i] = sh;
          }
        } 
        break;
      //-- DEVVAR_DOUBLEARRAY ------------------------
      case Tango::DEVVAR_DOUBLEARRAY: 
        {
          COPY_TG_CMD_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_FLOATARRAY -------------------------
      case Tango::DEVVAR_FLOATARRAY: 
        {
          COPY_TG_CMD_ARRAY(Tango::DevVarFloatArray, Tango::DevFloat, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_LONGARRAY --------------------------
      case Tango::DEVVAR_LONGARRAY: 
        { 
          COPY_TG_CMD_ARRAY(Tango::DevVarLongArray, Tango::DevLong, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_ULONGARRAY -------------------------
      case Tango::DEVVAR_ULONGARRAY: 
        {   
          COPY_TG_CMD_ARRAY(Tango::DevVarULongArray, Tango::DevULong, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_LONG64 -----------------------------
      case Tango::DEVVAR_LONG64ARRAY:
        { 
          COPY_TG_CMD_ARRAY(Tango::DevVarLong64Array, Tango::DevLong64, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_ULONG64 ----------------------------
      case Tango::DEVVAR_ULONG64ARRAY:
        { 
          COPY_TG_CMD_ARRAY(Tango::DevVarULong64Array, Tango::DevULong64, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_SHORTARRAY -------------------------
      case Tango::DEVVAR_SHORTARRAY: 
        {
          COPY_TG_CMD_ARRAY(Tango::DevVarShortArray, Tango::DevShort, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_USHORTARRAY ------------------------
      case Tango::DEVVAR_USHORTARRAY: 
        {
          COPY_TG_CMD_ARRAY(Tango::DevVarUShortArray, Tango::DevUShort, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEVVAR_CHARARRAY --------------------------
      case Tango::DEVVAR_CHARARRAY: 
        {
          COPY_TG_CMD_ARRAY(Tango::DevVarCharArray, Tango::DevUChar, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEV_STRING --------------------------------
      case Tango::DEV_STRING:
      case Tango::CONST_DEV_STRING: 
        {
          if ( ! _dyn_cmd->check_argument_data_type<Tango::DevString>(yat4tango::ARGIN) )
          {
            Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                           _CPTC_("could not extract data from command input argument [unexpected data type]"),
                                           _CPTC_("Decode::copy_tg_argin"));

          }
          Tango::ConstDevString s;
          _dyn_cmd->get_argin(s);
          to_lv_str(std::string(s), reinterpret_cast<LvStringHdl&>(dyn_cmd_argin_->num_data));
        } 
        break;
      //-- DEV_BOOLEAN -------------------------------
      case Tango::DEV_BOOLEAN: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevBoolean, _dyn_cmd, dyn_cmd_argin_->num_data);
        }  
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- DEV_ENUM ---------------------------------
      case Tango::DEV_ENUM:
#endif
      //-- DEV_SHORT ---------------------------------
      case Tango::DEV_SHORT: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevShort, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEV_USHORT --------------------------------
      case Tango::DEV_USHORT: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevUShort, _dyn_cmd, dyn_cmd_argin_->num_data);
        }  
        break;
      //-- DEV_LONG ---------------------------------
      case Tango::DEV_LONG: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevLong, _dyn_cmd, dyn_cmd_argin_->num_data);
        }  
        break;      
      //-- DEV_ULONG --------------------------------
      case Tango::DEV_ULONG: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevULong, _dyn_cmd, dyn_cmd_argin_->num_data);
        }  
        break;
      //-- DEV_LONG64 -------------------------------
      case Tango::DEV_LONG64: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevLong64, _dyn_cmd, dyn_cmd_argin_->num_data);
        }  
        break;
      //-- DEV_ULONG64 ------------------------------
      case Tango::DEV_ULONG64: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevULong64, _dyn_cmd, dyn_cmd_argin_->num_data);
        }  
        break;
      //-- DEV_FLOAT --------------------------------
      case Tango::DEV_FLOAT: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevFloat, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEV_DOUBLE -------------------------------
      case Tango::DEV_DOUBLE: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevDouble, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEV_STATE --------------------------------
      case Tango::DEV_STATE: 
        {
          COPY_TG_CMD_SCALAR(Tango::DevState, _dyn_cmd, dyn_cmd_argin_->num_data);
        } 
        break;
      //-- DEFAULT ----------------------------------
      default:
        Tango::Except::throw_exception(_CPTC_("unknown or unsupported TANGO data type"),
                                       _CPTC_("unexpected TANGO data type for command"),
                                       _CPTC_("DataAdapter::copy_tg_argin"));
        break;
    } // switch
  } // try        
  catch (const Tango::DevFailed&) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::copy_tg_argin"));
  }
}

//=============================================================================
// DataAdapter::to_lv_str
//=============================================================================
void DataAdapter::to_lv_str (const std::string& in, LvStringHdl& out)
  throw(Tango::DevFailed)
{ 
  MgErr err = ::NumericArrayResize(uB,
                                   1L,
                                   reinterpret_cast<UHandle*>(&out),
                                   in.size());
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::to_lv_str"));
  }
  
  (*out)->length = in.size();
  ::memcpy((*out)->data, in.c_str(), in.size());
}

//=============================================================================
// DataAdapter::to_std_str
//=============================================================================
void DataAdapter::to_std_str (const LvStringHdl& in, std::string& out)
  throw(Tango::DevFailed)
{
  if ( ! in || ! (*in) )
  {
    Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                   _CPTC_("unexpected null pointer to string"),
                                   _CPTC_("DataAdapter::to_std_str"));
  }
  out.assign(reinterpret_cast<char*>((*in)->data), reinterpret_cast<char*>((*in)->data + (*in)->length));
}

//=============================================================================
// DataAdapter::encode_attr
//=============================================================================
void DataAdapter::encode_attr (const AttributeInfo& attr_info,
                               LvAttributeValuePtr lv_attr,
                               Tango::DeviceAttribute& tg_attr)
    throw (Tango::DevFailed)
{
  //- set common values
  tg_attr.quality = Tango::ATTR_VALID;
  tg_attr.name = attr_info.name;
#ifdef WIN32
  struct _timeb now;
  ::_ftime(&now);
  tg_attr.time.tv_sec  = (CORBA::Long)now.time;
  tg_attr.time.tv_usec = now.millitm * 1000;
  tg_attr.time.tv_nsec = 0;
#else
  struct timeval now;
  ::gettimeofday(&now, NULL);
  tg_attr.time.tv_sec  = now.tv_sec;
  tg_attr.time.tv_usec = now.tv_usec;
  tg_attr.time.tv_nsec = 0;
#endif
  //- convert to TANGO type
  try 
  {
    switch ( attr_info.data_format ) 
    {
      //-- SCALAR ATTRIBUTE -------------------------------------------
      case Tango::SCALAR: 
      {
        //- handle data type
        switch ( attr_info.data_type ) 
        {
          //-- SCALAR::DEV_BOOLEAN -----------------------------------
          case Tango::DEV_BOOLEAN: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevBoolean, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_UCHAR -------------------------------------
          case Tango::DEV_UCHAR: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevUChar, lv_attr, tg_attr);
            } 
            break;
#if (TANGO_VERSION_MAJOR >= 9) 
          //-- SCALAR::DEV_ENUM --------------------------------------
		      case Tango::DEV_ENUM:
#endif
	        //-- SCALAR::DEV_SHORT -------------------------------------
          case Tango::DEV_SHORT: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevShort, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_USHORT ------------------------------------
          case Tango::DEV_USHORT: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevUShort, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_LONG --------------------------------------
          case Tango::DEV_LONG: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevLong, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_ULONG --------------------------------------
          case Tango::DEV_ULONG: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevULong, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_LONG64  -----------------------------------
          case Tango::DEV_LONG64:
            {
              ENCODE_ATTR_SCALAR(Tango::DevLong64, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_ULONG64  ----------------------------------
          case Tango::DEV_ULONG64: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevULong64, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_FLOAT -------------------------------------
          case Tango::DEV_FLOAT: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevFloat, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_DOUBLE ------------------------------------
          case Tango::DEV_DOUBLE: 
            {
              ENCODE_ATTR_SCALAR(Tango::DevDouble, lv_attr, tg_attr);
            } 
            break;
          //-- SCALAR::DEV_STRING ------------------------------------
          case Tango::DEV_STRING:
          case Tango::CONST_DEV_STRING:
            {
              std::string s;
              to_std_str(reinterpret_cast<LvStringHdl&>(lv_attr->data), s);
		          tg_attr << s;
            } 
            break;
          default:
            Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
                                           _CPTC_("unsupported TANGO data type for scalar attribute"),
                                           _CPTC_("DataAdapter::encode_attr"));
            break;
        } // switch::attr_type
      } 
      break;
      //-- SPECTRUM/IMAGE ATTRIBUTE -------------------------------------------
      case Tango::SPECTRUM: 
      case Tango::IMAGE: 
      {
        //- handle data type
        switch ( attr_info.data_type ) 
        {
          //-- SPECTRUM/IMAGE::DEV_BOOLEAN ------------------------------------
          case Tango::DEV_BOOLEAN: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarBooleanArray, Tango::DevBoolean, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM/IMAGE::DEV_UCHAR --------------------------------------
          case Tango::DEV_UCHAR: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarUCharArray, Tango::DevUChar, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM/IMAGE::DEV_SHORT --------------------------------------
          case Tango::DEV_SHORT: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarShortArray, Tango::DevShort, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM/IMAGE::DEV_USHORT --------------------------------------
          case Tango::DEV_USHORT: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarUShortArray, Tango::DevUShort, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM::DEV_LONG ----------------------------------------------
          case Tango::DEV_LONG: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarLongArray, Tango::DevLong, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM::DEV_ULONG ---------------------------------------------
          case Tango::DEV_ULONG: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarULongArray, Tango::DevULong, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM::DEV_LONG64  -------------------------------------------
          case Tango::DEV_LONG64: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarLong64Array, Tango::DevLong64, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM::DEV_ULONG64  -------------------------------------------
          case Tango::DEV_ULONG64: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarULong64Array, Tango::DevULong64, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM/IMAGE::DEV_FLOAT -------------------------------------
          case Tango::DEV_FLOAT: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarFloatArray, Tango::DevFloat, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM/IMAGE::DEV_DOUBLE -------------------------------------
          case Tango::DEV_DOUBLE: 
            {
              ENCODE_ATTR_ARRAY(Tango::DevVarDoubleArray, Tango::DevDouble, lv_attr, tg_attr);
            } 
            break;
          //-- SPECTRUM/IMAGE::DEVVAR_STRINGARRAY -----------------------------
          case Tango::DEV_STRING: 
            {
              if ( Tango::IMAGE == attr_info.data_format )
              {
                Tango::Except::throw_exception(_CPTC_("unsupported feature"),
                                               _CPTC_("there is currently no support for <image of strings> attribute"),
                                               _CPTC_("DataAdapter::encode_attr"));
              }
              LvStringArrayHdl h = reinterpret_cast<LvStringArrayHdl>(lv_attr->data);
              if ( ! h || ! (*h) )
              {
                Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                               _CPTC_("unexpected null pointer"),
                                               _CPTC_("DataAdapter::encode_attr"));;
              }
	            Tango::DevVarStringArray *str_array = new Tango::DevVarStringArray((*h)->length);
              if ( ! str_array ) 
              {
                Tango::Except::throw_exception(_CPTC_("memory error"),
                                               _CPTC_("memory allocation failed"),
                                               _CPTC_("DataAdapter::encode_attr"));
              }
              str_array->length((*h)->length);
              LvStringHdl sh = 0;
	            for (tbfl::size_t i = 0; i < (*h)->length; i++) 
              {
                sh = (*h)->data[i];
                (*str_array)[i] = CORBA::string_alloc((*sh)->length + 1);
                if ( ! (*str_array)[i] ) 
                {
                  delete str_array;
                  Tango::Except::throw_exception(_CPTC_("memory error"),
                                                 _CPTC_("memory allocation failed"),
                                                 _CPTC_("DataAdapter::encode_attr"));
                }
                ::memcpy((*str_array)[i], (*sh)->data, (*sh)->length);
                ::memset((*str_array)[i] + (*sh)->length, 0 , 1);
	            }
              tg_attr << str_array;
            } 
            break;
          default:
            Tango::Except::throw_exception(_CPTC_("unexpected or unsupported TANGO data type"),
                                           _CPTC_("unsupported TANGO data type for attribute"),
                                           _CPTC_("DataAdapter::encode_attr"));
            break;
        } //switch attr_type
      } 
      break;
      //-- UNKNOWN ATTRIBUTE ------------------------------------------
      default:
        std::string d = "unsupported TANGO format for attribute <" + attr_info.name + ">";
        Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
                                       _CPTC_(d.c_str()),
                                       _CPTC_("DataAdapter::encode_attr"));
        break;
    } // switch::attr_format
  } //try
  catch (const Tango::DevFailed&) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::encode_attr"));
  }
}

//=============================================================================
// DataAdapter::copy_lv_attr_value
//=============================================================================
void DataAdapter::copy_lv_attr_value (const AttributeInfo& attr_info,
                                      LvAttributeValuePtr lv_attr,
                                      LvClientRequestReplyDataBuffer& adb)
    throw (Tango::DevFailed)
{
  //- convert to TANGO type
  try
  {
    switch ( attr_info.data_format )
    {
      //-- SCALAR ATTRIBUTE -------------------------------------------
      case Tango::SCALAR:
      {
        //- handle data type
        switch ( attr_info.data_type )
        {
          //-- SCALAR::DEV_STATE -----------------------------------
          case Tango::DEV_STATE:
            {
              COPY_LV_ATTR_SCALAR_VALUE(tbfl::uint32, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_BOOLEAN -----------------------------------
          case Tango::DEV_BOOLEAN:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevBoolean, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_UCHAR -------------------------------------
          case Tango::DEV_UCHAR:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevUChar, lv_attr, adb);
            }
            break;
#if (TANGO_VERSION_MAJOR >= 9) 
		      //-- SCALAR::DEV_ENUM --------------------------------------
		      case Tango::DEV_ENUM:
#endif
	        //-- SCALAR::DEV_SHORT -------------------------------------
		      case Tango::DEV_SHORT:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevShort, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_USHORT ------------------------------------
          case Tango::DEV_USHORT:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevUShort, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_LONG --------------------------------------
          case Tango::DEV_LONG:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevLong, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_ULONG --------------------------------------
          case Tango::DEV_ULONG:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevULong, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_LONG64  -----------------------------------
          case Tango::DEV_LONG64:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevLong64, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_ULONG64  ----------------------------------
          case Tango::DEV_ULONG64:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevULong64, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_FLOAT -------------------------------------
          case Tango::DEV_FLOAT:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevFloat, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_DOUBLE ------------------------------------
          case Tango::DEV_DOUBLE:
            {
              COPY_LV_ATTR_SCALAR_VALUE(Tango::DevDouble, lv_attr, adb);
            }
            break;
          //-- SCALAR::DEV_STRING ------------------------------------
          case Tango::DEV_STRING:
          case Tango::CONST_DEV_STRING:
            {
              LvStringHdl h = reinterpret_cast<LvStringHdl>(lv_attr->data);
              if ( ! h || ! (*h) )
              {
                Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                               _CPTC_("unexpected null input argument"),
                                               _CPTC_("Encode::copy_lv_attr_value"));
              }
              adb.dim_x = 1;
              adb.dim_y = 0;
              adb.str_data = new Tango::DevString[1];
              adb.str_data[0] = CORBA::string_alloc((*h)->length + 1);
              ::memcpy(adb.str_data[0], (*h)->data, (*h)->length);
              ::memset(adb.str_data[0] + (*h)->length, 0 , 1);
            }
            break;
          default:
            Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
                                           _CPTC_("unsupported TANGO data type for scalar attribute"),
                                           _CPTC_("DataAdapter::copy_lv_attr_value"));
            break;
        } // switch::attr_type
      }
      break;
      //-- SPECTRUM/IMAGE ATTRIBUTE -------------------------------------------
      case Tango::SPECTRUM:
      case Tango::IMAGE:
      {
        //- handle data type
        switch ( attr_info.data_type )
        {
          //-- SPECTRUM/IMAGE::DEV_STATE ------------------------------------
          case Tango::DEV_STATE:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevState, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEV_BOOLEAN ------------------------------------
          case Tango::DEV_BOOLEAN:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevBoolean, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEV_UCHAR --------------------------------------
          case Tango::DEV_UCHAR:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevUChar, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEV_SHORT --------------------------------------
          case Tango::DEV_SHORT:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevShort, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEV_USHORT --------------------------------------
          case Tango::DEV_USHORT:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevUShort, lv_attr, adb);
            }
            break;
          //-- SPECTRUM::DEV_LONG ----------------------------------------------
          case Tango::DEV_LONG:
            {
       
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevLong, lv_attr, adb);
            }
            break;
          //-- SPECTRUM::DEV_ULONG ---------------------------------------------
          case Tango::DEV_ULONG:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevULong, lv_attr, adb);
            }
            break;
          //-- SPECTRUM::DEV_LONG64  -------------------------------------------
          case Tango::DEV_LONG64:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevLong64, lv_attr, adb);
            }
            break;
          //-- SPECTRUM::DEV_ULONG64  -------------------------------------------
          case Tango::DEV_ULONG64:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevULong64, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEV_FLOAT -------------------------------------
          case Tango::DEV_FLOAT:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevFloat, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEV_DOUBLE -------------------------------------
          case Tango::DEV_DOUBLE:
            {
              COPY_LV_ATTR_ARRAY_VALUE(Tango::DevDouble, lv_attr, adb);
            }
            break;
          //-- SPECTRUM/IMAGE::DEVVAR_STRINGARRAY -----------------------------
          case Tango::DEV_STRING:
            {
              adb.dim_x = lv_attr->dim_x;
              adb.dim_y = lv_attr->dim_y;
              tbfl::size_t offset = 0;
              LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr->data);
              if ( ! h || ! (*h) )
              {
                Tango::Except::throw_exception(_CPTC_("invalid input argument"),
                                               _CPTC_("unexpected null input argument"),
                                               _CPTC_("Encode::copy_lv_attr_value"));
              }
              const char * data = reinterpret_cast<const char*>((*h)->data);
              if ( Tango::SPECTRUM == attr_info.data_format )
              {
                if ( ! adb.dim_x )
                  return;
                adb.str_data = new Tango::DevString[adb.dim_x];

                for ( tbfl::size_t i = 0; i < static_cast<tbfl::size_t>(adb.dim_x); i++ )
                {
                  adb.str_data[i] = CORBA::string_dup(data + offset);
                  offset += ::strlen(adb.str_data[i]) + 1;
                }
              }
              else
              {
                if ( ! ( adb.dim_x * adb.dim_y ) )
                  return;
                adb.str_data = new Tango::DevString[adb.dim_x * adb.dim_y];
                for ( tbfl::size_t j = 0; j < static_cast<tbfl::size_t>(adb.dim_y); j++ )
                {
                  for ( tbfl::size_t i = 0; i < static_cast<tbfl::size_t>(adb.dim_x); i++ )
                  {
                    tbfl::size_t p = i + j * adb.dim_x;
                    *(adb.str_data +  p) = CORBA::string_dup(data + offset);
                    offset += ::strlen(*(adb.str_data +  p)) + 1;
                  }
                }
              }
            }
            break;
          default:
            Tango::Except::throw_exception(_CPTC_("unexpected or unsupported TANGO data type"),
                                           _CPTC_("unsupported TANGO data type for attribute"),
                                           _CPTC_("DataAdapter::copy_lv_attr_value"));
            break;
        } //switch attr_type
      }
      break;
      //-- UNKNOWN ATTRIBUTE ------------------------------------------
      default:
        std::string d = "unsupported TANGO format for attribute <" + attr_info.name + ">";
        Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
                                       _CPTC_(d.c_str()),
                                       _CPTC_("DataAdapter::copy_lv_attr_value"));
        break;
    } // switch::attr_format
  } //try
  catch (const Tango::DevFailed&)
  {
    throw;
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::copy_lv_attr_value"));
  }
}

//=============================================================================
// DataAdapter::copy_tg_attr_value
//=============================================================================
void DataAdapter::copy_tg_attr_value (Tango::WAttribute& tg_attr, LvClientRequest& crn)
    throw (Tango::DevFailed)
{
  //- extract datacopy_tg_attr_value
  try 
  {
    switch ( tg_attr.get_data_type() )
    {
      //-- SCALAR/SPECTRUM/IMAGE::DEV_STATE --------------
      case Tango::DEV_STATE: 
        {
          switch ( tg_attr.get_data_format() )
          {
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevState, tg_attr, crn);
              crn.data.wav.type = SCA_STATE;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevState, tg_attr, crn);
              crn.data.wav.type = SPE_STATE;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevState, tg_attr, crn);
              crn.data.wav.type = IMG_STATE;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_BOOLEAN ------------
      case Tango::DEV_BOOLEAN: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevBoolean, tg_attr, crn);
              crn.data.wav.type = SCA_BOOLEAN;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevBoolean, tg_attr, crn);
              crn.data.wav.type = SPE_BOOLEAN;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevBoolean, tg_attr, crn);
              crn.data.wav.type = IMG_BOOLEAN;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        } 
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_UCHAR --------------
      case Tango::DEV_UCHAR: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevUChar, tg_attr, crn);
              crn.data.wav.type = SCA_UCHAR;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevUChar, tg_attr, crn);
              crn.data.wav.type = SPE_UCHAR;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevUChar, tg_attr, crn);
              crn.data.wav.type = IMG_UCHAR;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        } 
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_SHORT --------------
      case Tango::DEV_SHORT: 
        {
          //-----------------------------------------------------------------------------
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevShort, tg_attr, crn);
              crn.data.wav.type = SCA_SHORT;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevShort, tg_attr, crn);
              crn.data.wav.type = SPE_SHORT;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevShort, tg_attr, crn);
              crn.data.wav.type = IMG_SHORT;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- SCALAR::DEV_ENUM --------------------------------------
      case Tango::DEV_ENUM:
        {
          switch ( tg_attr.get_data_format() )
          { 
	          case Tango::SCALAR:
		          COPY_TG_ATTR_SCALAR(Tango::DevShort, tg_attr, crn);
		          crn.data.wav.type = SCA_SHORT;
		          break;
	          default:
		          Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
				                                     _CPTC_("unsupported TANGO data type [spectrum and image of Tango::DevEnum are not supported]"),
				                                     _CPTC_("DataAdapter::decode_attr"));
		          break; 
	        }
	      }
	      break;
#endif
      //-- SCALAR/SPECTRUM/IMAGE::DEV_USHORT -------------
      case Tango::DEV_USHORT: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevUShort, tg_attr, crn);
              crn.data.wav.type = SCA_USHORT;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevUShort, tg_attr, crn);
              crn.data.wav.type = SPE_USHORT;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevUShort, tg_attr, crn);
              crn.data.wav.type = IMG_USHORT;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_LONG  --------------
      case Tango::DEV_LONG: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevLong, tg_attr, crn);
              crn.data.wav.type = SCA_LONG;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevLong, tg_attr, crn);
              crn.data.wav.type = SPE_LONG;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevLong, tg_attr, crn);
              crn.data.wav.type = IMG_LONG;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_ULONG  -------------
      case Tango::DEV_ULONG: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevULong, tg_attr, crn);
              crn.data.wav.type = SCA_ULONG;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevULong, tg_attr, crn);
              crn.data.wav.type = SPE_ULONG;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevULong, tg_attr, crn);
              crn.data.wav.type = IMG_ULONG;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
       //-- SCALAR/SPECTRUM/IMAGE::DEV_LONG64  -----------
      case Tango::DEV_LONG64: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevLong64, tg_attr, crn);
              crn.data.wav.type = SCA_LONG64;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevLong64, tg_attr, crn);
              crn.data.wav.type = SPE_LONG64;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevLong64, tg_attr, crn);
              crn.data.wav.type = IMG_LONG64;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      case Tango::DEV_ULONG64: 
        {
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevULong64, tg_attr, crn);
              crn.data.wav.type = SCA_ULONG64;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevULong64, tg_attr, crn);
              crn.data.wav.type = SPE_ULONG64;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevULong64, tg_attr, crn);
              crn.data.wav.type = IMG_ULONG64;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_FLOAT --------------
      case Tango::DEV_FLOAT: 
        { 
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevFloat, tg_attr, crn);
              crn.data.wav.type = SCA_FLOAT;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevFloat, tg_attr, crn);
              crn.data.wav.type = SPE_FLOAT;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevFloat, tg_attr, crn);
              crn.data.wav.type = IMG_FLOAT;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_DOUBLE -------------
      case Tango::DEV_DOUBLE: 
        {
          COPY_TG_ATTR_ARRAY(Tango::DevDouble, tg_attr, crn);
          switch ( tg_attr.get_data_format() )
          { 
            case Tango::SCALAR:
              COPY_TG_ATTR_SCALAR(Tango::DevDouble, tg_attr, crn);
              crn.data.wav.type = SCA_DOUBLE;
              break;
            case Tango::SPECTRUM:
              COPY_TG_ATTR_ARRAY(Tango::DevDouble, tg_attr, crn);
              crn.data.wav.type = SPE_DOUBLE;
              break;
            case Tango::IMAGE:
              COPY_TG_ATTR_ARRAY(Tango::DevDouble, tg_attr, crn);
              crn.data.wav.type = IMG_DOUBLE;
              break;
            default:
              crn.data.wav.type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      //-- SPECTRUM/IMAGE::DEV_STRING ------------
      case Tango::DEV_STRING: 
        switch ( tg_attr.get_data_format() )
        {
          case Tango::SCALAR:
            {
              Tango::DevString s;
              tg_attr.get_write_value(s);
              MgErr err = ::NumericArrayResize(uB,
                                               1L,
                                               reinterpret_cast<UHandle*>(&crn.data.wav.buffer),
                                               ::strlen(s) + 1);
              if (err != noErr)
              {
                Tango::Except::throw_exception(_CPTC_("memory error"),
                                               _CPTC_("memory allocation failed"),
                                               _CPTC_("DataAdapter::copy_tg_attr_value"));
              }
              LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(crn.data.wav.buffer);
              (*h)->length = ::strlen(s) + 1;
              ::memcpy((*h)->data, s, ::strlen(s));
              (*h)->data[::strlen(s)] = 0;
              crn.data.wav.dimx = 1;
              crn.data.wav.dimy = 0;
              crn.data.wav.type = SCA_STRING;
            } 
            break;
          case Tango::SPECTRUM:
          case Tango::IMAGE:
            {
              //-------------------------------------------------------------------------------------------
              //- big trick here!
              //-------------------------------------------------------------------------------------------
              //- data is generically passed to the Labview env. through <LvAttributeValue.data> - which
              //- is supposed to be an array of bytes reinterpreted by the extractor VIs. means that we 
              //- can't pass the result as a 1D array of strings. we choose to pass it as an array of bytes
              //- in which the strings (i.e. the ones contained into the Tango::DevVarStringArray) are 
              //- stored contiguously and separated by a '\0' character. the dedicated extractor VI will 
              //- have to rebuild the expected 1D (or 2D) array of srings (transparent op. for end user).
              //-------------------------------------------------------------------------------------------
              const Tango::ConstDevString *sa;
              tg_attr.get_write_value(sa);
              crn.data.wav.dimx = tg_attr.get_w_dim_x();
              crn.data.wav.dimy = tg_attr.get_w_dim_y();
              crn.data.wav.type = tg_attr.get_data_format() == Tango::SPECTRUM ? SPE_STRING : IMG_STRING;
              //- compute sizeof "generic u8" dest buffer: make room for the strings
              tbfl::size_t total_size = 0;
              tbfl::size_t num_strings = crn.data.wav.dimx * ( crn.data.wav.dimy ? crn.data.wav.dimy : 1 );
              for ( tbfl::size_t i = 0; i < num_strings; i++ )
              {  
                total_size += static_cast<tbfl::size_t>(::strlen(sa[i]));
              }
              //- compute sizeof "generic u8" dest buffer: make room for string separators
              total_size += num_strings - 1;
              //- resize dest buffer
              MgErr err = ::NumericArrayResize(uB,
                                               1L,
                                               reinterpret_cast<UHandle*>(&crn.data.wav.buffer),
                                               total_size);
              if (err != noErr)
              {
                Tango::Except::throw_exception(_CPTC_("memory error"),
                                               _CPTC_("memory allocation failed"),
                                               _CPTC_("DataAdapter::copy_tg_attr_value"));
              }
              //- reinterpret dest as <LvGenericNumArrayHdl>
              LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(crn.data.wav.buffer);
              (*h)->length = total_size;
              //- reset memory (will put '\0' between strings at the same time)
              char * p = reinterpret_cast<char*>((*h)->data);
              ::memset(p, 0, total_size);
              //- are we handling an image?
              bool handling_image = tg_attr.get_data_format() == Tango::IMAGE;
              //- copy strings to buffer
              for (tbfl::size_t i = 0; i < num_strings; i++)
              {
                tbfl::size_t str_len = static_cast<tbfl::size_t>(::strlen(sa[i]));
                ::memcpy(p, sa[i], str_len);
                p += str_len;
                if ( handling_image && i && ( (i + 1) % crn.data.wav.dimx ) == 0 )
                  *p = static_cast<char>(13); //- lines are separated by a </r> char
                p += 1;
              }
            }
            break;
        }
        break;
      //-- UNKNOWN TYPE -----------------------------------
      default: 
        Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
                                       _CPTC_("unsupported TANGO data type for attribute"),
                                       _CPTC_("DataAdapter::decode_attr"));
        break;
    } // switch attr_type
  } // try
  catch (const Tango::DevFailed &) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::decode_attr"));
  }
}

//=============================================================================
// DataAdapter::decode_attr
//=============================================================================
void DataAdapter::decode_attr (const AttributeInfo& attr_info,
                               Tango::DeviceAttribute& tg_attr,
                               LvAttributeValuePtr lv_attr)
    throw (Tango::DevFailed)
{
  //- extract data
  try 
  {
    bool is_rw = ( Tango::READ_WRITE == attr_info.writable )
                   || ( Tango::READ_WITH_WRITE == attr_info.writable );
    
    switch ( attr_info.data_type )
    {
      //-- SCALAR/SPECTRUM/IMAGE::DEV_STATE --------------
      case Tango::DEV_STATE: 
        {
          switch ( attr_info.data_format )
          {
            case Tango::SCALAR:
            {
              switch ( sizeof(Tango::DevState) )
              {
                case 2:
                  Decode<Tango::DevState, Tango::DevUShort>::attr_scalar(tg_attr, lv_attr);
                  break;
                case 4:
                  Decode<Tango::DevState, Tango::DevULong>::attr_scalar(tg_attr, lv_attr);
                  break;
                default:
                  Tango::Except::throw_exception(_CPTC_("internal error"),
                                                 _CPTC_("unexpected Tango::DevState size [neither 2 nor 4 bytes long]"),
                                                 _CPTC_("DataAdapter::decode_attr"));
                  break;
              }
              break;
            }
            default:
            {
              switch (sizeof(Tango::DevState))
              {
                case 2:
                  DECODE_ATTR_ARRAY(attr_info, Tango::DevVarStateArray, Tango::DevUShort, tg_attr, lv_attr);
                  break;
                case 4:
                  DECODE_ATTR_ARRAY(attr_info, Tango::DevVarStateArray, Tango::DevULong, tg_attr, lv_attr);
                  break;
                default:
                  Tango::Except::throw_exception(_CPTC_("internal error"),
                                                 _CPTC_("unexpected Tango::DevState size [neither 2 nor 4 bytes long]"),
                                                 _CPTC_("DataAdapter::decode_attr"));
                  break;
              }
            }
          } //- switch (frmt
        } 
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_BOOLEAN ------------
      case Tango::DEV_BOOLEAN: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarBooleanArray, Tango::DevBoolean, tg_attr, lv_attr);
        } 
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_UCHAR --------------
      case Tango::DEV_UCHAR: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarUCharArray, Tango::DevUChar, tg_attr, lv_attr);
        } 
        break;
#if (TANGO_VERSION_MAJOR >= 9) 
      //-- SCALAR::DEV_ENUM ------------------------------
	    case Tango::DEV_ENUM:
#endif
      //-- SCALAR/SPECTRUM/IMAGE::DEV_SHORT --------------
      case Tango::DEV_SHORT: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarShortArray, Tango::DevShort, tg_attr, lv_attr);
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_USHORT -------------
      case Tango::DEV_USHORT: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarUShortArray, Tango::DevUShort, tg_attr, lv_attr);
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_LONG  --------------
      case Tango::DEV_LONG: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarLongArray, Tango::DevLong, tg_attr, lv_attr);
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_ULONG  -------------
      case Tango::DEV_ULONG: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarULongArray, Tango::DevULong, tg_attr, lv_attr);
        }
        break;
       //-- SCALAR/SPECTRUM/IMAGE::DEV_LONG64  -----------
      case Tango::DEV_LONG64: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarLong64Array, Tango::DevLong64, tg_attr, lv_attr);
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_ULONG64  -----------
      case Tango::DEV_ULONG64: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarULong64Array, Tango::DevULong64, tg_attr, lv_attr);
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_FLOAT --------------
      case Tango::DEV_FLOAT: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarFloatArray, Tango::DevFloat, tg_attr, lv_attr);
        }
        break;
      //-- SCALAR/SPECTRUM/IMAGE::DEV_DOUBLE -------------
      case Tango::DEV_DOUBLE: 
        {
          DECODE_ATTR_ARRAY(attr_info, Tango::DevVarDoubleArray, Tango::DevDouble, tg_attr, lv_attr);
        }
        break;
      //-- SPECTRUM/IMAGE::DEV_STRING ------------
      case Tango::DEV_STRING: 
        switch ( attr_info.data_format )
        {
          case Tango::SCALAR:
            {
              tg_attr.exceptions(Tango::DeviceAttribute::isempty_flag | Tango::DeviceAttribute::wrongtype_flag);
              std::string s;
              tg_attr >> s;
              MgErr err = ::NumericArrayResize(uB,  1L, reinterpret_cast<UHandle*>(&lv_attr->data), s.size());
              if (err != noErr) 
              {
                Tango::Except::throw_exception(_CPTC_("memory error"),
                                               _CPTC_("memory allocation failed"),
                                               _CPTC_("DataAdapter::decode_attr"));
              }
              LvStringHdl h = reinterpret_cast<LvStringHdl>(lv_attr->data);
              (*h)->length = static_cast<tbfl::size_t>(s.size());
              ::memcpy((*h)->data, s.c_str(), (*h)->length);
            } 
            break;
          case Tango::SPECTRUM:
          case Tango::IMAGE:
            {
              //-------------------------------------------------------------------------------------------
              //- big trick here!
              //-------------------------------------------------------------------------------------------
              //- data is generically passed to the Labview env. through <LvAttributeValue.data> - which
              //- is supposed to be an array of bytes reinterpreted by the extractor VIs. means that we 
              //- can't pass the result as a 1D array of strings. we choose to pass it as an array of bytes
              //- in which the strings (i.e. the ones contained into the Tango::DevVarStringArray) are 
              //- stored contiguously and separated by a '\0' character. the dedicated extractor VI will 
              //- have to rebuild the expected 1D (or 2D) array of srings (transparent op. for end user).
              //-------------------------------------------------------------------------------------------
              Tango::DevVarStringArray * tango_array = 0;
              if ( (tg_attr >> tango_array) == false || ! tango_array )
              {
                delete tango_array;
                Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                               _CPTC_("data extraction failed failed [wrong data type?]"),
                                               _CPTC_("DataAdapter::decode_attr"));
              }
              //- get read dim of the incoming data (no write part for spectrum and images)
              Tango::AttributeDimension r_dim = tg_attr.get_r_dimension();
              //- compute sizeof "generic u8" dest buffer: make room for the strings
              tbfl::size_t total_size = 0;
              tbfl::size_t num_strings_in_tango_array = r_dim.dim_x * ( r_dim.dim_y ? r_dim.dim_y : 1 );
              for (tbfl::size_t i = 0; i < num_strings_in_tango_array; i++)
              {
                total_size += static_cast<tbfl::size_t>(::strlen((*tango_array)[i]));
              }
              //- compute sizeof "generic u8" dest buffer: make room for string separators
              total_size += num_strings_in_tango_array - 1;
              //- resize dest buffer
              MgErr err = ::NumericArrayResize(uB, 
                                               1L, 
                                               reinterpret_cast<UHandle*>(&lv_attr->data),
                                               total_size);
              if (err != noErr) 
              {
                delete tango_array;
                Tango::Except::throw_exception(_CPTC_("memory error"),
                                               _CPTC_("memory allocation failed"),
                                               _CPTC_("DataAdapter::decode_attr"));
              }
              //- reinterpret dest as <LvGenericNumArrayHdl>
              LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_attr->data);
              (*h)->length = total_size;
              //- reset memory (will put '\0' between strings at the same time)
              char * p = reinterpret_cast<char*>((*h)->data);
              ::memset(p, 0, total_size);
              //- are we handling an image?
              bool handling_image = Tango::IMAGE == attr_info.data_format;
              //- copy strings to buffer
              for (tbfl::size_t i = 0; i < num_strings_in_tango_array; i++) 
              {
                tbfl::size_t str_len = static_cast<tbfl::size_t>(::strlen((*tango_array)[i]));
                ::memcpy(p, (*tango_array)[i], str_len);
                p += str_len;
                if ( handling_image && i && ( (i + 1) % r_dim.dim_x ) == 0 )
                  *p = static_cast<char>(13); //- lines are separated by a <EOL> char
                p += 1;
              }
              delete tango_array;
            }
            break;
        }
        break;
      //-- UNKNOWN TYPE -----------------------------------
      default: 
        Tango::Except::throw_exception(_CPTC_("unexpected TANGO data type"),
                                       _CPTC_("unsupported TANGO data type for attribute"),
                                       _CPTC_("DataAdapter::decode_attr"));
        break;
    } // switch attr_type
  } // try
  catch (const Tango::DevFailed &) 
  {
    throw;
	}
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::decode_attr"));
  }
}

//=============================================================================
// DataAdapter::tg_to_lv_attribute_value
//=============================================================================
void DataAdapter::tg_to_lv_attribute_value (const AttributeInfo& attr_info,
                                               Tango::DeviceAttribute& tg_attr, 
                                               LvAttributeValuePtr lv_attr)
  throw (Tango::DevFailed)
{
  tbfl::int32 err = 0;

  if (! lv_attr) 
  {
    Tango::Except::throw_exception(_CPTC_("unexpected null pointer"),
                                   _CPTC_("unexpected null LvAttributeValuePtr as input argument"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
  }

  try 
  {
    LvStringHdl h = reinterpret_cast<LvStringHdl>(lv_attr->dev_name);
    if (! h || ! (*h))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("invalid LvAttributeValue [does NOT seem to be properly initialized on LabVIEW side]"),
                                     _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
    }

    const std::string& dev_name = attr_info.dev;
    if ((*h)->length != dev_name.size())
    {
      err = ::NumericArrayResize(uB,  1L, (UHandle*)&(lv_attr->dev_name), dev_name.size());
      if (err != noErr) 
      {
        Tango::Except::throw_exception(_CPTC_("memory error"),
                                       _CPTC_("memory allocation failed"),
                                       _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
      }
    }
    h = reinterpret_cast<LvStringHdl>(lv_attr->dev_name);
    (*h)->length = static_cast<tbfl::size_t>(dev_name.size());
    ::memcpy((*h)->data, dev_name.c_str(), (*h)->length);
      
    h = reinterpret_cast<LvStringHdl>(lv_attr->attr_name);
    if (! h || ! (*h))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("invalid LvAttributeValue [does NOT seem to be properly initialized on LabVIEW side]"),
                                     _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
    }

    if ((*h)->length != attr_info.name.size())
    {
      err = ::NumericArrayResize(uB,  1L, (UHandle*)&(lv_attr->attr_name), attr_info.name.size());
      if (err != noErr) 
      {
        Tango::Except::throw_exception(_CPTC_("memory error"),
                                       _CPTC_("memory allocation failed"),
                                       _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
      }
    }
    h = reinterpret_cast<LvStringHdl>(lv_attr->attr_name);
    (*h)->length = static_cast<tbfl::size_t>(attr_info.name.size());
    ::memcpy((*h)->data, attr_info.name.c_str(), (*h)->length);
      

    if ( tg_attr.has_failed() || tg_attr.is_empty() || tg_attr.quality == Tango::ATTR_INVALID )
    {
      //- set data type to its default value
      lv_attr->data_type = UNKNOWN_ATTR_DATA_TYPE;
    }
    else
    {
      switch ( attr_info.data_format )
      {
        case Tango::SCALAR:
        {
          switch ( attr_info.data_type )
          {
            case Tango::DEV_STRING:
              lv_attr->data_type = SCA_STRING;
              break;
            case Tango::DEV_BOOLEAN:
              lv_attr->data_type = SCA_BOOLEAN;
              break; 
            case Tango::DEV_UCHAR:
              lv_attr->data_type = SCA_UCHAR;
              break; 
            case Tango::DEV_SHORT:
              lv_attr->data_type = SCA_SHORT;
              break; 
            case Tango::DEV_USHORT:
              lv_attr->data_type = SCA_USHORT;
              break; 
            case Tango::DEV_LONG:
              lv_attr->data_type = SCA_LONG;
              break; 
            case Tango::DEV_ULONG:
              lv_attr->data_type = SCA_ULONG;
              break; 
            case Tango::DEV_LONG64:
              lv_attr->data_type = SCA_LONG64;
              break; 
            case Tango::DEV_ULONG64:
              lv_attr->data_type = SCA_ULONG64;
              break; 
            case Tango::DEV_FLOAT:
              lv_attr->data_type = SCA_FLOAT;
              break;
            case Tango::DEV_DOUBLE:
              lv_attr->data_type = SCA_DOUBLE;
              break; 
            case Tango::DEV_STATE:
              lv_attr->data_type = SCA_STATE;
              break;
            default:
              lv_attr->data_type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
        case Tango::SPECTRUM:
        {
          switch ( attr_info.data_type )
          {
            case Tango::DEV_STRING:
              lv_attr->data_type = SPE_STRING;
              break;
            case Tango::DEV_BOOLEAN:
              lv_attr->data_type = SPE_BOOLEAN;
              break; 
            case Tango::DEV_UCHAR:
              lv_attr->data_type = SPE_UCHAR;
              break; 
            case Tango::DEV_SHORT:
              lv_attr->data_type = SPE_SHORT;
              break; 
            case Tango::DEV_USHORT:
              lv_attr->data_type = SPE_USHORT;
              break; 
            case Tango::DEV_LONG:
              lv_attr->data_type = SPE_LONG;
              break; 
            case Tango::DEV_ULONG:
              lv_attr->data_type = SPE_ULONG;
              break; 
            case Tango::DEV_LONG64:
              lv_attr->data_type = SPE_LONG64;
              break; 
            case Tango::DEV_ULONG64:
              lv_attr->data_type = SPE_ULONG64;
              break;
            case Tango::DEV_FLOAT:
              lv_attr->data_type = SPE_FLOAT;
              break;
            case Tango::DEV_DOUBLE:
              lv_attr->data_type = SPE_DOUBLE;
              break; 
            case Tango::DEV_STATE:
              lv_attr->data_type = SPE_STATE;
              break;
            default:
              lv_attr->data_type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
        case Tango::IMAGE:
        { 
          switch ( attr_info.data_type )
          {
            case Tango::DEV_STRING:
              lv_attr->data_type = IMG_STRING;
              break;
            case Tango::DEV_BOOLEAN:
              lv_attr->data_type = IMG_BOOLEAN;
              break; 
            case Tango::DEV_UCHAR:
              lv_attr->data_type = IMG_UCHAR;
              break; 
            case Tango::DEV_SHORT:
              lv_attr->data_type = IMG_SHORT;
              break; 
            case Tango::DEV_USHORT:
              lv_attr->data_type = IMG_USHORT;
              break; 
            case Tango::DEV_LONG:
              lv_attr->data_type = IMG_LONG;
              break; 
            case Tango::DEV_ULONG:
              lv_attr->data_type = IMG_ULONG;
              break; 
            case Tango::DEV_LONG64:
              lv_attr->data_type = IMG_LONG64;
              break; 
            case Tango::DEV_ULONG64:
              lv_attr->data_type = IMG_ULONG64;
              break;
            case Tango::DEV_FLOAT:
              lv_attr->data_type = IMG_FLOAT;
              break;
            case Tango::DEV_DOUBLE:
              lv_attr->data_type = IMG_DOUBLE;
              break; 
            case Tango::DEV_STATE:
              lv_attr->data_type = IMG_STATE;
              break;
            default:
              lv_attr->data_type = UNKNOWN_ATTR_DATA_TYPE;
              break;
          }
        }
        break;
      } 
    }

    //- set attribute quality (both value and string)
    std::string s;
    if (tg_attr.has_failed() || tg_attr.is_empty())
    {
      lv_attr->quality_val = 1;
	    s = "INVALID";
    }
    else
    {
      switch (tg_attr.quality)
      {
        case Tango::ATTR_VALID:
	        lv_attr->quality_val = 0;
	        s = "VALID";
	        break;
        case Tango::ATTR_INVALID:
	        lv_attr->quality_val = 1;
	        s = "INVALID";
	        break;
        case Tango::ATTR_ALARM:
	        lv_attr->quality_val = 2;
	         s = "ALARM";
	        break;
        default:
	        lv_attr->quality_val = -1;
	        s = "UNKNOWN";
	        break;
      }
    }
    
    //- quality str
    h = reinterpret_cast<LvStringHdl>(lv_attr->quality_str);
    if (! h || ! (*h))
    {
      Tango::Except::throw_exception(_CPTC_("invalid parameter"),
                                     _CPTC_("invalid LvAttributeValue [does NOT seem to be properly initialized on LabVIEW side]"),
                                     _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
    }
    err = ::NumericArrayResize(uB,  1L, (UHandle*)&(lv_attr->quality_str), s.size());
    if (err != noErr)
    {
      Tango::Except::throw_exception(_CPTC_("memory error"),
                                     _CPTC_("memory allocation failed"),
                                     _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
    }
    h = reinterpret_cast<LvStringHdl>(lv_attr->quality_str);
    (*h)->length = static_cast<tbfl::size_t>(s.size());
    ::memcpy((*h)->data, s.c_str(), (*h)->length);

    //- set timestamp
    if (tg_attr.has_failed() || tg_attr.is_empty())
    {
      lv_attr->timestamp = 0;
    }
    else
    {
      //- set timestamp
      lv_attr->timestamp = kTIME_OFFSET +
                        (double)(tg_attr.time.tv_sec) +
				                 1.E-6 * (double)(tg_attr.time.tv_usec) +
				                 1.E-9 * (double)(tg_attr.time.tv_nsec);
    }

    //- set dims
    if (tg_attr.has_failed() || tg_attr.is_empty() || tg_attr.quality == Tango::ATTR_INVALID)
    {
      lv_attr->dim_x = 0;
      lv_attr->dim_y = 0;
    }
    else
    {
      Tango::AttributeDimension r_dim = tg_attr.get_r_dimension();
      lv_attr->dim_x = r_dim.dim_x;
      lv_attr->dim_y = r_dim.dim_y;
    }

    //- set individual error code & error stack
    if (tg_attr.has_failed() || tg_attr.is_empty() || tg_attr.quality == Tango::ATTR_INVALID)
    {
      //- set individual error code 
      lv_attr->error_code = -1;
      //- set individual error stack
      try
      {
        if (tg_attr.has_failed())
        {
          throw Tango::DevFailed(tg_attr.get_err_stack());
        }
        else
        {
          Tango::Except::throw_exception(_CPTC_("data error"),
                                         _CPTC_("empty or invalid attribute value received from device"),
                                         _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
        }
      }
      catch (const Tango::DevFailed &e)
      {
        //- set individual error stack
        ERROR_STACK->devfailed(e, lv_attr->error_stack);
      }
    }
    else
    {
      //- set individual error code 
      lv_attr->error_code = 0;
      //- set individual error stack
      ERROR_STACK->no_error("read_attributes", lv_attr->error_stack);
    }

    //- no data to decode...
    if (tg_attr.has_failed() || tg_attr.is_empty() || tg_attr.quality == Tango::ATTR_INVALID)
    {
      lv_attr->data = 0;
      return;
    }

    //- decode data (may throw an exception)
    try 
    {
      decode_attr(attr_info, tg_attr, lv_attr);
    }
    catch (const Tango::DevFailed& e) 
    {
      //- set individual error code 
      lv_attr->error_code = -1;
      //- set individual error stack
      ERROR_STACK->devfailed(e, lv_attr->error_stack);
    }
    catch (...) 
    {
      //- set individual error code 
      lv_attr->error_code = -1;
      //- set individual error stack 
      ERROR_STACK->unknown("DataAdapter::tg_to_lv_attribute_value", lv_attr->error_stack);
    }
  }
  catch (Tango::DevFailed& e)
  {
    Tango::Except::re_throw_exception(e,
                                      _CPTC_("data extraction failed"),
                                      _CPTC_("Tango exception caught while trying to extract attribute value"),
                                      _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
  }
  catch (...) 
  {
    Tango::Except::throw_exception(_CPTC_("data extraction failed"),
                                   _CPTC_("unknown exception caught while trying to extract attribute value"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_value"));
  }
}

//=============================================================================
// DataAdapter::lv_to_tg_attribute_info
//=============================================================================
void DataAdapter::lv_to_tg_attribute_info (const std::string& dev_name,
                                           const LvAttributeInfo& lai,
                                           Tango::AttributeInfo& tai)
  throw (Tango::DevFailed)
{
  //- the following fields can't be changed here: access, format, type and level

  //- LvAttributeInfo.dev_name
  to_std_str(reinterpret_cast<LvStringHdl>(lai.attr_name), tai.name);

  //- LvAttributeInfo.disp_label
  to_std_str(lai.disp_label, tai.label);

  //- LvAttributeInfo.disp_unit
  to_std_str(lai.disp_unit, tai.unit);

  //- LvAttributeInfo.disp_format
  to_std_str(lai.disp_format, tai.format);

  //- LvAttributeInfo.min_value
  to_std_str(lai.min_value, tai.min_value);

  //- LvAttributeInfo.max_value
  to_std_str(lai.max_value, tai.max_value);

  //- LvAttributeInfo.min_alarm
  to_std_str(lai.min_alarm, tai.min_alarm);

  //- LvAttributeInfo.max_alarm
  to_std_str(lai.max_alarm, tai.max_alarm);
}

//=============================================================================
// DataAdapter::tg_to_lv_attribute_info
//=============================================================================
void DataAdapter::tg_to_lv_attribute_info(const std::string& dev_name,
                                          const Tango::AttributeInfo & tai,
                                          LvAttributeInfo & lai)
  throw (Tango::DevFailed)
{
  //- LvAttributeInfo.dev_name
  tbfl::size_t l = static_cast<tbfl::size_t>(dev_name.size());
  MgErr err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.dev_name), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  LvStringHdl h = reinterpret_cast<LvStringHdl>(lai.dev_name);
  (*h)->length = l;
  ::memcpy((*h)->data, dev_name.c_str(), l);
  //- LvAttributeInfo.attr_name
  l = static_cast<tbfl::size_t>(tai.name.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.attr_name), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = reinterpret_cast<LvStringHdl>(lai.attr_name);
  (*h)->length = l;
  ::memcpy((*h)->data, tai.name.c_str(), l);
  //- LvAttributeInfo.access
  lai.access = static_cast<short int>(tai.writable);
  //- LvAttributeInfo.format
  lai.format = static_cast<short int>(tai.data_format);
  //- LvAttributeInfo.type
  lai.type = static_cast<short int>(tai.data_type);
  //- LvAttributeInfo.level
  lai.level = static_cast<short int>(tai.disp_level);
  //- LvAttributeInfo.disp_label
  l = static_cast<tbfl::size_t>(tai.label.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.disp_label), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.disp_label;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.label.c_str(), l);
  //- LvAttributeInfo.disp_unit
  l = static_cast<tbfl::size_t>(tai.unit.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.disp_unit), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.disp_unit;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.unit.c_str(), l);
  //- LvAttributeInfo.format
  l = static_cast<tbfl::size_t>(tai.format.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.disp_format), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.disp_format;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.format.c_str(), l);
  //- LvAttributeInfo.min_value
  l = static_cast<tbfl::size_t>(tai.min_value.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.min_value), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
      _CPTC_("memory allocation failed"),
      _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.min_value;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.min_value.c_str(), l);
  //- LvAttributeInfo.max_value
  l = static_cast<tbfl::size_t>(tai.max_value.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.max_value), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
      _CPTC_("memory allocation failed"),
      _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.max_value;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.max_value.c_str(), l);
  //- LvAttributeInfo.min_alarm
  l = static_cast<tbfl::size_t>(tai.min_alarm.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.min_alarm), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
      _CPTC_("memory allocation failed"),
      _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.min_alarm;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.min_alarm.c_str(), l);
  //- LvAttributeInfo.max_alarm
  l = static_cast<tbfl::size_t>(tai.max_alarm.size());
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.max_alarm), l);
  if (err != noErr)
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
      _CPTC_("memory allocation failed"),
      _CPTC_("DataAdapter::tg_to_lv_attribute_info"));
  }
  h = lai.max_alarm;
  (*h)->length = l;
  ::memcpy((*h)->data, tai.max_alarm.c_str(), l);
}

//=============================================================================
// DataAdapter::lv_to_tg_attribute_config
//=============================================================================
void DataAdapter::lv_to_tg_attribute_config (const std::string& dev_name,
                                             const LvAttributeInfo& lai,
                                             Tango::AttributeConfig& tac)
  throw (Tango::DevFailed)
{
  //- the following fields can't be changed here: access, format, type and level

  //- LvAttributeInfo.dev_name
  std::string s;
  to_std_str(reinterpret_cast<LvStringHdl>(lai.attr_name), s);
  tac.name = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.disp_label
  to_std_str(lai.disp_label, s);
  tac.label = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.disp_unit
  to_std_str(lai.disp_unit, s);
  tac.unit = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.disp_format
  to_std_str(lai.disp_format, s);
  tac.format = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.min_value
  to_std_str(lai.min_value, s);
  tac.min_value = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.max_value
  to_std_str(lai.max_value, s);
  tac.max_value = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.min_alarm
  to_std_str(lai.min_alarm, s);
  tac.min_alarm = CORBA::string_dup(s.c_str());

  //- LvAttributeInfo.max_alarm
  to_std_str(lai.max_alarm, s);
  tac.max_alarm = CORBA::string_dup(s.c_str());
}

//=============================================================================
// DataAdapter::tg_to_lv_attribute_config
//=============================================================================
void DataAdapter::tg_to_lv_attribute_config (const std::string& dev_name,
                                             const Tango::AttributeConfig & tac,
                                             LvAttributeInfo & lai)
  throw (Tango::DevFailed)
{
  //- LvAttributeInfo.dev_name
  tbfl::size_t l = static_cast<tbfl::size_t>(dev_name.size());
  MgErr err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.dev_name), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  LvStringHdl h = reinterpret_cast<LvStringHdl>(lai.dev_name);
  (*h)->length = l;
  ::memcpy((*h)->data, dev_name.c_str(), l);
  //- LvAttributeInfo.attr_name
  l = static_cast<tbfl::size_t>(::strlen(tac.name));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.attr_name), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = reinterpret_cast<LvStringHdl>(lai.attr_name);
  (*h)->length = l;
  ::memcpy((*h)->data, tac.name, l);
  //- LvAttributeInfo.access
  lai.access = static_cast<short int>(tac.writable);
  //- LvAttributeInfo.format
  lai.format = static_cast<short int>(tac.data_format);
  //- LvAttributeInfo.type
  lai.type = static_cast<short int>(tac.data_type);
  //- LvAttributeInfo.level
  lai.level = static_cast<short int>(Tango::DL_UNKNOWN);
  //- LvAttributeInfo.disp_label
  l = static_cast<tbfl::size_t>(::strlen(tac.label));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.disp_label), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.disp_label;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.label, l);
  //- LvAttributeInfo.disp_unit
  l = static_cast<tbfl::size_t>(::strlen(tac.unit));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.disp_unit), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.disp_unit;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.unit, l);
  //- LvAttributeInfo.format
  l = static_cast<tbfl::size_t>(::strlen(tac.format));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.disp_format), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.disp_format;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.format, l);
  //- LvAttributeInfo.min_value
  l = static_cast<tbfl::size_t>(::strlen(tac.min_value));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.min_value), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.min_value;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.min_value, l);
  //- LvAttributeInfo.max_value
  l = static_cast<tbfl::size_t>(::strlen(tac.max_value));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.max_value), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.max_value;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.max_value, l);
  //- LvAttributeInfo.min_alarm
  l = static_cast<tbfl::size_t>(::strlen(tac.min_alarm));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.min_alarm), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.min_alarm;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.min_alarm, l);
  //- LvAttributeInfo.max_alarm
  l = static_cast<tbfl::size_t>(::strlen(tac.max_alarm));
  err = ::NumericArrayResize(iB, 1L, (UHandle*)&(lai.max_alarm), l);
  if ( err != noErr )
  {
    Tango::Except::throw_exception(_CPTC_("memory error"),
                                   _CPTC_("memory allocation failed"),
                                   _CPTC_("DataAdapter::tg_to_lv_attribute_config"));
  }
  h = lai.max_alarm;
  (*h)->length = l;
  ::memcpy((*h)->data, tac.max_alarm, l);
}

//=============================================================================
// DataAdapter::tg_to_lv_device_property
//=============================================================================
void DataAdapter::tg_to_lv_device_property (const PropertyInfo& pi,
                                            std::vector<std::string>& tg_pv,
                                            LvDevicePropertyPtr lv_pv)
  throw (Tango::DevFailed)
{
  if ( tg_pv.empty() )
  {
    std::ostringstream oss;
    oss << "device property '" << pi.name << "' is undefined for device " << pi.dev;
    Tango::Except::throw_exception(_CPTC_("DEVICE_ERROR"),
                                   oss.str().c_str(),
                                   _CPTC_("DataAdapter::tg_to_lv_device_property"));
  }

  to_lv_str(pi.name, lv_pv->name);
  
  try
  {
    switch ( pi.data_type )
    {
      //-- DEVVAR_STRINGARRAY --------------------------
      case Tango::DEV_STRING:
      case Tango::CONST_DEV_STRING:
      case Tango::DEVVAR_STRINGARRAY:
        {
          //-------------------------------------------------------------------------------------------
          //- big trick here!
          //-------------------------------------------------------------------------------------------
          //- data is generically passed to the Labview env. through <LvAttributeValue.data> - which
          //- is supposed to be an array of bytes reinterpreted by the extractor VIs. means that we
          //- can't pass the result as a 1D array of strings. we choose to pass it as an array of bytes
          //- in which the strings (i.e. the ones contained into the Tango::DevVarStringArray) are
          //- stored contiguously and separated by a '\0' character. the dedicated extractor VI will
          //- have to rebuild the expected 1D (or 2D) array of srings (transparent op. for end user).
          //-------------------------------------------------------------------------------------------
          //- compute sizeof "generic u8" dest buffer: make room for the strings
          tbfl::size_t total_size = 0;
          for ( tbfl::size_t i = 0; i < tg_pv.size(); i++ )
          {
            total_size += tg_pv[i].size();
          }
          //- compute sizeof "generic u8" dest buffer: make room for string separators
          total_size += tg_pv.size() - 1;
          //- resize dest buffer
          MgErr err = ::NumericArrayResize(uB,
                                           1L,
                                           reinterpret_cast<UHandle*>(&lv_pv->value),
                                           total_size);
          if (err != noErr)
          {
            Tango::Except::throw_exception(_CPTC_("memory error"),
                                           _CPTC_("memory allocation failed"),
                                           _CPTC_("DataAdapter::tg_to_lv_device_property"));
          }
          //- reinterpret dest as <LvGenericNumArrayHdl>
          LvGenericNumArrayHdl h = reinterpret_cast<LvGenericNumArrayHdl>(lv_pv->value);
          (*h)->length = total_size;
          //- reset memory (will put '\0' between strings at the same time)
          char * p = reinterpret_cast<char*>((*h)->data);
          ::memset(p, 0, total_size);
          //- copy strings to buffer
          for ( tbfl::size_t i = 0; i < tg_pv.size(); i++ )
          {
            ::memcpy(p, tg_pv[i].c_str(), tg_pv[i].size());
            p += tg_pv[i].size() + 1;
          }
          lv_pv->type = (pi.data_type == Tango::DEVVAR_STRINGARRAY) ? SPE_STRING : SCA_STRING;
        }
        break;
      //-- DEVVAR_DOUBLEARRAY --------------------------
      case Tango::DEVVAR_DOUBLEARRAY:
        {
          DECODE_DEV_PROPERTY(double, tg_pv, lv_pv->value);
          lv_pv->type = SPE_DOUBLE;
        }
        break;
      //-- DEVVAR_FLOATARRAY ---------------------------
      case Tango::DEVVAR_FLOATARRAY:
        {
          DECODE_DEV_PROPERTY(float, tg_pv, lv_pv->value);
          lv_pv->type = SPE_FLOAT;
        }
        break;
      //-- DEVVAR_LONGARRAY ---------------------------
      case Tango::DEVVAR_LONGARRAY:
        {
          DECODE_DEV_PROPERTY(tbfl::int32, tg_pv, lv_pv->value);
          lv_pv->type = SPE_LONG;
        }
        break;
      //-- DEVVAR_ULONGARRAY ---------------------------
      case Tango::DEVVAR_ULONGARRAY:
        {
          DECODE_DEV_PROPERTY(tbfl::uint32, tg_pv, lv_pv->value);
          lv_pv->type = SPE_ULONG;
        }
        break;
      //-- DEVVAR_SHORTARRAY ---------------------------
      case Tango::DEVVAR_SHORTARRAY:
        {
          DECODE_DEV_PROPERTY(tbfl::int16, tg_pv, lv_pv->value);
          lv_pv->type = SPE_SHORT;
        }
        break;
      //-- DEVVAR_USHORTARRAY --------------------------
      case Tango::DEVVAR_USHORTARRAY:
        {
          DECODE_DEV_PROPERTY(tbfl::uint16, tg_pv, lv_pv->value);
          lv_pv->type = SPE_USHORT;
        }
        break;
      //-- DEV_BOOLEAN ---------------------------------
      case Tango::DEV_BOOLEAN:
        {
          std::vector<std::string> tg_pv_cpy = tg_pv;
          std::transform(tg_pv_cpy[0].begin(),tg_pv_cpy[0].end(), tg_pv_cpy[0].begin(), ::tolower);
          if ( tg_pv_cpy[0] == "false" || tg_pv_cpy[0] == "no" || tg_pv_cpy[0] == "0" )
          {
            tg_pv_cpy[0] = "0";
          }
          else if ( tg_pv_cpy[0] == "true" || tg_pv_cpy[0] == "yes" || tg_pv_cpy[0] == "1" )
          {
            tg_pv_cpy[0] = "1";
          }
          else
          { 
            std::ostringstream oss;
            oss << "device "
                << pi.dev
                << " has an invalid boolean property named "
                << pi.name
                << " [expected 'true' or 'false', got '"
                << tg_pv_cpy[0]
                << "']";
            Tango::Except::throw_exception(_CPTC_("DEVICE_ERROR"),
                                          oss.str().c_str(),
                                          _CPTC_("DataAdapter::tg_to_lv_device_property"));
          }
          DECODE_DEV_PROPERTY(bool, tg_pv_cpy, lv_pv->value);
          lv_pv->type = SCA_BOOLEAN;
        }
        break;
      //-- DEV_SHORT -----------------------------------
      case Tango::DEV_SHORT:
        {
          DECODE_DEV_PROPERTY(tbfl::int16, tg_pv, lv_pv->value);
          lv_pv->type = SCA_SHORT;
        }
        break;
      //-- DEV_SHORT -----------------------------------
      case Tango::DEV_USHORT:
        {
          DECODE_DEV_PROPERTY(tbfl::uint16, tg_pv, lv_pv->value);
          lv_pv->type = SCA_USHORT;
        }
        break;
      //-- DEV_LONG ------------------------------------
      case Tango::DEV_LONG:
        {
          DECODE_DEV_PROPERTY(tbfl::int32, tg_pv, lv_pv->value);
          lv_pv->type = SCA_LONG;
        }
        break;
      //-- DEV_ULONG -----------------------------------
      case Tango::DEV_ULONG:
        {
          DECODE_DEV_PROPERTY(tbfl::uint32, tg_pv, lv_pv->value);
          lv_pv->type = SCA_ULONG;
        }
        break;
      //-- DEV_FLOAT -----------------------------------
      case Tango::DEV_FLOAT:
        {
          DECODE_DEV_PROPERTY(float, tg_pv, lv_pv->value);
          lv_pv->type = SCA_FLOAT;
        }
        break;
      //-- DEV_DOUBLE ----------------------------------
      case Tango::DEV_DOUBLE:
        {
          DECODE_DEV_PROPERTY(double, tg_pv, lv_pv->value);
          lv_pv->type = SCA_DOUBLE;
        }
        break;
      //-- DEFAULT -------------------------------------
      default:
        Tango::Except::throw_exception(_CPTC_("unknown or unsupported TANGO data type"),
                                       _CPTC_("unsupported data type for device property"),
                                       _CPTC_("DataAdapter::tg_to_lv_device_property"));
        break;
    } // switch
  } //try
  catch (const Tango::DevFailed&)
  {
    throw;
  }
  catch (...)
  {
    Tango::Except::throw_exception(_CPTC_("unknown error"),
                                   _CPTC_("unknown exception caught"),
                                   _CPTC_("DataAdapter::tg_to_lv_device_property"));
  }
}
