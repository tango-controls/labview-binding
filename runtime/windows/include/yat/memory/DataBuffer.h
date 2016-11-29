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

#ifndef _YAT_DATA_BUFFER_H_
#define _YAT_DATA_BUFFER_H_


// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <cstring>
#include <yat/threading/SharedObject.h>

namespace yat 
{

// ============================================================================
//! \class Buffer 
//! \brief A buffer abstraction class.
//!  
//! This template class provides a simple buffer abstraction, i.e. a one-dimensionnal 
//! array (vector) of type \<T\> elements. \n
//! Implementation constraint: operator= must be defined for template parameter \<T\>.
// ============================================================================
template <typename T> 
class Buffer 
{
public:
  //- some typedefs
  typedef T value_type;
  typedef std::size_t size_type;
  
  //! \brief Constructor. 
  //!
  //! \param capacity The maximum number of elements of type \<T\> that can be stored 
  //! into the buffer.
  //! \param clear If set to "true", clears the associated memory block (i.e. sets each
  //! byte to 0), does nothing ortherwise.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  Buffer (size_t capacity = 0, bool clear = false);
 
  //! \brief Memory copy constructor. 
  //!
  //! Memory is copied from 'base' to 'base + length * sizeof(T)'.
  //! \param length The number of elements of type \<T\> to be copied into the buffer.
  //! \param base Address of the memory block to copy into the buffer.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  Buffer (size_t length, const T *base);

  //! \brief Copy constructor.
  //!
  //! \param buf The source buffer.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  Buffer (const Buffer<T> &buf);

  //! \brief Destructor. 
  //! 
  //! Releases underlying memory.
  virtual ~Buffer ();

  //! \brief Operator=.
  //!
  //! \param src The source buffer.
  Buffer<T>& operator= (const Buffer<T> &src);

  //! \brief Operator=. 
  //!
  //! Memory is copied from base to base + Buffer::length_ * sizeof(T). 
  //! \param base Address of the block to copy.
  Buffer<T>& operator= (const T *base);

  //! \brief Operator=. 
  //!
  //! Fills the buffer with the specified value.
  //! \param val The specified value.
  Buffer<T>& operator= (const T &val);
   
  //! \brief Fills the buffer with the specified value.
  //!
  //! \param val The specified value.
  void fill (const T& val);
  
  //! \brief Clears buffer's content. 
  //!
  //! This is a low level clear: set memory from Buffer::base_ to 
  //! Buffer::base_ + Buffer::length_ to 0.
  void clear ();

  //! \brief Returns a reference to the ith element.
  //!
  //! Returns a reference to the ith element of the buffer. No bound error checking.
  //! \param i Index of the element to return.
  T& operator[] (size_t i);

  //! \brief Returns a const reference to the ith element. 
  //!
  //! Returns a const reference to the ith element of the bueffer. No bound error checking.
  //! \param i Index of the element to return.
  const T& operator[] (size_t i) const;

  //! \brief Returns the size of each element in bytes.
  //!
  //! Returns sizeof(\<T\>).
  size_t elem_size () const;

  //! \brief Returns the number of bytes currently stored into the buffer.
  size_t size () const;

  //! \brief Returns the number of elements currently stored into the buffer. 
  size_t length () const;
  
  //! \brief Copy n elments of type T from the specified source to the buffer. 
  //! The number of bytes actually copied is n * sizeof(T). 
  //! The copied data is placed into the buffer at 'buffer base' address + offset.
  //! The buffer length is then set to current length + num_elements.
  //!
  //! \param num_elements Number of elements of type T to be copied.
  //! \param offset Offset in number of elements from the buffer base address.
  void memcpy (const void *src, size_t num_elements, size_t offset = 0);
  
  //! \brief Artificially changes the buffer length. 
  //!
  //! \param new_length New number of elements.
  //! \remark If new_length is greater than buffer capacity, then buffer 
  //! length is set to buffer capacity.
  void force_length (size_t new_length);
  
  //! \brief Returns the buffer capacity (i.e. max num of elements that can be stored into the buffer). 
  size_t capacity () const;
  
