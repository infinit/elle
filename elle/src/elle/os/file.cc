#include "file.hh"

#include <elle/Exception.hh>
#include <elle/printf.hh>

#include <boost/filesystem.hpp>

namespace elle
{
  namespace os
  {
    namespace file
    {
      size_t
      size(std::string const& path)
      {
        if (!boost::filesystem::exists(path))
          throw elle::Exception{elle::sprintf("file %s doesn't exist", path)};

        boost::system::error_code err;
        if (!boost::filesystem::is_directory(path, err))
        {
          if (err)
            throw elle::Exception{elle::sprintf("file %s doesn't exist", path)};

          auto size = boost::filesystem::file_size(path, err);

          if (err)
            throw elle::Exception{
              elle::sprintf("unable to retrieve %s size", path)};

          return size;
        }

        size_t size_ = 0;
        boost::filesystem::directory_iterator end_itr;

        for (boost::filesystem::directory_iterator itr(path, err);
             itr != end_itr;
             ++itr)
        {
          if (err)
            throw elle::Exception{
              elle::sprintf("unable to iterate over %s", path)};

          bool is_dir = boost::filesystem::is_directory(itr->status());
          if (is_dir)
          {
            size_ += size(itr->path().string());
          }
          else
          {
            size_ += boost::filesystem::file_size(itr->path(), err);

            if (err)
              throw elle::Exception{
                elle::sprintf("unable to retrieve %s size", path)};
          }
        }
        return size_;
      }
    }
  }
}
