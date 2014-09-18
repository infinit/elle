#include <iostream>

#include <elle/system/home_directory.hh>

#include <elle/os/environ.hh>
#ifdef INFINIT_WINDOWS
# include <elle/windows/string_conversion.hh>
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
      wchar_t path[MAX_PATH];
      if (SUCCEEDED(SHGetSpecialFolderPathW(NULL, path, CSIDL_PROFILE, 0)))
      {
        wchar_t path2[MAX_PATH];
        auto len = GetShortPathNameW(path, NULL, 0);
        if (GetShortPathNameW (path, path2, len) != 0)
        {
          return {elle::string::wstring_to_string(path2)};
        }
      }

      // Backup solution.
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
