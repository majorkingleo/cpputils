/*
 * @author Copyright (c) 2023 Martin Oberzalek
 *		   Copyright (c) 2023 Anton Paar GmbH
 */

#ifndef CPPUTILS_CPPUTILSSHARED_STATIC_STRING_H_
#define CPPUTILS_CPPUTILSSHARED_STATIC_STRING_H_

#include <array>
#include <string>
#include "static_vector.h"
#include <stdexcept>
#include <functional>
#include <cstring>
#include <ostream>

#include <CpputilsDebug.h>
#include <format.h>

namespace Tools {

class static_string_out_of_range_cut
{
public:
	void operator() ( std::size_t size ) {

	};
};

class static_string_out_of_range_except
{
public:
	void operator()( std::size_t size ) {
#if __cpp_exceptions > 0
			throw std::out_of_range("value too large");
#else
			std::abort();
#endif
	};
};

/**
 * A string class that uses no heap.
 * The maximum capacity is defined at compile time.
 * Same behavior as std::string.
 *
 */
template <std::size_t N,typename CharT, typename out_of_range_functor = static_string_out_of_range_cut>
class static_basic_string
{
	mutable Tools::static_vector<CharT,N+1> data;
	out_of_range_functor out_of_range;

public:

	typedef typename std::basic_string<CharT>::value_type value_type;
	typedef typename std::basic_string<CharT>::reference reference;
	typedef typename std::basic_string<CharT>::const_reference const_reference;
	typedef typename std::basic_string<CharT>::size_type size_type;
	typedef typename static_vector<CharT,N+1>::difference_type difference_type;
	typedef typename static_vector<CharT,N+1>::iterator iterator;
	typedef typename static_vector<CharT,N+1>::const_iterator const_iterator;
	typedef typename static_vector<CharT,N+1>::reverse_iterator reverse_iterator;
	typedef typename static_vector<CharT,N+1>::const_reverse_iterator const_reverse_iterator;
	typedef typename std::basic_string<CharT>::pointer pointer;
	typedef typename std::basic_string<CharT>::const_pointer const_pointer;

	static const size_type npos = std::basic_string<CharT>::npos;

public:
	static_basic_string()
	: data()
	  {
	  }

	static_basic_string( const static_basic_string & other )
	: static_basic_string()
	{
		assign(other);
	}

	static_basic_string( const pointer other )
	: static_basic_string()
	{
		assign(other);
	}

	static_basic_string( std::initializer_list<CharT> ilist )
	: static_basic_string()
	{
		assign(ilist.begin(), ilist.end());
	}

	static_basic_string( size_type count, CharT ch )
	: static_basic_string()
	{
		assign(count,ch);
	}

	static_basic_string( const static_basic_string& other, size_type pos )
	: static_basic_string()
	{
		assign(other,pos);
	}

	static_basic_string( const static_basic_string& other,
	              	  	 size_type pos, size_type count )
	: static_basic_string()
	{
		assign(other,pos,count);
	}

	static_basic_string( const CharT* s, size_type count )
	: static_basic_string()
	{
		assign(s,count);
	}

	static_basic_string( const CharT* s )
	: static_basic_string()
	{
		assign(s);
	}

	template< class InputIt >
	static_basic_string( InputIt first, InputIt last )
	: static_basic_string()
	{
		assign(first,last);
	}

	explicit static_basic_string( const std::basic_string_view<CharT> & t )
	: static_basic_string()
	{
		assign(t);
	}

	static_basic_string( const std::basic_string_view<CharT>& t, size_type pos, size_type n )
	: static_basic_string()
	{
		assign( t, pos, n );
	}

    static_basic_string( const std::basic_string<CharT> & s )
      : static_basic_string()
    {
      assign(s);
    }

	constexpr size_type capacity() const {
		return N;
	}

	size_type max_size() const noexcept {
		return N;
	}

	void reserve( size_type new_cap = 0 ) {
		if( new_cap > N ) {
			out_of_range( new_cap );
		}
	}

