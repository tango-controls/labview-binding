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

#ifndef __YAT_FILE_NAME_H__
#define __YAT_FILE_NAME_H__

#include <list>

#include <yat/CommonHeader.h>
#include <yat/utils/String.h>
#include <yat/utils/Callback.h>
#include <yat/memory/SharedPtr.h>
#include <yat/memory/UniquePtr.h>
#include <yat/memory/MemBuf.h>
#include <yat/threading/Task.h>
#include <yat/threading/Pulser.h>
#include <yat/time/Time.h>

#if defined(_MSC_VER)
  #include <sys\utime.h>
#else
  #include <utime.h>
#endif

#include <sys/types.h>

#if defined YAT_WIN32 && !defined(_CSTDIO_)
  #include <cstdio>
#endif

#ifdef YAT_LINUX
  #include <dirent.h>
  #include <unistd.h>
  #include <sys/stat.h>
  #include <errno.h>
  #include <sys/vfs.h>
#elif defined YAT_WIN32
  #include <sys\stat.h>
  #include <ctype.h>
  #include <direct.h>
  #include <errno.h>
  #include <windows.h>
#elif defined YAT_MACOSX 
  #include <errno.h>
  #include <sys/stat.h>
  #include <sys/param.h>
  #include <sys/mount.h>
#endif

#include <map>

