#ifndef ELLE_TYPES_HH
# define ELLE_TYPES_HH

# include <cstdint>
# include <iosfwd>
# include <string>
# include <typeinfo>

// XXX demangle + types check for the following macros

/// Returns a nice string for the given type
# define ELLE_PRETTY_TYPE(T)                                                  \
  ::elle::demangle(typeid(T).name())                                          \

/// Returns a nice string of the type of the given object
# define ELLE_PRETTY_OBJECT_TYPE(object)                                      \
  ELLE_PRETTY_TYPE(object)                                                    \

namespace elle
{
    typedef int8_t                        Integer8;
    typedef int16_t                       Integer16;
    typedef int32_t                       Integer32;
    typedef int64_t                       Integer64;

    typedef uint8_t                       Natural8;
    typedef uint16_t                      Natural16;
    typedef uint32_t                      Natural32;
    typedef uint64_t                      Natural64;

    typedef void                          Void;
    typedef bool                          Bool;
    typedef bool                          Boolean;
    typedef char                          Char;
    typedef char                          Character;
    typedef Natural8                      Byte;
    typedef double                        Real;

    typedef std::basic_string<Character>  String;

    typedef Natural64 Size;
    typedef Natural64 Index;
    typedef Natural64 Offset;

    ///
    /// this enumeration represents a return status.
    ///
    /// note that almost all the functions and methods of Elle return
    /// this type.
    ///
    enum class Status
      {
        Ok = 42,
        Error = 666
      };
}

#endif
