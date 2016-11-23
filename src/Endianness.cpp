// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   Endianness.cpp - CODE STOLEN FROM THE ACE FRAMEWORK 
//
// = ORIGINAL AUTHORS
//   Aniruddha Gokhale <gokhale@cs.wustl.edu>
//   Carlos O'Ryan<coryan@cs.wustl.edu>
//   Jeff Parsons <parsons@cs.wustl.edu>
//   Istvan Buki	<istvan.buki@euronet.be>
//
// = HACKER
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

//=============================================================================
// IMPL OPTION
//=============================================================================
#if ! defined(DISABLE_POLYMORPHIC_IMPL)

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include "CommonHeader.h"
#include "Endianness.h"

#if !defined (_LV_INLINE_)
  #include "Endianness.i"
#endif 

//=============================================================================
// ALIGN_BINARY
//=============================================================================
inline uintptr_t ALIGN_BINARY (uintptr_t ptr, uintptr_t alignment)
{
  uintptr_t const tmp = alignment - 1;
  return (ptr + tmp) & (~tmp);
}

//=============================================================================
// PTR_ALIGN_BINARY
//=============================================================================
inline char * PTR_ALIGN_BINARY (char const * ptr, uintptr_t alignment)
{
  return reinterpret_cast<char *>(ALIGN_BINARY(reinterpret_cast<uintptr_t>(ptr), alignment));
}

//=============================================================================
// PTR_ALIGN_BINARY
//=============================================================================
inline char * PTR_ALIGN_BINARY (unsigned char const * ptr, uintptr_t alignment)
{
  return PTR_ALIGN_BINARY(reinterpret_cast<char const *>(ptr), alignment);
}

