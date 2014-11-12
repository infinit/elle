#include "file.hh"

#include <elle/Exception.hh>
#include <elle/printf.hh>

# include <boost/filesystem.hpp>

namespace elle
{
  namespace os
  {
    namespace file
    {
      bool
      exists(std::string const& path)
      {
        return elle::os::file::exists(boost::filesystem::path{path});
      }

      bool
      exists(boost::filesystem::path const& path)
      {
        return boost::filesystem::exists(path);
      }

      uint64_t
      size(std::string const& path)
      {
        return elle::os::file::size(boost::filesystem::path{path});
      }

      uint64_t
      size(boost::filesystem::path const& path)
      {
        boost::system::error_code err;

        if (!elle::os::file::exists(path))
          throw boost::filesystem::filesystem_error(
            "File does not exist",
            path,
            err);

        if (!boost::filesystem::is_directory(path, err))
        {
          if (err)
            throw boost::filesystem::filesystem_error(
              "File is not accessible",
              path,
              err);

          auto size = boost::filesystem::file_size(path, err);
          if (err)
            throw boost::filesystem::filesystem_error(
              "Unable to retrieve file size",
              path,
              err);

          return size;
        }

        size_t size_ = 0;
        boost::filesystem::directory_iterator end_itr;

        for (boost::filesystem::directory_iterator itr(path, err);
             itr != end_itr;
             ++itr)
        {
          if (err)
            throw boost::filesystem::filesystem_error(
                "Unable to iterate over path",
                path,
                err);

          bool is_dir = boost::filesystem::is_directory(itr->status());
          if (is_dir)
          {
            size_ += size(itr->path().string());
          }
          else
          {
            size_ += boost::filesystem::file_size(itr->path(), err);

            if (err)
              throw boost::filesystem::filesystem_error(
                "Unable to retrieve file size",
                itr->path(),
                err);
          }
        }
        return size_;
      }
    }
  }
}
