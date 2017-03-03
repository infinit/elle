#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <typeinfo>

// XXX demangle + types check for the following macros

/// Returns a nice string for the given type
#define ELLE_PRETTY_TYPE(T)                                             \
  ::elle::demangle(typeid(T).name())                                    \

/// Returns a nice string of the type of the given object
#define ELLE_PRETTY_OBJECT_TYPE(object)                                 \
  ELLE_PRETTY_TYPE(object)                                              \

namespace elle
{
  using Void = void;
  using Char = char;
  using Character = char;
  using Byte = uint8_t;
  using Real = double;

  using String = std::basic_string<Character>;

  using Size = uint64_t;
  using Index = uint64_t;
  using Offset = uint64_t;

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
