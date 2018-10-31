#pragma once

#include <regex>

#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<std::regex>
    {
      using Type = std::string;

      static std::regex
      convert(Type const& repr)
      {
        return std::regex(repr);
      }
    };
  }
}
