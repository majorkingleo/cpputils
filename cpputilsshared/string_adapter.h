/*
 * @author Copyright (c) 2024 Martin Oberzalek
 *		   Copyright (c) 2024 Anton Paar GmbH
 */

#pragma once

#include <array>
#include <string>
#include "static_string.h"
#include <stdexcept>
#include <functional>
#include <cstring>
#include <ostream>
#include <optional>
#include <span_vector.h>
#include <CpputilsDebug.h>
#include <format.h>

namespace Tools {

/**
 * A string class that can use any other container as buffer.
 * The maximum capacity is the buffer size.
 * Same behavior as std::string.
 *
 */
template <typename CharT,
		  typename container_t = Tools::span_vector<CharT>,
		  typename out_of_range_functor = static_string_out_of_range_cut>
class basic_string_adapter
{
	mutable container_t data;
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
	basic_string_adapter()
	: data()
	  {
	  }

	explicit basic_string_adapter( const container_t & container )
	: data( container )
	{
	}

	constexpr size_type capacity() const {
		return data.capacity()-1;
	}

	size_type max_size() const noexcept {
		return data.capacity()-1;
	}

	void reserve( size_type new_cap = 0 ) {
		if( new_cap > capacity() ) {
			out_of_range( new_cap );
		}
	}

	void resize( size_type count ) {
		if( count > capacity() ) {
			out_of_range( count );
			count = capacity();
		}

		data.resize(count);
	}

	void resize( size_type count, CharT ch ) {
		if( count > capacity() ) {
			out_of_range( count );
			count = capacity();
		}

		data.resize(count, ch );
	}

	const_pointer c_str() const {
		data[data.size()] = '\0';
		return data.data();
	}

	basic_string_adapter& operator=( const basic_string_adapter& str ) {
		assign(str);
		return *this;
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	basic_string_adapter& operator=( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		assign(str);
		return *this;
	}

	basic_string_adapter& operator=( const CharT* s ) {
		assign(s);
		return *this;
	}

	basic_string_adapter& operator=( CharT ch ) {
		clear();
		resize(1,ch);
		return *this;
	}

	basic_string_adapter& operator=( std::initializer_list<CharT> ilist ) {
		assign(ilist);
		return *this;
	}

	basic_string_adapter& operator=( const std::basic_string_view<CharT>& t ) {
		assign(t);
		return *this;
	}

	basic_string_adapter& operator=( const std::basic_string<CharT>& t ) {
		assign(t);
		return *this;
	}

	operator std::basic_string<CharT> () {
		return std::basic_string<CharT>(data.begin(),data.end());
	}

	basic_string_adapter& assign( size_type count, CharT ch ) {
		resize(count);

		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = ch;
		}
		return *this;
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	basic_string_adapter& assign( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		resize(str.size());
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = str[i];
		}
		return *this;
	}

	basic_string_adapter& assign( const std::basic_string<CharT> & str ) {
		resize(str.size());
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = str[i];
		}
		return *this;
	}

