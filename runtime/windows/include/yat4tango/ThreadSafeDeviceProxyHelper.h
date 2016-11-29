//----------------------------------------------------------------------------
// Copyright (c) 2004-2016 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// YAT4Tango LIBRARY
//----------------------------------------------------------------------------
//
// Copyright (C) 2006-2009  The Tango Community
//
// The YAT4Tango library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as published 
// by the Free Software Foundation; either version 2 of the License, or (at 
// your option) any later version.
//
// The YAT4Tango library is distributed in the hope that it will be useful,
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

// =============================================================================
// WIN32 SPECIFIC
// =============================================================================
#if defined (WIN32)
# pragma warning(disable:4786)
#endif

#ifndef _THREAD_SAFE_DEVICE_PROXY_HELPER_H_
#define _THREAD_SAFE_DEVICE_PROXY_HELPER_H_

//==============================================================================
// DEPENDENCIES
//==============================================================================
#include <tango/TangoExceptionsHelper.h>
#include <yat4tango/ThreadSafeDeviceProxy.h>

// ==============================================================================
//! \name MACRO for manipulation of Device data.
//@{ 
// ==============================================================================
#define FULL_CMD_NAME(C) device_proxy_->dev_name() + "::" + C
#define FULL_ATTR_NAME(A) device_proxy_->dev_name() + "/" + A

//	Definition of macros to benefit from expansion of __FILE__ and __LINE__ keywords in Device caller source file
#define read_attribute(ATTR_NAME, VALUE) internal_read_attribute (ATTR_NAME, VALUE, __FILE__, __LINE__)
#define read_attribute_w(ATTR_NAME, VALUE) internal_read_attribute_w (ATTR_NAME, VALUE, __FILE__, __LINE__)
#define write_attribute(ATTR_NAME, VALUE) internal_write_attribute (ATTR_NAME, VALUE, __FILE__, __LINE__)
#define command(CMD_NAME) internal_command (CMD_NAME,  __FILE__, __LINE__)

// For VC++ 6 VA_ARG macro is not supported so ==>, we cannot override command_out functions
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_out internal_command_out
#else	// For compilers that support variable number of arguments
#define command_out(CMD_NAME, OUT, ...) internal_command_out (CMD_NAME, OUT, ## __VA_ARGS__, __FILE__, __LINE__ )
#endif

// For VC++ 6 VA_ARG macro is not supported so ==>, we cannot override command_in functions
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_in internal_command_in
#else	// For compilers that support variable number of arguments
#define command_in(CMD_NAME, IN, ...) internal_command_in (CMD_NAME, IN, ## __VA_ARGS__, __FILE__, __LINE__ )
#endif

// For VC++ 6 VA_ARG macro is not supported so ==>, we cannot override command_inout functions
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#else	// For compilers that support variable number of arguments
#define command_inout(CMD_NAME,  ...) internal_command_inout (CMD_NAME,  ## __VA_ARGS__, __FILE__, __LINE__ )
#endif
//@}

namespace yat4tango
{
	// ============================================================================
	//! \class HelperBase 
	//! \brief The ThreadSafeDeviceProxy helper base class. -- \b Obsolete class since Tango 7 --
	//!
	//! \remark Since Tango 7 (resp. 8), the Tango::DeviceProxy class is fully (resp. optionally)
	//! thread safe.
	//! 
	//! This class provides a helper implementation for ThreadSafeDeviceProxy usage.
	//! It inherits from the concrete helpers (CommandInOutHelper and AttributeHelper).
	//!
	//! Inherits from Tango::LogAdapter class (for logging management).
	//!
	// ============================================================================
	class YAT4TANGO_EXPORT HelperBase : public Tango::LogAdapter
	{
	public:
		
		//! \brief Gets the Device proxy.
		//!
		//! Returns the underlying Device proxy.
		inline Tango::DeviceProxy& get_device_proxy (void)
		{
			return this->device_proxy_->unsafe_proxy();
		}
		
		//! \brief Operator->.
		//!
		//! Returns the underlying Device proxy.
		inline Tango::DeviceProxy& operator-> (void)
		{
			return this->device_proxy_->unsafe_proxy();
		}
	protected:
		
