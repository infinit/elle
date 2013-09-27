#include "path.hh"

#include <elle/Exception.hh>
#include <elle/log.hh>
#include <elle/assert.hh>
#include <boost/filesystem.hpp>

#include <cassert>
#include <cstdint>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

ELLE_LOG_COMPONENT("elle.os.path");

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
          ELLE_ASSERT_GT(strings.size(), 0u);
          auto it = strings.begin(), end = strings.end();
          fs::path path{*it};
          for (; ++it != end;)
            {
              path /= *it;
            }
          return path.string();
        }

      } //!detail

      std::string
      current()
      {
        ELLE_TRACE("current directory: %s", fs::current_path().string());
        return fs::current_path().string();
      }

      bool exists(std::string const& path)
      {
        ELLE_TRACE("'%s' %s",
                   path,
                   fs::exists(path) ? "exists" : "doesn't exist");
        return fs::exists(path);
      }

      std::string absolute(std::string const& path, bool canonical)
      {
        if (canonical)
          return fs::canonical(path).string();
        else
          return fs::absolute(path).string();
      }

      bool
      is_absolute(std::string const& path)
      {
        ELLE_TRACE("'%s' is%s absolute",
                   path,
                   fs::path(path).is_absolute() ? "" : "n't");
        return fs::path(path).is_absolute();
      }

      bool is_directory(std::string const& path)
      {
        ELLE_TRACE("%s is%s directory",
                   path,
                   fs::is_directory(path) ? "" : "n't");
        return fs::is_directory(path);
      }

      void make_directory(std::string const& path)
      {
        ELLE_TRACE("create directory at '%s'", path);
        fs::create_directory(path);
      }

      void remove_directory(std::string const& path)
      {
        ELLE_TRACE("try removing '%s' directory", path);
        if (!is_directory(path))
          throw elle::Exception(path + " is not a directory");

        fs::remove_all(path);
      }

      void make_path(std::string const& path)
      {
        ELLE_TRACE("create directories at '%s'", path);
        fs::create_directories(path);
      }

      void
      make_symlink(std::string const& oldname,
                   std::string const& newname)
      {
        ELLE_TRACE("make symlink from '%s' to '%s'", oldname, newname);
        fs::create_symlink(oldname, newname);
      }

      bool
      check_symlink(std::string const& path)
      {
        ELLE_TRACE("%s is%s a symlink",
                   path,
                   fs::symlink_status(path).type() != fs::status_error ? ""
                                                                       : "n't");
        return (fs::symlink_status(path).type() != fs::status_error);
      }
    }
  }
}
