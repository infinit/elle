//
// Made by flementec@rd.skyrecon.com
// 
// Started on  Thu Jan 22 08:43:13 2009 flementec
// Last update Thu Jan 22 16:02:23 2009 texane
//



#ifndef SYSTEM_API_HH_INCLUDED
# define SYSTEM_API_HH_INCLUDED



#ifndef _POSIX_SYSTEM_API

#include "Win32.hh"

namespace SystemApi = Win32;

#else

#include "Posix.hh"

namespace SystemApi = Posix;

#endif



#endif // ! SYSTEM_API_HH_INCLUDED
