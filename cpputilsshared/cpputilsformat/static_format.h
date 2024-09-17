/**
 * Classes for type safe versions of sprintf() that are returning a std::string
 * @author Copyright (c) 2001 - 2024 Martin Oberzalek
 *
 * C++-11 version with variadic templates
 *
 * Examples:
 *    std::cout << format( "Hello %s, I have $05d$ in my pocket", "world", 5 ) << std::endl;
 *    std::cout << format( "Do not try this with printf: %s", 10101 ) << std::endl;
 *
 */

#pragma once

#if __cplusplus - 0 >= 201103L

#include <string>
#include <iomanip>
#include <iostream>

#include <sstream>
#include <cctype>
#include <variant>
#include <static_vector.h>
#include <static_string.h>
#include <cformat.h>
#include <type_traits>
#include <charconv>
#include <span>

namespace Tools {

  namespace StaticFormat
  {
    class BaseException : public std::exception
    {
      const char *err;

    public:
      BaseException(const BaseException& ex): std::exception() , err ( ex.err ) {}
      BaseException( const char* e ) : std::exception(), err( e ) {}
      BaseException & operator=(const BaseException& ex) { err = ex.err; return *this; }

      virtual const char* what() const throw() { return err; }
    };

    template<std::size_t N_SIZE>
    class BaseArg
    {
      bool _is_int;
      bool _is_string;

    public:
      BaseArg( bool is_int_ = false, bool is_string_  = false )
    : _is_int( is_int_ ),
      _is_string( is_string_ )
    {}
      virtual ~BaseArg() {}

      bool isInt() { return _is_int; }
      bool isString() { return _is_string; }

      virtual Tools::static_string<N_SIZE> doFormat( const Format::CFormat & cf ) = 0;
      virtual int get_int() {
        if( !isInt() ) {
#if __cpp_exceptions > 0
            throw BaseException( "expecting int arg" );
#else
            std::abort();
#endif
        }
        return 0;
      }

      template <class N> bool is_int( const N &n ) const { return false; }
      bool is_int( const int n ) const { return true; }
      bool is_int( const unsigned int n ) const { return true; }
      bool is_int( const short n ) const { return true; }
      bool is_int( const unsigned short ) const { return true; }
      bool is_int( const unsigned long ) const { return true; }
      bool is_int( const unsigned long long ) const { return true; }
      bool is_int( const long long ) const { return true; }

      template <class S> bool is_string( const S &s_ ) const { return false; }
      bool is_string( std::string & s_ ) const { return true; }
      bool is_string( const std::string & s_ ) const { return true; }
      bool is_string( std::string_view & s_ ) const { return true; }
      bool is_string( const std::string_view & s_ ) const { return true; }
      bool is_string( char* ) const { return true; }
      bool is_string( const char* ) const { return true; }
    };

    template<typename Arg, std::size_t N_SIZE>
    class RealArg : public BaseArg<N_SIZE>
    {
      const Arg arg;
    public:

      RealArg() : arg(nullptr) {}
      RealArg( const Arg & arg_ )
		: BaseArg<N_SIZE>( BaseArg<N_SIZE>::is_int( arg_ ), BaseArg<N_SIZE>::is_string( arg_ ) ),
		  arg(arg_)
		{
		}

      Tools::static_string<N_SIZE> doFormat( const Format::CFormat & cf ) override
      {
        return x2s( arg, cf );;
      }

      int get_int() override {
        BaseArg<N_SIZE>::get_int();
        return get_int(arg);
      }


    private:
      template<class T> int get_int( const T &t ) { return 0; }
      int get_int( int n ) { return (int)n; }
      int get_int( unsigned int n ) { return (int)n; }
      int get_int( short n ) { return (int)n; }
      int get_int( unsigned short n ) { return (int)n; }
      int get_int( long long n ) { return (int)n; }
      int get_int( unsigned long long n  ) { return (int)n; }
      int get_int( long n ) { return (int)n; }
      int get_int( unsigned long n ) { return (int)n; }


