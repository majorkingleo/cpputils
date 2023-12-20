#include "stderr_exception.h"
#include <string.h>

using namespace Tools;

const char *StderrException::get_file_name( const char *filename )
{
 const char *p = filename + strlen(filename) - 1;
    while( *p != '/' && *p != '\\'  && p > filename )  {
          p--;
    }
    return ++p;
}
