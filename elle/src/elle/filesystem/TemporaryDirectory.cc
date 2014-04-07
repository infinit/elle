#if defined(INFINIT_MACOSX)
# include <mach-o/dyld.h>
#elif defined(INFINIT_WINDOWS)
# include <windows.h>
#endif

#include <elle/assert.hh>
#include <elle/filesystem/TemporaryDirectory.hh>

static boost::filesystem::path executable_path()
{
#if defined(INFINIT_LINUX)
  return boost::filesystem::read_symlink("/proc/self/exe");
#elif defined(INFINIT_MACOSX)
  return _NSGetExecutablePath();
#elif defined(INFINIT_WINDOWS)
  return GetModuleFileName(0);
#else
  throw elle::Exception("unable to get executable path");
#endif
}

namespace elle
{
  namespace filesystem
  {
    TemporaryDirectory::TemporaryDirectory()
      : _path()
    {
      std::string file_pattern = "%%%%-%%%%-%%%%-%%%%";
      try
      {
        auto self = executable_path();
        file_pattern = self.filename().string() + "-" + file_pattern;
      }
      catch (...)
      {
        // No big deal.
      }
      auto pattern = boost::filesystem::temp_directory_path() / file_pattern;
      do
      {
        this->_path = boost::filesystem::unique_path(pattern);
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
