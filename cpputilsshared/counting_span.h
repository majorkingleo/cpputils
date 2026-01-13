#pragma once

#include <span>
#include <stdexcept>

#include <CpputilsDebug.h>
#include <format.h>
#include <map>

namespace Tools {

std::string name( void* ptr, const char *name = nullptr ) {

  static std::map<void*, std::string> s_names;
  static unsigned                     s_counter = 0;

  if( s_names.find( ptr ) != s_names.end() ) {
      return s_names[ptr];
  }

  if( ptr == nullptr ) {
      return "nullptr";
  }

  if( name ) {
      s_names[ptr] = Tools::format( "%s(%p)", name, ptr );
  }

  return s_names[ptr] = Tools::format( "%d(%p)", ++s_counter, ptr );
}


template<class T>
class counting_span;

template<class T>
class sub_counting_span
{
protected:
  std::span<T>              m_subspan {};
  sub_counting_span*        m_next    { nullptr };
  sub_counting_span*        m_prev    { nullptr };
  counting_span<T>*         m_parent  { nullptr };

public:
  sub_counting_span() = default;

  sub_counting_span( counting_span<T> & parent, sub_counting_span *prev, std::span<T> subspan )
  : m_subspan( subspan ),
    m_prev( prev ),
    m_parent( &parent )
  {}

  sub_counting_span( sub_counting_span && other );

  ~sub_counting_span()
  {
    CPPDEBUG( Tools::format( "%s %s", __FUNCTION__, name(this) ) );
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



































  sub_counting_span<T> subspan( std::size_t idx, std::size_t size )
  {
    check();

    auto diff = &m_subspan[idx] - &m_parent->m_origin[0];

    CPPDEBUG( Tools::format( "%s %s creating subspan idx %d => base_idx: %d", __FUNCTION__, name(this), idx, diff ));

    return m_parent->subspan( diff, size );
    /*
    auto sub = sub_counting_span( *m_parent, m_parent->m_last_child, m_subspan.subspan( idx, size ) );

    CPPDEBUG( Tools::format( "%s %p creating subspan: %s m_last_child: %s", __FUNCTION__, name(this), name(m_parent->m_last_child) ));

    if( m_parent->m_last_child ) {
        m_parent->m_last_child->m_next = &sub;
    }

    m_parent->m_last_child = &sub;

    if( m_next == nullptr ) {
        m_next = &sub;
    }

    return std::move(sub);
    */
  }

  friend class counting_span<T>;

protected:
  void check()
  {
    CPPDEBUG( Tools::format( "%s %p parent: %p", __FUNCTION__, name(this), m_parent ) );
    if( m_parent == nullptr ) {
        throw std::runtime_error( "use after free" );
    }
  }

  void disconnect_from_parent() {
    CPPDEBUG( Tools::format( "%s %p disconnecting", __FUNCTION__, name(this) ) );
    m_parent = nullptr;
    CPPDEBUG( Tools::format( "%s %p parent: %p", __FUNCTION__, name(this), m_parent ) );
  }

  void disconnect() {

    if( m_parent == nullptr ) {
        return;
    }
/*
    if( m_parent->m_first_child == this ) {
        m_parent->m_first_child = m_next;
    }
*/
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
class counting_span
{
    std::span<T>          m_origin;
    //sub_counting_span<T>* m_first_child { nullptr };
    sub_counting_span<T>* m_last_child  { nullptr };

public:

    counting_span( std::span<T> & origin )
    : m_origin( origin )
    {}

    counting_span( T* data, std::size_t size )
    : m_origin( data, size )
    {}

    ~counting_span()
    {
      // unregister all children
      auto child = m_last_child;
      while( child ) {
        auto next = child->m_prev;
        CPPDEBUG( Tools::format( "%s disconnect child %p", __FUNCTION__, name(child) ) );
        child->disconnect_from_parent();
        child = next;
      }

      //m_first_child = nullptr;
      m_last_child  = nullptr;
    }

    sub_counting_span<T> subspan( std::size_t idx, std::size_t size )
    {
      auto sub = sub_counting_span( *this, m_last_child, m_origin.subspan( idx, size ) );

      if( m_last_child ) {
          m_last_child->m_next = &sub;
      }

      m_last_child = &sub;
/*
      if( m_first_child == nullptr ) {
        m_first_child = &sub;
      }
*/
      return std::move(sub);
    }


    T & operator[]( std::size_t idx ) {
      return m_origin[idx];
    }

    const T & operator[]( std::size_t idx ) const {
      return m_origin[idx];
    }

    friend class sub_counting_span<T>;


    void print()
    {
      CPPDEBUG( Tools::format( "counting_span %p: last %p", this, name(m_last_child) ) );

      auto child = m_last_child;
      auto first_child = child;

      while( child ) {
        child = child->m_prev;
        if( child ) {
            first_child = child;
        }
      }

      child = first_child;
      while( child ) {
        CPPDEBUG( Tools::format( " %p <- %p -> %p%s", name(child->m_prev), name(child), name(child->m_next),  child->m_next ? "" : "\n" ) );
        child = child->m_next;
      }
    }

protected:


};

template<class T>
sub_counting_span<T>::sub_counting_span( sub_counting_span && other )
: m_subspan( std::move(other.m_subspan) ),
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
/*
  if( m_parent->m_first_child == &other ) {
      CPPDEBUG( Tools::format( "%s %p update first child", __FUNCTION__, name(this) ) );
      m_parent->m_first_child = this;
  }
*/
  if( m_parent->m_last_child == &other ) {
      CPPDEBUG( Tools::format( "%s %p update last child", __FUNCTION__, name(this) ) );
      m_parent->m_last_child = this;
  }

  CPPDEBUG( Tools::format( "%p <- %p -> %p", name(m_prev), name(this), name(m_next) ) );

  m_parent->print();

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
/*
  if( m_parent->m_first_child == &other ) {
      CPPDEBUG( Tools::format( "%s %p update first child", __FUNCTION__, name(this) ) );
      m_parent->m_first_child = this;
  }
*/
  if( m_parent->m_last_child == &other ) {
      CPPDEBUG( Tools::format( "%s %p update last child", __FUNCTION__, name(this) ) );
      m_parent->m_last_child = this;
  }

  other.clear();

  return *this;
}

} // namespace Tools
