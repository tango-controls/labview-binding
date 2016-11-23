//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 The Tango Community
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2011 The Tango Community
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

#ifndef _YAT4TANGO_TYPES_H_
#define _YAT4TANGO_TYPES_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat4tango/CommonHeader.h>

namespace yat4tango
{

// ============================================================================
//! \struct TangoTraits 
//! \brief Template structure for generic types treatments.
//!
// ============================================================================
template <typename T>
struct TangoTraits
{
};

// ============================================================================
//! TangoTraits specialization MACRO [deals with generic types].
// ============================================================================
# define TANGO_TRAITS(TangoDataType, TangoDataTypeID, DevAttrMemberType, DevAttrMemberName) \
  template<> struct TangoTraits<TangoDataType> \
  { \
    static const int type_id = TangoDataTypeID; \
    \
    typedef DevAttrMemberType DevVarArray_var; \
    static DevVarArray_var& corba_buffer (Tango::DeviceAttribute& dev_attr) \
    { \
      return dev_attr.DevAttrMemberName; \
    } \
  }

// ============================================================================
//! \name TangoTraits specializations [deal with generic types]
// ============================================================================
TANGO_TRAITS(Tango::DevBoolean, Tango::DEV_BOOLEAN, Tango::DevVarBooleanArray_var, BooleanSeq);
TANGO_TRAITS(Tango::DevUChar,   Tango::DEV_UCHAR,   Tango::DevVarCharArray_var,    UCharSeq);
TANGO_TRAITS(Tango::DevUShort,  Tango::DEV_USHORT,  Tango::DevVarUShortArray_var,  UShortSeq);
TANGO_TRAITS(Tango::DevShort,   Tango::DEV_SHORT,   Tango::DevVarShortArray_var,   ShortSeq);
TANGO_TRAITS(Tango::DevLong,    Tango::DEV_LONG,    Tango::DevVarLongArray_var,    LongSeq);
TANGO_TRAITS(Tango::DevFloat,   Tango::DEV_FLOAT,   Tango::DevVarFloatArray_var,   FloatSeq);
TANGO_TRAITS(Tango::DevDouble,  Tango::DEV_DOUBLE,  Tango::DevVarDoubleArray_var,  DoubleSeq);
TANGO_TRAITS(Tango::DevString,  Tango::DEV_STRING,  Tango::DevVarStringArray_var,  StringSeq);
#if (TANGO_VERSION_MAJOR >= 9)
TANGO_TRAITS(Tango::DevULong,   Tango::DEV_ULONG,   Tango::DevVarULongArray_var,   ULongSeq);
TANGO_TRAITS(Tango::DevULong64, Tango::DEV_ULONG64, Tango::DevVarULong64Array_var, ULong64Seq);
TANGO_TRAITS(Tango::DevLong64,  Tango::DEV_LONG64,  Tango::DevVarLong64Array_var,  Long64Seq);
TANGO_TRAITS(Tango::DevState,   Tango::DEV_STATE,   Tango::DevVarStateArray_var,   StateSeq);
TANGO_TRAITS(Tango::DevEncoded, Tango::DEV_ENCODED, Tango::DevVarEncodedArray_var, EncodedSeq);
#elif (TANGO_VERSION_MAJOR == 8)
TANGO_TRAITS(Tango::DevULong,   Tango::DEV_ULONG,   Tango::DevVarULongArray_var,   get_ULong_data());
TANGO_TRAITS(Tango::DevULong64, Tango::DEV_ULONG64, Tango::DevVarULong64Array_var, get_ULong64_data());
TANGO_TRAITS(Tango::DevLong64,  Tango::DEV_LONG64,  Tango::DevVarLong64Array_var,  get_Long64_data());
TANGO_TRAITS(Tango::DevState,   Tango::DEV_STATE,   Tango::DevVarStateArray_var,   get_State_data());
TANGO_TRAITS(Tango::DevEncoded, Tango::DEV_ENCODED, Tango::DevVarEncodedArray_var, get_Encoded_data());
#else
TANGO_TRAITS(Tango::DevULong,   Tango::DEV_ULONG,   Tango::DevVarULongArray_var,   ext->ULongSeq);
TANGO_TRAITS(Tango::DevULong64, Tango::DEV_ULONG64, Tango::DevVarULong64Array_var, ext->ULong64Seq);
TANGO_TRAITS(Tango::DevLong64,  Tango::DEV_LONG64,  Tango::DevVarLong64Array_var,  ext->Long64Seq);
TANGO_TRAITS(Tango::DevState,   Tango::DEV_STATE,   Tango::DevVarStateArray_var,   ext->StateSeq);
TANGO_TRAITS(Tango::DevEncoded, Tango::DEV_ENCODED, Tango::DevVarEncodedArray_var, ext->EncodedSeq);
#endif
/*
#define TANGO_CTYPE_TRAITS(TangoDataType, CType) \
template<> struct TangoCTypeTraits<TangoDataType> \
{ \
  static const int type_id = TangoDataType; \
  typedef CType ctype; \
}

TANGO_CTYPE_TRAITS(Tango::DevBoolean, bool);
TANGO_CTYPE_TRAITS(Tango::DevUChar, unsigned char);
TANGO_CTYPE_TRAITS(Tango::DevUShort, unsigned short);
TANGO_CTYPE_TRAITS(Tango::DevULong, unsigned long);
TANGO_CTYPE_TRAITS(Tango::DevULong64, yat::uint64);
TANGO_CTYPE_TRAITS(Tango::DevChar, char);
TANGO_CTYPE_TRAITS(Tango::DevShort, short);
TANGO_CTYPE_TRAITS(Tango::DevLong, long);
TANGO_CTYPE_TRAITS(Tango::DevLong64, yat::int64);
TANGO_CTYPE_TRAITS(Tango::DevFloat, float);
TANGO_CTYPE_TRAITS(Tango::DevDouble, double);
*/
} //- namespace

#endif //- _YAT4TANGO_TYPES_H_ 
