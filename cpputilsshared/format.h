/*
  Format a C++ library for typesafe string formating in printf style
      (C) 2001 - 2003 by Martin Oberzalek <kingleo@gmx.at>  

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
    CFormat() { reset(); }
    
    void set( std::ostream& out );
    
  private:    
    void reset();
  };
  
  /****************************************/

  typedef BaseException Error;

  template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
  class Format
  {
  private:
    struct Arg
    {
      bool is_int;
      bool is_string;
    };
    
    Arg args[30];

    std::string format;
    
	Arg0 var0;
Arg1 var1;
Arg2 var2;
Arg3 var3;
Arg4 var4;
Arg5 var5;
Arg6 var6;
Arg7 var7;
Arg8 var8;
Arg9 var9;
Arg10 var10;
Arg11 var11;
Arg12 var12;
Arg13 var13;
Arg14 var14;
Arg15 var15;
Arg16 var16;
Arg17 var17;
Arg18 var18;
Arg19 var19;
Arg20 var20;
Arg21 var21;
Arg22 var22;
Arg23 var23;
Arg24 var24;
Arg25 var25;
Arg26 var26;
Arg27 var27;
Arg28 var28;
Arg29 var29;
;

    unsigned int num_of_args;
    
    std::string s;
    
  public:
    Format( const std::string &format, Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25, Arg26 var26, Arg27 var27, Arg28 var28, Arg29 var29, unsigned int num_of_args );
    
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

    int get_int_arg( unsigned int num );
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

template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
  Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28, Arg29>::Format( std::string const &format_, Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25, Arg26 var26, Arg27 var27, Arg28 var28, Arg29 var29, unsigned int num_of_args_ )
  : format( format_ ), var0(var0), var1(var1), var2(var2), var3(var3), var4(var4), var5(var5), var6(var6), var7(var7), var8(var8), var9(var9), var10(var10), var11(var11), var12(var12), var13(var13), var14(var14), var15(var15), var16(var16), var17(var17), var18(var18), var19(var19), var20(var20), var21(var21), var22(var22), var23(var23), var24(var24), var25(var25), var26(var26), var27(var27), var28(var28), var29(var29), num_of_args( num_of_args_ )
{
  if( num_of_args > 30 )
    throw Error( "Number of args out of range" );

  gen_arg_list();
  
  parse();
}

template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
  int Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28, Arg29>::get_int_arg( unsigned int num )
{
  if( static_cast<unsigned int>(num) > num_of_args - 1 )
    throw Error( "The arg you wan't to use is out of range" );

  if( num < 0 )
    throw Error( "negativ number for arg number not allowed" );

  if( args[num].is_int ) 
    {
      switch( num )
	{
	  case 0: return get_int(var0);
case 1: return get_int(var1);
case 2: return get_int(var2);
case 3: return get_int(var3);
case 4: return get_int(var4);
case 5: return get_int(var5);
case 6: return get_int(var6);
case 7: return get_int(var7);
case 8: return get_int(var8);
case 9: return get_int(var9);
case 10: return get_int(var10);
case 11: return get_int(var11);
case 12: return get_int(var12);
case 13: return get_int(var13);
case 14: return get_int(var14);
case 15: return get_int(var15);
case 16: return get_int(var16);
case 17: return get_int(var17);
case 18: return get_int(var18);
case 19: return get_int(var19);
case 20: return get_int(var20);
case 21: return get_int(var21);
case 22: return get_int(var22);
case 23: return get_int(var23);
case 24: return get_int(var24);
case 25: return get_int(var25);
case 26: return get_int(var26);
case 27: return get_int(var27);
case 28: return get_int(var28);
case 29: return get_int(var29);

	}
    }
  else
    throw Error( "expecting int arg" );

  return 0; // should never be reached
}

template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
  void Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28, Arg29>::gen_arg_list()
{
  for( unsigned int i = 0; i < num_of_args; i++ )
    { 
      switch( i )
		{
		  case 0:
args[i].is_int = is_int( var0);
args[i].is_string = is_string( var0);
break;
case 1:
args[i].is_int = is_int( var1);
args[i].is_string = is_string( var1);
break;
case 2:
args[i].is_int = is_int( var2);
args[i].is_string = is_string( var2);
break;
case 3:
args[i].is_int = is_int( var3);
args[i].is_string = is_string( var3);
break;
case 4:
args[i].is_int = is_int( var4);
args[i].is_string = is_string( var4);
break;
case 5:
args[i].is_int = is_int( var5);
args[i].is_string = is_string( var5);
break;
case 6:
args[i].is_int = is_int( var6);
args[i].is_string = is_string( var6);
break;
case 7:
args[i].is_int = is_int( var7);
args[i].is_string = is_string( var7);
break;
case 8:
args[i].is_int = is_int( var8);
args[i].is_string = is_string( var8);
break;
case 9:
args[i].is_int = is_int( var9);
args[i].is_string = is_string( var9);
break;
case 10:
args[i].is_int = is_int( var10);
args[i].is_string = is_string( var10);
break;
case 11:
args[i].is_int = is_int( var11);
args[i].is_string = is_string( var11);
break;
case 12:
args[i].is_int = is_int( var12);
args[i].is_string = is_string( var12);
break;
case 13:
args[i].is_int = is_int( var13);
args[i].is_string = is_string( var13);
break;
case 14:
args[i].is_int = is_int( var14);
args[i].is_string = is_string( var14);
break;
case 15:
args[i].is_int = is_int( var15);
args[i].is_string = is_string( var15);
break;
case 16:
args[i].is_int = is_int( var16);
args[i].is_string = is_string( var16);
break;
case 17:
args[i].is_int = is_int( var17);
args[i].is_string = is_string( var17);
break;
case 18:
args[i].is_int = is_int( var18);
args[i].is_string = is_string( var18);
break;
case 19:
args[i].is_int = is_int( var19);
args[i].is_string = is_string( var19);
break;
case 20:
args[i].is_int = is_int( var20);
args[i].is_string = is_string( var20);
break;
case 21:
args[i].is_int = is_int( var21);
args[i].is_string = is_string( var21);
break;
case 22:
args[i].is_int = is_int( var22);
args[i].is_string = is_string( var22);
break;
case 23:
args[i].is_int = is_int( var23);
args[i].is_string = is_string( var23);
break;
case 24:
args[i].is_int = is_int( var24);
args[i].is_string = is_string( var24);
break;
case 25:
args[i].is_int = is_int( var25);
args[i].is_string = is_string( var25);
break;
case 26:
args[i].is_int = is_int( var26);
args[i].is_string = is_string( var26);
break;
case 27:
args[i].is_int = is_int( var27);
args[i].is_string = is_string( var27);
break;
case 28:
args[i].is_int = is_int( var28);
args[i].is_string = is_string( var28);
break;
case 29:
args[i].is_int = is_int( var29);
args[i].is_string = is_string( var29);
break;

		}
	}
}

template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
  std::string Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28, Arg29>::use_arg( unsigned int i, const CFormat &cf )
{
  if( i > num_of_args || i < 0 )
    throw Error( "out of arg range" );

  switch( i )
    {
	  case 0: return x2s( var0,cf);
case 1: return x2s( var1,cf);
case 2: return x2s( var2,cf);
case 3: return x2s( var3,cf);
case 4: return x2s( var4,cf);
case 5: return x2s( var5,cf);
case 6: return x2s( var6,cf);
case 7: return x2s( var7,cf);
case 8: return x2s( var8,cf);
case 9: return x2s( var9,cf);
case 10: return x2s( var10,cf);
case 11: return x2s( var11,cf);
case 12: return x2s( var12,cf);
case 13: return x2s( var13,cf);
case 14: return x2s( var14,cf);
case 15: return x2s( var15,cf);
case 16: return x2s( var16,cf);
case 17: return x2s( var17,cf);
case 18: return x2s( var18,cf);
case 19: return x2s( var19,cf);
case 20: return x2s( var20,cf);
case 21: return x2s( var21,cf);
case 22: return x2s( var22,cf);
case 23: return x2s( var23,cf);
case 24: return x2s( var24,cf);
case 25: return x2s( var25,cf);
case 26: return x2s( var26,cf);
case 27: return x2s( var27,cf);
case 28: return x2s( var28,cf);
case 29: return x2s( var29,cf);

    }

  return "";
}

template <class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
  void Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28, Arg29>::parse()
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

template<class Arg0>
inline std::string format( const std::string & fs,Arg0 var0) {
  return Format::Format<Arg0,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1).get_string();
}