namespace yat
{

#define SEP_PATHDOS     '\\'
#define SEP_PATHUNIX    '/'
#ifdef YAT_WIN32
  #define SEP_PATH      SEP_PATHDOS
#else
  #define SEP_PATH      SEP_PATHUNIX
#endif
#define SEP_EXT         '.'
#define SEP_DSK         ':'

#ifndef _MAX_PATH
  #define _MAX_PATH 1024
#endif


//! %File error message
const char ERR_CANNOT_CREATE_FOLDER[]     = "Cannot create directory '%s'";
//! %File error message
const char ERR_CANNOT_CREATE_LINK[]       = "Cannot create link; name = '%s', target='%s'";
//! %File error message
const char ERR_CANNOT_ENUM_DIR[]          = "Cannot enumerate directory '%s'";
//! %File error message
const char ERR_CANNOT_REMOVE_FILE[]       = "Cannot remove file '%s'";
//! %File error message
const char ERR_CANNOT_FETCH_INFO[]        = "Cannot fetch informations for file '%s'";
//! %File error message
const char ERR_CANNOT_RENAME_FILE[]       = "Cannot rename file '%s'";
//! %File error message
const char ERR_FILE_NOT_FOUND[]           = "File '%s' not found";
//! %File error message
const char ERR_DIR_NOT_FOUND[]            = "Directory '%s' not found";
//! %File error message
const char ERR_COPY_FAILED[]              = "A error has occured while copying from '%s' to '%s'";
//! %File error message
const char ERR_OPEN_FILE[]                = "Cannot open file '%s'";
//! %File error message
const char ERR_CANNOT_CREATE_WIN32[]      = "Cannot get handle for '%s' (Win32 API)";
//! %File error message
const char ERR_CANNOT_CHANGE_FILE_TIME[]  = "Cannot change file time for '%s'";
//! %File error message
const char ERR_CANNOT_GET_FILE_TIME[]     = "Cannot get file time for '%s'";
//! %File error message
const char ERR_READING_FILE[]             = "Error while reading file '%s'";
//! %File error message
const char ERR_WRITING_FILE[]             = "Error while writing file '%s'";
//! %File error message
const char ERR_STAT_FAILED[]              = "Cannot get informations about file '%s'";
//! %File error message
const char ERR_CHMOD_FAILED[]             = "Cannot change access for '%s' to '%o'";
//! %File error message
const char ERR_CHOWN_FAILED[]             = "Cannot change owner for '%s' to %d:%d";
//! %File error message
const char ERR_FSTYPE[]                   = "Error gathering file system information on '%s'";
//! %File error message
const char ERR_NOT_A_DIRECTORY[]          = "Is not a directory";
//! %File error message
const char ERR_DELETE_DIRECTORY[]         = "Cannot delete directory '%s'";
//! %File error message
const char ERR_BAD_DEST_PATH[]            = "Bad destination path '%s'";
//! %File error message
const char ERR_TEST_LINK[]                = "Cannot check entry '%s'";

//! Begining of cygwin absolute file names
const char FILE_CYGDRIVE[] = "\\cygdrive\\";


//=============================================================================
// Free function
//=============================================================================

//! \brief Makes a symbolic link on a file.
//! \param strLink Created link.
//! \param strTarget The file to link to.
int MakeLink(const std::string& strLink, const std::string& strTarget);


// ============================================================================
//! \class IOException 
//! \brief Predefined I/O exception (reason: IO_ERROR).
//!
//! Inherits from yat::Exception class.
// ============================================================================
class YAT_DECL IOException : public Exception
{
public:
  //! \brief Constructor.
  //! \param pcszDesc Exception description.
  //! \param pcszOrigin Exception origin.
  IOException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("IO_ERROR", pcszDesc, pcszOrigin)
  {  }
};

// ============================================================================
//! \class BadPathException 
//! \brief Predefined bad path exception (reason: BAD_PATH).
//!
//! Inherits from yat::Exception class.
// ============================================================================
class YAT_DECL BadPathException : public Exception
{
public:
  //! \brief Constructor.
  //! \param pcszDesc Exception description.
  //! \param pcszOrigin Exception origin.
  BadPathException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("BAD_PATH", pcszDesc, pcszOrigin)
  { }
};

// ============================================================================
//! \class BadPathConditionException 
//! \brief Predefined bad path conditions exception (reason: BAD_PATH_CONDITION).
//!
//! Current conditions don't allow to perform the action.\n
//! Inherits from yat::Exception class.
// ============================================================================
class YAT_DECL BadPathConditionException : public Exception
{
public:
  //! \brief Constructor.
  //! \param pcszDesc Exception description.
  //! \param pcszOrigin Exception origin.
  BadPathConditionException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("BAD_PATH_CONDITION", pcszDesc, pcszOrigin)
  { }
};

// ============================================================================
//! \class FileNotFoundException 
//! \brief Predefined file not found exception (reason: FILE_NOT_FOUND).
//!
//! Path or file not found.\n
//! Inherits from yat::Exception class.
// ============================================================================
class YAT_DECL FileNotFoundException : public Exception
{
public:
  //! \brief Constructor.
  //! \param pcszDesc Exception description.
  //! \param pcszOrigin Exception origin.
  FileNotFoundException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("FILE_NOT_FOUND", pcszDesc, pcszOrigin)
  { }
};

// ============================================================================
//! \class PermissionException 
//! \brief Predefined permission exception (reason: PERMISSION_ERROR).
//!
//! Permissions on file or path don't allow to perform the action.\n
//! Inherits from yat::Exception class.
// ============================================================================
class YAT_DECL PermissionException : public Exception
{
public:
  //! \brief Constructor.
  //! \param pcszDesc Exception description.
  //! \param pcszOrigin Exception origin.
  PermissionException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("PERMISSION_ERROR", pcszDesc, pcszOrigin)
  { }
};

// ============================================================================
//! \class BadDriveException 
//! \brief Predefined bad drive exception (reason: BAD_DRIVE).
//!
//! Bad drive name.\n
//! Inherits from yat::Exception class.
// ============================================================================
class YAT_DECL BadDriveException : public Exception
{
public:
  //! \brief Constructor.
  //! \param pcszDesc Exception description.
  //! \param pcszOrigin Exception origin.
  BadDriveException(const char *pcszDesc, const char *pcszOrigin):
  yat::Exception("BAD_DRIVE", pcszDesc, pcszOrigin)
  { }
};


//============================================================================
// Free function
//============================================================================

//! \brief Tests if specified character is a path separator.
//!
//! Returns 1 if \<c\> IS a path separator, 0 otherwise.
//! \param c Created link.
inline int IsSepPath(char c)
 { return c == SEP_PATHDOS || c == SEP_PATHUNIX; }

// ============================================================================
//! \class FileName 
//! \brief %File name manipulations.
//!
//! This class provides basic file manipulation functions, such as :
//! - file/path/link creation and access, 
//! - path conversion (DOS/UNIX), 
//! - file moving and removing,
//! - file information access and modification (date/time, owner, rights).
// ============================================================================
class YAT_DECL FileName
{
protected:
  //! %File name.
  std::string m_strFile;  

#ifdef YAT_LINUX
  static size_t s_copy_bloc_size;
#endif

public:
  