		//! \brief Constructor.
		//! 
		//! \param device_name The name of the target Device.
		//! \param client_device Reference to the client Device (for logging purpose).
		HelperBase (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
			: Tango::LogAdapter(client_device), device_proxy_(0)
		{
			_DEV_TRY_REACTION
				(
				 //- try
				 device_proxy_ = new ThreadSafeDeviceProxy(const_cast<std::string&>(device_name)),
				 //- what do we tried
				 std::string("new ThreadSafeDeviceProxy on ") + device_name,
				 //- origin
				 "HelperBase::HelperBase",
				 //- reaction
				 if (device_proxy_) {delete device_proxy_; device_proxy_ = 0;}
				);
		}
		
		//! \brief Destructor.
		virtual ~HelperBase ()
		{
			if (device_proxy_)
			{
				delete device_proxy_;
				device_proxy_ = 0;
			}
		}
		
		//! The underlying device proxy.
		adtb::ThreadSafeDeviceProxy * device_proxy_;
	};
	
	// ============================================================================
	//! \class CommandInOutHelper 
	//! \brief The ThreadSafeDeviceProxy helper for a Device command. -- \b Obsolete 
	//! class since Tango 7 --
	//!
	//! \remark Since Tango 7 (resp. 8), the Tango::DeviceProxy class is fully 
	//! (resp. optionally) thread safe.
	//!
	//! This class provides a helper implementation for a Device command usage.
	//! Some examples of ThreadSafeDeviceProxy usage are given here: 
	//! \ref ThreadSafe "Thread safe device proxy examples"
	//! 
	//! Inherits from yat4tango::HelperBase class.
	//!
	// ============================================================================
	class YAT4TANGO_EXPORT CommandInOutHelper : public virtual HelperBase
	{
	public:
		
		//! \brief Constructor.
		//!
		//! \param device_name The name of the target Device.
		//! \param client_device Reference to the client device (for logging purpose).		
		CommandInOutHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
			: HelperBase(device_name, client_device)
		{
			dd_out_.exceptions(Tango::DeviceData::isempty_flag | Tango::DeviceData::wrongtype_flag);
		}
		
		//! \brief Destructor.		
		virtual ~CommandInOutHelper ()
		{
			//- noop dtor
		}
		
