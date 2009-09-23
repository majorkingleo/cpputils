/*
 * $Log$
 * Revision 1.1  2009/01/27 12:37:37  wamas
 * Weiter n�tzliche Klassen.
 *
 * Revision 1.3  2008/08/06 12:24:55  wamas
 * new g++ fixes
 *
 * Revision 1.2  2007/02/09 17:25:35  wamas
 * GCC 2.95 port
 *
 * Revision 1.1.1.1  2006/03/17 19:49:16  wamas
 * own tools reponsitority
 *
 * Revision 1.2  2006/03/09 00:48:26  wamas
 * Added CVS Log Info
 *
 */

/*
  The CppDir classes are originally from the dirtree program
  http://home.pages.at/kingleo/development/cpp-en.html#dirtree

  and adoped, as improved by the leohtml project
  http://leohtml.sourceforge.net

  extended for xstow 
*/
#ifndef WIN32
#ifndef TOOLS_cppdir_h
#define TOOLS_cppdir_h

#include <string>
#include <list>

extern "C" {
#ifndef WIN32
#include <dirent.h>
#endif /* WIN32 */
#include <sys/types.h>

}

#include <iostream>

#include "range.h"

/** 
    This is a simple C++ interface for getting
    informations about files and directories.
*/

namespace Tools {

namespace CppDir 
{
    struct EnumFile {
	enum ETYPE ///< possible file types
	{
	    FIRST__  = -1,
	    UNKNOWN  =  0, ///< unknown file type
	    FIFO     =  1, ///< fifo
	    CHAR     =  2, ///< character device 
	    DIR      =  4, ///< directory
	    BLOCK    =  6, ///< blockdevice
	    REGULAR  =  8, ///< regular file
	    LINK     = 10, ///< link
	    LAST__
	};	
    };

    typedef EnumRange<EnumFile> EFILE;

  class File ///< A simple file
  {
  public:

    typedef unsigned long size_type;
    
  private:    
    std::string name; ///< name of the file
    EFILE type;        ///< file type

    long inode_number; ///< inode number
    bool valid;        ///< is a valid file?
    bool link;
    std::string path;  
    size_type file_size;
    size_type date;
    size_type access_date;
    bool err;
    bool can_read;
    bool can_write;
    bool can_exec;
    std::string fullpath;

  public:
    /** extract file informations from the dirent structure */
    File( struct dirent *d, std::string path ); 
    File( std::string path, std::string name );
    File( std::string file );

    bool is_valid() const { return valid; }
    bool operator!() const { return !valid; }

    /** even if the file is invalid the function returns the name of the file */
    std::string get_name() const { return name; }

    /** even if the file is invalid the function returns the path to the file */
    std::string get_path() const { return path; }
    
    std::string get_full_path() const { return fullpath; }

    EFILE       get_type() const { return type; }
    long        get_inode() const { return inode_number; }

    /** 
	If the file is a symlink the function returns true.
	If the file is invalid and this function returns true, the file
	is a broken symlink.
	Even if the file is a symlink all other informations like size and permissions, 
	except the name and path of course, are the data from the file that the symlink
	points to.
    */	
    bool        is_link() const { return link; }    
    
    /// name of the file the link points to
    std::string get_link_buf() const;

    /** returns the file size in bytes */
    size_type   get_size() const { return file_size; }
    size_type   get_ksize() const { return file_size / 1024; }
    size_type   get_msize() const { return get_ksize() / 1024; }
    size_type   get_gsize() const { return get_msize() / 1024; }

    size_type   get_date() const { return date; } /// last change date
    size_type   get_access_date() const { return access_date; } /// last access date

    /** the function returns true if you have read access at the file */
    bool        read_access() const { return can_read; }

    /** the function returns true if you have write access at the file */
    bool        write_access() const { return can_write; }

    /** the function returns true if the exec flag is set */
    bool        exec_access() const { return can_exec; }

  private:
    EFILE get_type( const std::string& name );
#ifndef WIN32
    inline bool in_groups( gid_t gid, const int size, gid_t list[] );
#endif
    };
    
  //-------------------

  class Directory /// class representing a directory
    {
    public:
      typedef std::list<File> file_list; 
      typedef file_list::iterator file_list_it; 

      std::string error;

    private:
      std::string name; /// directory name
      bool valid;       /// true if the directory is valid
      file_list files;  /// list of file within the directory
      bool is_open;     /// true if the directory is open
      
      DIR* dir;  /// directory HANDLE

    public:
      Directory( std::string pname );
      Directory( File file );
      ~Directory();

      bool is_valid() const { return valid; }
      bool operator!() const { return !valid; }

      /** returns the list of files that the directory contains */
      file_list get_files() { return files; }

    private:
      bool open( File file ); /// opens the directory
      bool open( std::string fname ); /// opens the directory
      void close(); /// closes the directory
    };

  std::ostream& operator << ( std::ostream& out , EFILE type );

  /// concats 2 directory names, or dir and filename .....
  std::string concat_dir( std::string path, std::string name );

  /// gets the current directory
  std::string pwd();

  /// the execution path of the application
  static std::string exec_path = pwd();

  /// returns the full path to a file
  std::string get_full_path( std::string file, std::string current_dir = CppDir::exec_path );

  /** splits a file name into the path to the file and the file name */
  void split_name( std::string file_name, std::string & path, std::string & name );

  /// removes the exec_path from a given path
  std::string simplify_path( std::string path );

  std::string readlink( const std::string &path );

  std::string beautify_path( std::string path );

  /// makes relativ path from path to dir (if possible)
  std::string make_relative( std::string path, std::string dir );

  /// checks if dir is in path
  bool is_in_dir( const std::string &path, const std::string &dir );
}

} // namespace Tools

#endif
#endif /* WIN32 */