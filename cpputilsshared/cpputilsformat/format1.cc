#include "format1.h"
#include <cstdlib>

namespace Tools {

int Format::skip_atoi( std::string s, ST start, ST& pos )
{
  pos = start;
  ST len = s.size();
  
  while( (pos < len) && isdigit( s[pos] ) )
    pos++;
  
  return atoi( s.substr( start, start-pos ).c_str() );
}


std::string Format::substitude( const std::string & str_orig, const std::string & what, const std::string & with, std::string::size_type start )
{
  std::string str( str_orig );
  std::string::size_type pos=start;

  if( what.empty() )
      return str;

  for(;;)
    {
      pos = str.find( what, pos );
      if( pos == std::string::npos )
        break;

      if( with.empty() )
    {
      std::string s = str.substr( 0, pos );
      s += str.substr( pos + what.size() );
      str = s;
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
} /* namespace Tools */

