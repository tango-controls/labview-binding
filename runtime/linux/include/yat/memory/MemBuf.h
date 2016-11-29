//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2014 The Tango Community
//
// Part of the code comes from the ACE Framework (asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
// The YAT library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU General Public License as published by the Free 
// Software Foundation; either version 2 of the License, or (at your option) 
// any later version.
//
// The YAT library is distributed in the hope that it will be useful,
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
 * \author See AUTHORS file
 */

#ifndef __YAT_MEMBUF_H__
#define __YAT_MEMBUF_H__

#include <yat/CommonHeader.h>

namespace yat
{

//! \brief 32 bits CRC calculation.
//!
//! This function offers two usages :
//! - pulInitValue set to NULL value : CRC is calculated for the input buffer,
//! - pulInitValue NOT set to NULL : CRC is calculated for a set of buffers. 
//! For example, with p1 & p2 two buffer pointers :
//! \n
//!    uint32 ulCrc = 0xFFFFFFFFL;\n
//!    ulCrc = crc( p1, uiP1Len, &ulCrc );\n
//!    ulCrc = crc( p2, uiP2Len, &ulCrc );\n
//!    ulCrc = ulCrc ^ 0xFFFFFFFFL; \n
//! \param pBuf Input buffer pointer.
//! \param uiLen Size of inputr buffer, in bytes.
//! \param pulInitValue Initial crc value.
uint32 crc( const byte *pBuf, uint32 uiLen, uint32 *pulInitValue = NULL );

//===========================================================================
// endian management
//===========================================================================

//! \brief Short value inversion : AB --\> BA
//! \param pS Short value to invert (in/out).
inline void invert_short(short *pS)
{
  // AB -> BA
  char *p = (char*)pS;
  char c = *p;
  *p = *(p+1);
  *(p+1) = c;
}

//! \brief Long value inversion : ABCD --\> DCBA
//! \param pL Long value to invert (in/out).
inline void invert_long(long *pL)
{
  // ABCD -> DCBA
  char *p = (char*)pL;
  char c = *p;
  *p = *(p+3);
  *(p+3) = c;
  c = *(p+1);
  *(p+1) = *(p+2);
  *(p+2) = c;
}

//! \brief Float value inversion : ABCD --\> DCBA
//! \param pF Float value to invert (in/out).
inline void invert_float(float *pF)
{
  // ABCD -> DCBA
  invert_long((long*)pF);
}

//! \brief Double value inversion : ABCDEFGH --\> HGFEDCBA
//! \param pD Double value to invert (in/out).
inline void invert_double(double *pD)
{
  // ABCDEFGH -> HGFEDCBA
  char *p = (char*)pD;
  char c = *p;
  *p = *(p+7);
  *(p+7) = c;

  c = *(p+1);
  *(p+1) = *(p+6);
  *(p+6) = c;

  c = *(p+2);
  *(p+2) = *(p+5);
  *(p+5) = c;

  c = *(p+3);
  *(p+3) = *(p+4);
  *(p+4) = c;
}

//! \brief Integer64 value inversion : ABCDEFGH --\> HGFEDCBA
//! \param pi64 Integer64 value to invert (in/out).
inline void invert_int64(int64 *pi64)
{
  // ABCDEFGH -> HGFEDCBA
  invert_double((double*)pi64);
}

//===================================================================
// Constantes
//===================================================================

// Latest crypying version
#define REVCRYPT_LATEST_VERSION -1
#define VARLENGTH_CRYPT          2

#define OFFSET(type,field) \
  (int)&(((type*)NULL)->field)

//===========================================================================
//! \class MemBuf 
//! \brief Auto-sized binary buffer with cryptographic capabilities.
//!
//! This class provides a managed buffer with :
//! - auto memory re-allocation in case used binary blocs length exceeds the current 
//! buffer capacity,
//! - insertion functions (including streaming functions) to fill the buffer,
//! - extraction functions (including streaming functions) to read the buffer,
//! - CRC calculation function.
//===========================================================================
class YAT_DECL MemBuf
{
public:
  //! \brief Constructor.
  //!
  //! \param uiLenBuf Buffer capacity in bytes.
  MemBuf(uint32 uiLenBuf=0) ;

  //! \brief Copy Constructor.
  //!
  //! \param buf The source buffer.
  MemBuf(const MemBuf& buf) ;

  //! \brief Destructor
  //!
  //! Releases memory only if buffer is owned by this instance.
  ~MemBuf() ;

  //! \brief Operator=.
  //!
  //! \param buf The source buffer.
  MemBuf& operator=(const MemBuf& buf);

