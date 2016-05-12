#include <elle/assert.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/system/self-path.hh>

namespace elle
{
  namespace filesystem
  {
    TemporaryDirectory::TemporaryDirectory()
      : _path()
      , _root()
    {
      std::string file_pattern = "%%%%-%%%%-%%%%-%%%%";
      try
      {
        auto self = system::self_path();
        file_pattern = self.filename().string() + "-" + file_pattern;
      }
      catch (...)
      {
        // No big deal.
      }
      auto pattern = boost::filesystem::temp_directory_path() / file_pattern;
      do
      {
        this->_root = boost::filesystem::unique_path(pattern);
      }
      while (!boost::filesystem::create_directories(this->_root));
      this->_path = this->_root;
    }

    TemporaryDirectory::TemporaryDirectory(std::string const& name)
      : TemporaryDirectory()
    {
      this->_path = this->_path / name;
      ELLE_ASSERT(boost::filesystem::create_directories(this->_path));
    }

    TemporaryDirectory::~TemporaryDirectory()
    {
      boost::filesystem::remove_all(this->_root);
    }
  }
}