  //! \brief %File system types
  enum FSType
  {
    //! Defaut value for Microsoft partition type.
    MS    = 0,
    //- Microsoft partition types.
    //! Microsoft FAT16 partition type.
    FAT16 = 0x04,
    //! Microsoft FAT32 partition type.
    FAT32 = 0x0B,
    //! Microsoft IFS partition type.
    IFS   = 0x07,
    //- Unix file system types.
    //! Unix AFFS file system type.
    AFFS  = 0xADFF,
    //! Unix EFS file system type.
    EFS   = 0x00414A53,
    //! Unix EXT file system type.
    EXT   = 0x137D,
    //! Unix EXT2 file system type.
    EXT2  = 0xEF53,
    //! Unix HPFS file system type.
    HPFS  = 0xF995E849,
    //! Unix ISOFS file system type.
    ISOFS = 0x9660,
    //! Unix MSDOS file system type.
    MSDOS = 0x4d44,
    //! Unix NFS file system type.
    NFS   = 0x6969,
    //! Unix PROC file system type.
    PROC  = 0x9fa0,
    //! Unix SMB file system type.
    SMB   = 0x517B,
    //! Apple filesystem types (TODO).
    HFS   = 0xffff
  };

  //! \brief information about a path
  class Info
  {
  public:
    bool        is_exist;
    uint64      size;
    Time        mod_time;
    uid_t       uid;
    gid_t       gid;
    mode_t      mode;
    bool        is_file;
    bool        is_dir;
    bool        is_link;
    std::string link_to;

    Info() { clear(); }

    void clear()
    {
      is_exist = is_file = is_dir = is_link = false;
      uid = gid = 0;
      mode = 0;
      size = 0;
      link_to.clear();
    }
  };

  // ============================================================================
  //! \interface IProgress
  //! \brief progress indicator
  //!
  //! This class provides a method to notify about progression while performing
  //! copy operations
  // ============================================================================
  class IProgress
  {
  public:
    //! start notification
    virtual void on_start(std::string file_name, int64 total_bytes) = 0;
    //! progression notification
    virtual bool on_progress(std::string file_name, int64 total_bytes, int64 copied_bytes, double elapsed_secs) = 0;
    //! complete notification
    virtual void on_complete(std::string file_name, int64 total_bytes, double elapsed_secs) = 0;
  };
  
  //- Throws a "FILE_ERROR" yat::Exception.
  static void ThrowExceptionFromErrno(const char *pszError, const char *pszMethod);

private:
  
  //! progress notification target
  IProgress* m_progress_target_p;

public:
  //! \brief Default constructor.
  FileName(): m_progress_target_p(0)  { }

  //! \brief Constructor from file name.
  //! 
  //! The specified name can be a path name, a file name or a full path file name.
  //! \param strFileName %File name.
  FileName(const std::string& strFileName): m_progress_target_p(0) { set(strFileName); }

  //! \brief Constructor from file and path names.
  //! \param strPath Path name.
  //! \param strName %File name.
  FileName(const std::string& strPath, const std::string& strName): m_progress_target_p(0)  { set(strPath, strName); }

  //! \brief virtual d-tor
  virtual ~FileName() {}

  //! \brief Tests if filename is a path.
  //! 
  //! Returns true if the filename is a path, false otherwise.
  bool is_path_name() const;

  //! \brief Tests if filename is set.
  //! 
  //! Returns true if the filename is set, false otherwise.
  bool is_empty() const { return m_strFile.empty(); }

  //! \brief join path fragment with current path
  //! 
  void join(const std::string& fragment);

  //! \brief Tests if file name is an existing path.
  //! 
  //! Returns true if the file name is an existing path, false otherwise.
  bool path_exist() const;

  //! \brief Tests if file exists.
  //! 
  //! Returns true if file exists, false otherwise.
  bool file_exist() const;

  //! \brief Tests if file is accessible.
  //! 
  //! Returns true if file is accessible, false otherwise.
  bool file_access() const;

  //! \brief Tests if filename is a empty directory.
  //! 
  /// Return true if filename is a empty directory, false otherwise.
  bool is_empty_dir() const;
  
  //! \brief Sets full file name.
  //!
  //! Converts separators according to the current operating system (UNIX\<--\>DOS).
  //! \param strFullName Full file name.
  void set(const std::string& strFullName);

  //! \brief Sets file and path names.
  //!
  //! Converts separators according to the current operating system (UNIX\<--\>DOS).
  //! \param strPath Path name.
  //! \param strName %File name.
  void set(const std::string& strPath, const std::string& strName);

  //! \brief Sets file, path and extension names.
  //!
  //! Converts separators according to the current operating system (UNIX\<--\>DOS).
  //! \param strPath Path name.
  //! \param strName %File name.
  //! \param strExt %File extension name.
  void set(const std::string& strPath, const std::string& strName, const std::string& strExt);

  //! \brief Sets full file name.
  //!
  //! Converts separators according to the current operating system (UNIX\<--\>DOS).
  //! \param pszFileName Full file name.
  void set_full_name(pcsz pszFileName);