	void resize( size_type count ) {
		if( count > N ) {
			out_of_range( count );
			count = N;
		}

		data.resize(count);
	}

	void resize( size_type count, CharT ch ) {
		if( count > N ) {
			out_of_range( count );
			count = N;
		}

		data.resize(count, ch );
	}

	const_pointer c_str() const {
		data[data.size()] = '\0';
		return data.data();
	}

	static_basic_string& operator=( const static_basic_string& str ) {
		assign(str);
		return *this;
	}

	static_basic_string& operator=( const CharT* s ) {
		assign(s);
		return *this;
	}

	static_basic_string& operator=( CharT ch ) {
		assign(ch);
		return *this;
	}

	static_basic_string& operator=( std::initializer_list<CharT> ilist ) {
		assign(ilist);
		return *this;
	}

	static_basic_string& operator=( const std::basic_string_view<CharT>& t ) {
		assign(t);
		return *this;
	}

	static_basic_string& operator=( const std::basic_string<CharT>& t ) {
		assign(t);
		return *this;
	}

	operator std::basic_string<CharT> () {
		return std::basic_string<CharT>(data.begin(),data.end());
	}

	static_basic_string& assign( size_type count, CharT ch ) {
		resize(count);

		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = ch;
		}
		return *this;
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	static_basic_string& assign( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		resize(str.size());
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = str[i];
		}
		return *this;
	}

