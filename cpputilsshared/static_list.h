/*
 * @author Copyright (c) 2023 Martin Oberzalek
 *		   Copyright (c) 2023 Anton Paar GmbH
 *
 */

#ifndef CPPUTILS_CPPUTILSSHARED_STATIC_LIST_H_
#define CPPUTILS_CPPUTILSSHARED_STATIC_LIST_H_

#include "static_vector.h"
#include <list>
#include <optional>
#include <algorithm>

namespace Tools {

/**
 * A list class that uses no heap.
 * The maximum capacity is defined at compile time. At difference to
 * static_vector all iterators staying valid on list manipulation.
 * Same behavior as std::list iterators.
 *
 */
template <typename T,std::size_t N>
class static_list
{
public:

	typedef typename static_vector<T,N>::value_type value_type;
	typedef typename static_vector<T,N>::reference reference;
	typedef typename static_vector<T,N>::const_reference const_reference;
	typedef typename static_vector<T,N>::size_type size_type;
	typedef typename static_vector<T,N>::difference_type difference_type;
	typedef typename static_vector<T,N>::pointer pointer;
	typedef typename static_vector<T,N>::const_pointer const_pointer;

protected:
	typedef static_vector<std::optional<T>,N> DATA_CONTAINER;
	DATA_CONTAINER data;

	typedef static_vector<std::optional<T>*,N> DATA_INDEX;
	DATA_INDEX index;

	size_type manipulation_count = 0;

public:

	class iterator
	{
	public:
		typedef typename static_list<T,N>::value_type value_type;
		typedef typename static_list<T,N>::reference reference;
		typedef typename static_list<T,N>::const_reference const_reference;
		typedef typename static_list<T,N>::size_type size_type;
		typedef typename static_list<T,N>::difference_type difference_type;

	private:
		mutable typename static_list<T,N>::DATA_INDEX::iterator pos;
		static_list<T,N> *parent = nullptr;
		mutable size_type manipulation_count = 0;
		typename static_list<T,N>::DATA_INDEX::value_type pointer_to_optional;
		bool is_end = false;

	public:

		iterator() = default;

		iterator( const iterator & other ) = default;

	protected:
		iterator( static_list<T,N> *parent_, typename static_list<T,N>::DATA_INDEX::iterator pos_ )
		: pos( pos_ ),
		  parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().end() == pos ? nullptr : *pos ),
		  is_end( parent->get_index().end() == pos )
		{

		}

		iterator( static_list<T,N> *parent_,
				  typename static_list<T,N>::DATA_INDEX::const_iterator pos_ )
		: parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().cend() == pos_ ? nullptr : *pos_ )
		{
			typename static_list<T,N>::DATA_INDEX::iterator it = parent->get_index().begin();
			typename static_list<T,N>::DATA_INDEX::const_iterator cit = parent->get_index().cbegin();

			while( it != parent->get_index().end() && cit != pos_ ) {
				++it;
				++cit;
			}

			pos = it;
			is_end = parent->get_index().end() == pos;
		}

	public:
		bool operator!=( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos == other_it.pos ) {
				return false;
			}

