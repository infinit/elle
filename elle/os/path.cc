
#include <cassert>

#include <boost/filesystem.hpp>

#include "path.hh"

namespace elle
{
  namespace os
  {
    namespace path
    {

      namespace fs = boost::filesystem;

      namespace detail
      {

        std::string join(std::vector<std::string> const& strings)
        {
          assert(strings.size() > 0);
          auto it = strings.begin(), end = strings.end();
          fs::path path{*it};
          for (; ++it != end;)
            {
              path /= *it;
            }
          return path.string();
        }

      } //!detail

      bool exists(std::string const& path)
      {
        return fs::exists(fs::path{path});
      }

    }
  }
}

