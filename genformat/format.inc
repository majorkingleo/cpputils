/*
  Format a C++ library for typesafe string formating in printf style
      (C) 2001 - 2009 by Martin Oberzalek <kingleo@gmx.at>  

   2004-01-01 remerged version 1.1 of Format with xstow's Format

  Examples:
     std::cout << format( "Hello %s, I have $05d$ in my pocket", "world", 5 ) << std::endl;
     std::cout << format( "Do not try this with printf: %s", 10101 ) << std::endl;

  
*/

#ifndef TOOLS_format_h
#define TOOLS_format_h

/* old Owil define */
#ifdef AIX
#ifdef Where
# undef Where
#endif
#endif

#include <string>
#include <iomanip>
#include <iostream>

#include "ref.h"

#define HAVE_STL_SSTREAM

#if __GNUC__ == 2
#undef HAVE_STL_SSTREAM
#endif

#ifdef HAVE_STL_SSTREAM
#  include <sstream>
#  include <cctype>
#  define IS_DIGIT( x ) std::isdigit( x )
#else
extern "C" {
#  include <ctype.h>
}
#  include <strstream>
#  define IS_DIGIT( x ) isdigit( x )
#endif  

namespace Tools {

namespace Format {
	class CFormat;
}

inline std::ostream& operator<<( std::ostream& out, Format::CFormat cf );

namespace Format
{
  typedef std::string::size_type ST;
  
  class CFormat
  {    
  public:
    
    enum Adjust
      {
	LEFT,
	RIGHT
      };
    
    enum Base
      {
	OCT,
	DEC,
	HEX
      };
    
    enum Floating
      {
	FIXED,
	SCIENTIFIC
      };
    
    bool valid;
    Adjust adjust;
    bool special;
    bool sign;
    bool grouping; // SUSv2 extension
    bool conversion; // glibc 2.2 extension
    bool zero;
    bool precision_explicit;
    bool internal;
    Base base;
    bool setupper;
    Floating floating;
    bool showbase;
    
    int width;
    int precision;
    int strlength;

    std::string format;

  public:    
    CFormat() :
	  valid(false),
	  adjust(RIGHT),
	  special(false),
	  sign(false),
	  grouping(false),
	  conversion(false),
	  zero(false),
	  precision_explicit(false),
	  internal(false),
	  base(DEC),
	  setupper(false),
	  floating(FIXED),
	  showbase(false),
	  width(0),
	  precision(6),
	  strlength(0),
	  format("")
	{ }
		
		void set( std::ostream& out );
  };
  
  /****************************************/

  typedef BaseException Error;

  template <TEMPLATECLASSES>
  class Format
  {
  private:
    struct Arg
    {
      bool is_int;
      bool is_string;
    };
    
    Arg args[MAXARGS];

    std::string format;
    
	DECLARATIONS;

    unsigned int num_of_args;
    
    std::string s;

  private:
	Format() {}
	Format(const Format & f) {}
	Format & operator=(const Format & f) {}

  public:
    Format( const std::string &format, TEMPLATELIST, unsigned int num_of_args );
    
    std::string get_string() const { return s; }
    
  private:
    void parse();

    template <class N> bool is_int( N &n ) { return false; }
    bool is_int( int &n ) { return true; }
    bool is_int( unsigned int &n ) { return true; }
    bool is_int( short &n ) { return true; }
    bool is_int( unsigned short ) { return true; }
    bool is_int( unsigned long ) { return true; }
    bool is_int( unsigned long long ) { return true; }
    bool is_int( long long ) { return true; }

    template <class S> bool is_string( S &s_ ) { return false; }
    bool is_string( std::string& s_ ) { return true; }
    bool is_string( const std::string& s_ ) { return true; }
    bool is_string( char* ) { return true; }
    bool is_string( const char* ) { return true; }

    int get_int_arg( int num );
    void gen_arg_list();
    std::string use_arg( unsigned int i, const CFormat &cf );

