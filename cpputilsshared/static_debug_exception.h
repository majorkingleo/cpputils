/**
 * Base Exception that also reports to stderr when thrown
 * @author Copyright (c) 2022 Martin Oberzalek
 */

#pragma once

#include <string>
#include <exception>
#include <static_format.h>
#include <CpputilsDebug.h>

#define STATIC_DEBUG_EXCEPTION( what ) \
	Tools::StaticDebugException( what, __FILE__, __LINE__ )

namespace Tools {

class StaticDebugException : public std::exception
{
  using string_t =  static_string<100>;

  const string_t err;
  const string_t combined_error;
  const string_t source_file;
  unsigned source_line;
  
 public:
  //StaticDebugException( const char* e, const std::string_view & source_file_ = std::string_view(), unsigned source_line_ = 0 );
  StaticDebugException( const std::string_view & e, const std::string_view & source_file_ = std::string_view(), unsigned source_line_ = 0 );

  // gcc4: error: looser throw specifier for `virtual ~StderrException()`
  // bcause std::exception has: ~exception() throw ()'
  virtual ~StaticDebugException() throw();

  virtual const char* what() const throw() { return combined_error.c_str(); }
  std::string_view get_simple_error() const throw() { return err; }

  static std::string_view get_file_name( const std::string_view & source_file );
};

} // namespace Tools



