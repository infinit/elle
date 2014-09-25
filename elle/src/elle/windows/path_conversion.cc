#include <elle/windows/path_conversion.hh>

namespace elle
{
  namespace path
  {
    std::string
    to_string(boost::filesystem::path const& path)
    {
      return elle::string::wstring_to_string(path.native());
    }
  }
}