  //! \brief Converts separators according to the current operating system
  //! (UNIX\<--\>DOS).
  //! \param pstr String to convert (input\/output argument).
  static void convert_separators(std::string* pstr);

  //! \brief Gets access mode from a string in a UNIX like format (ie "rwxrwxrwx").
  //! \param strAccess Input format.
  //! \remark Not implemented for WINDOWS plateform.
  static mode_t access_from_string(const std::string& strAccess);

  //! \brief Gets full file name.
  const std::string& full_name() const { return m_strFile; }

  //! \brief Gets file path.
  std::string path() const;

  //! \brief Gets filename without path and extension.
  std::string name() const;

  //! \brief Gets directory name (last path component before file name).
  std::string dir_name() const;

  //! \brief Gets filename relative to specified path.
  std::string rel_name(const char* pszPath) const;

  //! \brief Gets filename (with its extension) without path.
  std::string name_ext() const;

  //! \brief Gets file extension.
  std::string ext() const;

  //! \brief Creates a directory with specified rights.
  //!
  //! Directory name is *this* path name.
  //! \param mode Directory permissions in a UNIX like format (ie "rwxrwxrwx").
  //! \param uid User identifier.
  //! \param gid Group identifier.
  //! \exception BAD_PATH Thrown if path format is not correct.
  //! \exception BAD_DRIVE Thrown if drive access fails.
  //! \exception FILE_ERROR Thrown if directory creation fails.
  void mkdir(mode_t mode = 0, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) const 
    throw(Exception);

  //! \brief Returns true if the file name is an existing link, false otherwise.
  //!
  //! \exception FILE_ERROR Thrown if file access fails.
  //! \remark Not implemented for WINDOWS plateform.
  bool link_exist() const 
    throw(Exception);

  //! \brief Creates a symbolic link on *this* file.
  //! \param strTarget Symbolic link to create.
  //! \param uid User identifier.
  //! \param gid Group identifier.
  //! \exception FILE_ERROR Thrown if link creation fails or if owner modification fails.
  //! \remark Not implemented for WINDOWS plateform.
  void make_sym_link(const std::string& strTarget, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) const
    throw(Exception);

  //! \brief Deletes file.
  //!
  //! \exception FILE_ERROR Thrown if file deleting fails.
  void remove() 
    throw(Exception);
  
  //! \brief Removes directory.
  //! \param bRecursive If set to false, deletes only files and symbolic links. If set to true, 
  //! deletes files, symbolic links and recursively all child directories.
  //! \param bContentOnly If set to false, deletes only directory content. If set to true, 
  //! deletes directory content and the directory itself.
  //! \exception FILE_ERROR Thrown if file or directory deleting fails.
  void rmdir(bool bRecursive=false, bool bContentOnly=false) 
    throw(Exception);

  //! \brief Copies a directory and its whole content inside the destination directory.
  //! \param strDest Destination directory.
  //! \param bCreateDir If set to false, the destination directory must already exists. If set
  //! to true, the destination directory is created.
  //! \param modeDir Directory permissions in a UNIX like format (ie "rwxrwxrwx").
  //! \param uid User identifier.
  //! \param gid Group identifier.
  //! \exception BAD_PATH Thrown if destination directory name format is not correct.
  void dir_copy(const std::string& strDest, bool bCreateDir=false, mode_t modeDir = 0, uid_t uid = (uid_t)-1, gid_t gid = (uid_t)-1) 
    throw(Exception);

  //! \brief Returns file size in bytes.
  //!
  //! \exception FILE_ERROR Thrown if file size access fails.
  uint32 size() const 
    throw(Exception);

  //! \brief Returns file size in bytes: 64bits version for files bigger than 2GB.
  //!
  //! \exception FILE_ERROR Thrown if file size access fails.
  uint64 size64() const 
    throw(Exception);
  
  //! \brief Renames the file.
  //! \param strNewName New file name.
  //! \exception FILE_ERROR Thrown if file renaming fails.
  void rename(const std::string& strNewName) 
    throw(Exception);
  
  //! \brief Copies the file to the specified destination.
  //! \param strDest Destination file.
  //! \param bKeepMetaData If set to true and uid is root, the copy keeps metadata 
  //! (access mode, owner & group). Metadata are not kept otherwise.
  //! \exception FILE_NOT_FOUND Thrown if *this* file doesn't exist.
  //! \exception FILE_ERROR Thrown if file copy fails.
  //! \todo implement the progress notifications on Windows using the CopyFileEx functions family
  void copy(const std::string& strDest, bool bKeepMetaData=false) 
    throw(Exception);
  
