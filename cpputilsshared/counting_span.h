#pragma once

#include <span>
#include <stdexcept>

namespace Tools {

namespace internal {

template <class T>
class span_wrapper
{
public:
  // https://en.cppreference.com/w/cpp/container/span.html
  // same as defined in std::span
  using element_type            = T;
  using value_type              = std::remove_cv_t<T>;
  using size_type               = std::span<T>::size_type;
  using difference_type         = std::ptrdiff_t;
  using pointer                 = T*;
  using const_pointer           = const T*;
  using reference               = T&;
  using const_reference         = const T&;
  using iterator                = std::span<T>::iterator;
  using const_iterator          = std::span<T>::const_iterator;
  using reverse_iterator        = std::span<T>::reverse_iterator;
  using const_reverse_iterator  = std::span<T>::const_reverse_iterator;

private:

  std::span<T> & m_span;

protected:

  span_wrapper( std::span<T> & span )
  : m_span( span )
  {}

public:

  constexpr iterator begin() const noexcept {
    return m_span.begin();
  }

  constexpr const_iterator cbegin() const noexcept {
    return m_span.cbegin();
  }

  constexpr iterator end() const noexcept {
    return m_span.end();
  }

  constexpr reverse_iterator rend() const noexcept {
    return m_span.rend();
  }

  constexpr const_iterator cend() const noexcept {
    return m_span.cend();
  }

  constexpr const_reverse_iterator crend() const noexcept {
    return m_span.crend();
  }


  constexpr reverse_iterator rbegin() const noexcept {
    return m_span.rbegin();
  }

  constexpr const_reverse_iterator crbegin() const noexcept {
    return m_span.crbegin();
  }

  constexpr reference front() const {
    return m_span.front();
  }

  constexpr reference back() const {
    return m_span.back();
  }

#if __cpp_lib_span >= 202311L
  constexpr reference at( size_type pos ) const {
    return m_span.at( pos );
  }
#endif

  constexpr pointer data() const noexcept {
    return m_span.data();
  }

  constexpr size_type size() const noexcept {
    return m_span.size();
  }

  constexpr size_type size_bytes() const noexcept {
    return m_span.size_bytes();
  }

  constexpr bool empty() const noexcept {
    return m_span.empty();
  }


};

} // namespace internal


template<class T>
class counting_span;

template<class T>
class sub_counting_span : public internal::span_wrapper<T>
{
public:
  // https://en.cppreference.com/w/cpp/container/span.html
  // same as defined in std::span
  using element_type            = T;
  using value_type              = std::remove_cv_t<T>;
  using size_type               = std::span<T>::size_type;
  using difference_type         = std::ptrdiff_t;
  using pointer                 = T*;
  using const_pointer           = const T*;
  using reference               = T&;
  using const_reference         = const T&;
  using iterator                = std::span<T>::iterator;
  using const_iterator          = std::span<T>::const_iterator;
  using reverse_iterator        = std::span<T>::reverse_iterator;
  using const_reverse_iterator  = std::span<T>::const_reverse_iterator;

protected:
  std::span<T>              m_subspan {};
  sub_counting_span*        m_next    { nullptr };
  sub_counting_span*        m_prev    { nullptr };
  counting_span<T>*         m_parent  { nullptr };

public:
  sub_counting_span()
  : internal::span_wrapper<T>( m_subspan )
  {}

  sub_counting_span( counting_span<T> & parent, sub_counting_span *prev, std::span<T> subspan )
  : internal::span_wrapper<T>( m_subspan ),
    m_subspan( subspan ),
    m_prev( prev ),
    m_parent( &parent )
  {}

  sub_counting_span( sub_counting_span && other );

  ~sub_counting_span()
  {
    disconnect();
  }

  auto & operator=( sub_counting_span && other );

  T & operator[]( std::size_t idx ) {
    check();
    return m_subspan[idx];
  }

  const T & operator[]( std::size_t idx ) const {
    check();
    return m_subspan[idx];
  }

  sub_counting_span<T> subspan( std::size_t idx )
  {
   return subspan( idx, m_subspan.size() - idx );
  }

  sub_counting_span<T> subspan( std::size_t idx, std::size_t size )
  {
    check();

    auto diff = &m_subspan[idx] - &m_parent->m_origin[0];

    return m_parent->subspan( diff, size );
  }