    template <class S> std::string x2s( S ss, const CFormat &cf )
    {
#ifdef HAVE_STL_SSTREAM
	std::stringstream str;
	str << cf << ss;
	std::string st = str.str();
	return st;
#else
      std::strstream str;
      str << cf << ss << std::ends;
      std::string st = str.str();
      str.freeze(0);
      return st;
#endif
    }

	template<class T> int get_int( const T &t ) { return 0; }
	int get_int( int n ) { return n; }
    int get_int( unsigned int n ) { return n; }
    int get_int( short n ) { return n; }
    int get_int( unsigned short n ) { return n; }
    int get_int( long long n ) { return n; }
    int get_int( unsigned long long n  ) { return n; }
    int get_int( long n ) { return n; }
    int get_int( unsigned long n ) { return n; }

  };

  int skip_atoi( std::string s, ST start, ST& pos );

} // namespace Format

inline std::ostream& operator<<( std::ostream& out, Format::CFormat cf )
{
  cf.set( out );
  return out;
}

namespace Format { /* M$ Comiler can't handle it otherwiese */

template <TEMPLATECLASSES>
  Format<TEMPLATEDECL>::Format( std::string const &format_, TEMPLATELIST, unsigned int num_of_args_ )
  : format( format_ ), VARINIT, num_of_args( num_of_args_ ), s("")
{
  if( num_of_args > MAXARGS )
    throw Error( "Number of args out of range" );

  gen_arg_list();
  
  parse();
}

template <TEMPLATECLASSES>
  int Format<TEMPLATEDECL>::get_int_arg( int num )
{
  if( static_cast<unsigned int>(num) > num_of_args - 1 )
    throw Error( "The arg you wan't to use is out of range" );

  if( num < 0 )
    throw Error( "negativ number for arg number not allowed" );

  if( args[num].is_int ) 
    {
      switch( num )
	{
	  GETINT
	}
    }
  else
    throw Error( "expecting int arg" );

  return 0; // should never be reached
}

template <TEMPLATECLASSES>
  void Format<TEMPLATEDECL>::gen_arg_list()
{
  for( unsigned int i = 0; i < num_of_args; i++ )
    { 
      switch( i )
		{
		  GENARGLIST
		}
	}
}

template <TEMPLATECLASSES>
  std::string Format<TEMPLATEDECL>::use_arg( unsigned int i, const CFormat &cf )
{
  if( i > num_of_args )
    throw Error( "out of arg range" );

  switch( i )
    {
	  GENUSEARG
    }

  return "";
}

template <TEMPLATECLASSES>
  void Format<TEMPLATEDECL>::parse()
{
  if( format.empty() )
    return;

  unsigned int par = 0;
  unsigned int use_par = 0;
  ST pos = 0;
  ST len = format.size();
  s = "";
  bool had_precision = false;

  while( par < num_of_args && pos < len )
    { // while

      use_par = par;

      if( pos >= len )
	break;

      if( format[pos] != '%' )
	{
	  s += format[pos];
	  pos++;
	  continue;
	}

      // % digit found
      pos++;

      if( !(pos < len ) || (format[pos] == '%') )
	{
	  // %% -> %
	  s += format[pos];
	  pos++; 
	  continue;
	}
  
      // format string found

      ST start = pos - 1;
      CFormat cf;

      // process flags

      while( (pos < len) )
	{
	  bool finished = false;
	  
	  switch( format[pos] )
	    {
	    case '-' : cf.adjust = CFormat::LEFT; break;
	    case '+' : cf.sign = true; break;
	    case ' ' : cf.zero = false; break;
	    case '#' : cf.special = true; break;
	    case '\'': cf.grouping = true; break;
	    case 'I' : cf.conversion = true; break; 
	    case '0' : cf.zero = true; break;
	    default: finished = true; break;
	    }
	  
	  if( finished )
	    break;
	  
	  pos++;
	} //       while( (pos < len) )

      // get argument number
      if( pos < len )
	{
	  // search for the $ digit
	  unsigned int dp = pos;
	  
	  while( dp < len && IS_DIGIT( format[dp] ) ) 
	    dp++;
	  
	  if( dp < len && format[dp] == '$' )
	    {
	      use_par = skip_atoi( format, pos, pos ) - 1;
	      pos = dp + 1;
	    }
	}

      // get field with
      if( pos < len )
	{
	  if( IS_DIGIT( format[pos] ) )
	    cf.width = skip_atoi( format, pos, pos );
	  else if( format[pos] == '*' )
	    {
	      pos++;

	      // search for the $ digit
	      unsigned int dp = pos;
	      
	      while( dp < len && IS_DIGIT( format[dp] ) ) 
		dp++;

	      if( dp < len && format[dp] == '$' )
		{
		  cf.width = get_int_arg( skip_atoi( format, pos, pos ) - 1 );
		  // skip $ sign
		  pos++;
		}
	      else 
		{
		  cf.width = get_int_arg( par );

		  if( use_par == par )
		    use_par++;

		  par++;
		} 

	      if( cf.width < 0 )
		{
		  cf.width *= -1;
		  cf.adjust = CFormat::LEFT;
		}
	    }
	}

      // precision
      if( pos < len )
	{
	  if( format[pos] == '.' )
	    {
	      pos++;
	      if( !(pos < len) )
		return;
	      
	      had_precision = true;

	      if( IS_DIGIT( format[pos] ) )
		  cf.precision = skip_atoi( format, pos, pos );
	      else if( format[pos] == '*' )
		{
		  pos++;
		  
		  
		  // search for the $ digit
		  unsigned int dp = pos;
		  
		  while( dp < len && IS_DIGIT( format[dp] ) ) 
		    dp++;
		  
		  if( dp < len && format[dp] == '$' )
		    {
		      cf.precision = get_int_arg( skip_atoi( format, pos, pos ) - 1 );
		      // skip $ sign
		      pos++;
		    }
		  else 
		    {
		      cf.precision = get_int_arg( par );

		      if( use_par == par )
			use_par++;

		      par++;
		    }
		  
		  if( cf.precision == 0)
		    cf.precision_explicit = true;
		  
		  if( cf.precision < 0 )
		    cf.precision = 0;
		}
	      else
		cf.precision = 0;
	    }
	  
	}
    
      // lenght modifier
      /* 
	 they will be ignored 
	 cause we know the types of the parameter
      */      
      if( (pos < len) )
	{
	  bool hh = false;
	  bool ll = false;
	  bool found = false;
	  
	  switch( format[pos] )
	    {
	    case 'h': hh = true; found = true; break;
	    case 'l': ll = true; found = true; break;
	    case 'L':
	    case 'q':
	    case 'j':
	    case 'z':
	    case 't': found = true; break;
	    default: break;
	    }

	  if(found )
	    {
	      pos++;
	      
	      if( pos < len )
			{
			  if( hh == true )
				{
				  if( format[pos] == 'h' )
					pos++;
				}
			  else if( ll == true )
				{
				  if( format[pos] == 'l' )
					pos++;
				}
			} // if
	    } // found
	}

      // conversion specifier

      if( pos < len )
	{
	  bool invalid = false;
	   switch( format[pos] )
	     {
	     case 'd':
	     case 'i':
	       cf.base = CFormat::DEC;
	       if( cf.zero && (cf.adjust != CFormat::LEFT) )
		 cf.internal = true;
	       break;
	       		  
	     case 'X': cf.setupper = true;
	     case 'x': 
	       cf.base = CFormat::HEX;
	       if( cf.special )
		 cf.showbase = true;
	       break;
	       
	     case 'o': 
	       cf.base = CFormat::OCT;
	       if( cf.special )
		 cf.showbase = true;
	       break;

	       
	     case 'E': 
	       cf.setupper = true;

	     case 'e': 
	       if( cf.special )
		 cf.sign = true;
	       cf.floating = CFormat::SCIENTIFIC;
	       break;
	       
	     case 'F': // not supported
	     case 'f': 
	       if( cf.special )
		 cf.sign = true;
	       cf.floating = CFormat::FIXED;
	       break;
	       
	     case 's':
	       if( cf.zero ) 
		 cf.zero = false;
	       break;
	      

		 case 'p':
	       cf.base = CFormat::HEX;	
		   cf.showbase = true;	   
		   break;

	       // unsupported modifiers


	     case 'G':
	     case 'g':
	       
	     case 'A':
	     case 'a':
	       
	     case 'c':
	       
	     case 'C':
	     case 'S':
	     case 'P':
	     case 'n': break;
	       
	     default: invalid = true;
	     }

	   if( !invalid )
	     cf.valid = true;
	}  

      if( cf.valid )
	{
	  std::string str;
	  int upar = par;

	  if( use_par != par )
	    upar = use_par;

	  if( cf.base == CFormat::HEX && had_precision && cf.special )
	  {
	      CFormat f2;
	      f2.base = cf.base;
	      std::string ss = use_arg( upar, f2 );
	      cf.strlength = ss.size();
//	      printf( "str: %s\n", s.c_str() );
	  }

	  str = use_arg( upar, cf );

	  // cut string
	  if( had_precision && args[upar].is_string )
	    str = str.substr( 0, cf.precision );

	  s += str;

	  if( use_par == par )
	    par++;
	}
      else
	{
	  // copy the invalid format string
	  for( ST i = start;  i<= pos; i++ )
	    if( i < len )
	      s += format[i];
	}
  
      pos++;

    } // while

  if( pos < len )
    {
      while( pos < len )
	{
	  s += format[pos];
	  pos++;
	}
    }
}

} // namespace Format

