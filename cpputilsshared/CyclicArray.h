/*
 * @author Copyright (c) 2023 Martin Oberzalek
 *		   Copyright (c) 2023 Anton Paar GmbH
 * Basic version from: https://stackoverflow.com/a/56334722
 */

#ifndef SRC_CYCLICARRAY_H_
#define SRC_CYCLICARRAY_H_

#include <iostream>
#include <cstddef>
#include <array>
#include <queue>
#include <stdexcept>

template <typename T, std::size_t N = 2>
class CyclicArray
{
 public:
  typedef typename std::array<T, N>::value_type value_type;
  typedef typename std::array<T, N>::reference reference;
  typedef typename std::array<T, N>::const_reference const_reference;
  typedef typename std::array<T, N>::size_type size_type;
  typedef typename std::array<T, N>::difference_type difference_type;
  static constexpr size_t npos = N;

  class iterator
  {
  public:
	  typedef typename std::array<T, N>::value_type value_type;
	  typedef typename std::array<T, N>::reference reference;
	  typedef typename std::array<T, N>::const_reference const_reference;
	  typedef typename std::array<T, N>::size_type size_type;
	  typedef typename std::array<T, N>::difference_type difference_type;

  private:
	  CyclicArray<T,N> * data = nullptr;
	  size_t pos = 0;

  public:
	  iterator( const iterator & other ) = default;

	  iterator( CyclicArray<T,N> * data_, size_t pos_ )
	  : data( data_ ),
		pos( pos_ )
	  {}

	  bool operator!=( const iterator & other_it ) const {
		  if( pos == other_it.pos ) {
			  return false;
		  }

		  return true;
	  }

	  bool operator==( const iterator & other_it ) const {
		  if( pos != other_it.pos ) {
			  return false;
		  }

		  return true;
	  }

	  T & operator*() {
		  return data->at(pos);
	  }

	  const T & operator*() const {
		  return data->at(pos);
	  }

	  iterator & operator++() {
		  ++pos;
		  return *this;
	  }

	  iterator operator++(int) {
		  iterator res(*this);
		  ++(*this);
		  return res;
	  }

	  iterator & operator+=(int count) {
		  pos += count;
		  return *this;
	  }

	  iterator operator+(int count) const {
		  iterator ret(*this);
		  ret.pos += count;
		  return ret;
	  }

	  iterator operator-(int count) const {
		  iterator ret(*this);
		  ret.pos -= count;
		  return ret;
	  }

	  iterator & operator-=(int count) {
		  pos -= count;
		  return *this;
	  }


	  iterator operator--(int) {
		  iterator res(*this);
		  --(*this);
		  return res;
	  }

	  iterator & operator--() {
		  --pos;
		  return *this;
	  }

      T* operator->() {
        return &data->at(pos);
      }

      friend class const_iterator;
  };

  class const_iterator
    {
    public:
	  typedef typename std::array<T, N>::value_type value_type;
	  typedef typename std::array<T, N>::reference reference;
	  typedef typename std::array<T, N>::const_reference const_reference;
	  typedef typename std::array<T, N>::size_type size_type;
	  typedef typename std::array<T, N>::difference_type difference_type;

    private:
  	  const CyclicArray<T,N> * data = nullptr;
  	  size_t pos = 0;

    public:
  	  const_iterator( const const_iterator & other ) = default;

  	  const_iterator( const CyclicArray<T,N> * data_, size_t pos_ )
  	  : data( data_ ),
  		pos( pos_ )
  	  {}

	  const_iterator( const iterator & other )
	  : data( other.data ),
	    pos( other.pos )
	  {}

  	  bool operator!=( const const_iterator & other_it ) const {
  		  if( pos == other_it.pos ) {
  			  return false;
  		  }

  		  return true;
  	  }

  	  bool operator==( const const_iterator & other_it ) const {
  		  if( pos != other_it.pos ) {
  			  return false;
  		  }

  		  return true;
  	  }

  	  const T & operator*() const {
  		  return data->at(pos);
  	  }

  	  const_iterator & operator++() {
  		  ++pos;
  		  return *this;
  	  }

  	  const_iterator operator++(int) {
  		  iterator res(*this);
  		  ++(*this);
  		  return res;
  	  }

  	  const_iterator & operator+=(int count) {
  		  pos += count;
  		  return *this;
  	  }

  	  const_iterator operator+(int count) const {
  		  iterator ret(*this);
  		  ret.pos += count;
  		  return ret;
  	  }

  	  const_iterator operator-(int count) const {
  		  iterator ret(*this);
  		  ret.pos -= count;
  		  return ret;
  	  }

