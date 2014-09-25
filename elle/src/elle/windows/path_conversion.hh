#ifndef ELLE_WINDOWS_PATH_CONVERSION_HH
# define ELLE_WINDOWS_PATH_CONVERSION_HH

# include <string>

# include <boost/filesystem/path.hpp>

# include <elle/windows/string_conversion.hh>

namespace elle
{
  namespace path
  {
    std::string
    to_string(boost::filesystem::path const& path);
  }
}


#endif
