/*
 * @author Copyright (c) 2023 Martin Oberzalek
 *		   Copyright (c) 2023 Anton Paar GmbH
 */

#ifndef CPPUTILS_CPPUTILSSHARED_STATIC_VECTOR_H_
#define CPPUTILS_CPPUTILSSHARED_STATIC_VECTOR_H_

#include <array>
#include <vector>
#include <memory_resource>

namespace Tools {

#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored  "-Wreorder"  
#endif
/**
 * A vector class that uses no heap.
 * The maximum capacity is defined at compile time.
 * Same behavior as std::vector.
 *
 */
template <typename T,std::size_t N>
class static_vector : public std::pmr::vector<T>
{
	alignas(T) std::array<std::byte,sizeof(T)*N> buffer;
	std::pmr::monotonic_buffer_resource mbr{ buffer.data(), buffer.size(), std::pmr::null_memory_resource() };

public:

	typedef typename std::pmr::vector<T>::value_type value_type;
	typedef typename std::pmr::vector<T>::reference reference;
	typedef typename std::pmr::vector<T>::const_reference const_reference;
	typedef typename std::pmr::vector<T>::size_type size_type;
	typedef typename std::pmr::vector<T>::difference_type difference_type;
	typedef typename std::pmr::vector<T>::iterator iterator;
	typedef typename std::pmr::vector<T>::const_iterator const_iterator;
	typedef typename std::pmr::vector<T>::reverse_iterator reverse_iterator;

public:
	static_vector()
	: buffer(), // it is madatory to initialize the buffer first
	  mbr( buffer.data(), buffer.size(), std::pmr::null_memory_resource() ),
	  std::pmr::vector<T>( &mbr )
	  {
		std::pmr::vector<T>::reserve(N);
	  }

	static_vector( const static_vector & other )
	: static_vector()
	{
		std::pmr::vector<T>::assign(other.begin(),other.end());
	}


#if __cplusplus >= 202002
	constexpr
#endif
	static_vector( size_type count, const T& value )
	: static_vector()
	{
		std::pmr::vector<T>::assign( count, value );
	}

#if __cplusplus >= 202002
	constexpr
#endif
	explicit static_vector( size_type count )
	: static_vector()
	{
		std::pmr::vector<T>::resize(count);
	}

#if __cplusplus >= 202002
	constexpr
#endif
	static_vector( std::initializer_list<T> init )
	: static_vector()
	{
		std::pmr::vector<T>::assign(init);
	}

#if __cplusplus >= 202002
	constexpr
#endif
	static_vector & operator=( const static_vector & other ) {
		std::pmr::vector<T>::assign(other.begin(),other.end());
		return *this;
	}

#if __cplusplus >= 202002
	constexpr
#endif
	static_vector& operator=( std::initializer_list<T> ilist ) {
		std::pmr::vector<T>::assign(ilist);
		return *this;
	}

#if __cplusplus >= 202002
	constexpr
#endif
	static_vector & operator=( const std::vector<T> & other ) {
		std::pmr::vector<T>::assign(other.begin(),other.end());
		return *this;
	}

#if __cplusplus >= 202002
	constexpr
#endif
	static_vector & operator=( const std::pmr::vector<T> & other ) {
		std::pmr::vector<T>::assign(other.begin(),other.end());
		return *this;
	}

	operator std::vector<T> () {
		return std::vector<T>(std::pmr::vector<T>::begin(),std::pmr::vector<T>::end());
	}

	constexpr size_t capacity() const {
		return N;
	}
};

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
  
} // namespace Tools

#endif /* CPPUTILS_CPPUTILSSHARED_STATIC_VECTOR_H_ */