  //! \brief Sets the buffer capacity to specified value.
  //!
  //! \param new_capacity New buffer capacity in number of elements.
  //! \param keep_content If set to true, the current buffer content is maintained 
  //! (might be truncated if new capacity < current buffer capacity).
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  virtual void capacity (size_t new_capacity, bool keep_content = false);
  
  //! \brief Returns true is the buffer is empty, false otherwise.
  bool empty () const;

  //! \brief Returns the buffer base address.
  T * base () const;

  //! \brief Transfers underlying data ownership to caller. 
  //! Capacity is shrinked to buffer.length(). New owner must delete[] the returned buffer.
  //!
  //! \param base The buffer base address
  //! \param length The buffer length (i.e. num of elements of type T)
  void detach_data (T*& base, size_t& length);  

protected:

  //! \brief The buffer base address. 
  T * base_;

  //! \brief Maximum number of element of type \<T\>.
  size_t capacity_;
  
  //! \brief Current number of element of type \<T\>.
  size_t length_;
};

// ============================================================================
//! \class ImageBuffer 
//! \brief An image container abstraction class.
//!  
//! This template class inherits from yat::Buffer template class and provides an image 
//! container abstraction, i.e. a two-dimensionnal array of type \<T\> elements.
//!
//! Implementation constraint: operator= must be defined for template parameter \<T\>.
// ============================================================================
template <typename T>
class ImageBuffer : public yat::Buffer<T>
{
public:
  //! \brief Constructor (empty data). 
  //!
  //! \param width Width of the image in pixels.
  //! \param height Height of the image in pixels.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  ImageBuffer (size_t width = 0, size_t height = 0);
 
  //! \brief Memory copy Constructor.
  //! 
  //! Memory is copied from 'base' to 'base + width * height * sizeof(T)'.
  //! \param width Width of the image in pixels.
  //! \param height Height of the image in pixels.
  //! \param base Address of the block to copy.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  ImageBuffer (size_t width, size_t height, const T *base);

  //! \brief Copy Constructor.
  //!
  //! \param im The source image.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  ImageBuffer (const ImageBuffer<T>& im);

  //! \brief Constructor (with data). 
  //! 
  //! \param width Width of the image in pixels.
  //! \param height Height of the image in pixels.
  //! \param buf Buffer to copy data from.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  ImageBuffer (size_t width, size_t height, const yat::Buffer<T>& buf);

  //! \brief Destructor.
  virtual ~ImageBuffer();

  //! \brief Image width accessor. 
  size_t width () const;

  //! \brief Image height accessor.
  size_t height () const;

  //! \brief Setter for the dimensions of the image (set the length at width * height).
  void set_dimensions (size_t width, size_t height);

  //! \brief Changes the dimensions of the array and preserve data.
  void resize (size_t new_width, size_t new_height);

  //! \brief Operator=.
  //! 
  //! \param src The source image.
  ImageBuffer<T>& operator= (const ImageBuffer<T> &src);

  //! \brief Operator=. 
  //!
  //! Memory is copied from base to base + ImageBuffer::length_.
  //! \param base Address of the block to copy.
  ImageBuffer<T>& operator= (const T *base);

  //! \brief Operator=. 
  //!
  //! Fills the buffer with the specified value of type \<T\>.
  //! \param val The specified value.
  ImageBuffer<T>& operator= (const T &val);

protected:
  //! \brief Image width.
  size_t width_;

  //! \brief Image height.
  size_t height_;
};

// ============================================================================
//! \class SharedBuffer 
//! \brief A thread safe shared circular buffer abstraction class.
//!  
//! This template class inherits from yat::Buffer template class and yat::SharedObject
//! class. It provides a thread safe shared circular buffer abstraction 
//! (one-dimensionnal array).
//!
//! Implementation constraint: <operator=> must be defined for template parameter \<T\>.
// ============================================================================
template <typename T> 
class SharedBuffer : public Buffer<T>, public SharedObject
{
protected:
  //! \brief Constructor. 
  //!
  //! \param  capacity Maximum number of elements of type \<T\>
  //! that can be stored into the buffer.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  SharedBuffer (size_t capacity = 0);
 
