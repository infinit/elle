
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
        return fs::exists(path);
      }

      std::string absolute(std::string const& path, bool canonical)
      {
        if (canonical)
          return fs::canonical(path).string();
        else
          return fs::absolute(path).string();
      }

      bool is_directory(std::string const& path)
      {
        return fs::is_directory(path);
      }


      void make_directory(std::string const& path)
      {
        fs::create_directory(path);
      }

      void make_path(std::string const& path)
      {
        fs::create_directories(path);
      }

      void
      make_symlink(std::string const& oldname,
                   std::string const& newname)
      {
        fs::create_symlink(oldname, newname);
      }

    }
  }
}

