#include "extract.hh"


#include <fstream>
#include <stdexcept>
#include <sstream>

#include <boost/filesystem/operations.hpp>

namespace elle
{
  namespace serialize
  {

    namespace detail
    {
      std::unique_ptr<std::istream>
      new_ifstream(std::string const& path)
      {
        std::unique_ptr<std::istream> res(new std::ifstream(path));
        if (!res->good())
          throw boost::filesystem::filesystem_error(
            "Cannot open file '" + path + "'",
            boost::system::errc::make_error_code(
              boost::system::errc::no_such_file_or_directory));
        return res;
      }

      std::unique_ptr<std::istream>
      new_istringstream(std::string const& str)
      {
        return std::unique_ptr<std::istream>(
            new std::istringstream(str)
        );
      }
    }

  }
}