//=============================================================================
// Endianness::swap_2_array
//=============================================================================
void Endianness::swap_2_array (const char* orig, char* target, size_t n)
{
  if ( ! n )
    return;

  // We pretend that AMD64/GNU G++ systems have a Pentium CPU to
  // take advantage of the inline assembly implementation.

  // Later, we try to read in 32 or 64 bit chunks,
  // so make sure we don't do that for unaligned addresses.
#if _TBFL_SIZEOF_LONG_ == 8 && !((defined(__amd64__) || defined (__x86_64__)) && defined(__GNUG__))
  char const * const o8 = PTR_ALIGN_BINARY (orig, 8);
  while (orig < o8 && n > 0)
  {
    Endianness::swap_2(orig, target);
    orig += 2;
    target += 2;
    --n;
  }
#else
  char const * const o4 = PTR_ALIGN_BINARY(orig, 4);
  // this is an _if_, not a _while_. The mistmatch can only be by 2.
  if (orig != o4)
  {
    Endianness::swap_2(orig, target);
    orig += 2;
    target += 2;
    --n;
  }
#endif
  if ( ! n )
    return;

  //
  // Loop unrolling. Here be dragons.
  //

  // (n & (~3)) is the greatest multiple of 4 not bigger than n.
  // In the while loop ahead, orig will move over the array by 8 byte
  // increments (4 elements of 2 bytes).
  // end marks our barrier for not falling outside.
  char const * const end = orig + 2 * (n & (~3));

  // See if we're aligned for writting in 64 or 32 bit chunks...
#if _TBFL_SIZEOF_LONG_ == 8 && !((defined(__amd64__) || defined (__x86_64__)) && defined(__GNUG__))
  if (target == PTR_ALIGN_BINARY(target, 8))
#else
  if (target == PTR_ALIGN_BINARY(target, 4))
#endif
  {
    while (orig < end)
    {
#if defined (_TBFL_HAS_INTEL_ASSEMBLY_)
      unsigned int a =
        * reinterpret_cast<const unsigned int*> (orig);
      unsigned int b =
        * reinterpret_cast<const unsigned int*> (orig + 4);
      asm ( "bswap %1"      : "=r" (a) : "0" (a) );
      asm ( "bswap %1"      : "=r" (b) : "0" (b) );
      asm ( "rol $16, %1"   : "=r" (a) : "0" (a) );
      asm ( "rol $16, %1"   : "=r" (b) : "0" (b) );
      * reinterpret_cast<unsigned int*> (target) = a;
      * reinterpret_cast<unsigned int*> (target + 4) = b;
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined(_MSC_VER)
      __asm mov ecx, orig;
      __asm mov edx, target;
      __asm mov eax, [ecx];
      __asm mov ebx, 4[ecx];
      __asm bswap eax;
      __asm bswap ebx;
      __asm rol eax, 16;
      __asm rol ebx, 16;
      __asm mov [edx], eax;
      __asm mov 4[edx], ebx;
#elif _TBFL_SIZEOF_LONG_ == 8
      // 64 bit architecture.
      register unsigned long a =
        * reinterpret_cast<const unsigned long*> (orig);

      register unsigned long a1 = (a & 0x00ff00ff00ff00ffUL) << 8;
      register unsigned long a2 = (a & 0xff00ff00ff00ff00UL) >> 8;

      a = (a1 | a2);

      * reinterpret_cast<unsigned long*> (target) = a;
#else
      register tbfl::uint32 a =
        * reinterpret_cast<const tbfl::uint32*> (orig);
      register tbfl::uint32 b =
        * reinterpret_cast<const tbfl::uint32*> (orig + 4);

      register tbfl::uint32 a1 = (a & 0x00ff00ffU) << 8;
      register tbfl::uint32 b1 = (b & 0x00ff00ffU) << 8;
      register tbfl::uint32 a2 = (a & 0xff00ff00U) >> 8;
      register tbfl::uint32 b2 = (b & 0xff00ff00U) >> 8;

      a = (a1 | a2);
      b = (b1 | b2);

      * reinterpret_cast<tbfl::uint32*> (target) = a;
      * reinterpret_cast<tbfl::uint32*> (target + 4) = b;
#endif
      orig += 8;
      target += 8;
    }
  }
  else
  {
    // We're out of luck. We have to write in 2 byte chunks.
    while (orig < end)
    {
#if defined (_TBFL_HAS_INTEL_ASSEMBLY_)
      unsigned int a =
        * reinterpret_cast<const unsigned int*> (orig);
      unsigned int b =
        * reinterpret_cast<const unsigned int*> (orig + 4);
      asm ( "bswap %1" : "=r" (a) : "0" (a) );
      asm ( "bswap %1" : "=r" (b) : "0" (b) );
      // We're little endian.
      * reinterpret_cast<unsigned short*> (target + 2)
          = (unsigned short) (a & 0xffff);
      * reinterpret_cast<unsigned short*> (target + 6)
          = (unsigned short) (b & 0xffff);
      asm ( "shrl $16, %1" : "=r" (a) : "0" (a) );
      asm ( "shrl $16, %1" : "=r" (b) : "0" (b) );
      * reinterpret_cast<unsigned short*> (target + 0)
          = (unsigned short) (a & 0xffff);
      * reinterpret_cast<unsigned short*> (target + 4)
          = (unsigned short) (b & 0xffff);
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined (_MSC_VER)
      __asm mov ecx, orig;
      __asm mov edx, target;
      __asm mov eax, [ecx];
      __asm mov ebx, 4[ecx];
      __asm bswap eax;
      __asm bswap ebx;
      // We're little endian.
      __asm mov 2[edx], ax;
      __asm mov 6[edx], bx;
      __asm shr eax, 16;
      __asm shr ebx, 16;
      __asm mov 0[edx], ax;
      __asm mov 4[edx], bx;
#elif _TBFL_SIZEOF_LONG_ == 8
      // 64 bit architecture.
      register unsigned long a =
        * reinterpret_cast<const unsigned long*> (orig);

      register unsigned long a1 = (a & 0x00ff00ff00ff00ffUL) << 8;
      register unsigned long a2 = (a & 0xff00ff00ff00ff00UL) >> 8;

      a = (a1 | a2);

      tbfl::uint16 b1 = static_cast<tbfl::uint16> (a >> 48);
      tbfl::uint16 b2 = static_cast<tbfl::uint16> ((a >> 32) & 0xffff);
      tbfl::uint16 b3 = static_cast<tbfl::uint16> ((a >> 16) & 0xffff);
      tbfl::uint16 b4 = static_cast<tbfl::uint16> (a & 0xffff);

#if defined(_TBFL_LITTLE_ENDIAN_)
      * reinterpret_cast<tbfl::uint16*> (target) = b4;
      * reinterpret_cast<tbfl::uint16*> (target + 2) = b3;
      * reinterpret_cast<tbfl::uint16*> (target + 4) = b2;
      * reinterpret_cast<tbfl::uint16*> (target + 6) = b1;
#else
      * reinterpret_cast<tbfl::uint16*> (target) = b1;
      * reinterpret_cast<tbfl::uint16*> (target + 2) = b2;
      * reinterpret_cast<tbfl::uint16*> (target + 4) = b3;
      * reinterpret_cast<tbfl::uint16*> (target + 6) = b4;
#endif
#else
      register tbfl::uint32 a =
        * reinterpret_cast<const tbfl::uint32*> (orig);
      register tbfl::uint32 b =
        * reinterpret_cast<const tbfl::uint32*> (orig + 4);

      register tbfl::uint32 a1 = (a & 0x00ff00ff) << 8;
      register tbfl::uint32 b1 = (b & 0x00ff00ff) << 8;
      register tbfl::uint32 a2 = (a & 0xff00ff00) >> 8;
      register tbfl::uint32 b2 = (b & 0xff00ff00) >> 8;

      a = (a1 | a2);
      b = (b1 | b2);

      tbfl::uint32 c1 = static_cast<tbfl::uint16> (a >> 16);
      tbfl::uint32 c2 = static_cast<tbfl::uint16> (a & 0xffff);
      tbfl::uint32 c3 = static_cast<tbfl::uint16> (b >> 16);
      tbfl::uint32 c4 = static_cast<tbfl::uint16> (b & 0xffff);

#if defined(_TBFL_LITTLE_ENDIAN_)
      * reinterpret_cast<tbfl::uint16*> (target) = c2;
      * reinterpret_cast<tbfl::uint16*> (target + 2) = c1;
      * reinterpret_cast<tbfl::uint16*> (target + 4) = c4;
      * reinterpret_cast<tbfl::uint16*> (target + 6) = c3;
#else
      * reinterpret_cast<tbfl::uint16*> (target) = c1;
      * reinterpret_cast<tbfl::uint16*> (target + 2) = c2;
      * reinterpret_cast<tbfl::uint16*> (target + 4) = c3;
      * reinterpret_cast<tbfl::uint16*> (target + 6) = c4;
#endif
#endif
      orig += 8;
      target += 8;
    }
  }

  // (n & 3) == (n % 4).
  switch (n & 3) 
  {
    case 3:
      Endianness::swap_2(orig, target);
      orig += 2;
      target += 2;
    case 2:
      Endianness::swap_2(orig, target);
      orig += 2;
      target += 2;
    case 1:
      Endianness::swap_2(orig, target);
  }
}