  //! \brief Comparison operator.
  //!
  //! Returns true if buffers are equal, false otherwise.
  //! \param mb The source buffer.
  bool operator==(const MemBuf &mb) const;
  
  //! \brief Attachment from an external buffer.
  //!
  //! \param pBuf Pointer to memory area to attach.
  //! \param uiLen Memory area size.
  //! \param bOwner If set to true, this instance will own the buffer.
  void attach(char* pBuf, uint32 uiLen, bool bOwner = false);

  //! \brief Returns the size of used binary blocks in the buffer, in bytes.
  uint32 len() const  { return m_uiLen; }

  //! \brief Returns true if the buffer has no used binary block.
  int is_empty() const { return m_uiLen==0; }

  //! \brief Returns the buffer capacity in bytes (size of allocated storage space).
  uint32 buf_len() const { return m_uiLenBuf; }

  //! \brief Returns the buffer pointer.
  char* buf() const   { return m_pBuf; }

  //! \brief Returns the buffer pointer in *bytes* data type.
  byte *bytes() const { return (byte *)m_pBuf; }
   
  //! \brief Returns the current read position of the buffer.
  uint32 pos() const    { return m_uiPos; }

  //! \brief Sets new buffer capacity in bytes.
  //!
  //! \param ui New buffer capacity in bytes.
  //! \remark If the specified capacity is greater than current capacity, memory is 
  //! reallocated for the buffer.
  void set_len(uint32 ui);

  //! \brief Reallocates the specified memory size for the buffer.
  //!
  //! \param uiNewLenBuf New buffer capacity in bytes.
  //! \remark If the specified capacity is smaller than the size of
  //! used binary blocks in the buffer, this function makes nothing.
  void realloc(uint32 uiNewLenBuf);

  //! \brief Adds a binary block in the buffer.
  //!
  //! Inserts a binary block of data at the end of the buffer.
  //! \param p Pointer to the binary block to add.
  //! \param uiNb Size of the binary block in bytes.
  //! \remark If the block size is greater than the remaining capacity of the buffer, 
  //! memory is reallocated and the buffer capacity increased.
  void put_bloc(const void* p, uint32 uiNb);

  //! \brief Gets a binary block from the buffer.
  //!
  //! Reads the specified number of bytes from the current read position.\n
  //! If the number of bytes to read is greater than the size of the
  //! remaining binary blocks in buffer, returns (1) and does not fill \<p\>.
  //! Returns (0) otherwise.
  //! \param p Pointer to the binary block read from the buffer.
  //! \param uiNb Number of bytes to read from the buffer.
  //! \remark \<p\> must have been allocated before using this function.
  int get_bloc(void* p, uint32 uiNb);

  //! \brief Inserts a binary block in the buffer at given offset.
  //! 
  //! Inserts a binary block of data at the specified position. The current data
  //! at the specified position are moved after inserted data.
  //! \param p Pointer to the binary block to insert.
  //! \param uiNb Size of the binary block in bytes.
  //! \param uiPos Position in bytes.
  //! \remark If the block size is greater than the remaining capacity of the buffer, 
  //! memory is reallocated and the buffer capacity increased.
  void insert_bloc(const void* p, uint32 uiNb, uint32 uiPos);

  //! \brief Moves a part of the buffer.
  //!
  //! Copies a block of data from one position of the buffer to another.
  //! \param uiDst Destination position in the buffer, in bytes.
  //! \param uiSrc Source position in the buffer, in bytes.
  //! \param uiSize Block size, in bytes.
  void move_bloc(uint32 uiDst, uint32 uiSrc, uint32 uiSize);

  //! \brief Sets current read position to the beginning of the buffer.
  void rewind()  { m_uiPos = 0; }

  //! \brief Sets the current read position to specified offset.
  //!
  //! \param ui Read position in bytes.
  void set_pos(uint32 ui)
    {
//##      ASSERT(ui <= m_uiLen)
      m_uiPos = ui;
    }

  //! \brief Resets buffer without freeing memory.
  void empty()
  {
    m_uiPos = 0 ;
    m_uiLen = 0 ;
  }

  //! \brief Gives buffer ownership to *this* instance.
  //!
  //! \param bOwner If set to true, gives ownership to *this* instance.
  void set_owner(bool bOwner) { m_bOwner = bOwner; }

  //! \brief Gives buffer ownership to another instance.
  //!
  //! The ownership transfer is possible only if *this* instance owns the buffer.
  //! Returns (-1) if *this* instance does not own the buffer, (0) otherwise.
  //! \param pToHaveOwnership Pointer to the instance which gains the buffer ownership.
  int give_ownership(MemBuf* pToHaveOwnership);

  //! \brief Resets buffer with memory freeing.
  //!
  //! \remark Memory is deallocated if *this* instance owns the buffer.
  void reset();

