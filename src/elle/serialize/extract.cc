#include "extract.hh"

#include <fstream>
#include <stdexcept>
#include <sstream>

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
          throw std::runtime_error("Cannot open file '" + path + "'");
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