      template <class S> static std::string x2s( S ss, const Format::CFormat &cf )
      {
        std::stringstream str;
        str << cf << ss;
        return str.str();
      }
    };
/*
    template<std::size_t N_SIZE>
    class RealArg<const char,N_SIZE> : public BaseArg<N_SIZE>
    {
      const char *arg;
    public:

      RealArg() : arg(nullptr) {}
      RealArg( const char *arg_ )
		: BaseArg<N_SIZE>( false, true ),
		  arg(arg_)
		{
		}

      Tools::static_string<N_SIZE> doFormat( const Format::CFormat & cf ) override
      {
        return x2s( arg, cf );;
      }

    private:
      template<class T> int get_int( const T &t ) { return 0; }

      template <class S> static std::string x2s( S ss, const Format::CFormat &cf )
      {
        std::stringstream str;
        str << cf << ss;
        return str.str();
      }
    };
*/
    template <std::size_t N_SIZE, class BaseArgType, class CastTo>
    class RealArgCastFromChar : public BaseArg<N_SIZE>
        {
          const BaseArgType arg;
        public:
          RealArgCastFromChar( const BaseArgType & arg_ )
        : BaseArg<N_SIZE>( false, false ),
          arg(arg_)
        {}

          Tools::static_string<N_SIZE> doFormat( const Format::CFormat & cf ) override
          {
            std::stringstream str;
            str << cf;

            if( cf.numerical_representation )
              str << static_cast<CastTo>(arg);
            else
              str << arg;

            return str.str();
          }
        };

    template <std::size_t N_SIZE>
    class RealArg<unsigned char,N_SIZE> : public RealArgCastFromChar<N_SIZE,unsigned char,int>
    {
    public:
      RealArg( const unsigned char & arg_ )
        : RealArgCastFromChar<N_SIZE,unsigned char,int>( arg_ )
       {}
    };

    template <std::size_t N_SIZE>
    class RealArg<char,N_SIZE> : public RealArgCastFromChar<N_SIZE,char,int>
    {
    public:
      RealArg( const char & arg_ )
        : RealArgCastFromChar<N_SIZE,char,int>( arg_ )
       {}
    };

    template <std::size_t N_SIZE, class BaseArgType, class CastTo>
    class RealArgCastFromInt : public BaseArg<N_SIZE>
        {
          const BaseArgType arg;
        public:
          RealArgCastFromInt( const BaseArgType & arg_ )
        : BaseArg<N_SIZE>( true, false ),
          arg(arg_)
        {}

          Tools::static_string<N_SIZE> doFormat( const Format::CFormat & cf ) override
          {
            std::stringstream str;
            str << cf;

            if( cf.character_representation )
              str << static_cast<CastTo>(arg);
            else
              str << arg;

            return str.str();
          }

          int get_int() override {
            BaseArg<N_SIZE>::get_int();
            return arg;
          }
        };

    template <std::size_t N_SIZE>
    class RealArg<int,N_SIZE> : public RealArgCastFromInt<N_SIZE,int,char>
    {
    public:
      RealArg( const int & arg_ )
        : RealArgCastFromInt<N_SIZE,int,char>( arg_ )
       {}
    };

#define INT_REAL_ARG_CAST( type )\
    template <std::size_t N_SIZE> \
    class RealArg<type,N_SIZE> : public RealArgCastFromInt<N_SIZE,type,char>\
    {\
    public:\
      RealArg( const type & arg_ )\
        : RealArgCastFromInt<N_SIZE,type,char>( arg_ )\
       {}\
    }

    INT_REAL_ARG_CAST( unsigned int );
    INT_REAL_ARG_CAST( short );
    INT_REAL_ARG_CAST( unsigned short );
    INT_REAL_ARG_CAST( long );
    INT_REAL_ARG_CAST( unsigned long );

#undef INT_REAL_ARG_CAST