  //! \brief Memory copy constructor. 
  //! 
  //! Memory is copied from base to base + length * sizeof(\<T\>).
  //! \param length Number of elements of type \<T\> to be copied into the buffer. 
  //! \param base Address of the block to copy.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  SharedBuffer (size_t length, const T *base);

  //! \brief Copy constructor. 
  //! 
  //! Uses allocator associated with the source buffer.
  //! \param buf The source buffer.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  SharedBuffer (const Buffer<T> &buf);

  //! \brief Destructor. 
  //!
  //! Releases resources.
  virtual ~SharedBuffer ();

  //! \brief Returns a "shallow" copy of the shared buffer (avoids deep copy).
  SharedBuffer * duplicate ();
};

// ============================================================================
//! \class CircularBuffer 
//! \brief A circular buffer abstraction class.
//!  
//! This template class provides a (write only) circular buffer abstraction with
//! selectable locking strategy. \n
//! If object lock is not necessary, use a yat::NullMutex type (default value in 
//! the template definition), for example : 
//! \verbatim myBuffer = new CircularBuffer<mySimpleObjectType>(10); // defines a "simple" object buffer \endverbatim
//! 
//! If object lock is necessary, use a mutex type, for example :
//! \verbatim myBuffer = new CircularBuffer<mySharedObjectType, yat::Mutex>(10); // defines a shared object buffer \endverbatim
//! 
//! Implementation constraint: <operator=> must be defined for template parameter \<T\>.
// ============================================================================
template <typename T, typename L = yat::NullMutex>
class CircularBuffer
{
public:
  //! \brief Default constructor.
  //!
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  CircularBuffer ();

  //! \brief Constructor.
  //!
  //! \param capacity Maximum number of elements of type \<T\>
  //! that can be stored into the buffer.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  CircularBuffer (size_t capacity);
  
  //! \brief Destructor. 
  //!
  //! Releases resources.
  virtual ~CircularBuffer ();

  //! \brief Clears buffer's content.
  virtual void clear ();

  //! \brief Fills the buffer with the specified value of type \<T\>.
  //!
  //! \param val The specified value.
  void fill (const T& val);
  
  //! \brief Pushes the specified data into the circular buffer.
  //!
  //! Push is ignored if data buffer is frozen.
  //! \exception PROGRAMMING_ERROR Thrown when circular buffer is not 
  //! initialized properly (ex: capacity set to 0).
  void push (T new_element);

  //! \brief Freezes the buffer.
  //!
  //! Any data pushed into a frozen circular buffer is silently ignored.
  void freeze ();

  //! \brief Unfreezes the buffer.
  //!
  //! Data pushed into a frozen circular buffer is silently ignored (see CircularBuffer::freeze).
  void unfreeze ();

  //! \brief Returns the "chronologically ordered" circular buffer's content.
  //!
  //! \exception INTERNAL_ERROR Thrown on unexpected buffer size.
  const yat::Buffer<T> & ordered_data ();

  //! \brief Sets the buffer capacity to the specified value.
  //! 
  //! \param capacity New maximum number of elements of type \<T\> stored in the buffer.
  //! \exception OUT_OF_MEMORY Thrown if memory allocation fails.
  virtual void capacity (size_t capacity);

private:
  //- Locling stategy.
  L lock_;
  
  //- The write pointer.
  T * wp_; 

  //- Frozen flag.
  bool frozen_;

  //- The main buffer.
  yat::Buffer<T> data_;
  
  //- The chronologically ordered buffer.
  yat::Buffer<T> ordered_data_;

  //- Number of cycles.
  unsigned long num_cycles_;

  //- = Disallow these operations.
  //--------------------------------------------
  CircularBuffer& operator= (const CircularBuffer&);
  CircularBuffer(const CircularBuffer&);
};

} // namespace

#if defined (YAT_INLINE_IMPL)
# include <yat/memory/DataBuffer.i>
#endif // YAT_INLINE_IMPL

#include <yat/memory/DataBuffer.tpp>

#endif // _DATA_BUFFER_H_



