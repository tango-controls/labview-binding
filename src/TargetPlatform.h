// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   TargetPlatform.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//-----------------------------------------------------------------------------
// PLATFORM CONFIGURATION
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// WINDOWS on INTEL PENTIUM OR COMPATIBLE PROCESSORS
//-----------------------------------------------------------------------------
// supported compiler: Microsoft Vsual C++ [>= 8.0]
// supported processors: x86 and x64 
// endianness : little 
//-----------------------------------------------------------------------------
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
# define _TBFL_WINDOWS_
# define WIN32_LEAN_AND_MEAN
#pragma warning(disable:4005)
#pragma warning(disable:4267)
# if defined(_M_X64)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_NO_INLINE_ASM_
# elif defined(_M_IX86) && (_M_IX86 > 400)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
# else
#  error "sorry... no support for this processor!"
# endif
# if defined(_M_X64) && (defined(_WIN64) || defined(WIN64))
#  define _TBFL_HAS_64_BITS_PLATFORM_  1
#else
#  define _TBFL_HAS_64_BITS_PLATFORM_  0
#endif
# if (_MSC_VER < 1600)
#  error "Microsoft Visual C++ >= 7.0 required"
# elif (_MSC_VER >= 1600) //- v10
#  define _TBFL_HAS_INTRINSIC_BYTESWAP_
# endif
# define _TBFL_HAS_STATIC_OBJ_MANAGER_ 0
namespace tbfl
{
  typedef char int8;
  typedef unsigned char uint8;
  typedef short int16;
  typedef unsigned short uint16;
  #if defined(_WIN64) || defined(WIN64) 
    typedef int int32;
    typedef unsigned int uint32;
  #else
    typedef long int32;
    typedef unsigned long uint32;
  #endif
  typedef __int64 int64;
  typedef unsigned __int64 uint64;
}
//-----------------------------------------------------------------------------
// LINUX on INTEL PENTIUM OR COMPATIBLE PROCESSORS
//-----------------------------------------------------------------------------
// supported compiler: gcc [>= 4.2.0]
// supported processors: x86 and x64 
// endianness : little 
//-----------------------------------------------------------------------------
#elif defined(_linux_) || defined (__linux__)
# include <stdint.h>
# define _TBFL_LINUX_
# if defined(i386) || defined(__i386__)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_  0
# elif defined(i686) || defined(__i686__)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_  0
# elif defined(x86_64) || defined(__x86_64__)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_ 0 //-> 10.03.2011 - no 64 bits LV for linux
# elif defined(arm) || defined(__arm__)
#  define _TBFL_HAS_PENTIUM_ 0
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_ 0 //-> 10.03.2011 - no 64 bits LV for linux
# else
#  error "sorry... no support for this processor!"
# endif
# if ! defined(__GNUG__)
#  error "sorry... no support for this compiler - GCC compiler required"
# endif
# define _TBFL_HAS_STATIC_OBJ_MANAGER_ 0
# if _TBFL_HAS_PENTIUM_ 
#  define _TBFL_HAS_INTEL_ASSEMBLY_
# endif
# define _TBFL_HAS_BYTESWAP_H_
# define _TBFL_HAS_BSWAP_16_
# define _TBFL_HAS_BSWAP_32_
# define _TBFL_HAS_BSWAP_64_
namespace tbfl
{
  typedef int8_t   int8;
  typedef uint8_t  uint8;
  typedef int16_t  int16;
  typedef uint16_t uint16;
  typedef int32_t  int32;
  typedef uint32_t uint32;
  typedef int64_t  int64;
  typedef uint64_t uint64;
}
//-----------------------------------------------------------------------------
// MACOSX on INTEL PENTIUM
//-----------------------------------------------------------------------------
// supported compiler: gcc [>= 4.2.1]
// supported processors: x86 and x64 
// endianness : little 
//-----------------------------------------------------------------------------
#elif defined(__APPLE__) && defined (__MACH__)
# define _TBFL_MACOSX_
# if defined(i386) || defined(__i386__)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_  0
# elif defined(i686) || defined(__i686__)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_  0
# elif defined(x86_64) || defined(__x86_64__)
#  define _TBFL_HAS_PENTIUM_ 1
#  define _TBFL_LITTLE_ENDIAN_PLATFORM_ 1
#  define _TBFL_HAS_64_BITS_PLATFORM_  0  //-> 10.03.2011 - no 64 bits LV for MacOS X
# else
#  error "sorry... no support for this processor!"
# endif
# if ! defined(__GNUC__)
#  error "sorry... no support for this compiler - GCC compiler required"
# else
#  define _TBFL_HAS_STATIC_OBJ_MANAGER_ 0
# endif 
namespace tbfl
{
  typedef int8_t   int8;
  typedef uint8_t  uint8;
  typedef int16_t  int16;
  typedef uint16_t uint16;
  typedef int32_t  int32;
  typedef uint32_t uint32;
  typedef int64_t  int64;
  typedef uint64_t uint64;
}
//-----------------------------------------------------------------------------
// UNKNOWN/UNSUPPORTED PLATEFORM
//-----------------------------------------------------------------------------
#else
#  error "no support for this platform"
#endif

