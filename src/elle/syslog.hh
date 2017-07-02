#pragma once

#if defined INFINIT_WINDOWS

// TODO http://msdn.microsoft.com/en-us/library/windows/desktop/aa363680.aspx
enum
{
  LOG_DEBUG = 0,
  LOG_INFO = 1,
  LOG_WARNING = 2,
  LOG_ERR = 3,
};

enum
{
  LOG_DAEMON = 3,
};

void
syslog(int lvl, char const*, char const* str);

void
openlog(char const* ident, int logopt, int facility);

#else
# include <syslog.h>
#endif
