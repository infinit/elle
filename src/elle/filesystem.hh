#pragma once

#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>

#include <elle/compiler.hh>

namespace elle
{
  namespace bfs = boost::filesystem;
}

namespace std
{
  template<>
  struct ELLE_API hash<boost::filesystem::path>
  {
    using argument_type = boost::filesystem::path;
    using value_type = std::size_t;

    value_type
    operator()(argument_type const& p) const;
  };
}
