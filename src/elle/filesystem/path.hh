#pragma once

#include <boost/filesystem/path.hpp>

#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace fs = boost::filesystem;

  /// The base of the version family to which this file belongs.
  /// The base includes the (possible) directory.
  ///
  /// \pre the filename is `<BASE>.<NUM>`, where `<NUM>` is a
  /// decimal number.
  fs::path base(fs::path const& p);

  namespace serialization
  {
    template<>
    struct ELLE_API Serialize<fs::path>
    {
      using Type = std::string;
      static
      Type
      convert(fs::path& path);

      static
      fs::path
      convert(Type const& repr);
    };
  }
}