template<class Arg0, class Arg1>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1) {
  return Format::Format<Arg0, Arg1,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 2).get_string();
}

template<class Arg0, class Arg1, class Arg2>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2) {
  return Format::Format<Arg0, Arg1, Arg2,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 3).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 4).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 5).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 6).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 7).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 8).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 9).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 10).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 11).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 12).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 13).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 14).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14,char,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 15).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15,char,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 16).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16,char,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16,0,0,0,0,0,0,0,0,0,0,0,0,0, 17).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17,char,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17,0,0,0,0,0,0,0,0,0,0,0,0, 18).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18,char,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18,0,0,0,0,0,0,0,0,0,0,0, 19).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19,char,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19,0,0,0,0,0,0,0,0,0,0, 20).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20,char,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20,0,0,0,0,0,0,0,0,0, 21).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21,char,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21,0,0,0,0,0,0,0,0, 22).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22,char,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22,0,0,0,0,0,0,0, 23).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23,char,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23,0,0,0,0,0,0, 24).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24,char,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24,0,0,0,0,0, 25).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25,char,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25,0,0,0,0, 26).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25, Arg26 var26) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26,char,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26,0,0,0, 27).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25, Arg26 var26, Arg27 var27) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27,char,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26, var27,0,0, 28).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25, Arg26 var26, Arg27 var27, Arg28 var28) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28,char>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26, var27, var28,0, 29).get_string();
}

