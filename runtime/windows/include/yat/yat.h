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
// Copyright (C) 2006-2012  N.Leclercq & The Tango Community
//
// Part of the code comes from the ACE Framework (i386 asm bytes swaping code)
// see http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//
// The thread native implementation has been initially inspired by omniThread
// - the threading support library that comes with omniORB. 
// see http://omniorb.sourceforge.net/ for more about omniORB.
//
// Contributors form the TANGO community:
// See AUTHORS file 
//
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

#ifndef _YAT_H_
#define _YAT_H_

// ----------------------------------------------------------------------------
//! \mainpage YAT library
//! \tableofcontents
//! YAT is a general purpose C++ library.\n
//! Its purpose is to provide the developer with useful, easy to use and 
//! powerful C++ tools for the common (i.e. classical) developement tasks.\n 
//! The topics currently addressed by YAT library are:
//!   - \subpage genericPage "Generic containers"
//!   - \subpage streamsPage "Bit streams"
//!   - \subpage filePage "File access"
//!   - \subpage memoryPage "Memory management"
//!   - \subpage networkPage "TCP and UDP socket client"
//!   - \subpage plugPage "Plugins"
//!   - \subpage systPage "System utilities"
//!   - \subpage threadingPage "Threading"
//!   - \subpage TimePage "Time utilities"
//!   - \subpage utilitiesPage "General Utilities"
//!
//! \remark
//! Yat4Tango is a specialization - and also an extension - of the YAT library 
//! for the TANGO devices developers. For instance, the yat4Tango::DeviceTask 
//! is better adapted to the context of TANGO Device than the yat::Task.\n
//!
//! <b>Main Authors & Maintainers:</b>\n
//!   - Nicolas Leclercq - Synchrotron SOLEIL - France\n
//!   - Stephane Poirier - Synchrotron SOLEIL - France\n
//!
//! <b>Contributors:</b>\n
//!   - Julien Malik - Synchrotron SOLEIL - France - Several classes (such as Callback)\n
//!   - Frederic Pica - Synchrotron SOLEIL - France - Autools support\n 
//!   - Vincent Hardion - Synchrotron SOLEIL - France - Maven support\n
//!   - Jamon Sune - ALBA Synchrotron Light Source - Spain - Slot Class\n
//!   - Sergi Blanch Torne - ALBA Synchrotron Light Source - Spain - Backward compatibility for old YAT data types\n
//!   - Sonia Minolli - Nexeya - Doxygen'ized the API\n
//!
//! <b>YAT license:</b>
//!
//! Copyright (C) 2006-2012 The Tango Community (http://tango-controls.org)
//! 
//! Part of the code comes from the ACE Framework (i386 asm bytes swaping code).\n
//! See http://www.cs.wustl.edu/~schmidt/ACE.html for more about ACE
//! 
//! The thread native implementation has been initially inspired by omniThread 
//! (the threading support library that comes with omniORB).\n
//! See http://omniorb.sourceforge.net/ for more about omniORB.
//! 
//! The YAT library is free software; you can redistribute it and/or modify it 
//! under the terms of the GNU General Public License as published by the Free 
//! Software Foundation; either version 2 of the License, or (at your option) 
//! any later version.
//! 
//! The YAT library is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
//! Public License for more details.
//! 
//! See COPYING file for license details
//! 
//! <b>Main Contact:</b>\n
//!   Nicolas Leclercq - Synchrotron SOLEIL - France
// ----------------------------------------------------------------------------

#endif // _YAT_H_
