/*
 * @author Copyright (c) 2023 Martin Oberzalek
 *		   Copyright (c) 2023 Anton Paar GmbH
 * Basic version from: https://stackoverflow.com/a/56334722
 */

#ifndef SRC_CYCLICARRAY_H_
#define SRC_CYCLICARRAY_H_

#include "static_list.h"

namespace Tools {

template <typename T, std::size_t N>
class CyclicArray : public static_list<T,N>
{
 public:

  CyclicArray() = default;
  CyclicArray( const CyclicArray & other ) = default;

  CyclicArray( std::initializer_list<T> list )
  : static_list<T,N>( list )
  {
  }

  /**
   * throws std::out_of_range exception if buffer is full and discard_front is false
   **/
  void push_back(const T& v, bool discard_front = false ) {
    if (static_list<T,N>::size() + 1 > N) {
      if (discard_front) {          
    	  static_list<T,N>::pop_front();
      }
      else {
    	throw std::out_of_range("max size reached");
      }
    }

    static_list<T,N>::push_back( v );
  }

  /**
   * throws std::out_of_range exception if buffer is full and discard_front is false
   **/
  void push_back( T&& v, bool discard_front = false ) {
    if (static_list<T,N>::size() + 1 > N) {
      if (discard_front) {
    	  static_list<T,N>::pop_front();
      }
      else {
#if __cpp_exceptions > 0
    	throw std::out_of_range("max size reached");
#else
    	std:abort();
#endif
      }
    }

    static_list<T,N>::push_back( std::move(v) );
  }

};

} // namespace Tools

#endif /* SRC_CYCLICARRAY_H_ */