	static_basic_string& assign( const std::basic_string<CharT> & str ) {
		resize(str.size());
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = str[i];
		}
		return *this;
	}

	static_basic_string& assign( const std::basic_string_view<CharT> & str ) {
		resize(str.size());
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = str[i];
		}
		return *this;
	}

	template<std::size_t N2>
	static_basic_string& assign( const static_basic_string<N2,CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv( str.begin() + pos, count );
		assign(sv);
		return *this;
	}

	static_basic_string& assign( const std::basic_string<CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv( str.begin() + pos, count );
		assign(sv);
		return *this;
	}

	static_basic_string& assign( const std::basic_string_view<CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv = str.substr( pos, count == npos ? npos : pos + count );
		assign(sv);
		return *this;
	}


	static_basic_string& assign( const CharT* s, size_type count ) {
		resize(count);
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = s[i];
		}
		return *this;
	}

	static_basic_string& assign( const CharT* s ) {
		std::size_t len = std::strlen(s);
		assign( s, len );
		return *this;
	}

	template< class InputIt >
	static_basic_string& assign( InputIt first, InputIt last ) {

		data.clear();
		resize( std::distance(first,last) );

		size_type i = 0;
		for( auto it = first; it != last && i < size(); ++it, ++i ) {
			data[i] = *it;
		}

		return *this;
	}

	static_basic_string& assign( std::initializer_list<CharT> ilist ) {

		CPPDEBUG( "assign4" );

		data.clear();
		resize(ilist.size());

		size_type i = 0;
		for( auto it = ilist.begin(); it != ilist.end() && i < size(); ++it, ++i ) {
			data[i] = *it;
		}

		return *this;
	}

	CharT& at( size_type pos ) {
		return data.at(pos);
	}

	const CharT& at( size_type pos ) const {
		return data.at(pos);
	}

	CharT& operator[]( size_type pos ) {
		return data[pos];
	}

	const CharT& operator[]( size_type pos ) const {
		return data[pos];
	}

	CharT& front() {
		return data[0];
	}

	const CharT& front() const {
		return data[0];
	}

	CharT& back() {
		return data[size() - 1];
	}

	const CharT& back() const {
		return data[size() - 1];
	}


	operator std::basic_string_view<CharT>() const noexcept {
		return std::basic_string_view<CharT>(data.data(),data.size());
	}

	iterator begin() {
		return data.begin();
	}

	const_iterator begin() const {
		return data.begin();
	}

	const_iterator cbegin() const noexcept {
		return data.cbegin();
	}

	iterator end() {
		return data.end();
	}

	const_iterator end() const {
		return data.end();
	}

	reverse_iterator rbegin() {
		return data.rbegin();
	}

	const_reverse_iterator rbegin() const {
		return data.rbegin();
	}

	bool empty() const {
		return data.empty();
	}

	size_type size() const {
		return data.size();
	}

	size_type length() const {
		return data.size();
	}

	void shrink_to_fit() {}

	void clear() {
		data.clear();
	}

	static_basic_string& insert( size_type index, size_type count, CharT ch ) {
		data.insert(index,count,ch);
		return *this;
	}

	static_basic_string& erase( size_type index = 0, size_type count = npos ) {
		data.erase(index,count);
		return *this;
	}

	iterator erase( iterator position ) {
		return data.erase( position );
	}

	iterator erase( const_iterator position ) {
		return data.erase(position);
	}

	iterator erase( iterator first, iterator last ) {
		return data.erase( first, last );
	}

	iterator erase( const_iterator first, const_iterator last ) {
		return data.erase( first, last );
	}

	void push_back( CharT ch ) {

		if( data.size() == N ) {
			out_of_range(1);
			return;
		}

		data.push_back( ch );
	}

	void pop_back() {
		data.pop_back();
	}

	static_basic_string& append( size_type count, CharT ch ) {

		if( count + data.size() > N) {
			out_of_range( (count + data.size()) - N );
			count = N - data.size();
		}
		data.append(count,ch);
		return *this;
	}

	static_basic_string& append( const std::basic_string<CharT>& str ) {
		return append( std::basic_string_view<CharT>(str.begin(), str.end() ) );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& append( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		return append( std::basic_string_view<CharT>(str.begin(), str.end() ) );
	}

	static_basic_string& append( const std::basic_string<CharT>& str,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( str.begin(), str.end() ).substr(pos,count) );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& append( const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( str.begin(), str.end() ).substr(pos,count) );
	}

	static_basic_string& append( const CharT* s, size_type count ) {
		return append( std::basic_string_view<CharT>( s, count ) );
	}

	static_basic_string& append( const CharT* s ) {
		return append( std::basic_string_view<CharT>( s, std::strlen(s) ) );
	}

	static_basic_string& append( const std::basic_string_view<CharT>& str ) {

		std::size_t len_to_copy = str.size();
		if( len_to_copy + size() > N ) {
			out_of_range( (len_to_copy + size()) - N );
			len_to_copy = size() - N;
		}

		const std::size_t pos_to_start = size();
		resize(size() + len_to_copy);

		auto it = str.begin();
		for( size_type i = pos_to_start; i < data.size(), it != str.end(); i++, ++it ) {
			data[i] = *it;
		}

		return *this;
	}

	static_basic_string& append( const std::basic_string_view<CharT>& t,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( t ).substr( pos, count ) );
	}

	template< class InputIt >
	static_basic_string& append( InputIt first, InputIt last ) {

		std::size_t len_to_copy = std::distance(first, last);

		if( len_to_copy + size() > N ) {
			out_of_range( (len_to_copy + size()) - N );
			len_to_copy = size() - N;
		}

		const std::size_t pos_to_start = size();
		resize(size() + len_to_copy);

		auto it = first;
		for( size_type i = pos_to_start; i < data.size(); i++, ++it ) {
			data[i] = *it;
		}
		return *this;
	}

	static_basic_string& append( std::initializer_list<CharT> ilist ) {
		return append( ilist.begin(), ilist.end() );
	}

	static_basic_string& operator+=( const std::basic_string<CharT>& str ) {
		return append( str.begin(), str.end() );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& operator+=( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		return append( str.begin(), str.end() );
	}

	static_basic_string& operator+=( CharT ch ) {
		push_back( ch );
		return *this;
	}

	static_basic_string& operator+=( const CharT* s ) {
		return append( s );
	}

	static_basic_string& operator+=( std::initializer_list<CharT> ilist ) {
		return append( ilist );
	}

	static_basic_string& operator+=( const std::basic_string_view<CharT>& str ) {
		return append( str );
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       const std::basic_string<CharT>& str ) {
		auto it_start = begin() + pos;
		auto it_end = begin() + std::min(pos + count, size());

		static_basic_string left_over( it_end, end() );

		resize( std::distance( begin(), it_start ) );
		append( str );
		append( left_over );

		return *this;
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       const std::basic_string_view<CharT>& str ) {
		auto it_start = begin() + pos;
		auto it_end = begin() + std::min(pos + count, size());

		static_basic_string left_over( it_end, end() );

		resize( std::distance( begin(), it_start ) );
		append( str );
		append( left_over );

		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& replace( size_type pos, size_type count,
	                       const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		auto it_start = begin() + pos;
		auto it_end = begin() + std::min(pos + count, size());

		static_basic_string left_over( it_end, end() );

		resize( std::distance( begin(), it_start ) );
		append( str );
		append( left_over );

		return *this;
	}

	static_basic_string& replace( const_iterator first, const_iterator last,
	                       const std::basic_string<CharT>& str ) {

		static_basic_string left_over( last, end() );

		resize( std::distance( begin(), first ) );
		append( str );
		append( left_over );

		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& replace( const_iterator first, const_iterator last,
							const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {

		static_basic_string left_over( last, end() );

		resize( std::distance( begin(), first ) );
		append( str );
		append( left_over );

		return *this;
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       const std::basic_string<CharT>& str,
	                       size_type pos2, size_type count2 = npos ) {

		std::basic_string_view<CharT> sv = str;
		return replace( pos, count, sv.substr(pos2,count2) );
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       const std::basic_string_view<CharT>& str,
	                       size_type pos2, size_type count2 = npos ) {

		return replace( pos, count, str.substr(pos2,count2) );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& replace( size_type pos, size_type count,
							const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                       size_type pos2, size_type count2 = npos ) {
		return replace( pos, count, str.substr(pos2,count2) );
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       const CharT* cstr, size_type count2 ) {
		return replace( pos, count, std::basic_string_view<CharT>( cstr, count2 ) );
	}

	static_basic_string& replace( const_iterator first, const_iterator last,
	                       const CharT* cstr, size_type count2 ) {
		return replace( first, last, std::basic_string_view<CharT>( cstr, count2 ) );
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       const CharT* cstr ) {
		return replace( pos, count, std::basic_string_view<CharT>( cstr, std::strlen(cstr) ) );
	}

	static_basic_string& replace( const_iterator first, const_iterator last,
	                       const CharT* cstr ) {
		return replace( first, last, std::basic_string_view<CharT>( cstr, std::strlen(cstr) ) );
	}

	static_basic_string& replace( size_type pos, size_type count,
	                       size_type count2, CharT ch ) {
		return replace( pos, count, static_basic_string( count2, ch ) );
	}


	static_basic_string& replace( const_iterator first, const_iterator last,
	                       size_type count2, CharT ch ) {
		return replace( first, last, static_basic_string( count2, ch ) );
	}

	template< class InputIt >
	static_basic_string& replace( const_iterator first, const_iterator last,
	                       InputIt first2, InputIt last2 ) {
		return replace( first, last, std::basic_string_view<CharT>(first2,last2) );
	}

	static_basic_string& replace( const_iterator first, const_iterator last,
	                       std::initializer_list<CharT> ilist ) {
		return replace( first, last, std::basic_string_view<CharT>(ilist) );
	}


	size_type copy( CharT* dest, size_type count, size_type pos = 0 ) const {
		if( count == npos ) {
			count = size() - pos;
		}

		if( pos > size ) {
#if __cpp_exceptions > 0
			throw std::out_of_range( "invalid index");
#else
			std::abort();
#endif
		}

		std::memcpy( dest, data.data() + pos, count );
		return count;
	}

	void swap( static_basic_string& other ) {
		data.swap(other.data);
	}

	size_type find( const std::basic_string<CharT>& str, size_type pos = 0 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.find( str, pos );
	}

	size_type find( const CharT* s, size_type pos, size_type count ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.find( s, pos, count );
	}

	size_type find( const CharT* s, size_type pos = 0 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.find( s, pos );
	}

	size_type find( CharT ch, size_type pos = 0 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.find( ch, pos );
	}

	size_type find( const std::basic_string_view<CharT>& str, size_type pos = 0 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.find( str, pos );
	}

	size_type rfind( const std::basic_string<CharT>& str, size_type pos = npos ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.rfind( str, pos );
	}



	size_type rfind( const CharT* s, size_type pos, size_type count ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.rfind( s, pos, count );
	}

	size_type rfind( const CharT* s, size_type pos = npos ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.rfind( s, pos );
	}

	size_type rfind( CharT ch, size_type pos = npos ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.rfind( ch, pos );
	}

	size_type rfind( const std::basic_string_view<CharT>& str, size_type pos = npos ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.rfind( str, pos );
	}

	int compare( const std::basic_string<CharT>& str ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare(str);
	}

	int compare( const std::basic_string_view<CharT>& str ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare(str);
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	int compare( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare(str);
	}

	int compare( size_type pos1, size_type count1,
	             const std::basic_string<CharT>& str,
	             size_type pos2, size_type count2 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( pos1, count1, str, pos2, count2 = npos );
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	int compare( size_type pos1, size_type count1,
		             const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
		             size_type pos2, size_type count2 ) const {
			std::basic_string_view<CharT> sv(*this);
			return sv.compare( pos1, count1, str, pos2, count2 = npos );
	}

	int compare( const CharT* s ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( s );
	}

	int compare( size_type pos1, size_type count1,
	             const CharT* s ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( pos1, count1, s );
	}

	int compare( size_type pos1, size_type count1,
	             const CharT* s, size_type count2 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( pos1, count1, s, count2 );
	}


	int compare( size_type pos1, size_type count1,
	             const std::basic_string_view<CharT>& t ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( pos1, count1, t );
	}

	int compare( size_type pos1, size_type count1,
	             const std::basic_string_view<CharT>& t,
	             size_type pos2, size_type count2 = npos) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( pos1, count1, t, pos2, count2 );
	}

	constexpr bool
	    starts_with( std::basic_string_view<CharT> sv ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.starts_with( sv );
	}

	constexpr bool
	    starts_with( CharT ch ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.starts_with( ch );
	}

	constexpr bool
	    starts_with( const CharT* s ) const {
		std::basic_string_view<CharT> me(*this);
		return me.starts_with( s );
	}

	constexpr bool
	    ends_with( std::basic_string_view<CharT> sv ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.ends_with( sv );
	}

	constexpr bool
	    ends_with( CharT ch ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.ends_with( ch );
	}

	constexpr bool
	    ends_with( const CharT* s ) const {
		std::basic_string_view<CharT> me(*this);
		return me.ends_with( s );
	}

//#if __cpp_lib_string_contains >= 202011L
	constexpr bool
	    contains( std::basic_string_view<CharT> sv ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.contains( sv );
	}

	constexpr bool
	    contains( CharT ch ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.contains( ch );
	}

	constexpr bool
	    contains( const CharT* s ) const {
		std::basic_string_view<CharT> me(*this);
		return me.contains( s );
	}
//#endif

	static_basic_string substr( size_type pos = 0, size_type count = npos ) const {
		return static_basic_string( std::basic_string_view<CharT>(*this).substr(pos,count) );
	}
};

template <std::size_t N> class static_string : public static_basic_string<N,char> {
	// forward constructors
	using base = static_basic_string<N,char>;
	using base::base;
};

template <std::size_t N> class static_wstring : public static_basic_string<N,wchar_t> {
	// forward constructors
	using base = static_basic_string<N,wchar_t>;
	using base::base;
};

#if __cpp_char8_t > 0
template <std::size_t N> class static_u8string : public static_basic_string<N, char8_t> {
	// forward constructors
	using base = static_basic_string<N,char8_t>;
	using base::base;
};
#endif

template <std::size_t N> class static_u16string : public static_basic_string<N,char16_t> {
	// forward constructors
	using base = static_basic_string<N,char16_t>;
	using base::base;
};

template <std::size_t N> class static_u32string : public static_basic_string<N,char32_t> {
	// forward constructors
	using base = static_basic_string<N,char32_t>;
	using base::base;
};


template<std::size_t N1,typename CharT, typename out_of_range_functor1,std::size_t N2, typename out_of_range_functor2>
bool operator==( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const static_basic_string<N2,CharT,out_of_range_functor2>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	std::basic_string_view<CharT> b(rhs);
	return a == b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator==( const CharT* lhs,
                 const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs == b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator==( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
				 const CharT* rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a == rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator==( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
				 const std::basic_string_view<CharT> & rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a == rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator==( const std::basic_string_view<CharT> & lhs,
				 const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs == b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1,std::size_t N2, typename out_of_range_functor2>
bool operator<( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const static_basic_string<N2,CharT,out_of_range_functor2>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	std::basic_string_view<CharT> b(rhs);
	return a < b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string_view<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a < rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a < rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<(  const std::basic_string<CharT>& lhs,
				const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs < b;
}


template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const CharT * rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a < rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<( const CharT * lhs,
		        const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs < b;
}




template<std::size_t N1,typename CharT, typename out_of_range_functor1,std::size_t N2, typename out_of_range_functor2>
bool operator<=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const static_basic_string<N2,CharT,out_of_range_functor2>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	std::basic_string_view<CharT> b(rhs);
	return a <= b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string_view<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a <= rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a <= rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<=(  const std::basic_string<CharT>& lhs,
				const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs <= b;
}


template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const CharT * rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a <= rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator<=( const CharT * lhs,
		        const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs <= b;
}




template<std::size_t N1,typename CharT, typename out_of_range_functor1,std::size_t N2, typename out_of_range_functor2>
bool operator>( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const static_basic_string<N2,CharT,out_of_range_functor2>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	std::basic_string_view<CharT> b(rhs);
	return a > b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string_view<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a > rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a > rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>(  const std::basic_string<CharT>& lhs,
				const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs > b;
}


template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const CharT * rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a > rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>( const CharT * lhs,
		        const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs > b;
}


template<std::size_t N1,typename CharT, typename out_of_range_functor1,std::size_t N2, typename out_of_range_functor2>
bool operator>=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const static_basic_string<N2,CharT,out_of_range_functor2>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	std::basic_string_view<CharT> b(rhs);
	return a >= b;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string_view<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a >= rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const std::basic_string<CharT>& rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a >= rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>=(  const std::basic_string<CharT>& lhs,
				const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs >= b;
}


template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>=( const static_basic_string<N1,CharT,out_of_range_functor1>& lhs,
                 const CharT * rhs ) {
	std::basic_string_view<CharT> a(lhs);
	return a >= rhs;
}

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator>=( const CharT * lhs,
		        const static_basic_string<N1,CharT,out_of_range_functor1>& rhs ) {
	std::basic_string_view<CharT> b(rhs);
	return lhs >= b;
}

template< std::size_t N1, class CharT, typename out_of_range_functor1 >
std::basic_ostream<CharT>&
    operator<<( std::basic_ostream<CharT>& out, const static_basic_string<N1,CharT,out_of_range_functor1>& str ) {
	std::basic_string_view<CharT> s(str);
	return out << s;
}

} // namespace Tools

#endif /* CPPUTILS_CPPUTILSSHARED_STATIC_VECTOR_H_ */