  //! \brief Moves file to specified destination.
  //! \param strDest Destination file or directory.
  //! \exception FILE_NOT_FOUND Thrown if *this* file doesn't exist.
  //! \exception FILE_ERROR Thrown if file moving fails.  
  void move(const std::string& strDest) 
    throw(Exception);

  //! \brief Returns file last modification date & time.
  //! \param pTm Last modification time (in yat::Time format).
  //! \param bLocalTime If set to true, converts date & time to local time.
  //! \exception FILE_ERROR Thrown if file information access fails.
  //! Specific WIN32 error : file handle creation fails.
  void mod_time(Time *pTm, bool bLocalTime=false, bool stat_link=false) const 
    throw(Exception);

  //! \brief Sets file last modification date & time.
  //! \param tm Last modification time to set (in yat::Time format).
  //! \exception FILE_ERROR Thrown if file time modification fails.
  //! Specific WIN32 error : file handle creation fails.
  void set_mod_time(const Time& tm) const 
    throw(Exception);

  //! \brief get information about the current path
  //! \param info_p FileName::Info class
  void info( Info* info_p, bool follow_link=true ) const;

  //! \brief Changes file rights.
  //! \param mode %File rights in UNIX like format (ie "rwxrwxrwx").
  //! \exception FILE_ERROR Thrown if file rights modification fails.
  //! \remark Not implemented for WINDOWS plateform.
  void chmod(mode_t mode) 
    throw(Exception);

  //! \brief Recursively changes rights for directory and files.
  //! \param modeFile New file right in UNIX like format (ie "rwxrwxrwx").
  //! \param modeDir New directory rights in UNIX like format (ie "rwxrwxrwx").
  //! \param bCurrentLevel If set to true, changes mode to directory itself.
  //! \exception FILE_NOT_FOUND Thrown if directory doesn't exist.
  //! \exception FILE_ERROR Thrown if file or directory rights modification fails.
  void recursive_chmod(mode_t modeFile, mode_t modeDir, bool bCurrentLevel=true) 
    throw(Exception);

  //! \brief Recursively changes directory rights.
  //!
  //! Also changes the rights of the directory itself.
  //! \param mode New directory rights in UNIX like format (ie "rwxrwxrwx").
  //! \exception FILE_NOT_FOUND Thrown if directory doesn't exist.
  //! \exception FILE_ERROR Thrown if directory rights modification fails.
  void recursive_chmod_dir(mode_t mode) 
    throw(Exception);

  //! \brief Recursively changes file rights.
  //! \param mode New file rights in UNIX like format (ie "rwxrwxrwx").
  //! \exception FILE_NOT_FOUND Thrown if file doesn't exist.
  //! \exception FILE_ERROR Thrown if file rights modification fails.
  void recursive_chmod_file(mode_t mode) 
    throw(Exception);

  //! \brief Changes file owner.
  //! \param uid New user identifier.
  //! \param gid New group identifier.
  //! \exception FILE_ERROR Thrown if file owner modification fails.
  //! \remark Not implemented for WINDOWS plateform.
  void chown(uid_t uid, gid_t gid = (uid_t)-1) 
    throw(Exception);

  //! \brief Recursively changes file owner.
  //!
  //! Also changes the directories owner.
  //! \param uid New user identifier.
  //! \param gid New group identifier.
  //! \exception FILE_NOT_FOUND Thrown if file or directory doesn't exist.
  //! \exception FILE_ERROR Thrown if file or directory owner modification fails.
  void recursive_chown(uid_t uid, gid_t gid = (uid_t)-1) 
    throw(Exception);

  //! \brief Returns the filesystem type.
  //!
  //! The filesystem type is defined as a yat::FSType enumeration value.
  //! \exception FILE_ERROR Thrown if cannot access to filesystem information.
  //! \remark Not implemented for WINDOWS plateform.
  FSType file_system_type() const 
    throw(Exception);

  //! \brief Returns the filesystem identifier.
  //!
  //! \exception FILE_ERROR Thrown if cannot access to filesystem information.
  //! \remark Not implemented for WINDOWS plateform.
  fsid_t file_system_id() const 
    throw(Exception);

  //! \brief Try to create a r-lock
  void try_r_lock(bool async) const;

  //! Sets the bloc size (in bytes) for file copy operations.
  //! \param size Bloc size in bytes.
  static void set_copy_bloc_size(size_t size);
  
