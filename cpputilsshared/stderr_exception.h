/**
 * Base Exception that also reports to stderr when thrown
 * @author Copyright (c) 2022 Martin Oberzalek
 */

#ifndef TOOLS_STDERR_EXCEPTION_h
#define TOOLS_STDERR_EXCEPTION_h

#include <string>
#include <exception>
#include <format.h>

#define STDERR_EXCEPTION( what ) \
	Tools::StderrException( what, __FILE__, __LINE__ )

namespace Tools {

class StderrException : public std::exception
{
  const std::string err;
  const std::string combined_error;
  const std::string source_file;
  unsigned source_line;
  
 public:
  StderrException( const char* e, const std::string & source_file_ = std::string(), unsigned source_line_ = 0 )
  : std::exception(),
    err( e ),
	combined_error( Tools::format( "%s:%d %s", get_file_name(source_file_.c_str()), source_line_, err )),
	source_file(source_file_),
	source_line(source_line_)
  {
	  std::cerr << "Exception from: " << combined_error << std::endl;
  }

  StderrException( const std::string & e, const std::string & source_file_ = std::string(), unsigned source_line_ = 0 )
  : std::exception(),
    err( e ),
	combined_error( Tools::format( "%s:%d %s", get_file_name(source_file_.c_str()), source_line_, err )),
	source_file(source_file_),
	source_line(source_line_)
  {
	  std::cerr << "Exception from: " << combined_error << std::endl;
  }


  // gcc4: error: looser throw specifier for `virtual ~StderrException()`
  // bcause std::exception has: ~exception() throw ()'
  virtual ~StderrException() throw() {}

  virtual const char* what() const throw() { return combined_error.c_str(); }
  const std::string & get_simple_error() const throw() { return err; }

  static const char *get_file_name( const char *filename );
};

} // namespace Tools

#endif


