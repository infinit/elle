#pragma once

#include <filesystem>
#include <vector>

#include <elle/filesystem.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{
  /// The base of the version family to which this file belongs.
  /// The base includes the (possible) directory.
  ///
  /// \pre the filename is `<BASE>.<NUM>`, where `<NUM>` is a
  /// decimal number.
  fs::path base(fs::path const& p);

  /// All the paths inside directory @a d (not recursive).
  std::vector<fs::path>
  entries(fs::path const& d);

  /// The sorted list of existing versions of a file family.
  ///
  /// For instance /tmp/foo.1, /tmp/foo.42, /tmp/foo.5 -> [1, 5, 42].
  std::vector<int>
  rotate_versions(fs::path const& base);

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

namespace std
{
  template<>
  struct ELLE_API hash<std::filesystem::path>
  {
    std::size_t
    operator()(std::filesystem::path const& p) const;
  };
}