  //! Sets the progress notification target
  //! \param target_p pointer to the target
  void set_progress_target(IProgress* target_p) { m_progress_target_p = target_p; }
};

// ============================================================================
//! \class File 
//! \brief Temporary file class.
//!
//! This class provides basic file manipulation functions for a temporary file.
//! Inherits from FileName class.
// ============================================================================
class YAT_DECL File: public FileName
{
  public:
  //! \brief Default constructor.
  File()
  { }

  //! \brief Constructor from full file name.
  //! \param strFileName %File name.
  File(const std::string& strFileName)
  { set(strFileName); }

  //! \brief Constructor from path and file name. 
  //! \param strPath Path name.
  //! \param strName %File name.
  File(const std::string& strPath, const std::string& strName)
  { set(strPath, strName); }

  //! \brief Loads the specified MemBuf object with file content.
  //!
  //! \<pMemBuf\> must have been allocated by the caller.
  //! \param pMemBuf Pointer to the MemBuf object to load.
  //! \exception FILE_ERROR Thrown if file opening or reading fails.
  void load(MemBuf *pMemBuf) 
    throw(Exception);

  //! \brief Loads the specified string with file content.
  //!
  //! \<pString\> must have been allocated by the caller.
  //! \param pString Pointer to the string to load.
  //! \exception FILE_ERROR Thrown if file opening or reading fails.
  void load(std::string *pString) 
    throw(Exception);

  //! \brief Saves the specified string in the file.
  //! \param strContent String to save.
  //! \exception FILE_ERROR Thrown if file opening or writing fails.
  void save(const std::string& strContent) 
    throw(Exception);
};


// ============================================================================
//! \class TempFileName 
//! \brief Another temporary file class.
//!
//! Inherits from FileName class and provides a random function for file name.
// ============================================================================
class YAT_DECL TempFileName : public FileName
{
private:
  static long s_lLastNumber;

  //- Generate file name
  std::string GenerateRandomName();

public:
  //! \brief Default constructor.
  TempFileName();

  //! \brief Constructor from path name.
  //!
  //! %File name is generated by a random function.
  //! \param strPath Path name.
  TempFileName(const std::string& strPath);
};  

// ============================================================================
//! \class FileEnum 
//! \brief Directory enumeration class.
//!
//! This class provides an enumeration function for a directory path.
//! Inherits from FileName class.
// ============================================================================
class YAT_DECL FileEnum : public FileName
{
public:
  //! \brief Enumeration mode.
  enum EEnumMode
  {
    //! Enumerates only files.
    ENUM_FILE = 1,
    //! Enumerates only directories.
    ENUM_DIR = 2,
    //! Enumerates files & directories.
    ENUM_ALL = 3
  };

protected: 
  //! Find mode (FILE or DIR or ALL).
  EEnumMode m_eMode;  

#ifdef YAT_WIN32
  //! WIN32 Find handler.
  void* m_hFind;

  //! WIN32_FIND_DATA pointer.
  void* m_pfindData;

  //! WIN32 Indicates if an error occured since last find action.
  int m_bErrorOnLastFind;

  //! WIN32 First file boolean.
  bool m_bFirst;
#else
  //! Posix Directory pointer.
  DIR * m_dirDir;
#endif

  //! Current path.
  std::string m_strPath;    

public:

  //! \brief Constructor.
  //! \param strPath Directory path to enumerate.
  //! \param eMode Enumeration mode.
  //! \exception BAD_PATH Thrown if specified path is not correct.
  FileEnum(const std::string& strPath, EEnumMode eMode=ENUM_FILE) 
    throw(BadPathException);

  //! \brief Destructor.
  ~FileEnum();

  //! \brief Initializes search mask.
  //! 
  //! \param strPath Path to enumerate.
  //! \param eMode Enumeration mode.
  //! \exception BAD_PATH Thrown if specified path is not correct.
  void init(const std::string& strPath, EEnumMode eMode=ENUM_FILE) 
    throw(BadPathException);

  //! \brief Finds next file in directory.
  //! 
  //! Sets path and file values to the current file found in directory.\n
  //! Returns true if a file has been found, false if not.
  bool find() 
    throw(BadPathException, FileNotFoundException, Exception);

