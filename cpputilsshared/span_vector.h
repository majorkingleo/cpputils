/*
 * @author Copyright (c) 2024 Martin Oberzalek
 *
 * Kind of adapter that can be plugged onto a span and provides std::vector interface.
 * You can do write operation on the span, if it does not exceed the spans boundaries.
 *
 * This container does not take ownership of the underlying span.
 * On construction you have to assign a span with a miximum size, and and initional size.
 * Objects that are in the span won't be constructed, or deleted. The ownership stays at the spawn.
 *
 */
#pragma once

#include <span>
#include <cstddef>
#include <iterator>

namespace Tools {

template <typename T>
class span_vector
{
public:
	using value_type = std::span<T>::value_type;
	using size_type = std::span<T>::size_type;
	using difference_type = std::span<T>::difference_type;
	using reference = std::span<T>::reference;
	using const_reference = std::span<T>::const_reference;
	using pointer = value_type*;
	using const_pointer = const value_type*;

	class const_iterator;

	class iterator
	{
	public:
		using value_type = std::span<T>::value_type;
		using size_type = std::span<T>::size_type;
		using difference_type = std::span<T>::difference_type;
		using reference = std::span<T>::reference;
		using const_reference = std::span<T>::const_reference;
		using iterator_category = std::random_access_iterator_tag;
		//using iterator_category = std::input_iterator_tag;

	protected:
		span_vector *parent;
		size_type pos;
	public:
		explicit iterator(span_vector *parent_ = nullptr, size_type pos_ = 0) : parent( parent_), pos( pos_ ) {}
		iterator& operator++() { pos++; return *this; }
		iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }

		 bool operator==(iterator other) const { return pos == other.pos; }
		 bool operator!=(iterator other) const { return !(*this == other); }

		 reference operator*() const { return parent->operator[](pos); }

		 iterator& operator+=( difference_type amount ) {
			 pos += amount;
			 return *this;
		 }

		 iterator& operator-=( difference_type amount ) {
			 pos -= amount;
			 return *this;
		 }

		 iterator operator+( difference_type amount ) const {
			 return iterator(parent,pos+amount);
		 }

		 iterator operator-( difference_type amount ) const {
			 return iterator(parent,pos-amount);
		 }

		 difference_type operator-( const_iterator it ) const {
			 return static_cast<difference_type>(pos)-static_cast<difference_type>(it.pos);
		 }

		 size_type get_pos() {
			 return pos;
		 }
	};

	class const_iterator : public iterator
	{
	public:
		using base = iterator;
		using base::base;
		using base::operator=;

		explicit const_iterator(const span_vector *parent_ = nullptr, size_type pos_ = 0)
		: iterator( const_cast<span_vector *>(parent_), pos_ )
		{}

		const_iterator( const iterator & other )
		: iterator( other )
		{}

		const_reference operator*() const { return base::parent->operator[](base::pos); }
	};

	class reverse_iterator
	{
	public:
		using value_type = std::span<T>::value_type;
		using size_type = std::span<T>::size_type;
		using difference_type = std::span<T>::difference_type;
		using reference = std::span<T>::reference;
		using const_reference = std::span<T>::const_reference;
		using iterator_category = std::random_access_iterator_tag;
		//using iterator_category = std::input_iterator_tag;

	protected:
		span_vector *parent;
		difference_type pos;
	public:
		explicit reverse_iterator(span_vector *parent_ = nullptr, size_type pos_ = 0) : parent( parent_), pos( pos_ ) {}
		iterator& operator++() { pos--; return *this; }
		iterator operator++(int) { iterator retval = *this; --(*this); return retval; }

		 bool operator==(iterator other) const { return pos == other.pos; }
		 bool operator!=(iterator other) const { return !(*this == other); }

		 reference operator*() const { return parent->operator[](pos); }

		 iterator& operator+=( difference_type amount ) {
			 pos -= amount;
			 return *this;
		 }

		 iterator& operator-=( difference_type amount ) {
			 pos += amount;
			 return *this;
		 }

		 iterator operator+( difference_type amount ) const {
			 return iterator(parent,pos-amount);
		 }

		 iterator operator-( difference_type amount ) const {
			 return iterator(parent,pos+amount);
		 }

		 difference_type operator-( const_iterator it ) const {
			 return static_cast<difference_type>(pos)-static_cast<difference_type>(it.pos);
		 }

		 size_type get_pos() {
			 return pos;
		 }
	};

	class const_reverse_iterator : public reverse_iterator
	{
	public:
		using base = reverse_iterator;
		using base::base;
		using base::operator=;

		explicit const_reverse_iterator(const span_vector *parent_ = nullptr, size_type pos_ = 0)
		: reverse_iterator( const_cast<span_vector *>(parent_), pos_ )
		{}

		const_reverse_iterator( const iterator & other )
		: iterator( other )
		{}

		const_reference operator*() const { return base::parent->operator[](base::pos); }
	};

protected:
	std::span<T> buffer;
	std::size_t len;

public:
	explicit span_vector()
	: buffer(),
	  len(0)
	{}

    span_vector(const span_vector& other) = default;
    span_vector(span_vector&& other) = default;

