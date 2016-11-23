// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   Endianness.i - CODE STOLEN FROM THE FABULOUS ACE FRAMEWORK
//
// = ORIGINAL AUTHORS
//   Aniruddha Gokhale <gokhale@cs.wustl.edu>
//   Carlos O'Ryan<coryan@cs.wustl.edu>
//   Jeff Parsons <parsons@cs.wustl.edu>
//   Istvan Buki	<istvan.buki@euronet.be>
//   many others...
//
// = HACKER
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#if defined (_TBFL_HAS_INTRINSIC_BYTESWAP_)
//- take advantage of MSVC++ byte swapping compiler intrinsics (found in <stdlib.h>)
# pragma intrinsic (_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#elif defined (_TBFL_HAS_BSWAP_16_) || defined (_TBFL_HAS_BSWAP_32_) || defined (_TBFL_HAS_BSWAP_64_)
//- take advantage of local swapping implementation
# include <byteswap.h>
#endif

//- the number of bytes in a long
#include <limits.h>
#if !defined (_TBFL_SIZEOF_LONG_)
# if (ULONG_MAX) == 65535UL
#   define _TBFL_SIZEOF_LONG_ 2
# elif ((ULONG_MAX) == 4294967295UL)
#   define _TBFL_SIZEOF_LONG_ 4
# elif ((ULONG_MAX) == 18446744073709551615UL)
#   define _TBFL_SIZEOF_LONG_ 8
# else
#   error: unsupported long size, must be updated for this platform!
# endif
#endif

//=============================================================================
// Endianness::swap_2
//=============================================================================
LV_INLINE void
Endianness::swap_2 (const char *orig, char* target)
{
#if defined (_TBFL_HAS_INTRINSIC_BYTESWAP_)
  //- take advantage of MSVC++ compiler intrinsic byte swapping function
  *reinterpret_cast<unsigned short *> (target) =
    _byteswap_ushort (*reinterpret_cast<unsigned short const *> (orig));
#elif defined (_TBFL_HAS_BSWAP16_)
  *reinterpret_cast<uint16_t *> (target) =
    bswap16 (*reinterpret_cast<uint16_t const *> (orig));
#elif defined(_TBFL_HAS_INTEL_ASSEMBLY_)
  unsigned short a =
    *reinterpret_cast<const unsigned short*> (orig);
  asm( "rolw $8, %0" : "=r" (a) : "0" (a) );
  *reinterpret_cast<unsigned short*> (target) = a;
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined(_MSC_VER)
  __asm mov ebx, orig;
  __asm mov ecx, target;
  __asm mov ax, [ebx];
  __asm rol ax, 8;
  __asm mov [ecx], ax;
#else
  register tbfl::uint16 usrc = * reinterpret_cast<const tbfl::uint16*> (orig);
  register tbfl::uint16* udst = reinterpret_cast<tbfl::uint16*> (target);
  *udst = (usrc << 8) | (usrc >> 8);
#endif
}

//=============================================================================
// Endianness::swap_4
//=============================================================================
LV_INLINE void
Endianness::swap_4 (const char* orig, char* target)
{
#if defined (_TBFL_HAS_INTRINSIC_BYTESWAP_)
  //- take advantage of MSVC++ compiler intrinsic byte swapping function
  *reinterpret_cast<unsigned __int32 *> (target) =
    _byteswap_ulong (*reinterpret_cast<unsigned __int32 const *> (orig));
#elif defined (_TBFL_HAS_BSWAP32_)
  *reinterpret_cast<uint32_t *> (target) =
    bswap32 (*reinterpret_cast<uint32_t const *> (orig));
#elif defined(_TBFL_HAS_INTEL_ASSEMBLY_)
  //- we have _TBFL_HAS_PENTIUM_, so we know the sizeof's.
  register unsigned int j =
    *reinterpret_cast<const unsigned int*> (orig);
  asm ("bswap %1" : "=r" (j) : "0" (j));
  *reinterpret_cast<unsigned int*> (target) = j;
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined(_MSC_VER)
  __asm mov ebx, orig;
  __asm mov ecx, target;
  __asm mov eax, [ebx];
  __asm bswap eax;
  __asm mov [ecx], eax;
#else
  register tbfl::uint32 x = * reinterpret_cast<const tbfl::uint32*> (orig);
  x = (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24);
  * reinterpret_cast<tbfl::uint32*> (target) = x;
#endif
}

//=============================================================================
// Endianness::swap_8
//=============================================================================
LV_INLINE void
Endianness::swap_8 (const char* orig, char* target)
{
#if defined (_TBFL_HAS_INTRINSIC_BYTESWAP_)
  //- take advantage of MSVC++ compiler intrinsic byte swapping function
  *reinterpret_cast<unsigned __int64 *> (target) =
    _byteswap_uint64 (*reinterpret_cast<unsigned __int64 const *> (orig));
#elif defined (_TBFL_HAS_BSWAP64_)
  *reinterpret_cast<uint64_t *> (target) =
    bswap64 (*reinterpret_cast<uint64_t const *> (orig));
#elif (defined (__amd64__) || defined (__x86_64__)) && defined(__GNUG__)
  register unsigned long x =
    * reinterpret_cast<const unsigned long*> (orig);
  asm ("bswapq %1" : "=r" (x) : "0" (x));
  *reinterpret_cast<unsigned long*> (target) = x;
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined(__GNUG__)
  register unsigned int i =
    *reinterpret_cast<const unsigned int*> (orig);
  register unsigned int j =
    *reinterpret_cast<const unsigned int*> (orig + 4);
  asm ("bswap %1" : "=r" (i) : "0" (i));
  asm ("bswap %1" : "=r" (j) : "0" (j));
  *reinterpret_cast<unsigned int*> (target + 4) = i;
  *reinterpret_cast<unsigned int*> (target) = j;
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined(_MSC_VER)
  __asm mov ecx, orig;
  __asm mov edx, target;
  __asm mov eax, [ecx];
  __asm mov ebx, 4[ecx];
  __asm bswap eax;
  __asm bswap ebx;
  __asm mov 4[edx], eax;
  __asm mov [edx], ebx;
#elif _TBFL_SIZEOF_LONG_ == 8
  //- 64 bit architecture.
  register unsigned long x =
    * reinterpret_cast<const unsigned long*> (orig);
  register unsigned long x84 = (x & 0x000000ff000000ffUL) << 24;
  register unsigned long x73 = (x & 0x0000ff000000ff00UL) << 8;
  register unsigned long x62 = (x & 0x00ff000000ff0000UL) >> 8;
  register unsigned long x51 = (x & 0xff000000ff000000UL) >> 24;
  x = (x84 | x73 | x62 | x51);
  x = (x << 32) | (x >> 32);
  *reinterpret_cast<unsigned long*> (target) = x;
#else
  register tbfl::uint32 x =
    * reinterpret_cast<const tbfl::uint32*> (orig);
  register tbfl::uint32 y =
    * reinterpret_cast<const tbfl::uint32*> (orig + 4);
  x = (x << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24);
  y = (y << 24) | ((y & 0xff00) << 8) | ((y & 0xff0000) >> 8) | (y >> 24);
  * reinterpret_cast<tbfl::uint32*> (target) = y;
  * reinterpret_cast<tbfl::uint32*> (target + 4) = x;
#endif
}

//=============================================================================
// Endianness::swap_16
//=============================================================================
LV_INLINE void
Endianness::swap_16 (const char* orig, char* target)
{
  Endianness::swap_8(orig + 8, target);
  Endianness::swap_8(orig, target + 8);
}