  //! \brief Computes CRC on used binary blocks of the buffer.
  //!
  //! Returns a 32 bits CRC value.
  uint32 get_crc() const;

  //! \brief Gets pointer to current position.
  char *cur_pointer() const  { return (m_pBuf + m_uiPos); }

  //! \brief Input stream function for a boolean value.
  //! \param b Boolean value to add in buffer.
  MemBuf& operator<<(bool b);

  //! \brief Output stream function for a boolean value.
  //! \param b Boolean value read from buffer.
  MemBuf& operator>>(bool &b);

  //! \brief Input stream function for a character value.
  //! \param c Character value to add in buffer.
  MemBuf& operator<<(char c);
  
  //! \brief Output stream function for a character value.
  //! \param c Character value read from buffer.
  MemBuf& operator>>(char &c);

  //! \brief Input stream function for a byte value.
  //! \param uc Byte value to add in buffer.
  MemBuf& operator<<(byte uc);

  //! \brief Output stream function for a byte value.
  //! \param uc Byte value read from buffer.
  MemBuf& operator>>(byte &uc);

  //! \brief Input stream function for an integer16 value.
  //! \param s Integer16 value to add in buffer.
  MemBuf& operator<<(int16 s);

  //! \brief Output stream function for an integer16 value.
  //! \param s Integer16 value read from buffer.
  MemBuf& operator>>(int16 &s);

  //! \brief Input stream function for an unsigned integer16 value.
  //! \param us Unsigned integer16 value to add in buffer.
  MemBuf& operator<<(uint16 us);

  //! \brief Output stream function for an unsigned integer16 value.
  //! \param us Unsigned integer16 value read from buffer.
  MemBuf& operator>>(uint16 &us);

  //! \brief Input stream function for an integer32 value.
  //! \param l Integer32 value to add in buffer.
  MemBuf& operator<<(int32 l);

  //! \brief Output stream function for an integer32 value.
  //! \param l Integer32 value read from buffer.
  MemBuf& operator>>(int32 &l);

  //! \brief Input stream function for an unsigned integer32 value.
  //! \param ul Unsigned integer32 value to add in buffer.
  MemBuf& operator<<(uint32 ul);

  //! \brief Output stream function for an unsigned integer32 value.
  //! \param ul Unsigned integer32 value read from buffer.
  MemBuf& operator>>(uint32 &ul);

  //! \brief Input stream function for an integer64 value.
  //! \param i64 Integer64 value to add in buffer.
  MemBuf& operator<<(int64 i64);

  //! \brief Output stream function for an integer64 value.
  //! \param i64 Integer64 value read from buffer.
  MemBuf& operator>>(int64 &i64);

  //! \brief Input stream function for an unsigned integer64 value.
  //! \param i64 Unsigned integer64 value to add in buffer.
  MemBuf& operator<<(uint64 i64);

  //! \brief Output stream function for an unsigned integer64 value.
  //! \param i64 Unsigned integer64 value read from buffer.
  MemBuf& operator>>(uint64 &i64);

  //! \brief Input stream function for a float value.
  //! \param f Float value to add in buffer.
  MemBuf& operator<<(float f);

  //! \brief Output stream function for a float value.
  //! \param f Float value read from buffer.
  MemBuf& operator>>(float &f);

  //! \brief Input stream function for a double value.
  //! \param d Double value to add in buffer.
  MemBuf& operator<<(double d);

  //! \brief Output stream function for a double value.
  //! \param d Double value read from buffer.
  MemBuf& operator>>(double &d);

  //! \brief Input stream function for a binary buffer.
  //! \param psz Pointer to binary buffer to add in buffer.
  MemBuf& operator<<(const char* psz);

  //! \brief Input stream function for a string value.
  //! \param string String value to add in buffer.
  MemBuf& operator<<(const std::string& string);

  //! \brief Output stream function for a string value.
  //! \param string String value read from buffer.
  MemBuf& operator>>(std::string& string);

  //! \brief Input stream function for a MemBuf buffer.
  //! \param membuf MemBuf buffer to add in buffer.
  MemBuf& operator<<(const MemBuf& membuf);

private:
  //- Read position
  uint32  m_uiPos; 

  //- Size of used binary blocks in bytes.
  uint32  m_uiLen;         

  //- Buffer capacity (allocated size) in bytes.
  uint32  m_uiLenBuf;  

  //- Buffer pointer.
  char* m_pBuf;      

  //- If true the instance owns the buffer.
  bool  m_bOwner;            

  //- Re-allocation function.
  void realloc_with_margin(uint32 uiNewSize) ;

};

} // namespace

#endif // __MEMBUF_H___