	basic_string_adapter& assign( const std::basic_string_view<CharT> & str ) {
		resize(str.size());
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = str[i];
		}
		return *this;
	}

	template<std::size_t N2,typename other_out_of_range_functor>
	basic_string_adapter& assign( const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv( str.begin(), str.end() );
		sv = sv.substr( pos, count );
		assign(sv);
		return *this;
	}

	basic_string_adapter& assign( const std::basic_string<CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv( str.begin(), str.end() );
		sv = sv.substr( pos, count );
		assign(sv);
		return *this;
	}

	basic_string_adapter& assign( const std::basic_string_view<CharT>& str,
	                      size_type pos, size_type count = npos ) {

		std::basic_string_view<CharT> sv = str.substr( pos, count );
		assign(sv);
		return *this;
	}


	basic_string_adapter& assign( const CharT* s, size_type count ) {
		resize(count);
		for( size_type i = 0; i < data.size(); i++ ) {
			data[i] = s[i];
		}
		return *this;
	}

	basic_string_adapter& assign( const CharT* s ) {
		std::size_t len = std::strlen(s);
		assign( s, len );
		return *this;
	}

	template< class InputIt >
	basic_string_adapter& assign( InputIt first, InputIt last ) {

		data.clear();
		resize( std::distance(first,last) );

		size_type i = 0;
		for( auto it = first; it != last && i < size(); ++it, ++i ) {
			data[i] = *it;
		}

		return *this;
	}

	basic_string_adapter& assign( std::initializer_list<CharT> ilist ) {

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

	const_iterator cend() const {
		return data.cend();
	}

	const_reverse_iterator crend() const {
		return data.crend();
	}

	reverse_iterator rend() {
		return data.rend();
	}


	reverse_iterator rbegin() {
		return data.rbegin();
	}

	const_reverse_iterator rbegin() const {
		return data.rbegin();
	}

	const_reverse_iterator crbegin() const {
		return data.crbegin();
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

	// Inserts count copies of character ch at the position index.
	basic_string_adapter& insert( size_type index, size_type count, CharT ch ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		fit_string( index, count );

		data.insert(it,count,ch);
		return *this;
	}

	basic_string_adapter& insert( size_type index, const CharT* s ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}
		std::basic_string_view sv(s);

		fit_string( index, sv );

		data.insert(it,sv.begin(), sv.end());
		return *this;
	}


	basic_string_adapter& insert( size_type index, const CharT* s, size_type count ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}
		std::basic_string_view<CharT> sv(s,count);

		fit_string( index, sv );

		data.insert(it,sv.begin(), sv.end());
		return *this;
	}

	basic_string_adapter& insert( size_type index, const std::basic_string<CharT>& str ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);

		fit_string( index, sv );

		data.insert(it,sv.begin(), sv.end());
		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& insert( size_type index, const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);

		fit_string( index, sv );

		data.insert(it,sv.begin(), sv.end());
		return *this;
	}

	basic_string_adapter& insert( size_type index, const std::basic_string<CharT>& str,
	                      size_type s_index, size_type count = npos ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);
		sv = sv.substr(s_index,count);

		fit_string( index, sv );

		data.insert(it,sv.begin(), sv.end());
		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& insert( size_type index, const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                      size_type s_index, size_type count = npos ) {
		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(str);
		sv = sv.substr(s_index,count);

		fit_string( index, sv );

		data.insert(it,sv.begin(), sv.end());
		return *this;
	}

	iterator insert( const_iterator pos, CharT ch ) {
		size_type index = std::distance(cbegin(),pos);
		size_type count = 1;
		fit_string( index, count );

		return data.insert(pos,count,ch);
	}

	iterator insert( const_iterator pos, size_type count, CharT ch ) {
		size_type index = std::distance(cbegin(),pos);
		fit_string( index, count );

		return data.insert(pos,count,ch);
	}

	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last ) {
		size_type index = std::distance(cbegin(),pos);
		std::basic_string_view<CharT> sv(first,last);

		fit_string( index, sv );
		return data.insert(pos,sv.begin(),sv.end());
	}

	iterator insert( const_iterator pos, std::initializer_list<CharT> ilist ) {
		size_type index = std::distance(cbegin(),pos);
		std::basic_string_view<CharT> sv(ilist.begin(), ilist.end());

		fit_string( index, sv );
		return data.insert(pos,sv.begin(),sv.end());
	}

	basic_string_adapter& insert( size_type index, const std::basic_string_view<CharT>& t ) {

		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(t);
		fit_string( index, sv );

		data.insert(it,sv.begin(),sv.end());

		return *this;
	}

	basic_string_adapter& insert( size_type index, const std::basic_string_view<CharT>& t,
			 size_type t_index, size_type count = npos ) {

		auto it = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++it;
		}

		std::basic_string_view<CharT> sv(t.substr(t_index,count));
		fit_string( index, sv );
		data.insert(it,sv.begin(),sv.end());

		return *this;
	}

	basic_string_adapter& erase( size_type index = 0, size_type count = npos ) {
		auto first = data.begin();
		for( size_type i = 0; i < index; ++i ) {
			++first;
		}

		auto last = first;

		if( count == npos ) {
			last = data.end();
		} else {
			for( size_type i = 0; i < count; ++i ) {
				++last;
			}
		}

		data.erase(first,last);
		return *this;
	}

	iterator erase( const_iterator position ) {
		return data.erase(position);
	}

	iterator erase( const_iterator first, const_iterator last ) {
		return data.erase( first, last );
	}

	void push_back( CharT ch ) {

		if( data.size() == capacity() ) {
			out_of_range(1);
			return;
		}

		data.push_back( ch );
	}

	void pop_back() {
		data.pop_back();
	}

	basic_string_adapter& append( size_type count, CharT ch ) {
		fit_string(size(),count);
		data.insert(data.end(),count,ch);
		return *this;
	}

	basic_string_adapter& append( const std::basic_string<CharT>& str ) {
		return append( std::basic_string_view<CharT>(str.begin(), str.end() ) );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& append( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		return append( std::basic_string_view<CharT>(str.begin(), str.end() ) );
	}

	basic_string_adapter& append( const std::basic_string<CharT>& str,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( str.begin(), str.end() ).substr(pos,count) );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& append( const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( str.begin(), str.end() ).substr(pos,count) );
	}

	basic_string_adapter& append( const CharT* s, size_type count ) {
		return append( std::basic_string_view<CharT>( s, count ) );
	}

	basic_string_adapter& append( const CharT* s ) {
		return append( std::basic_string_view<CharT>( s, std::strlen(s) ) );
	}

	basic_string_adapter& append( const std::basic_string_view<CharT>& str ) {
		insert(end(),str.begin(),str.end());
		return *this;
	}

	basic_string_adapter& append( const std::basic_string_view<CharT>& t,
	                      size_type pos, size_type count = npos ) {
		return append( std::basic_string_view<CharT>( t ).substr( pos, count ) );
	}

	template< class InputIt >
	basic_string_adapter& append( InputIt first, InputIt last ) {
		return append( std::basic_string_view<CharT>( first, last ) );
	}

	basic_string_adapter& append( std::initializer_list<CharT> ilist ) {
		return append( ilist.begin(), ilist.end() );
	}

	basic_string_adapter& operator+=( const std::basic_string<CharT>& str ) {
		return append( str.begin(), str.end() );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& operator+=( const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {
		return append( str.begin(), str.end() );
	}

	basic_string_adapter& operator+=( CharT ch ) {
		push_back( ch );
		return *this;
	}

	basic_string_adapter& operator+=( const CharT* s ) {
		return append( s );
	}

	basic_string_adapter& operator+=( std::initializer_list<CharT> ilist ) {
		return append( ilist );
	}

	basic_string_adapter& operator+=( const std::basic_string_view<CharT>& str ) {
		return append( str );
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       const std::basic_string<CharT>& str ) {
		return replace( pos, count, std::basic_string_view<CharT>(str) );
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       const std::basic_string_view<CharT>& str ) {

		auto it_start = begin() + pos;
		auto it_end = begin() + std::min(pos + count, size());

		if( str.size() == count ) {
			size_type idx = 0;
			for( auto it = it_start; it != it_end; ++it ) {
				*it = str[idx];
			}
			return *this;
		}

		const size_type start = std::distance( begin(), it_start );
		const size_type len = size();
		const difference_type diff = str.size() - count;

		// make more space
		if( diff > 0) {
			size_type enlarge = diff;
			resize( len + enlarge );

			for( size_type i = len + enlarge -1;
					i > 0 &&
					i > start &&
					i - enlarge >= start;
					--i ) {
				std::swap(data[i], data[i-count]);
			}

		} else {

			// reduce space
			for( difference_type i = pos + count - 1; i < static_cast<difference_type>(len) -1; ++i ) {
				data[i] = data[i+1];
			}
		}

		const size_type len_to_copy = str.size();

		for( size_type i = pos, idx = 0; i < len_to_copy; ++i, ++idx ) {
			data[i] = str[idx];
		}

		if( diff < 0 ) {
			resize( len + diff );
		}

		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& replace( size_type pos, size_type count,
	                       const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {

		return replace( pos, count, std::basic_string_view<CharT>(str) );
		return *this;
	}

	basic_string_adapter& replace( const_iterator first, const_iterator last,
	                       const std::basic_string<CharT>& str ) {

		static_basic_string left_over( last, cend() );

		resize( std::distance( cbegin(), first ) );
		append( str );
		append( left_over );

		return *this;
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& replace( const_iterator first, const_iterator last,
							const static_basic_string<N2,CharT,other_out_of_range_functor>& str ) {

		static_basic_string left_over( last, cend() );

		resize( std::distance( cbegin(), first ) );
		append( str );
		append( left_over );

		return *this;
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       const std::basic_string<CharT>& str,
	                       size_type pos2, size_type count2 = npos ) {

		std::basic_string_view<CharT> sv = str;
		return replace( pos, count, sv.substr(pos2,count2) );
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       const std::basic_string_view<CharT>& str,
	                       size_type pos2, size_type count2 = npos ) {

		return replace( pos, count, str.substr(pos2,count2) );
	}

	template<std::size_t N2, typename other_out_of_range_functor>
	basic_string_adapter& replace( size_type pos, size_type count,
							const static_basic_string<N2,CharT,other_out_of_range_functor>& str,
	                       size_type pos2, size_type count2 = npos ) {
		return replace( pos, count, str.substr(pos2,count2) );
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       const CharT* cstr, size_type count2 ) {
		return replace( pos, count, std::basic_string_view<CharT>( cstr, count2 ) );
	}

	basic_string_adapter& replace( const_iterator first, const_iterator last,
	                       const CharT* cstr, size_type count2 ) {
		return replace( first, last, std::basic_string_view<CharT>( cstr, count2 ) );
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       const CharT* cstr ) {
		return replace( pos, count, std::basic_string_view<CharT>( cstr, std::strlen(cstr) ) );
	}

	basic_string_adapter& replace( const_iterator first, const_iterator last,
	                       const CharT* cstr ) {
		return replace( first, last, std::basic_string_view<CharT>( cstr, std::strlen(cstr) ) );
	}

	basic_string_adapter& replace( size_type pos, size_type count,
	                       size_type count2, CharT ch ) {
		return replace( pos, count, static_basic_string( count2, ch ) );
	}


	basic_string_adapter& replace( const_iterator first, const_iterator last,
	                       size_type count2, CharT ch ) {
		return replace( first, last, static_basic_string( count2, ch ) );
	}

	template< class InputIt >
	basic_string_adapter& replace( const_iterator first, const_iterator last,
	                       InputIt first2, InputIt last2 ) {
		return replace( first, last, std::basic_string_view<CharT>(first2,last2) );
	}

	basic_string_adapter& replace( const_iterator first, const_iterator last,
	                       std::initializer_list<CharT> ilist ) {
		return replace( first, last, std::basic_string_view<CharT>(ilist.begin(),ilist.end()) );
	}

	basic_string_adapter& replace( const_iterator first, const_iterator last,
								  const std::basic_string_view<CharT> & t ) {

		basic_string_adapter left_over( last, cend() );

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

		std::memcpy( dest, data.data() + pos, count );
		return count;
	}

	void swap( basic_string_adapter& other ) {
		data.swap(other.data);
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

	std::basic_string_view<CharT> substr( size_type pos = 0, size_type count = npos ) const {
		return std::basic_string_view<CharT>(*this).substr(pos,count);
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
		if( size() + count > capacity() ) {
			out_of_range( capacity() - size() + count );

			size_type pos = index + count;
			if( pos >= capacity() ) {
				count = capacity() - index;
				resize(index);
			} else {
				const size_t new_size = capacity() - count;
				resize(new_size);
			}
		}
	}
};

} // namespace Tools

