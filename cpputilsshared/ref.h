/*
+*  VERSID:    "%Z% @(#)$Header$";
+*  $Log$
+*  Revision 1.1  2008/02/25 10:41:38  wamas
+*  MOBERZA TS-115979 Men� Transporteinheiten Erstellen hinzugefuegt,
+*  sowie wood und andere C++ Dateien.
+*
+*  Revision 1.1  2007/02/19 07:14:04  wamas
+*  sis: Eingecheckt
+*
+*  Revision 1.1.2.1  2006/05/11 08:27:36  wamas
+*  Toolsbox Umstellung
+*
+*  Revision 1.1.2.2  2005/07/27 16:29:59  wamas
+*  MOBERZA add Versid and logging stuff
+*
+*
*/
#ifndef TOOLS_ref_h
#define TOOLS_ref_h

#include <exception>

namespace Tools {

class BaseException : std::exception
{
  const char *err;

 public:
  BaseException( const char* err ) : err( err ) {}

  virtual const char* what() const throw() { return err; }

};

/**
   \class Ref
   \brief simple reference counting smart pointer template
   
   the class does not allow the usage of object arrays
*/
template <class V> class Ref
{
 public:
  
  /// exception class
  class BadRef : public BaseException
    {
    public:
      BadRef() : BaseException( "bad reference" ) {}      
    };
  
  typedef V        value_type;
  typedef V*       pointer;
  typedef V&       reference;
  typedef const V& const_reference;
  typedef const V* const_pointer;
  
 private:
  
  struct Rep
  {
    int count;    ///< the refernece counter
    pointer obj;  ///< pointer to the object
    bool del;     ///< if this value is true, the object will be deleted, count <= 0
    
    Rep( int c, pointer o, bool del = true ) : count(c), obj( o ), del( del ) {}
    
    ~Rep() 
    { 
      if( del) 
	{
	  delete obj; 
	}
    }
  };
  
  Rep *rep;
  
 public:
  
  Ref() : rep(0) {}
  
  /**
     \param v    pointer to the object
     \param del  if the value is true (thats the default) the object will
     be deleted if the reference counter gets lower or equal zero
  */
  explicit Ref( pointer v, bool del ) : rep( new Rep(1, v, del) ) {}
  Ref( pointer v ) : rep( new Rep( 1, v, true ) ) {}
  
  explicit Ref( reference v, bool del ) : rep( new Rep( 1, &v, del ) ) {}
  explicit Ref( reference v ) : rep( new Rep( 1, &v, false ) ) {}
  
  explicit Ref( const_reference v, bool del ) : rep( new Rep( 1, &v, del ) ) {}
  explicit Ref( const_reference v ) : rep( new Rep( 1, &v, false ) ) {}
  
  Ref( const Ref &r ) : rep( r.rep )
    {
      if( rep )
	rep->count++;
    }    
  
  ~Ref() { unlink(); }
  
  bool operator!() const { return !rep; }
  
  /// returns true if the class points to an object
  bool valid() const { return rep; }
    
  /// returns the address of the holded object. 
  /** returns 0 if the there is no object */
  pointer address()
    {
      if( rep )
	return rep->obj;
      
      return 0;
    }
  
  const_pointer address() const 
    {
      if( rep )
	return rep->obj;

      return 0;
    }

  /// returns the number of references
  /** if there the class does not points to an object it will return 0 */
  int count() const 
    { 
      if( rep ) 
	return rep->count; 
      else 
	return 0; 
    }

  /** 
      decreases the reference counter of the old object
      and, if required, the old object will be destroyed;
      gets the object of the other Ref class and increases
      it's reference counter.
  */
  Ref<V>& operator=( const Ref& r )
    {
      unlink();
      rep = r.rep;
      
      if( rep )
	rep->count++;

      return *this;
    }


  /// returns the object
  /** throws BadRef if there is no object to point to */
  pointer operator->() 
    {
      if( !rep )
	throw BadRef();
      
      return (rep->obj);
    }

  /// returns the object
  /** throws BadRef if there is no object to point to */
  const_pointer operator->() const 
    {
      if( !rep )
	throw BadRef();
      
      return (rep->obj);
    }

  /// returns a reference to the object
  /** throws BadRef if there is no object to point to */
  reference operator*() { return *operator->(); }

  /// returns the object
  /** throws BadRef if there is no object to point to */
  const_reference operator*() const { return *operator->(); }

  /// compairs the object with a given object of the same type
  bool operator==( const_reference v ) const
    {
      if( rep )
	return *(rep->obj) == v;
      
      return false;
    }

  /// compairs the objects
  bool operator==( const Ref<V> &r ) const 
    {
      if( rep )
	if( r.rep )
	  return *(rep->obj) == *(r.rep->obj);

      return false;
    }

  bool operator!=( const_reference v ) const
    { 
      if( rep )
	return *(rep->obj) != v;
      
      return true;
    }

  bool operator!=( const Ref<V> &r ) const
    { 
      if( rep )
	if( r.rep )
	  return *(rep->obj) != *(r.rep->obj);

      return true;
    }

  operator V() const { return *(address()); }
  operator V*() const { return address(); }

  operator V() { return *(address()); }
  operator V*() { return address(); }

 private:

  operator void*() { return address(); }
  operator void*() const { return address(); }

  /// decreases the reference counter and if required deletes the object
  void unlink()
    {
      if( rep )
	{
	  if( --rep->count <= 0 )
	    {
	      delete rep;
	      rep = 0;
	    }
	}
    }
};

} // namespace Tools

#endif
