/**
 * utf conversion utilities
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */

#ifndef CPPUTILS_CPPUTILSSHARED_UTF8_UTIL_H_
#define CPPUTILS_CPPUTILSSHARED_UTF8_UTIL_H_

#include <string>

#if __cplusplus >= 201103

#if __cplusplus >= 2020
# define CPPUTILS_CPPUTILSSHARED_U8STRING_AVAILABLE
# if __GNUC__ <= 9
#  undef CPPUTILS_CPPUTILSSHARED_U8STRING_AVAILABLE
#   if _GLIBCXX_USE_CHAR8_T
#     define CPPUTILS_CPPUTILSSHARED_U8STRING_AVAILABLE
#   endif
# endif
#endif

namespace Tools {

class Utf8Util
{
public:
	static bool isUtf8( const std::string & text );

#if defined(_BSD_SOURCE) || defined(_SVID_SOURCE) || defined(_XOPEN_SOURCE)
	static bool isAsciiOnly( const std::string & text );
#endif

	static std::wstring utf8toWString( const std::string & text );

#if CPPUTILS_CPPUTILSSHARED_U8STRING_AVAILABLE
	static std::wstring utf8toWString( const std::u8string & text );
	static std::wstring toWcharString16( const std::u8string & text );
	static std::wstring toWcharString32( const std::u8string & text );
#endif

	static std::wstring toWcharString16( const std::string & text );
	static std::wstring toWcharString32( const std::string & text );

	static std::string wStringToUtf8( const std::wstring & text );

	static std::string utf16toString( const std::wstring & text );
	static std::string utf32toString( const std::wstring & text );
};

} // namespace Tools

#endif

#endif /* CPPUTILS_CPPUTILSSHARED_UTF8_UTIL_H_ */