			return true;
		}

		bool operator==( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos != other_it.pos ) {
				return false;
			}

			return true;
		}

		T & operator*() {
			check( *this );
			return (*pos)->value();
		}

		const T & operator*() const {
			check( *this );
			return (*pos)->value();
		}

		iterator & operator++() {
			check(*this);
			++pos;
			pointer_to_optional = *pos;
			return *this;
		}

		iterator operator++(int) {
			iterator res(*this);
			++(*this);
			return res;
		}

		iterator & operator+=(int count) {
			check(*this);
			pos += count;
			pointer_to_optional = *pos;
			is_end = pos == parent->get_index().end();
			return *this;
		}

		iterator operator+(int count) const {
			iterator ret(*this);
			ret += count;
			return ret;
		}

		iterator operator-(int count) const {
			iterator ret(*this);
			ret -= count;
			return ret;
		}

		iterator & operator-=(int count) {
			check(*this);
			pos -= count;
			pointer_to_optional = *pos;
			return *this;
		}


		iterator operator--(int) {
			iterator res(*this);
			--(*this);
			return res;
		}

		iterator & operator--() {
			check(*this);
			--pos;
			pointer_to_optional = *pos;
			is_end = pos == parent->get_index().end();
			return *this;
		}

		T* operator->() {
			check( *this );
			return &(*pos)->value();
		}

		// reset the pos iterator to the correct position, if
		// a manipulation of the list happened
		template<class Iterator>
		static void check( Iterator & it )
		{
			auto actual_manipulation_count = it.parent->get_manipulation_count();
			if( actual_manipulation_count == it.manipulation_count ) {
				return;
			}

			it.manipulation_count = actual_manipulation_count;

			// if we pointed to the end, point again to the end
			if( it.is_end ) {
				it.pos = it.parent->get_index().end();
				return;
			}

			// nothing changed
			if( *it.pos == it.pointer_to_optional ) {
				return;
			}

			auto & index = it.parent->get_index();

			for( auto it_index = index.begin(); it_index != index.end(); it_index++ ) {
				if( *it_index == it.pointer_to_optional ) {
					it.pos = it_index;
					return;
				}
			}

			throw std::out_of_range("index not found");
		}

		 typename static_list<T,N>::DATA_INDEX::iterator get_it() const {
			 check(*this);
			 return pos;
		 }

		friend class static_list<T,N>::const_iterator;
		friend class static_list<T,N>::reverse_iterator;
		friend class static_list<T,N>;
	};

	class const_iterator
	{
	public:
		typedef typename static_list<T,N>::value_type value_type;
		typedef typename static_list<T,N>::reference reference;
		typedef typename static_list<T,N>::const_reference const_reference;
		typedef typename static_list<T,N>::size_type size_type;
		typedef typename static_list<T,N>::difference_type difference_type;

	private:
		mutable typename static_list<T,N>::DATA_INDEX::const_iterator pos;
		const static_list<T,N> *parent = nullptr;
		mutable size_type manipulation_count = 0;
		typename static_list<T,N>::DATA_INDEX::value_type pointer_to_optional;
		bool is_end = false;

	public:
		const_iterator() = default;

		const_iterator( const const_iterator & other ) = default;

	protected:
		const_iterator( const static_list<T,N> *parent_, typename static_list<T,N>::DATA_INDEX::iterator pos_ )
		: pos( pos_ ),
		  parent( parent_ ),
		  manipulation_count( data->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().cend() == pos ? nullptr : *pos  ),
		  is_end( parent->get_index().end() == pos )
		{}

		const_iterator( const static_list<T,N> *parent_, typename static_list<T,N>::DATA_INDEX::const_iterator pos_ )
		: pos( pos_ ),
		  parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().cend() == pos ? nullptr : *pos  ),
		  is_end( parent->get_index().end() == pos )
		{}

	public:
		const_iterator( const iterator & other )
		: pos( other.pos ),
		  parent( other.parent ),
		  manipulation_count( other.manipulation_count ),
		  pointer_to_optional( other.pointer_to_optional ),
		  is_end( other.is_end )
		{}

		bool operator!=( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos == other_it.pos ) {
				return false;
			}

			return true;
		}

		bool operator==( const iterator & other_it ) const {
			iterator::check( *this );
			iterator::check( other_it );

			if( pos != other_it.pos ) {
				return false;
			}

			return true;
		}

		bool operator!=( const const_iterator & other_it ) const {
			iterator::check( *this );
			iterator::check( other_it );

			if( pos == other_it.pos ) {
				return false;
			}

			return true;
		}

		bool operator==( const const_iterator & other_it ) const {
			iterator::check( *this );
			iterator::check( other_it );

			if( pos != other_it.pos ) {
				return false;
			}

			return true;
		}


		T & operator*() {
			iterator::check( *this );
			return (*pos)->value();
		}

		const T & operator*() const {
			check( *this );
			return (*pos)->value();
		}

		const_iterator & operator++() {
			iterator::check(*this);
			++pos;
			pointer_to_optional = *pos;
			return *this;
		}

		const_iterator operator++(int) {
			const_iterator res(*this);
			++(*this);
			return res;
		}

		const_iterator & operator+=(int count) {
			iterator::check(*this);
			pos += count;
			pointer_to_optional = *pos;
			return *this;
		}

		const_iterator operator+(int count) const {
			iterator ret(*this);
			ret += count;
			return ret;
		}

		const_iterator operator-(int count) const {
			iterator ret(*this);
			ret -= count;
			return ret;
		}

		const_iterator & operator-=(int count) {
			iterator::check(*this);
			pos -= count;
			pointer_to_optional = *pos;
			return *this;
		}


		const_iterator operator--(int) {
			iterator res(*this);
			--(*this);
			return res;
		}

		const_iterator & operator--() {
			iterator::check(*this);
			--pos;
			pointer_to_optional = *pos;
			return *this;
		}

		const T* operator->() {
			iterator::check( *this );
			return &(*pos)->value();
		}

		typename static_list<T,N>::DATA_INDEX::const_iterator get_it() const {
			iterator::check(*this);
			return pos;
		}

		friend class static_list<T,N>;
	};

	class reverse_iterator
	{
	public:
		typedef typename static_list<T,N>::value_type value_type;
		typedef typename static_list<T,N>::reference reference;
		typedef typename static_list<T,N>::const_reference const_reference;
		typedef typename static_list<T,N>::size_type size_type;
		typedef typename static_list<T,N>::difference_type difference_type;

	private:
		mutable typename static_list<T,N>::DATA_INDEX::iterator pos;
		static_list<T,N> *parent = nullptr;
		mutable size_type manipulation_count = 0;
		typename static_list<T,N>::DATA_INDEX::value_type pointer_to_optional;
		bool is_end = false;

	public:

		reverse_iterator() = default;

		reverse_iterator( const reverse_iterator & other ) = default;

	protected:
		reverse_iterator( static_list<T,N> *parent_, typename static_list<T,N>::DATA_INDEX::iterator pos_ )
		: pos( pos_ ),
		  parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().end() == pos ? nullptr : *pos ),
		  is_end( parent->get_index().end() == pos )
		{

		}

		reverse_iterator( static_list<T,N> *parent_,
				  typename static_list<T,N>::DATA_INDEX::const_iterator pos_ )
		: parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().end() == pos_ ? nullptr : *pos_ )
		{
			typename static_list<T,N>::DATA_INDEX::iterator it = parent->get_index().begin();
			typename static_list<T,N>::DATA_INDEX::const_iterator cit = parent->get_index().cbegin();

			while( it != parent->get_index().end() && cit != pos_ ) {
				++it;
				++cit;
			}

			pos = it;
			is_end = parent->get_index().end() == pos;
		}

	public:
		bool operator!=( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos == other_it.pos ) {
				return false;
			}

			return true;
		}

		bool operator==( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos != other_it.pos ) {
				return false;
			}

			return true;
		}

		T & operator*() {
			iterator::check( *this );
			return (*pos)->value();
		}

		const T & operator*() const {
			iterator::check( *this );
			return (*pos)->value();
		}

		reverse_iterator & operator++() {
			iterator::check(*this);

			// don't step beyond the end
			if( is_end ) {
				return *this;
			}

			if( pos == parent->get_index().begin() ) {
				is_end = true;
				pos = parent->get_index().end();
				pointer_to_optional = nullptr;
				return *this;
			}

			--pos;
			pointer_to_optional = *pos;
			return *this;
		}

		reverse_iterator operator++(int) {
			reverse_iterator res(*this);
			++(*this);
			return res;
		}

		reverse_iterator & operator+=(int count) {
			iterator::check(*this);
			pos += count;
			pointer_to_optional = *pos;
			is_end = pos == parent->get_index().end();
			return *this;
		}

		reverse_iterator operator+(int count) const {
			reverse_iterator ret(*this);
			ret += count;
			return ret;
		}

		reverse_iterator operator-(int count) const {
			reverse_iterator ret(*this);
			ret -= count;
			return ret;
		}

		reverse_iterator & operator-=(int count) {
			iterator::check(*this);
			pos -= count;
			pointer_to_optional = *pos;
			return *this;
		}


		reverse_iterator operator--(int) {
			reverse_iterator res(*this);
			--(*this);
			return res;
		}

		reverse_iterator & operator--() {
			iterator::check(*this);

			if( is_end ) {
				pos = parent->get_index().begin();
				pointer_to_optional = *pos;
				is_end = false;
				return *this;
			}

			++pos;

			if( pos == parent->get_index().end() ) {
				pointer_to_optional = nullptr;
				is_end = false;
				return *this;
			}

			pointer_to_optional = *pos;
			return *this;
		}

		T* operator->() {
			iterator::check( *this );
			return &(*pos)->value();
		}

		typename static_list<T,N>::DATA_INDEX::iterator get_it() const {
			iterator::check(*this);
			return pos;
		}

		friend class static_list<T,N>;
	};

	class const_reverse_iterator
	{
	public:
		typedef typename static_list<T,N>::value_type value_type;
		typedef typename static_list<T,N>::reference reference;
		typedef typename static_list<T,N>::const_reference const_reference;
		typedef typename static_list<T,N>::size_type size_type;
		typedef typename static_list<T,N>::difference_type difference_type;

	private:
		mutable typename static_list<T,N>::DATA_INDEX::iterator pos;
		const static_list<T,N> *parent = nullptr;
		mutable size_type manipulation_count = 0;
		typename static_list<T,N>::DATA_INDEX::value_type pointer_to_optional;
		bool is_end = false;

	public:

		const_reverse_iterator() = default;

		const_reverse_iterator( const const_reverse_iterator & other ) = default;

	protected:
		const_reverse_iterator( const static_list<T,N> *parent_, typename static_list<T,N>::DATA_INDEX::iterator pos_ )
		: pos( pos_ ),
		  parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().end() == pos ? nullptr : *pos ),
		  is_end( parent->get_index().end() == pos )
		{

		}

		const_reverse_iterator( const static_list<T,N> *parent_,
				  typename static_list<T,N>::DATA_INDEX::const_iterator pos_ )
		: parent( parent_ ),
		  manipulation_count( parent->get_manipulation_count() ),
		  pointer_to_optional( parent->get_index().end() == pos_ ? nullptr : *pos_ )
		{
			typename static_list<T,N>::DATA_INDEX::iterator it = parent->get_index().begin();
			typename static_list<T,N>::DATA_INDEX::const_iterator cit = parent->get_index().cbegin();

			while( it != parent->get_index().end() && cit != pos_ ) {
				++it;
				++cit;
			}

			pos = it;
			is_end = parent->get_index().end() == pos;
		}

	public:
		bool operator!=( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos == other_it.pos ) {
				return false;
			}

			return true;
		}

		bool operator==( const iterator & other_it ) const {
			check( *this );
			check( other_it );

			if( pos != other_it.pos ) {
				return false;
			}

			return true;
		}

		const T & operator*() const {
			iterator::check( *this );
			return (*pos)->value();
		}

		const_reverse_iterator & operator++() {
			iterator::check(*this);

			// don't step beyond the end
			if( is_end ) {
				return *this;
			}

			if( pos == parent->get_index().begin() ) {
				is_end = true;
				pos = parent->get_index().end();
				pointer_to_optional = nullptr;
				return *this;
			}

			--pos;
			pointer_to_optional = *pos;
			return *this;
		}

		const_reverse_iterator operator++(int) {
			const_reverse_iterator res(*this);
			++(*this);
			return res;
		}

		const_reverse_iterator & operator+=(int count) {
			iterator::check(*this);
			pos += count;
			pointer_to_optional = *pos;
			is_end = pos == parent->get_index().end();
			return *this;
		}

		const_reverse_iterator operator+(int count) const {
			const_reverse_iterator ret(*this);
			ret.pos += count;
			return ret;
		}

		const_reverse_iterator operator-(int count) const {
			const_reverse_iterator ret(*this);
			ret.pos -= count;
			return ret;
		}

		const_reverse_iterator & operator-=(int count) {
			iterator::check(*this);
			pos -= count;
			pointer_to_optional = *pos;
			return *this;
		}


		const_reverse_iterator operator--(int) {
			const_reverse_iterator res(*this);
			--(*this);
			return res;
		}

		const_reverse_iterator & operator--() {
			iterator::check(*this);

			if( is_end ) {
				pos = parent->get_index().begin();
				pointer_to_optional = *pos;
				is_end = false;
				return *this;
			}

			++pos;

			if( pos == parent->get_index().end() ) {
				pointer_to_optional = nullptr;
				is_end = false;
				return *this;
			}

			pointer_to_optional = *pos;
			return *this;
		}

		T* operator->() {
			iterator::check( *this );
			return &(*pos)->value();
		}

		typename static_list<T,N>::DATA_INDEX::iterator get_it() const {
			iterator::check(*this);
			return pos;
		}

		friend class static_list<T,N>;
	};