//=============================================================================
// Endianness::swap_4_array
//=============================================================================
void Endianness::swap_4_array (const char* orig, char* target, size_t n)
{
  if ( ! n )
    return;

#if _TBFL_SIZEOF_LONG_ == 8
  // Later, we read from *orig in 64 bit chunks,
  // so make sure we don't generate unaligned readings.
  char const * const o8 = PTR_ALIGN_BINARY (orig, 8);
  // The mismatch can only be by 4.
  if (orig != o8)
  {
    Endianness::swap_4(orig, target);
    orig += 4;
    target += 4;
    --n;
  }
#endif  /* _TBFL_SIZEOF_LONG_ == 8 */

  if ( ! n )
    return;

  //
  // Loop unrolling. Here be dragons.
  //

  // (n & (~3)) is the greatest multiple of 4 not bigger than n.
  // In the while loop, orig will move over the array by 16 byte
  // increments (4 elements of 4 bytes).
  // ends marks our barrier for not falling outside.
  char const * const end = orig + 4 * (n & (~3));

#if _TBFL_SIZEOF_LONG_ == 8
  // 64 bits architecture.
  // See if we can write in 8 byte chunks.
  if (target == PTR_ALIGN_BINARY (target, 8))
  {
    while (orig < end)
    {
      register unsigned long a =
        * reinterpret_cast<const long*> (orig);
      register unsigned long b =
        * reinterpret_cast<const long*> (orig + 8);

#if defined(_TBFL_HAS_INTEL_ASSEMBLY_)
      asm ("bswapq %1" : "=r" (a) : "0" (a));
      asm ("bswapq %1" : "=r" (b) : "0" (b));
      asm ("rol $32, %1" : "=r" (a) : "0" (a));
      asm ("rol $32, %1" : "=r" (b) : "0" (b));
#else
      register unsigned long a84 = (a & 0x000000ff000000ffL) << 24;
      register unsigned long b84 = (b & 0x000000ff000000ffL) << 24;
      register unsigned long a73 = (a & 0x0000ff000000ff00L) << 8;
      register unsigned long b73 = (b & 0x0000ff000000ff00L) << 8;
      register unsigned long a62 = (a & 0x00ff000000ff0000L) >> 8;
      register unsigned long b62 = (b & 0x00ff000000ff0000L) >> 8;
      register unsigned long a51 = (a & 0xff000000ff000000L) >> 24;
      register unsigned long b51 = (b & 0xff000000ff000000L) >> 24;

      a = (a84 | a73 | a62 | a51);
      b = (b84 | b73 | b62 | b51);
#endif

      * reinterpret_cast<long*> (target) = a;
      * reinterpret_cast<long*> (target + 8) = b;

      orig += 16;
      target += 16;
    }
  }
  else
  {
    // We are out of luck, we have to write in 4 byte chunks.
    while (orig < end)
    {
      register unsigned long a =
        * reinterpret_cast<const long*> (orig);
      register unsigned long b =
        * reinterpret_cast<const long*> (orig + 8);

#if defined(_TBFL_HAS_INTEL_ASSEMBLY_)
      asm ("bswapq %1" : "=r" (a) : "0" (a));
      asm ("bswapq %1" : "=r" (b) : "0" (b));
      asm ("rol $32, %1" : "=r" (a) : "0" (a));
      asm ("rol $32, %1" : "=r" (b) : "0" (b));
#else
      register unsigned long a84 = (a & 0x000000ff000000ffL) << 24;
      register unsigned long b84 = (b & 0x000000ff000000ffL) << 24;
      register unsigned long a73 = (a & 0x0000ff000000ff00L) << 8;
      register unsigned long b73 = (b & 0x0000ff000000ff00L) << 8;
      register unsigned long a62 = (a & 0x00ff000000ff0000L) >> 8;
      register unsigned long b62 = (b & 0x00ff000000ff0000L) >> 8;
      register unsigned long a51 = (a & 0xff000000ff000000L) >> 24;
      register unsigned long b51 = (b & 0xff000000ff000000L) >> 24;

      a = (a84 | a73 | a62 | a51);
      b = (b84 | b73 | b62 | b51);
#endif

      tbfl::uint32 c1 = static_cast<tbfl::uint32> (a >> 32);
      tbfl::uint32 c2 = static_cast<tbfl::uint32> (a & 0xffffffff);
      tbfl::uint32 c3 = static_cast<tbfl::uint32> (b >> 32);
      tbfl::uint32 c4 = static_cast<tbfl::uint32> (b & 0xffffffff);

#if defined (_TBFL_LITTLE_ENDIAN_)
      * reinterpret_cast<tbfl::uint32*> (target + 0) = c2;
      * reinterpret_cast<tbfl::uint32*> (target + 4) = c1;
      * reinterpret_cast<tbfl::uint32*> (target + 8) = c4;
      * reinterpret_cast<tbfl::uint32*> (target + 12) = c3;
#else
      * reinterpret_cast<tbfl::uint32*> (target + 0) = c1;
      * reinterpret_cast<tbfl::uint32*> (target + 4) = c2;
      * reinterpret_cast<tbfl::uint32*> (target + 8) = c3;
      * reinterpret_cast<tbfl::uint32*> (target + 12) = c4;
#endif
      orig += 16;
      target += 16;
    }
  }
#else  /* _TBFL_SIZEOF_LONG_ != 8 */

  while (orig < end)
  {
#if _TBFL_HAS_PENTIUM_ && defined (__GNUG__)
    register unsigned int a =
      *reinterpret_cast<const unsigned int*> (orig);
    register unsigned int b =
      *reinterpret_cast<const unsigned int*> (orig + 4);
    register unsigned int c =
      *reinterpret_cast<const unsigned int*> (orig + 8);
    register unsigned int d =
      *reinterpret_cast<const unsigned int*> (orig + 12);

    asm ("bswap %1" : "=r" (a) : "0" (a));
    asm ("bswap %1" : "=r" (b) : "0" (b));
    asm ("bswap %1" : "=r" (c) : "0" (c));
    asm ("bswap %1" : "=r" (d) : "0" (d));

    *reinterpret_cast<unsigned int*> (target) = a;
    *reinterpret_cast<unsigned int*> (target + 4) = b;
    *reinterpret_cast<unsigned int*> (target + 8) = c;
    *reinterpret_cast<unsigned int*> (target + 12) = d;
#elif ! defined(_TBFL_HAS_NO_INLINE_ASM_) && _TBFL_HAS_PENTIUM_ && defined (_MSC_VER)
    __asm mov eax, orig
    __asm mov esi, target
    __asm mov edx, [eax]
    __asm mov ecx, 4[eax]
    __asm mov ebx, 8[eax]
    __asm mov eax, 12[eax]
    __asm bswap edx
    __asm bswap ecx
    __asm bswap ebx
    __asm bswap eax
    __asm mov [esi], edx
    __asm mov 4[esi], ecx
    __asm mov 8[esi], ebx
    __asm mov 12[esi], eax
#else
    register tbfl::uint32 a =
      * reinterpret_cast<const tbfl::uint32*> (orig);
    register tbfl::uint32 b =
      * reinterpret_cast<const tbfl::uint32*> (orig + 4);
    register tbfl::uint32 c =
      * reinterpret_cast<const tbfl::uint32*> (orig + 8);
    register tbfl::uint32 d =
      * reinterpret_cast<const tbfl::uint32*> (orig + 12);

    // Expect the optimizer reordering this A LOT.
    // We leave it this way for clarity.
    a = (a << 24) | ((a & 0xff00) << 8) | ((a & 0xff0000) >> 8) | (a >> 24);
    b = (b << 24) | ((b & 0xff00) << 8) | ((b & 0xff0000) >> 8) | (b >> 24);
    c = (c << 24) | ((c & 0xff00) << 8) | ((c & 0xff0000) >> 8) | (c >> 24);
    d = (d << 24) | ((d & 0xff00) << 8) | ((d & 0xff0000) >> 8) | (d >> 24);

    * reinterpret_cast<tbfl::uint32*> (target) = a;
    * reinterpret_cast<tbfl::uint32*> (target + 4) = b;
    * reinterpret_cast<tbfl::uint32*> (target + 8) = c;
    * reinterpret_cast<tbfl::uint32*> (target + 12) = d;
#endif
      orig += 16;
      target += 16;
    }

#endif /* _TBFL_SIZEOF_LONG_ == 8 */

  // (n & 3) == (n % 4).
  switch (n & 3) 
  {
    case 3:
      Endianness::swap_4(orig, target);
      orig += 4;
      target += 4;
    case 2:
      Endianness::swap_4(orig, target);
      orig += 4;
      target += 4;
    case 1:
      Endianness::swap_4(orig, target);
  }
}

//=============================================================================
// Endianness::swap_8_array
//=============================================================================
void Endianness::swap_8_array (const char* orig, char* target, size_t n)
{
  if ( ! n )
    return;

  char const * const end = orig + 8 * n;

  while (orig < end)
  {
    swap_8 (orig, target);
    orig += 8;
    target += 8;
  }
}

//=============================================================================
// Endianness::swap_16_array
//=============================================================================
void Endianness::swap_16_array (const char* orig, char* target, size_t n)
{
  if ( ! n )
    return;

  char const * const end = orig + 16 * n;

  while (orig < end)
  {
    swap_16 (orig, target);
    orig += 16;
    target += 16;
  }
}

#endif //- DISABLE_POLYMORPHIC_IMPL

