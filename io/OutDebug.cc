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


void OutDebug::add( const char *file, unsigned line, const char *function, const std::string & s )
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

void OutDebug::add( const char *file, unsigned line, const char *function, const std::wstring & s )
{
	add( file, line, function,  DetectLocale::w2out(s) );
}

#endif