public:
	static_list()
	{
		// instance all optionals
		data.resize(N);
	}

	static_list( const static_list & other )
	: static_list()
	{
		assign(other.begin(),other.end());
	}

	static_list( const std::list<T> & other )
	: static_list()
	{
		assign(other.begin(),other.end());
	}

	static_list( const std::pmr::list<T> & other )
	: static_list()
	{
		assign(other.begin(),other.end());
	}

	static_list( size_type count, const T& value )
	: static_list()
	{
		assign( count, value );
	}

	explicit static_list( size_type count )
	: static_list()
	{
		resize(count);
	}

	static_list( std::initializer_list<T> init )
	: static_list()
	{
		assign(init);
	}

	template< class InputIt >
	static_list( InputIt first, InputIt last )
	: static_list()
	{
		assign(first,last);
	}

	static_list & operator=( const static_list & other ) {
		assign(other.begin(),other.end());
		return *this;
	}

	static_list& operator=( std::initializer_list<T> ilist ) {
		assign(ilist);
		return *this;
	}

	static_list & operator=( const std::list<T> & other ) {
		assign(other.begin(),other.end());
		return *this;
	}

	static_list & operator=( const std::pmr::list<T> & other ) {
		assign(other.begin(),other.end());
		return *this;
	}

	operator std::list<T> () {
		return std::list<T>(begin(),end());
	}

	operator std::pmr::list<T> () {
		return std::pmr::list<T>(begin(),end());
	}

	constexpr size_t capacity() const {
		return N;
	}

	/**
	 * Appends the given element value to the end of the container.
	 * No iterators or references are invalidated.
	 */
	void push_back( const T & v ) {
		auto last_entry = push_at_free_pos(v);
		index.push_back(last_entry);

		++manipulation_count;
	}

	/**
	 * Appends the given element value to the end of the container.
	 * No iterators or references are invalidated.
	 */
	void push_back( T && v ) {
		auto last_entry = push_at_free_pos(std::move(v));
		index.push_back(last_entry);

		++manipulation_count;
	}


	iterator begin() {
		return iterator(this,index.begin());
	}

	const_iterator begin() const {
		return const_iterator(this,index.begin());
	}

	const_iterator cbegin() const {
		return iterator(this,index.begin());
	}

	iterator end() {
		return iterator(this,index.end());
	}

	const_iterator end() const {
		return const_iterator(this,index.end());
	}

	const_iterator cend() const {
		return iterator(this,index.end());
	}

	reverse_iterator rbegin() {
		return reverse_iterator(this,--index.end());
	}

	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(iterator(this,--index.end()));
	}

	const_reverse_iterator crbegin() const {
		return const_reverse_iterator(iterator(this,--index.end()));
	}

	reverse_iterator rend() {
		// special behavior for this iterator, point to the end (means same as std::list::crend() )
		return reverse_iterator(this,index.end());
	}

	const_reverse_iterator rend() const {
		// special behavior for this iterator, point to the end (means same as std::list::crend() )
		return const_reverse_iterator(iterator(this,index.end()));
	}

	const_reverse_iterator crend() const {
		// special behavior for this iterator, point to the end (means same as std::list::crend() )
		return const_reverse_iterator(iterator(this,index.end()));
	}

	void assign( size_type count, const T & value ) {
		data.assign( count, value );
		rebuild_index();
	}

	template< class InputIt >
	void assign( InputIt first, InputIt last ) {
		data.assign( first, last );
		data.resize(N);
		rebuild_index();
	}

	void assign( std::initializer_list<T> ilist ) {
		data.clear();
		for( const T & t : ilist ) {
			data.push_back( t );
		}
		data.resize(N);
		rebuild_index();
	}

	/**
	 * Inserts value before pos.
	 * return: Iterator pointing to the inserted value.
	 * No iterators or references are invalidated.
	 */
	iterator insert( const_iterator pos, const T& value ) {
		++manipulation_count;
		auto last_value = push_at_free_pos( value );
		return iterator(this,index.insert(pos.get_it(), last_value ));
	}

	/**
	 * Inserts value before pos.
	 * return: Iterator pointing to the inserted value.
	 * No iterators or references are invalidated.
	 */
	iterator insert( const_iterator pos, T && value ) {
		++manipulation_count;
		auto last_value = push_at_free_pos( std::move(value) );
		return iterator(this,index.insert(pos.get_it(), last_value ));
	}

	/**
	 * inserts count copies of the value before pos.
	 * return: iterator pointing to the first element inserted, or pos if count == 0.
	 * No iterators or references are invalidated.
	 */
	iterator insert( const_iterator pos, size_type count, const T& value ) {

		if( count == 0 ) {
			return iterator(this, pos.get_it());
		}

		iterator ret;
		auto insert_pos = pos.get_it();

		for( size_type i = 0; i < count; ++i, ++pos ) {
			auto last_value = push_at_free_pos( value );

			++manipulation_count;
			iterator it = iterator(this,index.insert(pos.get_it(), last_value ));

			if( i == 0 ) {
				ret = it;
			}
		}

		return ret;
	}

	/**
	 * return: Iterator pointing to the first element inserted, or pos if first == last.
	 * No iterators or references are invalidated.
	 */
	template< class InputIt >
	iterator insert( const_iterator pos, InputIt first, InputIt last ) {

		if( first == last ) {
			return iterator( this, pos.get_it() );
		}

		std::optional<iterator> ret;
		iterator insert_pos( this, pos.get_it() );

		for( auto it = first; it != last; ++it ) {
			auto last_value = push_at_free_pos( *it );

			++manipulation_count;
			iterator rit = iterator(this,index.insert(insert_pos.get_it(), last_value ));

			if( !ret.has_value() ) {
				ret = rit;
			}
		}

		return ret.value();
	}

	/**
	 * Inserts elements from initializer list ilist before pos.
	 * return: Iterator pointing to the first element inserted, or pos if ilist is empty.
	 */
	iterator insert( const_iterator pos, std::initializer_list<T> ilist ) {

		if( ilist.begin() == ilist.end() ) {
			return iterator(this,pos.get_it());
		}

		std::optional<iterator> ret;
		iterator insert_pos( this, pos.get_it());

		for( auto it = ilist.begin(); it != ilist.end(); ++it, ++insert_pos ) {
			auto last_value = push_at_free_pos( *it );

			++manipulation_count;
			iterator rit = iterator(this,index.insert(insert_pos.get_it(), last_value ));

			if( !ret.has_value() ) {
				ret = rit;
			}
		}

		return ret.value();
	}

	size_type size() const noexcept {
		return index.size();
	}

	/**
	 * Removes the element at pos.
	 * If pos refers to the last element, then the end() iterator is returned.
	 * References and iterators to the erased elements are invalidated. Other references and iterators are not affected.
	 */
	iterator erase( const_iterator pos ) {

		if( auto it_end = end(); pos == it_end ) {
			return it_end;
		}

		(*pos.get_it())->reset();

		++manipulation_count;
		return iterator(this,index.erase(pos.pos));
	}

	/**
	 * Removes the elements in the range first, last.
	 * References and iterators to the erased elements are invalidated. Other references and iterators are not affected.
	 */
	iterator erase( const_iterator first, const_iterator last ) {

		if( first == last ) {
			return iterator(this,last.get_it());
		}

		iterator it = iterator(this,first.get_it());
		while( it != last && it != end() ) {
			it = erase(it);
		}

		return it;
	}

	bool empty() {
		return index.empty();
	}

	[[nodiscard]] bool empty() const noexcept {
		return index.empty();
	}

	reference front() {
		return index.front()->value();
	}

	const_reference front() const {
		return index.front()->value();
	}

	reference back() {
		return index.back()->value();
	}

	const_reference back() const {
		return index.back()->value();
	}

	constexpr size_type max_size() const noexcept {
		return N;
	}

	void clear() noexcept {
		data.clear();
		data.resize(N);
		index.clear();
		++manipulation_count;
	}

	/**
	 * Appends a new element to the end of the container.
	 * return: A reference to the inserted element.
	 * No iterators or references are invalidated.
	 */
	template< class... Args >
	reference emplace_back( Args&&... args ) {
		auto last_element = emplace_at_free_pos( args... );
		++manipulation_count;
		index.push_back( last_element );
		return last_element;
	}

	/**
	 * Inserts a new element into the container directly before pos.
	 * return: Iterator pointing to the emplaced element.
	 * No iterators or references are invalidated.
	 */
	template< class... Args >
	iterator emplace( const_iterator pos, Args&&... args ) {
		auto last_element = emplace_at_free_pos( args... );
		++manipulation_count;
		return iterator(this,index.insert(pos.get_it(),last_element));
	}

	/**
	 * Removes the last element of the container.
	 * Calling pop_back on an empty container results in undefined behavior.
	 * References and iterators to the erased element are invalidated.
	 */
	void pop_back() {
		index.back()->reset();
		index.pop_back();
		++manipulation_count;
	}

	/**
	 * Prepends the given element value to the beginning of the container.
	 * No iterators or references are invalidated.
	 */
	void push_front( const T& value ) {
		auto last_element = push_at_free_pos( value );
		index.insert(index.begin(),last_element);
		++manipulation_count;
	}

	/**
	 * Prepends the given element value to the beginning of the container.
	 * No iterators or references are invalidated.
	 */
	void push_front( T&& value ) {
		auto last_element = push_at_free_pos( std::move(value) );
		index.insert(index.begin(),last_element);
		++manipulation_count;
	}

	/**
	 * Inserts a new element to the beginning of the container.
	 * return: A reference to the inserted element.
	 * No iterators or references are invalidated.
	 */
	template< class... Args >
	reference emplace_front( Args&&... args ) {
		auto last_element = emplace_at_free_pos( args... );
		index.insert(index.begin(),last_element);
		++manipulation_count;
		return last_element->value();
	}

	/**
	 * Removes the first element of the container.
	 * References and iterators to the erased element are invalidated.
	 */
	void pop_front() {
		index.front()->reset();
		index.erase(index.begin());
		++manipulation_count;
	}

	/**
	 * Resizes the container to contain count elements, does nothing if count == size().
	 * References and iterators to erased elements are invalidated.
	 */
	void resize( size_type count ) {
		if( count == size() ) {
			return;
		}

		while( size() > count ) {
			pop_back();
		}

		while( size() < count ) {
			push_back( T() );
		}
	}

	/**
	 * Resizes the container to contain count elements, does nothing if count == size().
	 * References and iterators to erased elements are invalidated.
	 */
	void resize( size_type count, const value_type& value ) {
		if( count == size() ) {
			return;
		}

		while( size() > count ) {
			pop_back();
		}

		while( size() < count ) {
			push_back( value );
		}
	}

	/**
	 * Reverses the order of the elements in the container.
	 * No references or iterators become invalidated.
	 */
	void reverse() noexcept {
		std::reverse(index.begin(), index.end());
		++manipulation_count;
	}

	/**
	 * Removes all elements satisfying specific criteria.
	 * Invalidates only the iterators and references to the removed elements.
	 * Removes all elements that are equal to value (using operator==).
	 */
	size_type remove( const T& value ) {
		size_type count = 0;

		auto it_end = end();
		for( iterator it = begin(); it != it_end; ) {
			if( *it == value ) {
				it = erase(it);
				++count;
			} else {
				++it;
			}
		}

		return count;
	}

	/**
	 * Removes all elements satisfying specific criteria.
	 * Invalidates only the iterators and references to the removed elements.
	 * Removes all elements for which predicate p returns true.
	 */
	template< class UnaryPredicate >
	size_type remove_if( UnaryPredicate p ) {
		size_type count = 0;

		auto it_end = end();
		for( iterator it = begin(); it != it_end; ) {
			if( p(*it) ) {
				it = erase(it);
				++count;
			} else {
				++it;
			}
		}

		return count;
	}

