#include "OutDebug.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
#include <DetectLocale.h>
#include <filesystem>

#ifndef DISABLE_CPPUTILS_DETECT_LOCALE

using namespace Tools;

OutDebug::OutDebug(  ColoredOutput::Color color_ )
: Debug(),
  ColoredOutput(),
  color( color_ ),
  prefix(),
  print_line_and_file_info( true )
{

}

#ifdef __cpp_lib_string_view
using string_type_t = std::string_view;
#else
using string_type_t = std::string;
#endif

void OutDebug::add( const char *file, unsigned line, const char *function, const string_type_t & s )
{
	if( print_line_and_file_info ) {

		std::filesystem::path f( file );

		std::cout << color_output( color, f.filename().string() );
		std::cout << ':' << line
				  << " ";
	}

	if( !prefix.empty() ) {
		 std::cout << color_output( color, DetectLocale::w2out(prefix) );
		 std::cout << " ";
	}

	std::cout << s << '\n';
}
#ifdef __cpp_lib_string_view
void OutDebug::add( const char *file, unsigned line, const char *function, const std::wstring_view & s )
{
	add( file, line, function, DetectLocale::w2out(std::wstring(s)) );
}
#else
void OutDebug::add( const char *file, unsigned line, const char *function, const std::wstring & s )
{
	add( file, line, function, DetectLocale::w2out(s) );
}
#endif

#endif
