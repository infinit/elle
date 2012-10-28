#ifndef CONFIG_HH
#define CONFIG_HH

#cmakedefine INFINIT_LINUX
#cmakedefine INFINIT_MACOSX
#cmakedefine INFINIT_WINDOWS

#cmakedefine HAVE_SETXATTR

# if defined(INFINIT_MACOSX)
#  define HAVE_STATFS
# endif

#cmakedefine INFINIT_DEBUG

#endif
