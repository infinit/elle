#pragma once

#ifdef ELLE_WINDOWS
# include <winsock2.h>
# include <windows.h>
# ifdef interface
#  undef interface
# endif
#endif
