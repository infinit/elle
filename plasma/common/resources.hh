#ifndef RESOURCES_HPP
# define RESOURCES_HPP

# include "elle/system/Platform.hh"

# define _INFINIT_HOST_PROTOCOL "http"
# define _INFINIT_HOST "download.infinit.im"
# define _INFINIT_DOWNLOAD_DIR ""

# ifdef WIN32
#  define _INFINIT_FILENAME_SUFFIX ".exe"
# elif defined(INFINIT_MACOSX)
#  define _INFINIT_FILENAME_SUFFIX ".dmg"
# else
#  define _INFINIT_FILENAME_SUFFIX ""
# endif

# ifdef __x86_64__
#  define _INFINIT_ARCH "64"
# else // yes, nodoby has a 36 bit computer :p
#  define _INFINIT_ARCH "32"
# endif

# ifdef INFINIT_WINDOWS
#  define _INFINIT_PLATFORM "win" _INFINIT_ARCH
# elif defined(INFINIT_LINUX)
#  define _INFINIT_PLATFORM "linux" _INFINIT_ARCH
# elif defined(INFINIT_MACOSX)
#  define _INFINIT_PLATFORM "macosx" _INFINIT_ARCH
# else
#  error "unsupported platform"
# endif

# define INFINIT_UPDATER_FILENAME "8updater" _INFINIT_FILENAME_SUFFIX

# define INFINIT_BASE_URL \
  _INFINIT_HOST_PROTOCOL ":/" "/" _INFINIT_HOST "/" _INFINIT_DOWNLOAD_DIR "/" _INFINIT_PLATFORM

# define INFINIT_UPDATER_URI \
  (INFINIT_BASE_URL "/bin/" INFINIT_UPDATER_FILENAME)

# define INFINIT_RELEASE_URI \
  (INFINIT_BASE_URL "/" "manifest.xml")

# define INFINIT_META_URL "http://infinit.im:12345"

# ifdef INFINIT_WINDOWS
#  error "TODO !"
#  define INFINIT_HOME_DIRECTORY "Infinit"
# else
#  define INFINIT_HOME_DIRECTORY ".config/infinit"
# endif


# define WATCHDOG_SERVER_NAME "INFINIT_WATCHDOG_LOCAL_SOCKET"

#endif // RESOURCES_HPP
