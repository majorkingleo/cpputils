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
#include <optional>
#include <span>
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
template <std::size_t N,
          typename CharT,
		  typename out_of_range_functor = static_string_out_of_range_cut,
		  typename container_t = Tools::static_vector<CharT,N+1>>
class static_basic_string
{
	mutable container_t buffer;
	out_of_range_functor out_of_range;

public:

	typedef typename std::basic_string<CharT>::value_type value_type;
	typedef typename std::basic_string<CharT>::reference reference;
	typedef typename std::basic_string<CharT>::const_reference const_reference;
	typedef typename std::basic_string<CharT>::size_type size_type;
	typedef typename container_t::difference_type difference_type;
	typedef typename container_t::iterator iterator;
	typedef typename container_t::const_iterator const_iterator;
	typedef typename container_t::reverse_iterator reverse_iterator;
	typedef typename container_t::const_reverse_iterator const_reverse_iterator;
	typedef typename std::basic_string<CharT>::pointer pointer;
	typedef typename std::basic_string<CharT>::const_pointer const_pointer;

	static const size_type npos = std::basic_string<CharT>::npos;

public:
	static_basic_string()
	: buffer()
	  {
	  }

	explicit static_basic_string( const container_t & container )
	: buffer( container )
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

		buffer.resize(count);
	}

	void resize( size_type count, CharT ch ) {
		if( count > N ) {
			out_of_range( count );
			count = N;
		}

		buffer.resize(count, ch );
	}

	const_pointer c_str() const {
		buffer[buffer.size()] = '\0';
		return buffer.data();
	}

	pointer data() {
		return buffer.data();
	}

	const_pointer data() const {
		return buffer.data();
	}

	static_basic_string& operator=( const static_basic_string& str ) {
		assign(str);
		return *this;
	}

	template<std::size_t N2 ,typename other_out_of_range_functor, typename other_container_t>
	static_basic_string& operator=( const static_basic_string<N2,CharT,other_out_of_range_functor,other_container_t>& str ) {
		assign(str);
		return *this;
	}

	static_basic_string& operator=( const CharT* s ) {
		assign(s);
		return *this;
	}

	static_basic_string& operator=( CharT ch ) {
		clear();
		resize(1,ch);
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
		return std::basic_string<CharT>(buffer.begin(),buffer.end());
	}

	operator std::span<CharT> () {
		return std::span<CharT>(buffer.data(),buffer.size());
	}


	static_basic_string& assign( size_type count, CharT ch ) {
		resize(count);

		for( size_type i = 0; i < buffer.size(); i++ ) {
			buffer[i] = ch;
		}
		return *this;
	}

	template<std::size_t N2,typename other_out_of_range_functor, typename other_container_t>
	static_basic_string& assign( const static_basic_string<N2,CharT,other_out_of_range_functor,other_container_t>& str ) {
		resize(str.size());
		for( size_type i = 0; i < buffer.size(); i++ ) {
			buffer[i] = str[i];
		}
		return *this;
	}

	static_basic_string& assign( const std::basic_string<CharT> & str ) {
		resize(str.size());
		for( size_type i = 0; i < buffer.size(); i++ ) {
			buffer[i] = str[i];
		}
		return *this;
	}

	static_basic_string& assign( const std::basic_string_view<CharT> & str ) {
		resize(str.size());
		for( size_type i = 0; i < buffer.size(); i++ ) {
			buffer[i] = str[i];
		}
		return *this;
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	static_basic_string& assign( const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv( str.begin(), str.end() );
		sv = sv.substr( pos, count );
		assign(sv);
		return *this;
	}

	static_basic_string& assign( const std::basic_string<CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv( str.begin(), str.end() );
		sv = sv.substr( pos, count );
		assign(sv);
		return *this;
	}

	static_basic_string& assign( const std::basic_string_view<CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv = str.substr( pos, count );
		assign(sv);
		return *this;
	}


	static_basic_string& assign( const CharT* s, size_type count ) {
		resize(count);
		for( size_type i = 0; i < buffer.size(); i++ ) {
			buffer[i] = s[i];
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

		buffer.clear();
		resize( std::distance(first,last) );

		size_type i = 0;
		for( auto it = first; it != last && i < size(); ++it, ++i ) {
			buffer[i] = *it;
		}

		return *this;
	}

	static_basic_string& assign( std::initializer_list<CharT> ilist ) {

		buffer.clear();
		resize(ilist.size());

		size_type i = 0;
		for( auto it = ilist.begin(); it != ilist.end() && i < size(); ++it, ++i ) {
			buffer[i] = *it;
		}

		return *this;
	}

	CharT& at( size_type pos ) {
		return buffer.at(pos);
	}

	const CharT& at( size_type pos ) const {
		return buffer.at(pos);
	}

	CharT& operator[]( size_type pos ) {
		return buffer[pos];
	}

	const CharT& operator[]( size_type pos ) const {
		return buffer[pos];
	}

	CharT& front() {
		return buffer[0];
	}

	const CharT& front() const {
		return buffer[0];
	}

	CharT& back() {
		return buffer[size() - 1];
	}

	const CharT& back() const {
		return buffer[size() - 1];
	}


	operator std::basic_string_view<CharT>() const noexcept {
		return std::basic_string_view<CharT>(buffer.data(),buffer.size());
	}

	iterator begin() {
		return buffer.begin();
	}

	const_iterator begin() const {
		return buffer.begin();
	}

	const_iterator cbegin() const noexcept {
		return buffer.cbegin();
	}

	iterator end() {
		return buffer.end();
	}

	const_iterator end() const {
		return buffer.end();
	}

	const_iterator cend() const {
		return buffer.cend();
	}

	const_reverse_iterator crend() const {
		return buffer.crend();
	}

	reverse_iterator rend() {
		return buffer.rend();
	}


	reverse_iterator rbegin() {
		return buffer.rbegin();
	}

	const_reverse_iterator rbegin() const {
		return buffer.rbegin();
	}

	const_reverse_iterator crbegin() const {
		return buffer.crbegin();
	}

	bool empty() const {
		return buffer.empty();
	}

	size_type size() const {
		return buffer.size();
	}

	size_type length() const {
		return buffer.size();
	}

	void shrink_to_fit() {}

	void clear() {
		buffer.clear();
	}

	// Inserts count copies of character ch at the position index.
	static_basic_string& insert( size_type index, size_type count, CharT ch ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		fit_string( index, count );

		buffer.insert(it,count,ch);
		return *this;
	}

	static_basic_string& insert( size_type index, const CharT* s ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}
		std::basic_string_view sv(s);

		fit_string( index, sv );

		buffer.insert(it,sv.begin(), sv.end());
		return *this;
	}


	static_basic_string& insert( size_type index, const CharT* s, size_type count ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}
		std::basic_string_view<CharT> sv(s,count);

		fit_string( index, sv );

		buffer.insert(it,sv.begin(), sv.end());
		return *this;
	}

	static_basic_string& insert( size_type index, const std::basic_string<CharT>& str ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);

		fit_string( index, sv );

		buffer.insert(it,sv.begin(), sv.end());
		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& insert( size_type index, const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);

		fit_string( index, sv );

		buffer.insert(it,sv.begin(), sv.end());
		return *this;
	}

	static_basic_string& insert( size_type index, const std::basic_string<CharT>& str,
	                      size_type s_index, size_type count = npos ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);
		sv = sv.substr(s_index,count);

		fit_string( index, sv );

		buffer.insert(it,sv.begin(), sv.end());
		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& insert( size_type index, const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                      size_type s_index, size_type count = npos ) {
		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);
		sv = sv.substr(s_index,count);

		fit_string( index, sv );

		buffer.insert(it,sv.begin(), sv.end());
		return *this;
	}

	iterator insert( const_iterator pos, CharT ch ) {
		size_type index = std::distance(cbegin(),pos);
		size_type count = 1;
		fit_string( index, count );

		return buffer.insert(pos,count,ch);
	}

	iterator insert( const_iterator pos, size_type count, CharT ch ) {
		size_type index = std::distance(cbegin(),pos);
		fit_string( index, count );

		return buffer.insert(pos,count,ch);
	}

	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last ) {
		size_type index = std::distance(cbegin(),pos);
		std::basic_string_view<CharT> sv(first,last);

		fit_string( index, sv );
		return buffer.insert(pos,sv.begin(),sv.end());
	}

	iterator insert( const_iterator pos, std::initializer_list<CharT> ilist ) {
		size_type index = std::distance(cbegin(),pos);
		std::basic_string_view<CharT> sv(ilist.begin(), ilist.end());

		fit_string( index, sv );
		return buffer.insert(pos,sv.begin(),sv.end());
	}

	static_basic_string& insert( size_type index, const std::basic_string_view<CharT>& t ) {

		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(t);
		fit_string( index, sv );

		buffer.insert(it,sv.begin(),sv.end());

		return *this;
	}

	static_basic_string& insert( size_type index, const std::basic_string_view<CharT>& t,
			 size_type t_index, size_type count = npos ) {

		auto it = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(t.substr(t_index,count));
		fit_string( index, sv );
		buffer.insert(it,sv.begin(),sv.end());

		return *this;
	}

	static_basic_string& erase( size_type index = 0, size_type count = npos ) {
		auto first = buffer.begin();
		for( size_type i = 0; i < index; ++i ) {
			++first;
		}

		auto last = first;

		if( count == npos ) {
			last = buffer.end();
		} else {
			for( size_type i = 0; i < count; ++i ) {
				++last;
			}
		}

		buffer.erase(first,last);
		return *this;
	}

	iterator erase( const_iterator position ) {
		return buffer.erase(position);
	}

	iterator erase( const_iterator first, const_iterator last ) {
		return buffer.erase( first, last );
	}

	void push_back( CharT ch ) {

		if( buffer.size() == N ) {
			out_of_range(1);
			return;
		}

		buffer.push_back( ch );
	}

	void pop_back() {
		buffer.pop_back();
	}

	static_basic_string& append( size_type count, CharT ch ) {
		fit_string(size(),count);
		buffer.insert(buffer.end(),count,ch);
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
		insert(end(),str.begin(),str.end());
		return *this;
	}

	static_basic_string& append( const std::basic_string_view<CharT>& t,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( t ).substr( pos, count ) );
	}

	template< class InputIt >
	static_basic_string& append( InputIt first, InputIt last ) {
		return append( std::basic_string_view<CharT>( first, last ) );
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
		return replace( pos, count, std::basic_string_view<CharT>(str) );
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

		return replace( pos, count, std::basic_string_view<CharT>(str) );
		return *this;
	}

	static_basic_string& replace( const_iterator first, const_iterator last,
	                       const std::basic_string<CharT>& str ) {

		static_basic_string left_over( last, cend() );

		resize( std::distance( cbegin(), first ) );
		append( str );
		append( left_over );

		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	static_basic_string& replace( const_iterator first, const_iterator last,
							const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {

		static_basic_string left_over( last, cend() );

		resize( std::distance( cbegin(), first ) );
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
		return replace( first, last, std::basic_string_view<CharT>(ilist.begin(),ilist.end()) );
	}

	static_basic_string& replace( const_iterator first, const_iterator last,
								  const std::basic_string_view<CharT> & t ) {

		static_basic_string left_over( last, cend() );

		resize( std::distance( cbegin(), first ) );
		append( t );
		append( left_over );

		return *this;
	}

	size_type copy( CharT* dest, size_type count, size_type pos = 0 ) const {
		if( count == npos || pos + count > size() ) {
			count = size() - pos;
		}

		if( pos > size() ) {
#if __cpp_exceptions > 0
			throw std::out_of_range( "invalid index");
#else
			std::abort();
#endif
		}

		std::memcpy( dest, buffer.data() + pos, count );
		return count;
	}

	void swap( static_basic_string& other ) {
		buffer.swap(other.buffer);
	}

	size_type find( const std::basic_string<CharT>& str, size_type pos = 0 ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.find( str, pos );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	size_type find( const static_basic_string<N2,CharT,other_out_of_range_functor>& str, size_type pos = 0 ) const {
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

	template<std::size_t N2, typename other_out_of_range_functor>
	size_type rfind(const static_basic_string<N2,CharT,other_out_of_range_functor>& str, size_type pos = npos ) const {
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
		return sv.compare(std::basic_string_view<CharT>(str));
	}

	int compare( size_type pos1, size_type count1,
	             const std::basic_string<CharT>& str,
	             size_type pos2, size_type count2 = npos ) const {
		std::basic_string_view<CharT> sv(*this);
		return sv.compare( pos1, count1, str, pos2, count2 );
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	int compare( size_type pos1, size_type count1,
		             const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
		             size_type pos2, size_type count2 = npos ) const {
			std::basic_string_view<CharT> sv(*this);
			return sv.compare( pos1, count1, std::basic_string_view<CharT>(str), pos2, count2 );
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

#if __cpp_lib_string_contains >= 202011L
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
#else
	constexpr bool
	    contains( std::basic_string_view<CharT> sv ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.find( sv ) != std::string::npos;
	}

	constexpr bool
	    contains( CharT ch ) const noexcept {
		std::basic_string_view<CharT> me(*this);
		return me.find( ch ) != std::string::npos;
	}

	constexpr bool
	    contains( const CharT* s ) const {
		std::basic_string_view<CharT> me(*this);
		return me.find( s ) != std::string::npos;
	}
#endif

	static_basic_string substr( size_type pos = 0, size_type count = npos ) const {
		return static_basic_string( std::basic_string_view<CharT>(*this).substr(pos,count) );
	}

private:
	void fit_string( size_type index, std::basic_string_view<CharT> & sv) {

		size_type sv_size = sv.size();
		fit_string( index, sv_size );

		if( sv_size != sv.size() ) {
			sv = sv.substr( 0, sv_size );
		}
	}

	void fit_string( size_type index, size_type & count ) {
		if( size() + count > N ) {
			out_of_range( N - size() + count );

			size_type pos = index + count;
			if( pos >= N ) {
				count = N - index;
				resize(index);
			} else {
				const size_t new_size = N - count;
				resize(new_size);
			}
		}
	}
};

template <std::size_t N> class static_string : public static_basic_string<N,char> {
public:
	// forward constructors
	using base = static_basic_string<N,char>;
	using base::base;
	using base::operator=;
};

template <std::size_t N> class static_wstring : public static_basic_string<N,wchar_t> {
public:
	// forward constructors
	using base = static_basic_string<N,wchar_t>;
	using base::base;
	using base::operator=;
};

#if __cpp_char8_t > 0
template <std::size_t N> class static_u8string : public static_basic_string<N, char8_t> {
public:
	// forward constructors
	using base = static_basic_string<N,char8_t>;
	using base::base;
	using base::operator=;
};
#endif

template <std::size_t N> class static_u16string : public static_basic_string<N,char16_t> {
public:
	// forward constructors
	using base = static_basic_string<N,char16_t>;
	using base::base;
	using base::operator=;
};

template <std::size_t N> class static_u32string : public static_basic_string<N,char32_t> {
public:
	// forward constructors
	using base = static_basic_string<N,char32_t>;
	using base::base;
	using base::operator=;
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

template<std::size_t N1,typename CharT, typename out_of_range_functor1>
bool operator==( const std::basic_string<CharT> & lhs,
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

template< std::size_t N1, class CharT, typename out_of_range_functor1, typename container_t >
std::basic_ostream<CharT>&
    operator<<( std::basic_ostream<CharT>& out, const static_basic_string<N1,CharT,out_of_range_functor1, container_t>& str ) {
	std::basic_string_view<CharT> s(str);
	return out << s;
}

} // namespace Tools

#endif /* CPPUTILS_CPPUTILSSHARED_STATIC_VECTOR_H_ */
