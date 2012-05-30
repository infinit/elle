#ifndef  ELLE_TYPES_HH
# define ELLE_TYPES_HH

# include <cstdint>
# include <iosfwd>
# include <string>

# include <openssl/bn.h>

namespace elle {

    typedef int8_t                        Integer8;
    typedef int16_t                       Integer16;
    typedef int32_t                       Integer32;
    typedef int64_t                       Integer64;

    typedef uint8_t                       Natural8;
    typedef uint16_t                      Natural16;
    typedef uint32_t                      Natural32;
    typedef uint64_t                      Natural64;

    typedef ::BIGNUM                      Large;

    typedef void                          Void;
    typedef bool                          Bool;
    typedef bool                          Boolean;
    typedef char                          Char;
    typedef char                          Character;
    typedef Natural8                      Byte;
    typedef double                        Real;

    typedef std::basic_string<Character>  String;

    enum class Null { Nil };
    extern Null const none;


    ///
    /// this enumeration represents a return status.
    ///
    /// note that almost all the functions and methods of Elle return
    /// this type.
    ///
    enum class Status
      {
        Unknown,

        False = false,
        True = true,

        Ok = 42,
        Error = 666
      };

    inline bool operator ==(Status s, Boolean b)
      {
        if (b == true)
          return s == Status::True;
        else
          return s == Status::False;
      }
    inline bool operator !=(Status s, Boolean b)
      {
        if (b == true)
          return s == Status::False;
        else
          return s == Status::True;
      }
    inline bool operator ==(Boolean b, Status s)
      {
        if (b == true)
          return s == Status::True;
        else
          return s == Status::False;
      }
    inline bool operator !=(Boolean b, Status s)
      {
        if (b == true)
          return s == Status::False;
        else
          return s == Status::True;
      }
}

std::ostream& operator <<(std::ostream& out, elle::Large const& value);
std::ostream& operator <<(std::ostream& out, elle::Null);

#endif /* ! TYPES_HH */