template<class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Arg10, class Arg11, class Arg12, class Arg13, class Arg14, class Arg15, class Arg16, class Arg17, class Arg18, class Arg19, class Arg20, class Arg21, class Arg22, class Arg23, class Arg24, class Arg25, class Arg26, class Arg27, class Arg28, class Arg29>
inline std::string format( const std::string & fs,Arg0 var0, Arg1 var1, Arg2 var2, Arg3 var3, Arg4 var4, Arg5 var5, Arg6 var6, Arg7 var7, Arg8 var8, Arg9 var9, Arg10 var10, Arg11 var11, Arg12 var12, Arg13 var13, Arg14 var14, Arg15 var15, Arg16 var16, Arg17 var17, Arg18 var18, Arg19 var19, Arg20 var20, Arg21 var21, Arg22 var22, Arg23 var23, Arg24 var24, Arg25 var25, Arg26 var26, Arg27 var27, Arg28 var28, Arg29 var29) {
  return Format::Format<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12, Arg13, Arg14, Arg15, Arg16, Arg17, Arg18, Arg19, Arg20, Arg21, Arg22, Arg23, Arg24, Arg25, Arg26, Arg27, Arg28, Arg29>( fs, var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26, var27, var28, var29, 30).get_string();
}





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

	template<typename Arg0>
PrintF& operator()( const std::string & fs,const Arg0& var0) {
 if( check() ) out << format( fs,var0);
 return *this;
}
template<typename Arg0, typename Arg1>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1) {
 if( check() ) out << format( fs,var0, var1);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2) {
 if( check() ) out << format( fs,var0, var1, var2);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3) {
 if( check() ) out << format( fs,var0, var1, var2, var3);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23, const Arg24& var24) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23, const Arg24& var24, const Arg25& var25) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23, const Arg24& var24, const Arg25& var25, const Arg26& var26) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26, typename Arg27>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23, const Arg24& var24, const Arg25& var25, const Arg26& var26, const Arg27& var27) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26, var27);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26, typename Arg27, typename Arg28>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23, const Arg24& var24, const Arg25& var25, const Arg26& var26, const Arg27& var27, const Arg28& var28) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26, var27, var28);
 return *this;
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26, typename Arg27, typename Arg28, typename Arg29>
PrintF& operator()( const std::string & fs,const Arg0& var0, const Arg1& var1, const Arg2& var2, const Arg3& var3, const Arg4& var4, const Arg5& var5, const Arg6& var6, const Arg7& var7, const Arg8& var8, const Arg9& var9, const Arg10& var10, const Arg11& var11, const Arg12& var12, const Arg13& var13, const Arg14& var14, const Arg15& var15, const Arg16& var16, const Arg17& var17, const Arg18& var18, const Arg19& var19, const Arg20& var20, const Arg21& var21, const Arg22& var22, const Arg23& var23, const Arg24& var24, const Arg25& var25, const Arg26& var26, const Arg27& var27, const Arg28& var28, const Arg29& var29) {
 if( check() ) out << format( fs,var0, var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15, var16, var17, var18, var19, var20, var21, var22, var23, var24, var25, var26, var27, var28, var29);
 return *this;
}


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