  /* machst das
template <class A, class B, class C, class D, class E, class F, class G, class H>
  inline std::string format( std::string fs, A a, B b, C c, D d, E e, F f, G g, H h )
{
  return Format::Format<A,B,C,D,E,F,G,H>( fs, a, b, c, d, e, f, g, h, 8).get_string();
}
  */

GENFORMAT



namespace Format
{
  template<typename ostream> class PrintF
  {
  private:

    int level;
    int dlevel;

    int dmodule;
    int module;
    

  public:

    ostream &out;
    
  public:

    PrintF( ostream &out_ = std::cout, int module_ = -1, int debug_level_ = -1 ) 
      : level( debug_level_ ), dlevel( debug_level_ ), 
      dmodule( module_), module( module_),  out( out_ ) 
      {}

    void set_debug_level( int dlevel_ ) { dlevel = dlevel_; }
    void set_module( int module_ ) { dmodule = module_; }

    PrintF operator()( int module_ )
      {
	PrintF printf( *this );
	printf.module = module_;

	return printf;
      }

    PrintF operator[]( int level_ )
      {
	PrintF printf( *this );
	printf.level = level_;

	return printf;
      }

    template<typename T> PrintF& operator<<( const T &t ) 
      { 
	if( check() )
	  out << t; 

	return *this; 
      }

    // io manipulator overloading
    PrintF& operator<<(ostream& (*f)(ostream&))
      {
	if( check() )
	  out << f;

	return *this;
      }

    PrintF& operator()( std::string fs )
    {
      if( check() )
	out << fs;

      return *this;
    }

	GENPRINTF

    bool check( int module_, int level_ ) const
      {
	if( module_ == dmodule || dmodule == -1 )
	  {
	    if( dlevel == -1 )
	      return true;
	    
	    if( level_ <= dlevel )
	      return true;
	  }

	return false;	
      }

  private:
    
    bool check() const { return check( module, level ); }

  };

}

} // namespace Tools

#undef IS_DIGIT

#endif
