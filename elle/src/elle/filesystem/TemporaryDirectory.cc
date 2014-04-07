#if defined(INFINIT_MACOSX)
# include <mach-o/dyld.h>
# include <sys/param.h>
#endif

#include <elle/assert.hh>
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/windows.h>

static boost::filesystem::path executable_path()
{
#if defined(INFINIT_LINUX)
  return boost::filesystem::read_symlink("/proc/self/exe");
#elif defined(INFINIT_MACOSX)
  uint32_t size = MAXPATHLEN;
  char result[size];
  if (_NSGetExecutablePath(result, &size) == 0)
    return result;
  else
    throw elle::Exception("unable to get executable path");
#elif defined(INFINIT_WINDOWS)
  char result[1024];
  auto size = GetModuleFileName(0, result, sizeof(result));
  return std::string(result, size);
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
