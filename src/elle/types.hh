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
  using Integer8 = int8_t;
  using Integer16 = int16_t;
  using Integer32 = int32_t;
  using Integer64 = int64_t;

  using Natural8 = uint8_t;
  using Natural16 = uint16_t;
  using Natural32 = uint32_t;
  using Natural64 = uint64_t;

  using Void = void;
  using Char = char;
  using Character = char;
  using Byte = Natural8;
  using Real = double;

  using String = std::basic_string<Character>;

  using Size = Natural64;
  using Index = Natural64;
  using Offset = Natural64;

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
