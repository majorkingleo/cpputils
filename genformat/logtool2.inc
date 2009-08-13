#ifndef SCPP_LOGTOOL_HH
#define SCPP_LOGTOOL_HH

/**********************************************************************
+*  VERSID:    "%Z% @(#)$Header$";
+*  FILE:      logtool.hh
+*  CONTENTS:  logtool C++ template
+*  PURPOSE:
+*  NOTES:
+*  $Log$
+*  Revision 1.1  2009/04/08 15:53:03  wamas
+*  MOBERZA Format und Logtool auf 30 Argumente erweitert.
+*  Über das genformat Tool erweiterbar.
+*
+*  Revision 1.4  2009/03/27 10:55:22  wamas
+*  Fixes and extensions
+*
+*  Revision 1.3  2008/12/23 10:23:46  wamas
+*  Bugfix: Format output must be written to "%s" in LogPrintf
+*  user: mmattl
+*
+*  Revision 1.2  2008/09/02 12:16:52  wamas
+*  Umbau auf cpputils
+*
+*  Revision 1.1.1.1  2008/08/14 14:11:32  moberza
+*  Initial import
+*
+*  Revision 1.1  2008/02/25 10:46:07  wamas
+*  MOBERZA TS-115979 Menü Transporteinheiten Erstellen hinzugefuegt,
+*  sowie wood und andere C++ Dateien
+*
+*  Revision 1.1.2.1  2006/05/11 08:27:35  wamas
+*  Toolsbox Umstellung
+*
+*  Revision 1.1.2.1  2006/01/21 23:19:31  wamas
+*  C++ LogPrintf version added
+*
+*
+*
+**********************************************************************/


#include <format.h>

#ifndef NOWAMAS

extern "C" {
#include <logtool.h>
}

#ifdef LogPrintf
#undef LogPrintf
#endif

#else

enum {
  LT_DEBUG,
  LT_ALERT,
  LT_NOTIFY,
  LT_TRACE
};

int _LogSetLocation( const char *file, int line );
int _LogPrintf( const char *fac, int level, const char *str , const char *str2 );

#endif

#define LogPrintf _LogSetLocation(__FILE__,__LINE__)?0:_CPPLogPrintf

GENLOGPRINTF
/*
template <class A, class B, class C, class D, class E, class F, class G, class H>
  int _CPPLogPrintf( const std::string & fac, int level, std::string fs, A a, B b, C c, D d, E e, F f, G g, H h )
{
  return _LogPrintf( fac.c_str(), level, "%s", Tools::Format::Format<A,B,C,D,E,F,G,H>( fs, a, b, c, d, e, f, g, h, 8).get_string().c_str() );
}
*/

int _CPPLogPrintf( const std::string & fac, int level, std::string fs );

#endif

