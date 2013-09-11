#include "file.hh"

#include <elle/Exception.hh>

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
          throw elle::Exception{"file " + path + " doesn't exist"};

        if (!boost::filesystem::is_directory(path))
          return boost::filesystem::file_size(path);

        size_t size_ = 0;
        boost::filesystem::directory_iterator end_itr;

        for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr)
        {
          if (boost::filesystem::is_directory(itr->status()))
          {
            size_ += size(itr->path().string());
          }
          else
          {
            size_ += boost::filesystem::file_size(itr->path());
          }
        }
        return size_;
      }
    }
  }
}
