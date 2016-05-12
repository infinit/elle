#include <elle/system/self-path.hh>

#include <elle/windows.h>

namespace elle
{
  namespace system
  {
    boost::filesystem::path
    self_path()
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
  }
}
