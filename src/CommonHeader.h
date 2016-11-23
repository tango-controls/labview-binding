// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   CommonHeader.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//-----------------------------------------------------------------------------
// DEPENDENCIES
//-----------------------------------------------------------------------------
#include "TargetPlatform.h"
#include "yat/time/Timer.h"

//-----------------------------------------------------------------------------
// DEBUG 
//-----------------------------------------------------------------------------
#if !defined _TBFL_DEBUG_
# define _LV_INLINE_
#endif

//-----------------------------------------------------------------------------
// EXTERN "C" EXPORT
//-----------------------------------------------------------------------------
#if defined(WIN32)
# define LVB_EXPORT extern "C" __declspec (dllexport)
# define LVB_CALLING_CONV __cdecl
#else
# define LVB_EXPORT extern "C"
# define LVB_CALLING_CONV
#endif

//-----------------------------------------------------------------------------
// INLINE
//-----------------------------------------------------------------------------
#if defined(_LV_INLINE_)
# define LV_INLINE inline
#else
# define LV_INLINE
#endif

//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define _CPTC_(X) static_cast<const char*>(X)
//-----------------------------------------------------------------------------
#define _DECLARE_MAP_(KEY, VAL, PREFIX) \
  typedef std::map<KEY, VAL> PREFIX##Map; \
  typedef std::map<KEY, VAL>::iterator PREFIX##MapIterator; \
  typedef std::map<KEY, VAL>::const_iterator PREFIX##MapConstIterator;
//-----------------------------------------------------------------------------
#define _DECLARE_VECTOR_(TYPE, PREFIX) \
  typedef std::vector<TYPE> PREFIX##Vector; \
  typedef std::vector<TYPE>::iterator PREFIX##VectorIterator; \
  typedef std::vector<TYPE>::const_iterator PREFIX##VectorConstIterator;
//-----------------------------------------------------------------------------
#define _DECLARE_QUEUE_(TYPE, PREFIX) \
  typedef std::deque<TYPE> PREFIX##Queue; \
  typedef std::deque<TYPE>::iterator PREFIX##QueueIterator; \
  typedef std::deque<TYPE>::const_iterator PREFIX##QueueConstIterator;
//-----------------------------------------------------------------------------
#define _BEST_EFFORT_TRY_(ACT) \
  try { ACT;} catch ( ... ) {/* ignore error */}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// LABVIEW SUPPORT
//-----------------------------------------------------------------------------
#include <extcode.h>

//-----------------------------------------------------------------------------
// TANGO SUPPORT
//-----------------------------------------------------------------------------
#include <tango.h>

//-----------------------------------------------------------------------------
// OTHERS
//-----------------------------------------------------------------------------
#include "DebugTrace.h"
#include "Types.h"

//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------
extern const int kNoError;
extern const int kError;

//-----------------------------------------------------------------------------
// PSEUDO CONSTS
//-----------------------------------------------------------------------------
#define kTIME_OFFSET 2082844800.