		//---------------------------------------------------------------------------
		//! \brief Executes a command with no input and no output argument.
		//!
		//! Executes a DEV_VOID/DEV_VOID Tango command on the underlying Device.
		//! \param cmd_name The command's name.
		//! \param file File extension (predefined system MACRO).
        //! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif	
		void internal_command (const std::string& cmd_name,
			                     std::string file,
			                     int line)
		{
			if (this->device_proxy_)
			{
				_DEV_TRY_FILE_LINE
					(
  					 //- try
  					 (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name)),
  					 //- what do we tried
  					 std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					 //- origin
  					 "CommandInOutHelper::command",
  					 file,
  					 line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//! \brief Executes a command with input and output arguments.
		//!
		//! Executes a \<_OUT\>/\<_IN\> Tango command on the underlying Device.
		//! - Template parameter \<_IN\> is the generic type for argin; must be supported 
		//! by DeviceData::operator<<.
		//! - Template parameter \<_OUT\> is the generic type for argout; must be supported 
		//! by DeviceData::operator>>.
		//!
		//! \param cmd_name The command's name.
		//! \param argin Input argument.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _IN, class _OUT>
			void internal_command_inout (const std::string& cmd_name, 
                                   const _IN& argin, 
                                   _OUT& argout, 
                                   std::string file = __FILE__, 
                                   int line= __LINE__)
		{
			if (device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in << const_cast<_IN&>(argin);
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
  					//- what do we tried
  					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_inout",
  					file,
  					line
					);
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_ >> argout,
  					//- what do we tried
  					std::string("DeviceData::operator>> on data returned by ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_inout",
  					file,
  					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif				
		//---------------------------------------------------------------------------
		//! \brief Executes a command with input and output arguments.
		//!
		//! Executes a DevVar\<X\>StringArray/DevVar\<X\>StringArray Tango command on the 
		//! underlying Device.
		//! - Template parameter \<_IN\> is the generic type for numerical part of argin; 
		//! must be supported by DeviceData::insert().
		//! - Template parameter \<_OUT\> is the generic type for numerical part of argout; 
		//! must be supported by DeviceData::extract().
		//!
		//! \param cmd_name The command's name.
		//! \param _nv_in Numerical part of the input DevVar\<X\>StringArray.
		//! \param _sv_in String part of the input DevVar\<X\>StringArray.
		//! \param _nv_out Numerical part of the output DevVar\<X\>StringArray.
		//! \param _sv_out String part of the output DevVar\<X\>StringArray.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _IN, class _OUT>
		void internal_command_inout (const std::string& cmd_name,
                              	 const std::vector<_IN>& _nv_in,
                              	 const std::vector<std::string>& _sv_in,
                              	 std::vector<_OUT>& _nv_out,
                              	 std::vector<std::string>& _sv_out,
                              	 std::string file= __FILE__,
                              	 int line= __LINE__)
		{
			if (this->device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in.insert(const_cast<std::vector<_IN>&>(_nv_in), 
					           const_cast<std::vector<std::string>&>(_sv_in));
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
  					//- what do we tried
  					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_inout",
  					file,
  					line
					);
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_.extract(_nv_out, _sv_out),
  					//- what do we tried
  					std::string("DeviceData::extract on data returned by ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_inout",
  					file,
  					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarDoubleStringArray*.
		//!
		//! \param cmd_name The command's name.
		//! \param argin Input argument.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _IN>
			void internal_command_inout (const std::string& cmd_name, 
                                   const _IN& argin,
                                   Tango::DevVarDoubleStringArray* argout,
                                   std::string file = __FILE__,
                                   int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}

		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarLongStringArray*.
		//!
		//! \param cmd_name The command's name.
		//! \param argin Input argument.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _IN>
		void internal_command_inout (const std::string& cmd_name, 
                                 const _IN& argin, 
                                 Tango::DevVarLongStringArray* argout,
                                 std::string file = __FILE__, 
                                 int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}

		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarDoubleStringArray&.
		//!
		//! \param cmd_name The command's name.
		//! \param argin Input argument.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _IN>
		void internal_command_inout (const std::string& cmd_name,
                                 const _IN& argin, 
                                 Tango::DevVarDoubleStringArray& argout,
                                 std::string file = __FILE__,
                                 int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}
		
		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarLongStringArray&.
		//!
		//! \param cmd_name The command's name.
		//! \param argin Input argument.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _IN>
		void internal_command_inout (const std::string& cmd_name, 
                                 const _IN& argin, 
                                 Tango::DevVarLongStringArray& argout,
                                 std::string file= __FILE__, 
                                 int line= __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
			                             	 static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}


		//---------------------------------------------------------------------------
		//! \brief Executes a command with input argument.
		//!
		//! Executes a DEV_VOID/ARG_IN Tango command on the underlying Device.
		//! - Template parameter \<_IN\> is the generic type argin; 
		//! must be supported by DeviceData::operator<<.
		//!
		//! \param cmd_name The command's name.
		//! \param argin Input argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion
#endif
		template <class _IN>
		void internal_command_in (const std::string& cmd_name,
                              const _IN& argin,
                              std::string file = __FILE__,
                              int line = __LINE__)
		{
			if (this->device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in << const_cast<_IN&>(argin);
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					(device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
  					//- what do we tried
  					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_in",
  					file,
  					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//! \brief Executes a command with input argument.
		//!
		//! Executes a DEV_VOID/DevVar\<X\>StringArray Tango command on the underlying Device.
		//! - Template parameter \<_IN\> is the generic type for numerical part of argin; 
		//! must be supported by DeviceData::insert().
		//!
		//! \param cmd_name The command's name.
		//! \param _nv_in Numerical part of the input DevVar\<X\>StringArray.
		//! \param _sv_in String part of the input DevVar\<X\>StringArray.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _IN>
			void internal_command_in (const std::string& cmd_name, 
                          			const std::vector<_IN>& _nv_in, 
                          			const std::vector<std::string>& _sv_in, 
                                std::string file = __FILE__,
                                int line = __LINE__)
		{
			if (this->device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in.insert(const_cast<std::vector<_IN>&>(_nv_in), 
					           const_cast<std::vector<std::string>&>(_sv_in));
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					(device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
  					//- what do we tried
  					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_in",
  					file,
  					line
					);
			}
		}
		
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//! \brief Executes a command with output argument.
		//!
		//! Executes a ARG_OUT/DEV_VOID Tango command on the underlying Device.
		//! - Template parameter \<_OUT\> is the generic type for argout; must be 
		//! supported by DeviceData::operator>>.
		//!
		//! \param cmd_name The command's name.
		//! \param argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _OUT>
			void internal_command_out (const std::string& cmd_name, 
                                 _OUT& argout,  
                                 std::string file = __FILE__, 
                                 int line = __LINE__)
		{
			if (this->device_proxy_)
			{
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name)),
  					//- what do we tried
  					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_out",
  					file,
  					line
					);
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_ >> argout,
  					//- what do we tried
  					std::string("DeviceData::operator>> on data returned by ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_out",
  					file,
  					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		
		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarDoubleStringArray*.
		//!
		//! \param dummy Used to have same number of parameter for the command_out methods.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _OUT>  
			void internal_command_out(_OUT dummy, 
                                Tango::DevVarDoubleStringArray* argout,
                                std::string file = __FILE__,
                                int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends;
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"),
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}

		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarLongStringArray*.
		//!
		//! \param dummy Used to have same number of parameter for the command_out methods.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _OUT>    
			void internal_command_out (_OUT dummy, 
                                 Tango::DevVarLongStringArray* argout,
                                 std::string file = __FILE__, 
                                 int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}

		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarDoubleStringArray&.
		//!
		//! \param dummy Used to have same number of parameter for the command_out methods.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
		template <class _OUT> 
			void internal_command_out(_OUT dummy, 
                                Tango::DevVarDoubleStringArray& argout,
                                std::string file = __FILE__, 
                                int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}

		//---------------------------------------------------------------------------
		//! \brief Overloaded command to avoid usage of DevVar\<X\>Array for argout.
		//!
		//! Implementation for argout type: DevVarLongStringArray&.
		//!
		//! \param dummy Used to have same number of parameter for the command_out methods.
		//! \param[out] argout Output argument.
		//! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------

		template <class _OUT> 
			void internal_command_out (_OUT dummy, 
                                 Tango::DevVarLongStringArray& argout,
                                 std::string file = __FILE__, 
                                 int line = __LINE__)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o;
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				                             static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"),
				                             static_cast<const char*>(o.str().c_str()));
		}

		//---------------------------------------------------------------------------
		//! \brief Executes a command with output argument.
		//!
		//! Executes a DEV_VOID/DevVar\<X\>StringArray Tango command on the underlying Device.
		//! - Template parameter \<_OUT\> is the generic type for argout; must be 
		//! supported by DeviceData::extract().
		//!
		//! \param cmd_name The command's name.
		//! \param _nv_out Numerical part of the output DevVar\<X\>StringArray.
		//! \param _sv_out String part of the output DevVar\<X\>StringArray.
        //! \param file File extension (predefined system MACRO).
		//! \param line Line number extension (predefined system MACRO).
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _OUT>
		void internal_command_out (const std::string& cmd_name,
                          		 std::vector<_OUT>& _nv_out,
                          		 std::vector<std::string>& _sv_out,
                          		 std::string file = __FILE__,
                          		 int line = __LINE__)
		{
			if (this->device_proxy_)
			{
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name)),
  					//- what do we tried
  					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_out",
  					file,
  					line
					);
				_DEV_TRY_FILE_LINE
					(
  					//- try
  					dd_out_.extract(_nv_out, _sv_out),
  					//- what do we tried
  					std::string("DeviceData::extract on data returned by ") + FULL_CMD_NAME(cmd_name),
  					//- origin
  					"CommandInOutHelper::command_out" ,
  					file,
  					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif

private:
	//- placed here as a workaround due to CORBA::any_var limitations
	Tango::DeviceData dd_out_;
};


// ============================================================================
//! \class AttributeHelper 
//! \brief The ThreadSafeDeviceProxy helper for a Device attribute. -- \b Obsolete 
//! class since Tango 7 --
//!
//! \remark Since Tango 7 (resp. 8), the Tango::DeviceProxy class is fully (resp. optionally)
//! thread safe.
//!
//! This class provides a helper implementation for a Device attribute usage.
//! Some examples of ThreadSafeDeviceProxy usage are given here: 
//! \ref ThreadSafe "Thread safe device proxy examples"
//! 
//! Inherits from yat4tango::HelperBase class.
//!
// ============================================================================
class YAT4TANGO_EXPORT AttributeHelper : public virtual HelperBase
{
public:
	
	//! \brief Constructor.
	//!
	//! \param device_name Name of the target Device.
	//! \param client_device Reference to the client Device (for logging purpose).
	AttributeHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
		: HelperBase(device_name, client_device)
	{
		da_out_.exceptions(Tango::DeviceAttribute::isempty_flag | Tango::DeviceAttribute::wrongtype_flag);
	}
	
	//! \brief Destructor.
	virtual ~AttributeHelper ()
	{
		//- noop dtor
	}

	//! \brief Gets Device's attribute.
	//!
	//! Returns the last read Device attribute.
	Tango::DeviceAttribute get_device_attribute () 
	{
		return da_out_;
	}

	//---------------------------------------------------------------------------
	//! \brief Writes the specified attribute.
	//!
	//! Writes the specified value in the specified attribute.
	//! - Template parameter \<_VAL\> is the generic type for attribute's value; 
	//! must be supported by DeviceAttribute::operator<<.
	//!
	//! \param attr_name The attribute's name.
	//! \param attr_value The attribute's value.
	//! \param file File extension (predefined system MACRO).
	//! \param line Line number extension (predefined system MACRO).
	//---------------------------------------------------------------------------
	template <class _VAL>
	void internal_write_attribute (const std::string& attr_name, 
                                 const _VAL& attr_value,
                                 std::string file = __FILE__,
                                 int line = __LINE__)
	{
		if (this->device_proxy_)
		{
			Tango::DeviceAttribute da;
			da.set_name(const_cast<std::string&>(attr_name));
			da << const_cast<_VAL&>(attr_value);
			_DEV_TRY_FILE_LINE
				(
  				//- try
  				(device_proxy_->write_attribute)(da),
  				//- what do we tried
  				std::string("write_attribute on ") + FULL_ATTR_NAME(attr_name),
  				//- origin
  				"AttributeHelper::write_attribute",
  				file,
  				line
				);
		}
	}

	//---------------------------------------------------------------------------
	//! \brief Reads the specified attribute.
	//!
	//! - Template parameter \<_VAL\> is the generic type for attribute's value; 
	//! must be supported by DeviceAttribute::operator>>.
	//!
	//! \param attr_name The attribute's name.
	//! \param[out] attr_value The read attribute's value.
	//! \param file File extension (predefined system MACRO).
	//! \param line Line number extension (predefined system MACRO).
	//---------------------------------------------------------------------------
	template <class _VAL>
	void internal_read_attribute (const std::string& attr_name,
                                _VAL& attr_value,
                                std::string file = __FILE__,
                                int line = __LINE__)
	{
		if (this->device_proxy_)
		{
			_DEV_TRY_FILE_LINE
				(
  				//- try
  				da_out_ = (device_proxy_->read_attribute)(const_cast<std::string&>(attr_name)),
  				//- what do we tried
  				std::string("read_attribute on ") + FULL_ATTR_NAME(attr_name),
  				//- origin
  				"AttributeHelper::read_attribute", 
  				file,
  				line
				);
			_DEV_TRY_FILE_LINE
				(
  				//- try
  				da_out_ >> attr_value,
  				//- what do we tried
  				std::string("DeviceAttribute::operator>> on data returned by ") + FULL_ATTR_NAME(attr_name),
  				//- origin
  				"AttributeHelper::read_attribute", 
  				file,
  				line
				);
		}
	}

	//---------------------------------------------------------------------------
	//! \brief Reads the specified attribute and gets its write value.
	//!
	//! - Template parameter \<_VAL\> is the generic type for write attribute's value; 
	//! must be supported by DeviceAttribute::operator>>.
	//!
	//! \param attr_name The attribute's name.
	//! \param[out] w_attr_value The write attribute's value.
	//! \param file File extension (predefined system MACRO).
	//! \param line Line number extension (predefined system MACRO).
	//---------------------------------------------------------------------------
	template <class _VAL>
		void internal_read_attribute_w (const std::string& attr_name, 
                                    _VAL& w_attr_value,
                                    std::string file = __FILE__,
                                    int line = __LINE__)
	{
		if (this->device_proxy_)
		{
			_DEV_TRY_FILE_LINE
				(
  				//- try
  				da_out_ = (device_proxy_->read_attribute)(const_cast<std::string&>(attr_name)),
  				//- what do we tried
  				std::string("read_attribute on ") + FULL_ATTR_NAME(attr_name),
  				//- origin
  				"AttributeHelper::read_attribute_w", 
  				file,
  				line
				);
			//- create an AttributeProxy to get the type of the attribute
			_DEV_TRY_FILE_LINE
				(
  				//- try
  				attr_proxy_ = new Tango::AttributeProxy(FULL_ATTR_NAME(attr_name)),
  				//- what do we tried
  				std::string("new Tango::AttributeProxy : ") + FULL_ATTR_NAME(attr_name),
  				//- origin
  				"AttributeHelper::read_attribute_w", 
  				file,
  				line
				);
			//- Switch on attribute type (to be completed with all tango types!)
			switch(attr_proxy_->get_config().data_type)
			{
		
			case 2://short
				{
					w_attr_value = da_out_.ShortSeq[1];
					break;
				}	
			case 3://long
				{
					w_attr_value = da_out_.LongSeq[1];
					break;
				}
			case 5://double
				{
					w_attr_value = da_out_.DoubleSeq[1];
					break;
				}
			//case 8:string ...
			}
			//- Delete the attribute proxy
			if(attr_proxy_)
			{
				delete attr_proxy_;
				attr_proxy_ = 0;
			}
		}
	}
	
private:
	//- placed here as a workaround due to CORBA::any_var limitations
	Tango::DeviceAttribute da_out_;
	Tango::AttributeProxy* attr_proxy_;
};

// ============================================================================
//! \class ThreadSafeDeviceProxyHelper 
//! \brief The ThreadSafeDeviceProxy helper for Device attributes and commands. -- \b Obsolete 
//! class since Tango 7 --
//! 
//! \remark Since Tango 7 (resp. 8), the Tango::DeviceProxy class is fully (resp. optionally)
//! thread safe.
//!
//! This class provides a helper implementation for Device attributes and
//! commands usage.
//! Some examples of ThreadSafeDeviceProxy usage are given here: 
//! \ref ThreadSafe "Thread safe device proxy examples"
//! 
//! Inherits from CommandInOutHelper (for the commands helper) and AttributeHelper
//! (for the attributes helper) classes.
//!
// ============================================================================
class YAT4TANGO_EXPORT ThreadSafeDeviceProxyHelper : public CommandInOutHelper, public AttributeHelper
{
public:
	
	//! \brief Constructor.
	//!
	//! \param target_device_name Name of the target Device.
	//! \param host_device Reference to the client Device (for logging purpose).
	ThreadSafeDeviceProxyHelper (const std::string& target_device_name, Tango::DeviceImpl *host_device = 0)
		: HelperBase(target_device_name, host_device),
		  CommandInOutHelper(target_device_name, host_device),
		  AttributeHelper(target_device_name, host_device)
	{
		//- noop ctor
	}
	
	//! \brief Destructor.
	virtual ~ThreadSafeDeviceProxyHelper ()
	{
		//- noop dtor
	}
};

} //- namespace yat4tango

#endif // _THREAD_SAFE_DEVICE_PROXY_HELPER_H_