  	  const_iterator & operator-=(int count) {
  		  pos -= count;
  		  return *this;
  	  }


  	  const_iterator operator--(int) {
  		  iterator res(*this);
  		  --(*this);
  		  return res;
  	  }

  	  const_iterator & operator--() {
  		  --pos;
  		  return *this;
  	  }

      const T* operator->() const {
          return &data->at(pos);
      }
    };


  typedef typename std::reverse_iterator<iterator> reverse_iterator;
  typedef typename std::reverse_iterator<const_iterator> const_reverse_iterator;

 public:

  CyclicArray() = default;
  CyclicArray( const CyclicArray & other ) = default;

  CyclicArray( std::initializer_list<T> list )
  {
	  if( list.size() > N ) {
		  throw std::out_of_range("initialation size exceeds max size");
	  }

	  for( const T & t : list ) {
		  push_back( t );
	  }
  }

  ~CyclicArray() {
  	clear();
  }

  /**
   ** throws exception if buffer is full
   **/
  void push_back(const T& v) {
    if (size_ + 1 > N) {
      throw;
    }
    new (&array_[(front_ + size_) % N]) T(v);
    ++size_;
  }

  /**
   ** throws std::out_of_range exception if buffer is full and discard_front is false
   **/
  void push_back(const T& v, bool discard_front ) {
    if (size_ + 1 > N)
    {
      if (discard_front) {          
        pop_front();
      }
      else {
        throw std::out_of_range("max size reached");
      }
    }
    new (&array_[(front_ + size_) % N]) T(v);
    ++size_;
  }

  void pop_front() {
  	if (size_ < 1) {
  	  throw;
  	}

  	front().~T();
  	++front_;
  	--size_;
  	if (front_ >= N)
  	  front_ = 0;
  }

  const_reference front() const {
  	return array_[front_];
  }

  reference front() {
  	return array_[front_];
  }

  size_type size() const {
    return size_;
  }

  constexpr size_type capacity() const {
	  return N;
  }

  bool empty() const {
	 return (size() == 0);
  }

  const_reference operator[]( size_t n ) const {
  	return array_[(front_+n)%N];
  }

  reference operator[]( size_t n ) {
  	return array_[(front_+n)%N];
  }

  reference at( size_t pos ) {
	  if (pos >= size()) {
		  throw std::out_of_range("out of range");
	  }
	  return array_[(front_+pos)%N];
  }

  const_reference at( size_t pos ) const {
	  if (pos >= size()) {
		  throw std::out_of_range("out of range");
	  }
	  return array_[(front_+pos)%N];
  }

  iterator begin() {
	  return iterator(this,0);
  }

  const_iterator begin() const {
  	  return const_iterator(this,0);
  }

  reverse_iterator rbegin() {
	  return reverse_iterator(iterator(this,size_));
  }

  const_reverse_iterator crbegin() const {
	  return reverse_iterator(const_iterator(this,size_));
  }

  iterator end() {
	  return iterator(this,size_);
  }

  const_iterator end() const {
	  return const_iterator(this,size_);
  }

  reverse_iterator rend() {
	  return reverse_iterator(iterator(this,0));
  }

  const_reverse_iterator crend() const {
	  return reverse_iterator(const_iterator(this,0));
  }

  void clear() {
    while( !empty() ) {
      pop_front();
    }
  }

  iterator erase( iterator pos );

  constexpr size_type max_size() const {
	  return N;
  }

 private:
  size_type front_ = 0;
  size_type size_ = 0;
  std::array<T, N> array_{};
};

template<typename T, std::size_t N>
typename CyclicArray<T,N>::iterator operator+(  int c, const typename CyclicArray<T,N>::iterator & it )
{
	typename CyclicArray<T,N>::iterator res(it);
	res += c;
	return res;
}

template<typename T, std::size_t N>
typename CyclicArray<T,N>::iterator operator-(  int c, const typename CyclicArray<T,N>::iterator & it )
{
	typename CyclicArray<T,N>::iterator res(it);
	res -= c;
	return res;
}


template<class T, std::size_t N>
typename CyclicArray<T,N>::iterator CyclicArray<T,N>::erase( iterator pos )
{
	if( pos == begin() ) {
	  pop_front();
	  return begin();
	}

	if( pos == end() ) {
	  return end();
	}

	if( empty() ) {
	  return end();
	}

	if( size() == 1 ) {
	  clear();
	  return end();
	}

	iterator ret = pos;

	for( iterator next = pos + 1; next != end(); pos++, next++ ) {
		*pos = std::move(*(next));
	}

	--size_;

	return ret;
}

#endif /* SRC_CYCLICARRAY_H_ */
