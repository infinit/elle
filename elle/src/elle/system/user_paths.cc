#include <iostream>

#include <boost/filesystem/operations.hpp>

#include <elle/os/environ.hh>
#include <elle/system/user_paths.hh>
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
#if defined(INFINIT_WINDOWS)
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
      struct passwd* pw = ::getpwuid(geteuid());
      if (pw != nullptr && pw->pw_dir != nullptr)
        return {pw->pw_dir};
      else
        return {os::getenv("HOME", "/tmp")};
#endif
    }

    boost::filesystem::path
    download_directory()
    {
      auto candidate_dir = home_directory() / "Downloads";
      if (exists(candidate_dir) && is_directory(candidate_dir))
        return candidate_dir;
      return home_directory();
    }
  }
}
