#include <elle/filesystem/TemporaryDirectory.hh>

#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/system/self-path.hh>

using namespace std::literals;

ELLE_LOG_COMPONENT("elle.TemporaryDirectory");

namespace elle
{
  namespace filesystem
  {
    TemporaryDirectory::TemporaryDirectory()
    {
      auto fpattern = "%%%%-%%%%-%%%%-%%%%"s;
      try
      {
        fpattern = print("{}-{}",
                         system::self_path().filename().string(), fpattern);
      }
      catch (...)
      {
        // No big deal.
      }
      auto pattern = fs::temp_directory_path() / fpattern;
      do
      {
        this->_root = fs::unique_path(pattern);
      }
      while (!fs::create_directories(this->_root));
      this->_path = this->_root;
    }

    TemporaryDirectory::TemporaryDirectory(std::string const& name)
      : TemporaryDirectory()
    {
      this->_path = this->_path / name;
      ELLE_ASSERT(fs::create_directories(this->_path));
    }

    TemporaryDirectory::~TemporaryDirectory()
    {
      ELLE_DUMP("{}: cleaning", this->_root);
      fs::remove_all(this->_root);
    }
  }
}