protected:
	template<class ForwardIt, class Compare>
	static void selection_sort(ForwardIt begin, ForwardIt end, Compare comp)
	{
		for (ForwardIt it = begin; it != end; ++it) {
			std::iter_swap(it, std::min_element(it, end, comp ));
		}
	}

public:
	/**
	 * Sorts the elements and preserves the order of equivalent elements.
	 * No references or iterators become invalidated.
	 */
	void sort() {
		selection_sort( begin(), end(), std::less<T>() );
	}

	template< class Compare >
	void sort( Compare comp ) {
		selection_sort( begin(), end(), comp );
	}

	/**
	 * Removes all consecutive duplicate elements from the container.
	 * Only the first element in each group of equal elements is left.
	 * Invalidates only the iterators and references to the removed elements.
	 * Uses p to compare the elements.
	 * return: The number of elements removed.
	 */
	template< class BinaryPredicate >
	size_type unique( BinaryPredicate p ) {

		size_type count = 0;
		auto it_end = end();
		for( auto it = begin(), last = it_end; it != it_end; ) {

			if( last == it_end ) {
				last = it;
				 ++it;
				continue;
			}

			if( p( *last, *it ) ) {
				it = erase(it);
				++count;
				continue;
			}

			last = it;
			++it;
		}

		return count;
	}

	/**
	 * Removes all consecutive duplicate elements from the container.
	 * Only the first element in each group of equal elements is left.
	 * Invalidates only the iterators and references to the removed elements.
	 * Uses operator== to compare the elements.
	 * return: The number of elements removed.
	 */
	size_type unique() {
		return unique( std::equal_to<T>() );
	}

