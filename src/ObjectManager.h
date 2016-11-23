// ============================================================================
//
// = CONTEXT
//   This is part of the Tango binding for LabVIEW
//
// = FILENAME
//   ObjectManager.h
//
// = AUTHOR
//   Nicolas Leclercq - Synchrotron SOLEIL - France
//
// ============================================================================

#pragma once

//=============================================================================
// CLASS : ObjectManager [SINGLETON]
//=============================================================================
class ObjectManager
{
  //- just make some compilers happy: give ObjectManager a friend
  friend class DummyClass;

public:

#if (_TBFL_HAS_STATIC_OBJ_MANAGER_ == 0)

  static void init () 
    throw (Tango::DevFailed);

  static void fini () 
    throw (Tango::DevFailed);
#endif

private:
#if (_TBFL_HAS_STATIC_OBJ_MANAGER_ == 0)
  static ObjectManager * instance;
#else
  static ObjectManager instance;
#endif

  // = Disallow these operations (except for friend classes).
  //---------------------------------------------------------
  ObjectManager();
  virtual ~ObjectManager();
  ObjectManager (const ObjectManager&);
  void operator= (const ObjectManager&); 
};

   