    template<std::size_t N_SIZE,typename VECTOR_LIKE, typename Arg> void add_argsx(  VECTOR_LIKE & v_args, Arg & arg )
    {
      v_args.push_back( RealArg<Arg,N_SIZE>(arg) );
    }

    template<std::size_t N_SIZE, typename VECTOR_LIKE, typename A, typename... Arg> void add_args( VECTOR_LIKE & v_args, A & a, Arg&... arg )
    {
      add_argsx<N_SIZE>( v_args, a );
      add_args<N_SIZE>( v_args, arg... );
    }

    template<std::size_t N_SIZE,typename VECTOR_LIKE>
    inline void add_args( VECTOR_LIKE & v_args )
    {

    }

    class FormatBase
	{
    	const std::string_view & format;

	protected:
    	FormatBase(  const std::string_view &format_ )
    	: format( format_ )
    	{}
    	virtual ~FormatBase() {}

    	std::span<char> parse( std::span<char> & target_buffer,
    						   std::span<char> & formating_buffer,
							   std::string::size_type num_of_args );
        int skip_atoi( const std::string_view & s, std::string::size_type start, std::string::size_type & pos ) const;

        virtual int get_int_arg( int num ) = 0;

        virtual std::string_view use_arg( unsigned int i, const Tools::Format::CFormat &cf ) = 0;

        virtual bool is_string_arg( int num_arg ) const = 0;

        std::string_view substitude( const std::string_view & str_orig,
        							 std::span<char> & formating_buffer,
									 const std::string_view & what,
									 const std::string_view & with,
        							 std::string::size_type start = 0 ) const;
	};

    template<std::size_t N_ARGS,std::size_t N_SIZE,typename VECTOR_LIKE>
    class Format : public FormatBase
    {
    private:
      struct Arg
      {
        bool is_int;
        bool is_string;
      };

      VECTOR_LIKE & args;

      static constexpr unsigned int num_of_args = N_ARGS;

      Tools::static_string<N_SIZE> s;
      Tools::static_string<N_SIZE> use_arg_buffer;

    private:
      Format() = delete;
      Format(const Format & f) = delete;
      Format & operator=(const Format & f) = delete;

    public:
      Format( const std::string_view &format_, VECTOR_LIKE & args_ )
      : FormatBase( format_ ),
		args(args_),
        s()
      {
         parse();
      }

      const Tools::static_string<N_SIZE> & get_string() const { return s; }

    private:
      void parse() {
    	  s.resize(N_SIZE);
    	  // including the null byte space
    	  std::span<char> span_in( &s[0], s.capacity()+1 );

    	  std::array<char,N_SIZE+1> formating_buffer;
    	  std::span<char> span_buf( &formating_buffer[0], formating_buffer.size() );

    	  std::span<char> span_out = FormatBase::parse( span_in,
    			  	  	  	  	  	  	  	  	  	    span_buf,
														num_of_args );

    	  s.resize( span_out.size() );
      }

      int get_int_arg( int num ) override;

      std::string_view use_arg( unsigned int i, const Tools::Format::CFormat &cf ) override;

      template<class T> int get_int( const T &t ) { return 0; }
      int get_int( int n ) { return (int)n; }
      int get_int( unsigned int n ) { return (int)n; }
      int get_int( short n ) { return (int)n; }
      int get_int( unsigned short n ) { return (int)n; }
      int get_int( long long n ) { return (int)n; }
      int get_int( unsigned long long n  ) { return (int)n; }
      int get_int( long n ) { return (int)n; }
      int get_int( unsigned long n ) { return (int)n; }

      bool is_string_arg( int num_arg ) const override {
    	  bool is_str = false;
		   std::visit( [&is_str]( auto & real_arg ) {
			  if( real_arg.isString() ) {
				  is_str = true;
			  }
		   },  args[num_arg] );

    	  return is_str;
      }
    }; // class Format



    namespace pack_real_args_impl {
		// https://stackoverflow.com/a/62089731/20079418

		// end of recursive call: tuple is forwared using `type`
		template <std::size_t N_SIZE, typename T, typename... Ts>
		struct unique_impl {using type = std::remove_reference<T>::type;};

