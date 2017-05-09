#pragma once

#if defined _MSC_VER
# include <SDKDDKVer.h>
#endif

#if defined INFINIT_WINDOWS
# include <winsock2.h>
#endif

#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic push

// /usr/include/sys/poll.h:1:2: warning: #warning redirecting incorrect #include <sys/poll.h> to <poll.h> [-Wcpp]
//  #warning redirecting incorrect #include <sys/poll.h> to <poll.h>
//   ^~~~~~~

# pragma GCC diagnostic ignored "-Wcpp"
#endif

#include <boost/asio.hpp>

#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
