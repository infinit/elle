#include <elle/assert.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

namespace elle
{
  namespace filesystem
  {
    TemporaryDirectory::TemporaryDirectory()
      : _path()
    {
      auto output_pattern =
        boost::filesystem::temp_directory_path() / "%%%%-%%%%-%%%%-%%%%";
      do
      {
        this->_path = boost::filesystem::unique_path(output_pattern);
      }
      while (!boost::filesystem::create_directories(this->_path));
    }

    TemporaryDirectory::TemporaryDirectory(std::string const& name)
      : TemporaryDirectory()
    {
      this->_path = this->_path / name;
      ELLE_ASSERT(boost::filesystem::create_directories(this->_path));
    }

    TemporaryDirectory::~TemporaryDirectory()
    {
      boost::filesystem::remove_all(this->_path.parent_path());
    }
  }
}
