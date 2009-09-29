/*
+*  VERSID:    "%Z% @(#)$Header$";
+*  $Log$
+*  Revision 1.5  2009/09/23 12:42:27  wamas
+*  Weiterentwicklung
+*
+*  Revision 1.4  2009/09/08 17:49:57  wamas
+*  Bugfixes and Updates
+*
+*  Revision 1.3  2008/09/23 10:39:53  wamas
+*  Initiale Inventurliste Schicken.
+*
+*  Revision 1.2  2008/09/02 16:34:16  wamas
+*  Merged with clp
+*
+*  Revision 1.1.1.1  2008/08/14 14:11:32  moberza
+*  Initial import
+*
+*  Revision 1.2  2008/06/24 14:31:59  wamas
+*  Fixed leading zeros.
+*
+*  Revision 1.1  2008/02/25 10:41:38  wamas
+*  MOBERZA TS-115979 Men� Transporteinheiten Erstellen hinzugefuegt,
+*  sowie wood und andere C++ Dateien.
+*
+*  Revision 1.1  2007/02/19 07:14:04  wamas
+*  sis: Eingecheckt
+*
+*  Revision 1.1.2.1  2006/05/11 08:27:36  wamas
+*  Toolsbox Umstellung
+*
+*  Revision 1.1.2.2  2005/07/27 16:29:59  wamas
+*  MOBERZA add Versid and logging stuff
+*
+*
*/
#ifndef TOOLS_string_utils_h
#define TOOLS_string_utils_h

#include <string>
#include <vector>

#define HAVE_STL_SSTREAM

#if __GNUC__ == 2
#undef HAVE_STL_SSTREAM
#endif

#ifdef HAVE_STL_SSTREAM
#  include <sstream>
#  include <cctype>
#  define strstream stringstream
#  define ENDS
#else
#  include <ctype.h>
#  include <strstream>
#  define ENDS << std::ends;
#  define STRSTREAM
#endif  

namespace Tools {

std::string toupper( std::string s );
std::string tolower( std::string s );
std::string strip( const std::string& str, const std::string& what = " \t\n\r" );
std::string strip_leading( const std::string& str, const std::string& what = " \t\n\r" );
bool is_int( const std::string &s );
std::vector<std::string> split_simple( std::string str, std::string seperator = " \t\n", int max = -1 );
std::vector<std::string> split_string( std::string str, std::string seperator, int max = -1 );
std::vector<std::string> split_and_strip_simple( std::string str, const std::string & sep = " \t\n", int max = -1 );

inline bool is_bool( const bool &b ) { return true; }
template<class T> bool is_bool( const T &t ) { return false; }


inline bool get_bool( const bool & b ) { return b; }

// dummy function
template<class T> T get_bool( bool b ) { return T(); }


bool s2bool( const std::string &s );

/// convert a string to anything
template <class T> T s2x( const std::string& s )
{
    if( is_bool( T() ) )
    {
      bool b = s2bool( s );

      return get_bool<T>(b);
    }

  std::strstream str;

  str << s ENDS;
  T t;
  str >> t;

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return t;
}


/// convert a string to anything
template <class T> T s2x( const std::string& s, const T & init )
{
    if( is_bool( T() ) )
    {
	return s2bool( s );
    }

  std::strstream str;
  
  str << s ENDS;
  T t(init);
  str >> t;

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return t;
}


/// converts anything to a string
template<class T>std::string x2s( T what )
{
  std::strstream str;
  
  str << what ENDS;
  std::string s( str.str() );

#ifdef STRSTREAM
  str.freeze(0);
#endif

  return s;
}

std::string x2s( const bool b );


std::string text_right_format( std::string text, unsigned int max_size, unsigned int spaces );

inline std::string auto_linebreak( std::string text, unsigned int max_size = 90 )
{
  return text_right_format( text, max_size, 0 );
}

std::string substitude( std::string str, std::string what, std::string with );

std::string prepand( std::string str, std::string what, std::string prefix );

inline bool icase_cmp( const std::string &a, const std::string &b )
{
    if( a.size() != b.size() )
	return false;
    
    if( a == b )
	return true;

    if( toupper( a ) == toupper( b ) )
	return true;

    return false;
}

std::string bin_encode( const std::string &s );
std::string bin_decode( const std::string &s );

std::string group_thousand( const std::string &s, unsigned digit = 3, const std::string &sep = "." );

std::string escape( const std::string &s );

std::vector<std::string> split_safe( const std::string &s, const std::string &sep = " \n\t");

template <class T> std::string createInStatement( const T & list )
{
  std::string res;

  if( list.empty() )
	return res;

  for( typename T::const_iterator it = list.begin(); it != list.end(); it++ )
	{
	  if( !res.empty() )
		  res += ", ";

	  res += "'";
	  res += *it;
	  res += "'";		
	}  

  return "(" + res + ")";
}

} // namespace Tools

#undef STRSTREAM
#undef strstream
#undef ENDS

#endif
