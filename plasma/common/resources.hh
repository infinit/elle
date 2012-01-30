#ifndef RESOURCES_HPP
# define RESOURCES_HPP

# include "elle/system/Platform.hh"

# define _INFINIT_HOST_PROTOCOL "http"
# define _INFINIT_HOST "hotgloupi.fr"
# define _INFINIT_DOWNLOAD_DIR "downloads"

# ifdef WIN32
#  define _INFINIT_FILENAME_SUFFIX ".exe"
# else
#  define _INFINIT_FILENAME_SUFFIX ""
# endif

# ifdef __x86_64__
#  define _INFINIT_ARCH "64"
# else // yes, nodoby has a 36 bit computer :p
#  define _INFINIT_ARCH "32"
# endif

# ifdef INFINIT_WINDOWS
#  define _INFINIT_PLATFORM "windows" _INFINIT_ARCH
# elif defined(INFINIT_UNIX)
#  define _INFINIT_PLATFORM "linux" _INFINIT_ARCH
# elif defined(INFINIT_MACOSX)
#  define _INFINIT_PLATFORM "macosx" _INFINIT_ARCH
# else
#  error "Unkwown platform !"
# endif

# define INFINIT_UPDATER_FILENAME "updater" _INFINIT_FILENAME_SUFFIX

# define INFINIT_BASE_URL \
  _INFINIT_HOST_PROTOCOL ":/" "/" _INFINIT_HOST "/" _INFINIT_DOWNLOAD_DIR "/" _INFINIT_PLATFORM

# define INFINIT_UPDATER_URI \
  (INFINIT_BASE_URL "/" INFINIT_UPDATER_FILENAME)

# define INFINIT_RELEASE_URI \
  (INFINIT_BASE_URL "/" "last_release.xml")

# ifdef INFINIT_WINDOWS
#  define INFINIT_HOME_DIRECTORY "Infinit"
# else
#  define INFINIT_HOME_DIRECTORY ".infinit"
# endif

#endif // RESOURCES_HPP
