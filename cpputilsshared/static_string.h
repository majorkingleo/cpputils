/*
 * @author Copyright (c) 2023 Martin Oberzalek
 *		   Copyright (c) 2023 Anton Paar GmbH
 */

#ifndef CPPUTILS_CPPUTILSSHARED_STATIC_STRING_H_
#define CPPUTILS_CPPUTILSSHARED_STATIC_STRING_H_

#include <array>
#include <string>
#include <memory_resource>
#include <stdexcept>

namespace Tools {

/**
 * A string class that uses no heap.
 * The maximum capacity is defined at compile time.
 * Same behavior as std::string.
 *
 */
template <std::size_t N,typename CharT>
class static_basic_string : public std::pmr::basic_string<CharT>
{
	alignas(CharT) std::array<std::byte,sizeof(CharT)*(N+1)> buffer;
	std::pmr::monotonic_buffer_resource mbr{ buffer.data(), buffer.size(), std::pmr::null_memory_resource() };

public:

	typedef typename std::pmr::basic_string<CharT>::value_type value_type;
	typedef typename std::pmr::basic_string<CharT>::reference reference;
	typedef typename std::pmr::basic_string<CharT>::const_reference const_reference;
	typedef typename std::pmr::basic_string<CharT>::size_type size_type;
	typedef typename std::pmr::basic_string<CharT>::difference_type difference_type;
	typedef typename std::pmr::basic_string<CharT>::iterator iterator;
	typedef typename std::pmr::basic_string<CharT>::const_iterator const_iterator;
	typedef typename std::pmr::basic_string<CharT>::reverse_iterator reverse_iterator;
	typedef typename std::pmr::basic_string<CharT>::pointer pointer;
	typedef typename std::pmr::basic_string<CharT>::const_pointer const_pointer;

public:
	static_basic_string()
	: buffer(),
	  mbr( buffer.data(), buffer.size(), std::pmr::null_memory_resource() ),
	  std::pmr::basic_string<CharT>( &mbr )
	  {
		//std::pmr::basic_string<CharT>::reserve(N+1);
	  }

	static_basic_string( const static_basic_string & other )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(other);
	}

	static_basic_string( const pointer other )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(other);
	}

	static_basic_string( std::initializer_list<CharT> ilist )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(ilist.begin(), ilist.end());
	}

	static_basic_string( size_type count, CharT ch )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(count,ch);
	}

	static_basic_string( const static_basic_string& other, size_type pos )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(other,pos);
	}

	static_basic_string( const static_basic_string& other,
	              	  	 size_type pos, size_type count )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(other,pos,count);
	}

	static_basic_string( const CharT* s, size_type count )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(s,count);
	}

	static_basic_string( const CharT* s )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(s);
	}

	template< class InputIt >
	static_basic_string( InputIt first, InputIt last )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(first,last);
	}

	template< class StringViewLike >
	explicit static_basic_string( const StringViewLike& t )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign(t);
	}

	template< class StringViewLike >
	static_basic_string( const StringViewLike& t, size_type pos, size_type n )
	: static_basic_string()
	{
		std::pmr::basic_string<CharT>::assign( t, pos, n );
	}

    static_basic_string( const std::basic_string<CharT> & s )
      : static_basic_string()
    {
      std::pmr::basic_string<CharT>::assign(s);
    }

	constexpr size_type capacity() const {
		return N;
	}

	size_type max_size() const noexcept {
		return N;
	}

	void reserve( size_type new_cap = 0 ) {
		if( new_cap > N ) {
#if __cpp_exceptions > 0
			throw std::out_of_range("value too large");
#else
			std::abort();
#endif
		}

		std::pmr::basic_string<CharT>::reserve(new_cap);
	}

	void resize( size_type count ) {
		if( count > N ) {
#if __cpp_exceptions > 0
			throw std::out_of_range("value too large");
#else
			std::abort();
#endif
		}

		std::pmr::basic_string<CharT>::resize(count);
	}

	const_pointer c_str() const {
		const CharT * cdata = std::pmr::basic_string<CharT>::data();
		CharT * d  = const_cast<CharT*>( cdata );
		*(d + std::pmr::basic_string<CharT>::size()) = 0;

		return d;
	}

	static_basic_string& operator=( const static_basic_string& str ) {
		std::pmr::basic_string<CharT>::assign(str);
		return *this;
	}

	static_basic_string& operator=( const CharT* s ) {
		std::pmr::basic_string<CharT>::assign(s);
		return *this;
	}

	static_basic_string& operator=( CharT ch ) {
		std::pmr::basic_string<CharT>::assign(ch);
		return *this;
	}

	static_basic_string& operator=( std::initializer_list<CharT> ilist ) {
		std::pmr::basic_string<CharT>::assign(ilist);
		return *this;
	}

	template<class StringViewLike>
	static_basic_string& operator=( const StringViewLike& t ) {
		std::pmr::basic_string<CharT>::assign(t);
		return *this;
	}

	operator std::basic_string<CharT> () {
		return std::basic_string<CharT>(std::pmr::basic_string<CharT>::begin(),std::pmr::basic_string<CharT>::end());
	}
};

template <std::size_t N> class static_string : public static_basic_string<N,char> {};
template <std::size_t N> class static_wstring : public static_basic_string<N,wchar_t> {};
#if __cpp_char8_t > 0
template <std::size_t N> class static_u8string : public static_basic_string<N, char8_t> {};
#endif
template <std::size_t N> class static_u16string : public static_basic_string<N,char16_t> {};
template <std::size_t N> class static_u32string : public static_basic_string<N,char32_t> {};


} // namespace Tools

#endif /* CPPUTILS_CPPUTILSSHARED_STATIC_VECTOR_H_ */