public:
	// non standard access operators
	T & operator[]( size_t index ) {
		return static_list<T,N>::get_index()[index]->value();
	}

	const T & operator[]( size_t index ) const {
		return static_list<T,N>::get_index()[index]->value();
	}

	T & at( size_t index ) {
		return static_list<T,N>::get_index().at(index)->value();
	}

	const T & at( size_t index ) const {
		return static_list<T,N>::get_index().at(index)->value();
	}

protected:

	void rebuild_index() {
		index.clear();
		for( auto & d : data ) {
			if( d.has_value() ) {
				index.push_back( &d );
			}
		}

		++manipulation_count;
	}

	typename DATA_INDEX::value_type push_at_free_pos( const T & v ) {
		for( auto & d : data ) {
			if( !d.has_value() ) {
				d = v;
				return &d;
			}
		}
		throw std::out_of_range("max capacity reached");
	}

    typename DATA_INDEX::value_type push_at_free_pos( T && v ) {
		for( auto & d : data ) {
			if( !d.has_value() ) {
				d = std::move(v);
				return &d;
			}
		}
		throw std::out_of_range("max capacity reached");
	}

	template< class... Args >
    typename DATA_INDEX::value_type emplace_at_free_pos( Args&&... args ) {
		for( auto & d : data ) {
			if( !d.has_value() ) {
				d.emplace(args...);
				return &d;
			}
		}
		throw std::out_of_range("max capacity reached");
	}

	// internal functions

	size_type get_manipulation_count() const {
		return manipulation_count;
	}

	auto & get_index() {
		return index;
	}

	const auto & get_index() const {
		return index;
	}
};

} // namespace Tools

#endif /* CPPUTILS_CPPUTILSSHARED_STATIC_LIST_H_ */
