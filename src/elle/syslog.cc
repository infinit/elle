#include <elle/syslog.hh>

#if defined INFINIT_WINDOWS

# include <iostream>

void
syslog(int lvl, char const*, char const* str)
{
  auto const l = (lvl == LOG_DEBUG     ? "[debug]"
                  : lvl == LOG_INFO    ? "[info]"
                  : lvl == LOG_WARNING ? "[warning]"
                  : lvl == LOG_ERR     ? "[error]"
                  :                      "[unknown]");
  std::cerr << l << str << '\n';
}

void
openlog(char const*, int, int)
{}
#endif
