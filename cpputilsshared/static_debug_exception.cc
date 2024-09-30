#include "static_debug_exception.h"
#include <string.h>

using namespace Tools;
/*
StaticDebugException::StaticDebugException( const char* e, const std::string_view & source_file_ , unsigned source_line_ )
: std::exception(),
  err( e ),
	combined_error( Tools::static_format<100>( "%s:%d %s", get_file_name(source_file_), source_line_, err )),
	source_file(source_file_),
	source_line(source_line_)
{
	CPPDEBUG( static_format<100>( "Exception from: %s", combined_error ) );
}
*/

StaticDebugException::StaticDebugException( const std::string_view & e, const std::string_view & source_file_, unsigned source_line_ )
  : std::exception(),
    err( e ),
	combined_error( (std::string_view)Tools::static_format<100>( "%s:%d %s", get_file_name(source_file_), source_line_, err )),
	source_file(source_file_),
	source_line(source_line_)
{
	CPPDEBUG( static_format<100>( "Exception from: %s", combined_error ) );
}

StaticDebugException::~StaticDebugException() throw()
{

}

std::string_view StaticDebugException::get_file_name( const std::string_view & filename )
{
	std::string::size_type p = filename.find_last_of( "/\\");

	if( p == std::string::npos ) {
		return filename;
	}

	return filename.substr(p+1);
}
