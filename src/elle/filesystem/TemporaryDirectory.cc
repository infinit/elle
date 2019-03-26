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
      auto fpattern =
        []
        {
          try
          {
            return print("{}-XXXXXX", system::self_path().filename());
          }
          catch (...)
          {
            return "elle-XXXXXX"s;
          }
        }();
      auto pattern = (fs::temp_directory_path() / fpattern).string();
      char buf[pattern.size() + 1];
      memcpy(buf, pattern.c_str(), pattern.size());
      buf[pattern.size()] = 0;
      if (!mkdtemp(buf))
        elle::err("unable to create temporary directory: {}", strerror(errno));
      this->_path = buf;
    }

    TemporaryDirectory::TemporaryDirectory(std::string const& name)
      : TemporaryDirectory()
    {
      this->_path = this->_path / name;
      ELLE_ASSERT(fs::create_directories(this->_path));
    }

    TemporaryDirectory::~TemporaryDirectory()
    {
      ELLE_DUMP("{}: cleaning", this->_path);
      fs::remove_all(this->_path);
    }
  }
}
