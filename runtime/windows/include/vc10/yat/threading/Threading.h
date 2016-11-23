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
// Copyright (C) 2006-2011  N.Leclercq & The Tango Community
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
/*!
 * \author See AUTHORS file
 */

#ifndef _THREADING_H_
#define _THREADING_H_


// ============================================================================
//! \page threadingPage Threading documentation
//! \tableofcontents
//! The threading utilities provide a set of classes implementing classical threading 
//! concepts as well as the specific task notion. \n
//!
//! \section sec1 Classical threading concepts
//! The threading utilities provide the following classical threading concepts :
//! - thread
//! - mutex 
//! - semaphore
//! - barrier
//! 
//! \subsection ssec11 Thread concept 
//! The Thread class is a basic implementation of the thread concept. It's an abstract class
//! that must be derived to be used.\n
//! A Thread has a state, an identifier and a priority, managed by an internal mutex. 
//! It provides both "detached" and "undetached" (i.e. joinable) behaviour.\n
//! For example, a Thread class can be used to implement a simple data acquisition thread. 
//! For more complex threaded treatments, use the Task concept described below.
//!
//! \subsection ssec12 Mutex concept
//! The Yat Mutex implementation provides three classes :
//! - Mutex class : a basic mutex implementation.
//! - Automutex template : a "lock safe" mutex (i.e. mutex will be unlocked whatever the exit path of the thread code is).
//! - NullMutex class :  a "do nothing" mutex implementation (useful to control thread safety through "real" or "do nothing" mutex).
//!
//! \subsection ssec13 Semaphore concept
//! The Semaphore class is a basic implementation of the semaphore concept. \n
//! The AutoSemaphore class provides a "lock safe" semaphore (i.e. semaphore will be released whatever the exit path of the thread code is).
//!
//! \subsection ssec12 Barrier concept
//! The Barrier class is a basic implementation of threads synchronization point.
//!
//! \section sec2 Task concept
//! \subsection ssec21 Task class description
//! The yat task provides a simplified thread concept which hides shared memory management by using stimulation messages.\n
//! The Task class inherits from the Thread class and manage a message queue (MessageQ class). The task activity is only stimulated by 
//! message reception (predefined yat message or user defined messages), which can be synchronous or asynchronous.\n
//! 
//! - Asynchronous mechanism : the task posts a message and waits for the message to be properly posted (not handled). 
//! An exception is thrown if timeout expires.
//! - Synchronous mechanism: the task posts a waitable message and waits for the message to be handled by the recipient. 
//! An exception is thrown if timeout expires (on post or treatment).
//!
//! \subsection ssec22 Task life cycle
//! Here are the main actions to realize in order to use a Task class :
//! - Inherits from yat::Task to implement your own task 
//! (override the message manager function handle_message() which is called when a message is received in the message queue).
//! - Define specific commands to activate the task (it is recommended not to expose messages but only a command like interface,
//! and define a private function to self-post a message).
//! - Usage :\n
//!   myTask = new userTask(); // Creates my task object\n
//!   myTask->go(); // Starts my task with a synchronous management\n
//!   ...\n
//!   myTask->myCommand(); // Stimulates my task (i.e. sends a specific message)\n
//!   ...\n
//!   myTask->exit(); // Stops my task\n
//!
//! \subsection ssec23 Messages
//! A yat message is a yat::SharedObject defined by a type, a priority, a waitable attribute and some associated data.
//! - Message type : specifies if message is a yat predefined message or user message
//! - Message priority : useful in message queue management (a highest priority message will be handled first)
//! - Waitable : specifies if message can be used in synchronous mechanism
//! - data : data sent with message
//! 
//! \remark A message type can be PERIODIC or TIMEOUT :
//! - PERIODIC : message sent every specified period (best effort)
//! - TIMEOUT : message sent if no message received after specified timeout
//!
//! \subsection ssec24 Message queue 
//! The MessageQ message queue is a FIFO message queue for equal priority messages, 
//! i.e. an incoming message is put in the message queue before messages with lowest priority.\n
//! 
//! In order to prevent message queue congestion, two water marks are used :
//! - High Water Mark : if message number in queue < HWM, a message can be posted
//! - Low Water Mark : if message number in queue > HWM, wait until message number in queue < LWM before posting a message
//! 
//! \remark The MessageQ class also provides statistics on message queue running.
//!
//! \subsection ssec25 Shared object concept
//! The SharedObject class is a basic thread safe reference counter implementation. It's an abstract class
//! that must be derived to be used.\n
//! Message class is an example of ShareObject implementation.
//! \remark It is recommended not to use a SharedObject class alone, but through a template class (see yat::SharedPtr).
//!
//! \subsection ssec26 Condition concept
//! The Condition class implements a "thread safe" conditional evaluation. The condition is associated to a mutex to be hold at each
//! condition handling.
//! \remark It is recommended to use a yat::AutoMutex mutex.
//!
//! \section sec3 Threading classes
//! Links to threading classes : \n
//!   - yat::Task
//!   - yat::Thread
//!   - yat::Message
//!   - yat::MessageQ
//!   - yat::Barrier
//!   - yat::Condition
//!   - yat::Mutex
//!   - yat::NullMutex
//!   - yat::AutoMutex
//!   - yat::Semaphore
//!   - yat::AutoSemaphore
//!   - yat::SharedObject
//!   - yat::SyncAccess
//!   - yat::ThreadingUtilities
// ============================================================================


#endif // _THREADING_H_
