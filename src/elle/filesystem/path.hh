#pragma once

#include <boost/filesystem/path.hpp>

#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace bfs = boost::filesystem;

  namespace serialization
  {
    template<>
    struct ELLE_API Serialize<bfs::path>
    {
      using Type = std::string;
      static
      Type
      convert(bfs::path& path);

      static
      bfs::path
      convert(Type const& repr);
    };
  }
}
