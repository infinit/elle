#include "insert.hh"

#include <fstream>
#include <stdexcept>
#include <sstream>

namespace elle
{
  namespace serialize
  {
    namespace detail
    {

      std::unique_ptr<std::ostream>
      new_ofstream(std::string const& path)
      {
        std::unique_ptr<std::ostream> res(new std::ofstream(path));
        if (!res->good())
          throw std::runtime_error("Cannot open file '" + path + "'");
        return res;
      }

      std::unique_ptr<std::ostream>
      new_ostringstream()
      {
        return std::unique_ptr<std::ostream>{new std::ostringstream{}};
      }

      void
      _flush_ostringstream(std::ostream& out,
                           std::string& str)
      {
        str = static_cast<std::ostringstream&>(out).str();
      }

    }
  }
}
