// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   Endianness.h - CODE STOLEN FROM THE FABULOUS ACE FRAMEWORK
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
/*
 * The current implementation assumes that the host has	1-byte,
 * 2-byte and 4-byte integral types, and that it has single
 * precision and double	precision IEEE floats.
 * Those assumptions are pretty	good these days, with Crays being
 * the only known exception.
 */
//=============================================================================


#ifndef	_ENDIANNESS_H_
#define	_ENDIANNESS_H_

// ============================================================================
class Endianness
{
public:
  //- byte swapping for scalars
  static void swap_2  (const char *orig, char *target);
  static void swap_4  (const char *orig, char *target);
  static void swap_8  (const char *orig, char *target);
  static void swap_16 (const char *orig, char *target);

  //- byte swapping for arrays
  static void swap_2_array  (const char *orig, char *target, size_t length);
  static void swap_4_array  (const char *orig, char *target, size_t length);
  static void swap_8_array  (const char *orig, char *target, size_t length);
  static void swap_16_array (const char	*orig, char *target, size_t length);
};

//=============================================================================
// INLINED CODE
//=============================================================================
#if defined (_LV_INLINE_)
  #include "Endianness.i"
#endif 

#endif //- _ENDIANNESS_H_