  //! Closes the directory enumeration.
  void close();
};

// ============================================================================
//! \class CfgFile 
//! \brief Reads a "Windows configuration file" style file.
//!
//! This class reads a file which content is in a "Windows configuration file" style, 
//! i.e. :\n
//!   \[section\]\n
//!   key=value\n
//!   \<string\>\n
//!   object:\n
//!   member=value\n
//!   -\n
//!
//! It adds objects definitions capability over the '.ini' specification
//!
//! Inherits from File class.
// ============================================================================
class YAT_DECL CfgFile : public File
{
public:
  //! \brief List of values.
  typedef std::vector<std::string> Values;
  //! \brief Map of (key, value).
  typedef std::map<std::string, std::string> Parameters;
  //! \brief (key, value) vector
  typedef std::vector<Parameters> ObjectCollection;
  //! \brief Map of vector of (key, value)
  typedef std::map<std::string, ObjectCollection> Objects;
  
private:
  class Section
  {
  public:
    Values m_vecSingleValues;
    Parameters m_dictParameters;
    Objects m_objects;
  };

  mutable std::map<std::string, Section> m_dictSection;
  mutable std::string m_strSection;
  
public:
  //! \brief Constructor.
  //! \param strFile %File name.
  CfgFile(const std::string& strFile);

  //! \brief Default Constructor.
  CfgFile() { }

  //! \brief Reads the configuration file.
  //!
  //! \exception FILE_ERROR Thrown if file opening or reading fails.
  void load() 
    throw(Exception);

  //! \brief Reads the configuration file.
  //!
  //! \exception FILE_ERROR Thrown if file opening or reading fails.
  void load( const std::string& file_full_name )
    throw(Exception);

  //! \brief Reads from a string.
  //!
  void load_from_string(const std::string& input);

  //! \brief get section names
  void get_sections(std::list<std::string>* list_p ) const;
  
  //! \brief Sets the current section.
  //! \param strSection New section name.
  //! \param bThrowException If set to true, exceptions are thrown.
  //! \exception NO_DATA Thrown if section not found.
  bool set_section(const std::string& strSection, bool bThrowException=true) const
    throw( Exception );
    
  //! \brief Gets section values.
  //!
  //! Returns a vector of \<values\>.
  //! \param strSection Section name. If set to NULL, gets current section values.
  const Values& get_values(const std::string& strSection="") const;

  //! \brief Gets section dictionary.
  //!
  //! Returns a map of (\<value name\>, \<value\>).
  const Parameters& get_parameters() const;
  
  //! \brief Check if section contains at least one object of the specified type
  bool has_object(const std::string& object_type) const;
  
  //! \brief Get section's objects collection by object type name
  const ObjectCollection& get_objects(const std::string& object_type) const;
  
  //! \brief Get section's unique object according to its type
  const Parameters& get_unique_object(const std::string& object_name) const;
  
  //! \brief Tests if a value name is found in the current section.
  //! 
  //! Returns true if the value name is found, false otherwise.
  //! \param strParamName Value name.
  bool has_parameter(const std::string& strParamName) const;
  
  //! \brief Gets a value from its name, in the current section.
  //!
  //! Returns nil string if value name not found in the section.
  //! \param strParamName Value name.
  std::string get_param_value(const std::string& strParamName) const;

};

#define FOR_EACH_VALUES_IN_CFG_SECTION(cfg_file, section, x) \
    cfg_file.SetSection(section); \
    for( CfgFile::Values::const_iterator x = cfg_file.GetValues().begin(); x != cfg_file.GetValues().end(); x++)

// ============================================================================
//! \class DirectoryWatcher 
//! \brief Watches changes in a directory: new files/directories/links or removed files.
//!
//! This class provides a wathdog service for a directory: detects any change in directory
//! content. The changes are detected since the last call of the watch function.
//!
//! Inherits from FileName class.
// ============================================================================
class YAT_DECL DirectoryWatcher : public FileName
{
public:
  //! File name object.
  typedef yat::SharedPtr<FileName> FileNamePtr;

  //! \brief List of file name objects.
  typedef std::vector<FileNamePtr> FileNamePtrVector;

  //! \brief Enumeration values for directory content memorization.
  typedef enum
  {
    //! The directory content at object construction time is memorized.
    ENUM_FIRST=0,  
    //! The directory content at object construction time is not memorized,
    //! the first call to GetChanges() will return the whole directory content.
    NO_FIRST_ENUM
  } WatchMode;
  
private:
  struct Entry
  {
    FileNamePtr ptrFile;
    Time tmLastModTime;
    bool bRemoved;
    
    Entry(const std::string& strFullName);
  };
  
  typedef yat::SharedPtr<Entry> EntryPtr;
  typedef std::map<uint64, EntryPtr> EntryMap;
  
