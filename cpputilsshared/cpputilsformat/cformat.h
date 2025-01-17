/**
 * Helper class for formating iostream with printf style format strings.
 * std::string version
 * @author Copyright (c) 2001 - 2022 Martin Oberzalek
 */

#ifndef _TOOLS_CFORMAT_H
#define _TOOLS_CFORMAT_H

#include <optional>
#include <string>
#include <ostream>

namespace Tools {
  namespace Format {

    class CFormat
    {
    public:

      enum Adjust
      {
        LEFT,
        RIGHT
      };

      enum Base
      {
        OCT = 8,
        DEC = 10,
        HEX = 16
      };

      enum Floating
      {
        FIXED,
        SCIENTIFIC
      };

      bool valid;
      Adjust adjust;
      bool special;
      bool sign;
      bool grouping; // SUSv2 extension
      bool conversion; // glibc 2.2 extension
      bool zero;
      bool precision_explicit;
      bool internal;
      Base base;
      bool setupper;
      Floating floating;
      bool showbase;

      int width;
      int precision; // -1 when not set
      static const int default_precision; // 6, use it if precision is -1
      int strlength;

#if __cplusplus >= 201703L
      std::string_view format;
#endif

      bool numerical_representation; // cast a character to int
      bool character_representation; // cast a int to char

    public:
      CFormat() :
        valid(false),
        adjust(RIGHT),
        special(false),
        sign(false),
        grouping(false),
        conversion(false),
        zero(false),
        precision_explicit(false),
        internal(false),
        base(DEC),
        setupper(false),
        floating(FIXED),
        showbase(false),
        width(0),
        precision(-1),
        strlength(0),
#if __cplusplus >= 201703L
        format(),
#endif
        numerical_representation(false),
        character_representation(false)
    { }

      void set( std::ostream& out );
    };

  } // /namespace Format

  inline std::ostream& operator<<( std::ostream& out, Format::CFormat cf )
  {
    cf.set( out );
    return out;
  }

} // /namespace Tools

#endif  /* _TOOLS_CFORMAT_H */
