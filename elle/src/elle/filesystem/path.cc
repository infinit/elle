#include <elle/filesystem/path.hh>

namespace elle
{
  namespace serialization
  {
    Serialize<boost::filesystem::path>::Type
    Serialize<boost::filesystem::path>::convert(boost::filesystem::path& path)
    {
      return path.generic_string();
    }

    boost::filesystem::path
    Serialize<boost::filesystem::path>::convert(
      Serialize<boost::filesystem::path>::Type const& repr)
    {
      return boost::filesystem::path(repr);
    }
  }
}
