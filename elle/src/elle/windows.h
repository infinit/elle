#ifndef WINDOWS_H
# define WINDOWS_H

# ifdef INFINIT_WINDOWS
#  include <winsock2.h>
#  include <windows.h>
#  ifdef interface
#   undef interface
#  endif
# endif

#endif
