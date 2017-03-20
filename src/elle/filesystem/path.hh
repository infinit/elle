#pragma once

#include <boost/filesystem/path.hpp>

#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    template<>
    struct ELLE_API Serialize<boost::filesystem::path>
    {
      using Type = std::string;
      static
      Type
      convert(boost::filesystem::path& path);

      static
      boost::filesystem::path
      convert(Type const& repr);
    };
  }
}
