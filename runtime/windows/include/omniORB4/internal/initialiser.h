// -*- Mode: C++; -*-
//                            Package   : omniORB2
// initialiser.h              Created on: 16/8/99
//                            Author    : Sai-Lai Lo (sll)
//
//    Copyright (C) 1999 AT&T Research Cambridge
//
//    This file is part of the omniORB library.
//
//    The omniORB library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Library General Public
//    License as published by the Free Software Foundation; either
//    version 2 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Library General Public License for more details.
//
//    You should have received a copy of the GNU Library General Public
//    License along with this library; if not, write to the Free
//    Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//    02111-1307, USA
//
//
// Description:
//

#ifndef __INITIALISER_H__
#define __INITIALISER_H__

OMNI_NAMESPACE_BEGIN(omni)

// Singletons created per compilation unit. The attach method is called
// when the ORB is initialised. The detach method is called when the ORB
// is destroyed.
class omniInitialiser {
public:
  virtual void attach() = 0;
  virtual void detach() = 0;

  static void install(omniInitialiser* init);
  // Hook to allow modules to add initialisers dynamically. These
  // initialisers are run after all statically declared ones. Not
  // thread safe.

  class init_t {
    static int count;
  public:
    init_t(void);
  };
};

OMNI_NAMESPACE_END(omni)

#endif  // __INITIALISER_H__
