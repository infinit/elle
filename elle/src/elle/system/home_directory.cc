#include "home_directory.hh"

#include <elle/os/environ.hh>

#ifdef INFINIT_WINDOWS
# include <shlobj.h>
#else
# include <pwd.h>
#endif

namespace elle
{
  namespace system
  {

    boost::filesystem::path
    home_directory()
    {
#ifdef INFINIT_WINDOWS
      TCHAR path[MAX_PATH];

      if (SUCCEEDED(SHGetFolderPath(NULL,
                                    CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
                                    nullptr,
                                    0,
                                    path)))
        return {path};
      else
        return (
          boost::filesystem::path{os::getenv("SystemDrive", "C:")} /
          boost::filesystem::path{os::getenv("HOMEPATH", "")}
        );
#else
      struct passwd* pw = ::getpwuid(getuid());
      if (pw != nullptr && pw->pw_dir != nullptr)
        return {pw->pw_dir};
      else
        return {os::getenv("HOME", "/tmp")};
#endif
    }

  }
}