  EntryMap  m_mapEntry;            // Directory content  
  Time      m_tmDirModTime;        // Last modification time of the directory
  Time      m_tmLocalModTime;      // Last time the the directory's modification time has changfes
  bool      m_bDirectoryHasChanged;
  WatchMode m_eMode;
  
  bool priv_has_changed(bool bReset);
  
public:

  //! \brief Constructor.
  //! \param strDirectoryPath Directory path to whatch.
  //! \param eMode Watch mode.
  //! \exception BAD_PATH Thrown if specified path is not correct.
  DirectoryWatcher(const std::string& strDirectoryPath, WatchMode eMode = ENUM_FIRST)
     throw( Exception);
  
  //! \brief Gets new, changed and removed file names since last call.
  //! \param vecNewFilesPtr List of new file names.
  //! \param vecChangedFileNamePtr List of changed file names.
  //! \param vecRemovedFileNamePtr List of deleted file names.
  //! \exception BAD_PATH Thrown if current path is not correct.
  void get_changes(FileNamePtrVector *vecNewFilesPtr, FileNamePtrVector *vecChangedFileNamePtr=NULL,
                    FileNamePtrVector *vecRemovedFileNamePtr=NULL) 
    throw( Exception);
  
  //! \brief Has the directory changed ?
  //! 
  //! Returns true if a new, changed or removed file has been detected, false othsewise.
  bool has_changed();
};

//! Asynchronous lock callback definition
YAT_DEFINE_CALLBACK(FileLockCallback, Thread::IOArg);

// ============================================================================
//! \class LockFile
//! \brief Manage locks
//!
//! Create a read (i.e. shared) or write (i.e. exclusive) on a file
//! This class uses the fcntl system call to ensure compatibility with NFS
// ============================================================================
class YAT_DECL LockFile
{
public:

  //! lock type
  enum Type
  {
    READ,
    WRITE
  };

  //! \brief c-tor
  LockFile(const FileName& fn, Type t);

  //! \brief d-tor
  virtual ~LockFile();

  //! \brief try to lock the file, do not wait until the lock is released
  //! \return 'true' if succeed, false if the file is already locked
  bool try_lock();

  //! \brief lock the file: wait until the file is effectively locked
  void lock();

  //! \brief unlock previously locked file
  void unlock();

  //! \name Asynchronous lock
  //! A set of experimental methods for asynchronous locking
  //!@{

    //! \brief asynchronously lock the file
    //! Asynchronously lock the file: when the lock is acquired the callback is called
    void async_lock(FileLockCallback lock_cb, FileLockCallback err_cb);

    //! \brief asynchronously lock the file
    //! Asynchronously lock the file: when the lock is acquired a message is posted
    void async_lock(Task* task_p, MessageType lock_msg, MessageType err_msg);

    //! \brief get errors list in case of lock failure
    Exception::ErrorList get_errors();

  //!@}

private:

  struct ASyncLock
  { // async lock related information
    Task*                task_p;
    std::size_t          lock_msg;
    std::size_t          err_msg;
    FileLockCallback     lock_callback;
    FileLockCallback     err_callback;
    UniquePtr<Pulser>    pulser_ptr;
    Exception::ErrorList errors;
  };

  FileName             m_file_name;
  Type                 m_type;
  int                  m_fd;
  int                  m_lock_cmd;
  bool                 m_wait_for_lock; // asynchonously waiting for the lock...
  UniquePtr<ASyncLock> m_async_ptr;
  
  bool priv_lock( int lock_cmd );
  void lock_callback( Thread::IOArg );
  void start_async_lock();
};

// ============================================================================
//! \class RLockFile 
//! \brief Helper class for read lock
//!
//! Create read lock (i.e. shared lock) on a file
// ============================================================================
class YAT_DECL RLockFile : public LockFile
{
  //! \brief c-tor
  RLockFile(const FileName& fn) : LockFile(fn, LockFile::READ) {}
};

// ============================================================================
//! \class WLockFile 
//! \brief Helper class for write lock
//!
//! Create write lock (i.e. exclusive lock) on a file
// ============================================================================
class YAT_DECL WLockFile : public LockFile
{
  //! \brief c-tor
  WLockFile(const FileName& fn) : LockFile(fn, LockFile::WRITE) {}
};

// ============================================================================
//! \class AutoLockFile 
//! \brief Helper class that automatically lock/unlock the file
//!
//! The AutoLockFile class provides an auto lock/unlock mechanism.\n
// ============================================================================
class YAT_DECL AutoLockFile
{
public:

  AutoLockFile(LockFile* lock_p);
  ~AutoLockFile();

private:
  LockFile* m_lock_p;
};

}


#endif