  sub_counting_span<T> first( size_type count ) {
    return subspan( 0, count );
  }

  sub_counting_span<T> last( size_type count ) {
    return subspan( m_subspan.size() - count - 1 );
  }

  friend class counting_span<T>;

protected:
  void check()
  {
    if( m_parent == nullptr ) {
        throw std::runtime_error( "use after free" );
    }
  }

  void disconnect_from_parent() {
    m_parent = nullptr;
  }

  void disconnect() {

    if( m_parent == nullptr ) {
        return;
    }

    if( m_parent->m_last_child == this ) {
        m_parent->m_last_child = m_prev;
    }

    if( m_prev ) {
        m_prev->m_next = m_next;
    }

    if( m_next ) {
        m_next->m_prev = m_prev;
    }

    m_parent = nullptr;
    m_prev   = nullptr;
    m_next   = nullptr;
  }

  void clear() {
    m_parent = nullptr;
    m_prev   = nullptr;
    m_next   = nullptr;
    m_subspan = {};
  }
};

template<class T>
class counting_span : public internal::span_wrapper<T>
{
public:
  // https://en.cppreference.com/w/cpp/container/span.html
  // same as defined in std::span
  using element_type            = T;
  using value_type              = std::remove_cv_t<T>;
  using size_type               = std::span<T>::size_type;
  using difference_type         = std::ptrdiff_t;
  using pointer                 = T*;
  using const_pointer           = const T*;
  using reference               = T&;
  using const_reference         = const T&;
  using iterator                = std::span<T>::iterator;
  using const_iterator          = std::span<T>::const_iterator;
  using reverse_iterator        = std::span<T>::reverse_iterator;
  using const_reverse_iterator  = std::span<T>::const_reverse_iterator;

private:
    std::span<T>          m_origin;
    sub_counting_span<T>* m_last_child  { nullptr };

public:

    counting_span( std::span<T> & origin )
    : internal::span_wrapper<T>( m_origin ),
      m_origin( origin )
    {}

    counting_span( T* data, std::size_t size )
    : internal::span_wrapper<T>( m_origin ),
      m_origin( data, size )
    {}

    ~counting_span()
    {
      // unregister all children
      auto child = m_last_child;
      while( child ) {
        auto next = child->m_prev;
        child->disconnect_from_parent();
        child = next;
      }

      m_last_child  = nullptr;
    }

    sub_counting_span<T> subspan( std::size_t idx )
    {
     return subspan( idx, m_origin.size() - idx );
    }

    sub_counting_span<T> subspan( std::size_t idx, std::size_t size )
    {
      auto sub = sub_counting_span( *this, m_last_child, m_origin.subspan( idx, size ) );

      if( m_last_child ) {
          m_last_child->m_next = &sub;
      }

      m_last_child = &sub;

      return std::move(sub);
    }

    sub_counting_span<T> first( size_type count ) {
      return subspan( 0, count );
    }

    sub_counting_span<T> last( size_type count ) {
      return subspan( m_origin.size() - count - 1 );
    }

    T & operator[]( std::size_t idx ) {
      return m_origin[idx];
    }

    const T & operator[]( std::size_t idx ) const {
      return m_origin[idx];
    }

    friend class sub_counting_span<T>;
};

template<class T>
sub_counting_span<T>::sub_counting_span( sub_counting_span && other )
: internal::span_wrapper<T>( m_subspan ),
  m_subspan( std::move(other.m_subspan) ),
  m_next( other.m_next ),
  m_prev( other.m_prev ),
  m_parent( other.m_parent )
{
  check();
  if( m_prev ) {
      m_prev->m_next = this;
  }

  if( m_next ) {
      m_next->m_prev = this;
  }

  if( m_parent->m_last_child == &other ) {
      m_parent->m_last_child = this;
  }

  other.clear();
}

template<class T>
auto & sub_counting_span<T>::operator=( sub_counting_span && other )
{
  disconnect();

  m_subspan = std::move( other.m_subspan );
  m_next    = other.m_next;
  m_prev    = other.m_prev;
  m_parent  = other.m_parent;

  if( m_prev ) {
      m_prev->m_next = this;
  }

  if( m_next ) {
      m_next->m_prev = this;
  }

  if( m_parent->m_last_child == &other ) {
      m_parent->m_last_child = this;
  }

  other.clear();

  return *this;
}

} // namespace Tools
