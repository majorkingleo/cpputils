/*
 * static_format.cc
 *
 *  Created on: 17.09.2024
 *      Author: martin.oberzalek
 */
#include "static_format.h"
#include <string_adapter.h>

using namespace Tools;

namespace Tools::StaticFormat {

std::span<char> FormatBase::parse( std::span<char> & buffer,
								   std::span<char> & formating_buffer,
								   std::string::size_type num_of_args )
{
  using CFormat = Tools::Format::CFormat;

  if( format.empty() )
    return {};

  unsigned int par = 0;
  unsigned int use_par = 0;
  std::string::size_type pos = 0;
  std::string::size_type  len = format.size();

  span_vector<char> vbuffer(buffer);
  basic_string_adapter<char> s( vbuffer );
  s.clear();


  while( par < num_of_args && pos < len )
    { // while
      bool had_precision = false;

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

      std::string::size_type start = pos - 1;
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
          unsigned int dp = (int)pos;

          while( dp < len && isdigit( format[dp] ) )
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
          if( isdigit( format[pos] ) )
            cf.width = skip_atoi( format, pos, pos );
          else if( format[pos] == '*' )
            {
              pos++;

              // search for the $ digit
              unsigned int dp = (int)pos;

              while( dp < len && isdigit( format[dp] ) )
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
                return std::span<char>(&s[0],s.size());

              had_precision = true;

              if( isdigit( format[pos] ) )
                cf.precision = skip_atoi( format, pos, pos );
              else if( format[pos] == '*' )
                {
                  pos++;


                  // search for the $ digit
                  unsigned int dp = (int)pos;

                  while( dp < len && isdigit( format[dp] ) )
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
          case 'u':
          case 'i':
            cf.numerical_representation = true;
            cf.base = CFormat::DEC;
            if( cf.zero && (cf.adjust != CFormat::LEFT) )
              cf.internal = true;
            break;

          case 'X': cf.setupper = true;
			/* Fallthrough */
          case 'x':
            cf.numerical_representation = true;
            cf.base = CFormat::HEX;
            if( cf.special )
              cf.showbase = true;
            break;

          case 'o':
            cf.numerical_representation = true;
            cf.base = CFormat::OCT;
            if( cf.special )
              cf.showbase = true;
            break;


          case 'E':
            cf.setupper = true;
			/* Fallthrough */

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
            if( cf.precision < 0 ) {
            	// %5.f  => %5.0f
            	if( had_precision ) {
            		cf.precision = 0;
            	} else {
            		cf.precision = 6;
            	}
            }
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
            break;

          case 'c':
            cf.character_representation = true;
            break;

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
    	  span_vector<char> v_formating_buffer(formating_buffer);
    	  basic_string_adapter<char> str( v_formating_buffer );

          int upar = par;

          if( use_par != par )
            upar = use_par;

          if( cf.base == CFormat::HEX && had_precision && cf.special )
            {
              CFormat f2;
              f2.base = cf.base;
              std::string_view ss = use_arg( upar, f2 );
              cf.strlength = (int)ss.size();
              //        printf( "str: %s\n", s.c_str() );
            }

          str = use_arg( upar, cf );

          bool is_string = is_string_arg( upar );
          // std::cout << "had_precision: " << had_precision << " is_string: " << is_string << std::endl;

          // cut string
          if( had_precision && is_string )
            str = str.substr( 0, cf.precision );

          // cut null bytes out of the string
          // can happen when std::string.resize() is called
          // eg: std::string foo="foo";
          //     foo.resize(4);
          //     std::cout << (foo + "bar").c_str();
          //     will result in only the string "foo"
          //     we avoid this by cutting zeor bytes out

          for( std::string::size_type p = 0; p < str.size(); p++ )
            {
              if( str[p] ==  '\0' )
                {
                  str = str.substr( 0, p );
                  break;
                }
            }

          s += std::string_view(str);

          if( use_par == par )
            par++;
        }
      else
        {
          // copy the invalid format string
          for( std::string::size_type  i = start;  i<= pos; i++ )
            if( i < len )
              s += format[i];
        }

      pos++;

    } // while

  if( pos < len )
    {
      s += substitude( format.substr(pos), formating_buffer, "%%", "%" );
    }

  return std::span<char>(&s[0],s.size());
}


int FormatBase::skip_atoi( const std::string_view & s, std::string::size_type start, std::string::size_type & pos ) const
{
  pos = start;
  std::string::size_type len = s.size();

  while( (pos < len) && isdigit( s[pos] ) )
    pos++;

  std::string_view sv = s.substr( start, start-pos );

  int res = 0;

  std::from_chars( sv.data(), sv.data() + sv.size(), res );
  return res;
}

std::string_view FormatBase::substitude( const std::string_view & str_orig,
										 std::span<char> & formating_buffer,
										 const std::string_view & what,
										 const std::string_view & with,
										 std::string::size_type start  ) const
{
	span_vector<char> v_formating_buffer(formating_buffer);
	basic_string_adapter<char> str( v_formating_buffer );

	std::string::size_type pos=start;

	if( what.empty() ) {
		return str_orig;
	}

	str = str_orig;

	for(;;)
	{
	  pos = str.find( what, pos );
	  if( pos == std::string::npos ) {
		break;
	  }

	  if( with.empty() )
		{
		  str.erase( pos, what.size() );
		  continue;
		}
	  else
		{
		  str.replace( pos, what.size(), with );
		}

	  pos += with.size();
	}

  return str;
}

namespace {

template<class T>
void format_int( Tools::StaticFormat::FormatingAdapter<char> & out, const T & value )
{
	auto & cf = out.cf;

	std::to_chars_result res = std::to_chars( out.data(), out.data() + out.size() -1, value, cf.base );

	if( res.ec != std::errc() ) {
		out.clear();
		return;
	}

	const std::size_t len_written = res.ptr - out.data();

	Tools::span_vector<char> vbuffer(out.buffer, len_written );
	Tools::basic_string_adapter<char> s( vbuffer );

	if( cf.special && cf.precision > static_cast<int>(s.size()) ) {
		s.insert(0, cf.precision - s.size(), '0' );
	}

	// prepand 0x in case of hexadezimal output
	if( out.cf.base == Tools::Format::CFormat::HEX
			&& out.cf.special
			&& out.cf.showbase
			/*&& out.cf.adjust == Tools::Format::CFormat::RIGHT */) {
		s.insert(0, "0x" );
	}

	int missing_width = out.cf.width - s.size();
	/*
	if( out.cf.base == Tools::Format::CFormat::HEX &&
		out.cf.showbase ) {
		missing_width -= 2; // 0x
	}*/

	if( cf.width > static_cast<int>(s.size()) ) {
		s.insert(0, missing_width, cf.zero ? '0' : ' ' );
	}

	CPPDEBUG( format( "cf.zero: %d", cf.zero ) );



	if( cf.setupper ) {
		std::transform( s.begin(), s.end(), s.begin(), ::toupper);
	}

	out.resize( s.size() );
}

} // namespace

template <class BaseArgType, class CastTo>
std::span<char> RealArgCastFromInt<BaseArgType,CastTo>::doFormat( const std::span<char> & formating_buffer,
																  const Tools::Format::CFormat & cf )
{
	Tools::span_vector<char> vbuffer(formating_buffer);
	Tools::basic_string_adapter<char> s( vbuffer );

	if( cf.character_representation ) {
		s += static_cast<CastTo>(arg);
		return { s.data(), s.size() };
	}

	FormatingAdapter<char> fa { formating_buffer, cf };
	format_int( fa, arg );

	return fa.buffer;
}

std::span<char> RealArg<char>::doFormat( const std::span<char> & formating_buffer, const Tools::Format::CFormat & cf )
{
	if( cf.numerical_representation ) {

		FormatingAdapter<char> fa { formating_buffer, cf };
		format_int( fa, static_cast<int>(arg) );

		return fa.buffer;
	}

	Tools::span_vector<char> vbuffer(formating_buffer);
	Tools::basic_string_adapter<char> s( vbuffer );

	s += arg;

	return { s.data(), s.size() };
}

std::span<char> RealArg<unsigned char>::doFormat( const std::span<char> & formating_buffer, const Tools::Format::CFormat & cf )
{
	if( cf.numerical_representation ) {

		FormatingAdapter<char> fa { formating_buffer, cf };

		format_int( fa, static_cast<unsigned int>(arg) );

		return fa.buffer;
	}

	Tools::span_vector<char> vbuffer(formating_buffer);
	Tools::basic_string_adapter<char> s( vbuffer );

	s += arg;

	return { s.data(), s.size() };
}

} // namespace Tools::StaticFormat


static Tools::StaticFormat::RealArgCastFromInt<int,char>            dummy1(0);
static Tools::StaticFormat::RealArgCastFromInt<unsigned int,char>   dummy2(0);
static Tools::StaticFormat::RealArgCastFromInt<short,char>          dummy3(0);
static Tools::StaticFormat::RealArgCastFromInt<unsigned short,char> dummy4(0);
static Tools::StaticFormat::RealArgCastFromInt<long,char>           dummy5(0);
static Tools::StaticFormat::RealArgCastFromInt<unsigned long,char>  dummy6(0);


Tools::StaticFormat::FormatingAdapter<char> & operator<<( Tools::StaticFormat::FormatingAdapter<char> & out, const std::string_view & s )
{
	Tools::span_vector<char> vbuffer(out.buffer);
	Tools::basic_string_adapter<char> str( vbuffer );

	str += s;

	out.buffer = { str.data(), str.size() };

	return out;
}

Tools::StaticFormat::FormatingAdapter<char> & operator<<( Tools::StaticFormat::FormatingAdapter<char> & out, const char * s )
{
	Tools::span_vector<char> vbuffer(out.buffer);
	Tools::basic_string_adapter<char> str( vbuffer );

	str += s;

	out.buffer = { str.data(), str.size() };

	return out;
}

namespace {

template<class T>
void format_double( Tools::StaticFormat::FormatingAdapter<char> & out, const T & value )
{
	std::chars_format fmt = std::chars_format::general;

	if( out.cf.base == Tools::Format::CFormat::HEX ) {
		fmt = std::chars_format::hex;
	}

	switch( out.cf.floating ) {
		case Tools::Format::CFormat::FIXED: fmt |= std::chars_format::fixed; break;
		case Tools::Format::CFormat::SCIENTIFIC: fmt |= std::chars_format::scientific; break;
	}

	int precision = out.cf.precision;
	if( precision < 0 ) {
		precision = out.cf.default_precision;
	}

	std::to_chars_result res = std::to_chars( out.data(), out.data() + out.size() -1,
			value,
			fmt,
			precision );

	if( res.ec != std::errc() ) {
		out.clear();
		return;
	}

	std::size_t len_written = res.ptr - out.buffer.data();

	Tools::span_vector<char> vbuffer(out.buffer,len_written);
	Tools::basic_string_adapter<char> str( vbuffer );


	// sprintf( buffer, "%f", 155.1 ) );
	// default printf precision is 6, so correct it, of less zeros are generated
	std::string::size_type pos_comma = str.find('.');
	std::size_t written_precision = str.size() - (pos_comma + 1);

	CPPDEBUG( format( "pos_comma: %d written_precision: %d", pos_comma, written_precision) );

	if( (pos_comma != std::string::npos) && (written_precision < precision) ) {
		str.insert(str.end(), precision - written_precision, '0' );
	}

	int missing_width = out.cf.width - str.size();

	if( out.cf.base == Tools::Format::CFormat::HEX && out.cf.showbase ) {
		missing_width -= 2; // minus 0x
	}

	/* sprintf( buffer, "%05.f", 3.32 ) */
	if( out.cf.width > static_cast<int>(str.size()) ) {
		str.insert(0, missing_width, out.cf.zero ? '0' : ' ' );
	}

	// prepand 0x in case of hexadezimal output
	if( out.cf.base == Tools::Format::CFormat::HEX && out.cf.showbase ) {
		if( out.cf.setupper ) {
			str.insert(0, "0x" );
		} else {
			str.insert(0, "0X" );
		}
	}
/*
	if( cf.setupper ) {
		std::transform( s.begin(), s.end(), s.begin(), ::toupper);
	}

	if( cf.width > s.size() ) {
		s.insert(0, cf.width - s.size(), cf.zero ? '0' : ' ' );
	}
	*/

	out.resize(str.size());
}

} // namespace

Tools::StaticFormat::FormatingAdapter<char> & operator<<( Tools::StaticFormat::FormatingAdapter<char> & out, const double & value )
{
	format_double( out, value );
	return out;
}