	span_vector( const std::span<T> & buffer_, std::size_t initial_size_ = 0 )
	: buffer( buffer_ ),
	  len( initial_size_ )
	{}

	span_vector( T* data_, std::size_t len_, std::size_t initial_size_ = 0 )
	: buffer( data_, len_ ),
	  len( initial_size_ )
	{}

	void set( const std::span<T> & buffer_, std::size_t initial_size_ = 0 )
	{
		buffer = buffer_;
		len = initial_size_;
	}

	void set( T* data_, std::size_t len_, std::size_t initial_size_ = 0 )
	{
		buffer = std::span<T>(data_,len_);
		len = initial_size_;
	}

	span_vector & operator=( const span_vector & other ) {

		if( capacity() < other.size() ) {
			throw std::out_of_range(  "capacity exceeded" );
		}

		len = other.size();

		for( size_type i = 0; i < other.size(); ++i ) {
			buffer[i] = other[i];
		}

		return *this;
	}

	span_vector & operator=( const std::span<T> & other ) {
		if( capacity() < other.size() ) {
			throw std::out_of_range( "capacity exceeded" );
		}

		len = other.size();

		for( size_type i = 0; i < other.size(); ++i ) {
			buffer[i] = other[i];
		}

		return *this;
	}

	span_vector & operator=( std::initializer_list<T> ilist ) {
		assign( ilist );
		return *this;
	}

	void assign( size_type count, const T& value ) {
		if( capacity() < count ) {
			throw std::out_of_range( "capacity exceeded" );
		}

		len = count;

		for( size_type i = 0; i < count; ++i ) {
			buffer[i] = value;
		}
	}

	template<class InputIt>
	void assign( InputIt first, InputIt last, typename std::iterator_traits<InputIt>::iterator_category* = nullptr ) // https://stackoverflow.com/a/11899057
	{
		size_type count = std::distance( first, last );

		if( capacity() < count ) {
			throw std::out_of_range( "capacity exceeded" );
		}

		len = 0;

		for( InputIt it = first; it != last; ++it ) {
			buffer[len] = *it;
			len++;
		}
	}

	void assign( std::initializer_list<T> ilist ) {
		assign( ilist.begin(), ilist.end() );
	}


	size_type size() const {
		return len;
	}

	size_type capacity() const {
		return buffer.size();
	}

	bool empty() const {
		return len == 0;
	}

	size_type max_size() const {
		return capacity();
	}

	// std::length_error if new_cap > max_size().
	void reserve( size_type new_cap ) {
		if( new_cap > max_size() ) {
			throw std::length_error("capacity exceeded");
		}
	}

	// does nothing
	void shrink_to_fit() {}

	reference at( size_type pos ) {
		if( pos >= len ) {
			throw std::out_of_range("pos exceeds vector limit");
		}

		return buffer[pos];
	}

	const_reference at( size_type pos ) const {
		if( pos >= len ) {
			throw std::out_of_range("pos exceeds vector limit");
		}

		return buffer[pos];
	}

	reference operator[]( size_type pos ) {
		return buffer[pos];
	}

	const_reference operator[]( size_type pos ) const {
		return buffer[pos];
	}

	// Calling front on an empty container causes out of range exception
	reference front() {
		if( empty() ) {
			throw std::out_of_range("pos exceeds vector limit");
		}

		return buffer.front();
	}

	// Calling front on an empty container causes out of range exception
	reference front() const {
		if( empty() ) {
			throw std::out_of_range("pos exceeds vector limit");
		}

		return buffer.front();
	}

	// Calling back on an empty container causes out of range exception
	reference back() {
		if( empty() ) {
			throw std::out_of_range("pos exceeds vector limit");
		}

		return buffer.back();
	}

	// Calling back on an empty container causes out of range exception
	reference back() const {
		if( empty() ) {
			throw std::out_of_range("pos exceeds vector limit");
		}

		return buffer.back();
	}

	T* data() {
		return buffer.data();
	}

	const T* data() const {
		return buffer.data();
	}


	iterator begin() {
		if( empty() ) {
			return end();
		}

		return iterator(this,0);
	}

	const_iterator begin() const {
		if( empty() ) {
			return end();
		}

		return const_iterator(this,0);
	}

	const_iterator cbegin() const noexcept {
		if( empty() ) {
			return cend();
		}

		return const_iterator(this,0);
	}

	iterator end() noexcept {
		if( empty() ) {
			return iterator(this,1);
		}

		return iterator(this,len);
	}

	const_iterator end() const noexcept {
		if( empty() ) {
			return const_iterator(this,1);
		}

		return const_iterator(this,len);
	}

	const_iterator cend() const noexcept {
		if( empty() ) {
			return const_iterator(this,1);
		}

		return const_iterator(this,len);
	}

	constexpr void clear() noexcept {
		len = 0;
	}

	iterator insert( const_iterator pos, const T& value ) {
		if( pos == cend() ) {
			push_back( value );
			return iterator(this,len-1);
		}

		if( len + 1 > capacity() ) {
			throw std::length_error("capacity exceeded");
		}

		size_type start = std::distance( cbegin(), pos );

		for( size_type i = len; i > 0 && i > start; --i ) {
			std::swap(buffer[i], buffer[i-1]);
		}

		buffer[start] = value;
		len++;

		return iterator(this,start);
	}