		// recursive call: 1. Consumes the first type of the variadic arguments,
		//                    if not repeated add it to the tuple.
		//                 2. Call this again with the rest of arguments
		template <std::size_t N_SIZE, template<class...> class Tuple, typename... Ts, typename U, typename... Us>
		struct unique_impl<N_SIZE, Tuple<Ts...>,  U, Us...>
			: std::conditional_t<(std::is_same_v<RealArg<U,N_SIZE>, Ts> || ...) // but pack the type in RealArg class
							   , unique_impl<N_SIZE,Tuple<Ts...>, Us...>
							   , unique_impl<N_SIZE,Tuple<Ts..., RealArg<U,N_SIZE>>, Us...>> {}; // but pack the type in RealArg class

		// forward definition
		template <std::size_t N_SIZE,class Tuple>
		struct unique_tuple;

		// class specialization so that tuple arguments can be extracted from type
		template <std::size_t N_SIZE, template<class...>class Tuple, typename... Ts>
		struct unique_tuple<N_SIZE,Tuple<Ts...>> : public unique_impl<N_SIZE,Tuple<>, Ts...> {};

    } // namespace pack_real_args_impl

  } // namespace StaticFormat
} // /namespace Tools

namespace Tools {
  template <std::size_t N_SIZE, typename... Args>
  Tools::static_string<N_SIZE> static_format( const std::string_view & format, Args... args )
  {
	using namespace StaticFormat;

	constexpr auto N_ARGS = sizeof...(Args);
	using variant = pack_real_args_impl::unique_tuple<N_SIZE,std::variant<Args...>>::type;
	using vector = Tools::static_vector<variant,N_ARGS>;

    vector v_args;

    // std::cout << " v_args member size: %d " << sizeof(variant) << std::endl;

    StaticFormat::add_args<N_SIZE>( v_args, args... );

    StaticFormat::Format<N_ARGS,N_SIZE,vector> f2( format, v_args );

    // std::cout << demangle( typeid(variant).name() ) << std::endl;

    return f2.get_string();
  }
} // /namespace Tools
#endif

namespace Tools::StaticFormat {

template<std::size_t N_ARGS,std::size_t N_SIZE,typename VECTOR_LIKE>
int Format<N_ARGS,N_SIZE,VECTOR_LIKE>::get_int_arg( int num )
{
    if( static_cast<unsigned int>(num) > num_of_args - 1 ) {
#if __cpp_exceptions > 0
      throw BaseException( "The arg you wan't to use is out of range" );
#else
      std::abort();
#endif
    }

    if( num < 0 ) {
#if __cpp_exceptions > 0
      throw BaseException( "negativ number for arg number not allowed" );
#else
      std::abort();
#endif
    }

    bool is_int = false;
    std::visit( [&is_int]( auto & real_arg ) {
  	  if( real_arg.isInt() ) {
  		  is_int = true;
  	  }
    }, args[num] );

    if( is_int )
      {
        int value = 0;
        std::visit( [&value]( auto & real_arg ) {
      	  value = real_arg.get_int();
        }, args[num] );

        return value;
      }
    else {
#if __cpp_exceptions > 0
      throw BaseException( "expecting int arg" );
#else
      std::abort();
#endif
    }

    return 0; // should never be reached
}

template<std::size_t N_ARGS,std::size_t N_SIZE,typename VECTOR_LIKE>
std::string_view Format<N_ARGS,N_SIZE,VECTOR_LIKE>::use_arg( unsigned int i, const Tools::Format::CFormat &cf )
{
  if( i > num_of_args ) {
#if __cpp_exceptions > 0
    throw BaseException( "out of arg range" );
#else
    std::abort();
#endif
  }

  std::visit( [this,&cf]( auto & real_arg ){
	  use_arg_buffer = real_arg.doFormat(cf);
  }, args[i] );

  return use_arg_buffer;
}


} // namespace Tools::StaticFormat