	iterator insert( const_iterator pos, T&& value ) {
		if( pos == cend() || empty() ) {
			push_back( value );
			return iterator(this,len-1);
		}

		if( len + 1 > capacity() ) {
			throw std::length_error("capacity exceeded");
		}

		size_type start = std::distance( cbegin(), pos );

		for( size_type i = len; i > 0 && i > start; --i ) {
			std::swap(buffer[i], buffer[i-1]);
		}

		buffer[start] = std::move(value);
		len++;

		return iterator(this,start);
	}

	iterator insert( const_iterator pos,
	                 size_type count, const T& value ) {
		if( empty() ) {
			for( size_type i = 0; i < count; ++i ) {
				push_back( value );
			}
			return iterator(this,0);
		}

		if( pos == cend() ) {
			const size_type start = len > 0 ? len : 0;
			for( size_type i = 0; i < count; ++i ) {
				push_back( value );
			}
			return iterator(this,start);
		}


		if( len + count > capacity() ) {
			throw std::length_error("capacity exceeded");;
		}

		const size_type start = std::distance( cbegin(), pos );

		for( difference_type i = len + count -1;
				i > 0 &&
				i > static_cast<difference_type>(start) &&
				i - static_cast<difference_type>(count) >= static_cast<difference_type>(start);
				--i ) {
			std::swap(buffer[i], buffer[i-count]);
		}

		for( size_type i = start; i < start + count; ++i ) {
			buffer[i] = value;
		}

		len += count;

		return iterator(this,start);
	}

	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last, typename std::iterator_traits<InputIt>::iterator_category* = nullptr ) {
		if( empty() ) {
			for( auto it = first; it != last; ++it ) {
				push_back( *it );
			}
			return iterator(this,0);
		}

		if( pos == cend() ) {
			const size_type start = len > 0 ? len : 0;
			for( auto it = first; it != last; ++it ) {
				push_back( *it );
			}
			return iterator(this,start);
		}

		const size_type count = std::distance( first, last );

		if( len + count > capacity() ) {
			throw std::length_error("capacity exceeded");
		}

		const size_type start = std::distance( cbegin(), pos );

		for( difference_type i = len + count -1;
						i > 0 &&
						i > static_cast<difference_type>(start) &&
						i - static_cast<difference_type>(count) >= static_cast<difference_type>(start);
						--i ) {
			std::swap(buffer[i], buffer[i-count]);
		}

		size_type i = start;
		for( auto it = first; it != last; ++it, ++i ) {
			buffer[i] = *it;
		}

		len += count;

		return iterator(this,start);
	}

	iterator insert( const_iterator pos, std::initializer_list<T> ilist ) {
		return insert( pos, ilist.begin(), ilist.end() );
	}

	iterator erase( const_iterator pos ) {
		if( empty() ) {
			return end();
		}

		if( pos == end() ) {
			return end();
		}

		if( size() == 1 ) {
			len = 0;
			return end();
		}

		const size_type start = std::distance( cbegin(), pos );

		for( size_type i = start; i + 1 < len; ++i ) {
			std::swap(buffer[i], buffer[i+1]);
		}

		len--;

		if( start == len ) {
			return end();
		}

		return iterator(this,start);
	}

	iterator erase( const_iterator first, const_iterator last ) {
		if( empty() ) {
			return end();
		}

		const size_type count = std::distance( first, last );

		// clear
		if( count == len ) {
			len = 0;
			return end();
		}

		const size_type start = std::distance( cbegin(), first );

		for( size_type i = start; i < len - count; ++i ) {
			std::swap(buffer[i], buffer[i+count]);
		}

		len -= count;

		return iterator(this,start);
	}

	void push_back( const T& value ) {
		if( len >= max_size() ) {
			throw std::length_error("capacity exceeded");
		}

		buffer[len] = value;
		len++;
	}

	void push_back( T&& value ) {
		if( len >= max_size() ) {
			throw std::length_error("capacity exceeded");
		}

		buffer[len] = std::move(value);
		len++;
	}

	void pop_back() {
		if( !empty() ) {
			len--;
		}
	}

	void resize( size_type count ) {
		resize( count, T() );
	}

	void resize( size_type count, const value_type& value ) {
		if( count > max_size() ) {
			throw std::length_error("capacity exceeded");
		}

		for( size_type i = len; i < count; ++i ) {
			buffer[i] = value;
		}

		len = count;
	}

	void swap( span_vector & other )
	{
		std::swap( buffer, other.buffer );
		std::swap( len, other.len );
	}
};

} // namespace Tools;

template <class T>
bool operator==( const Tools::span_vector<T> & v1, const Tools::span_vector<T> & v2 ) {
	if( v1.size() != v2.size() ) {
		return false;
	}

	for( typename Tools::span_vector<T>::size_type i = 0; i < v1.size(); ++i ) {
		if( v1[i] != v2[i] ) {
			return false;
		}
	}

	return true;
}

