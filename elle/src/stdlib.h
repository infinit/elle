#pragma once

// Avoid deprecation warning about daemon.
#if __APPLE__
# define daemon daemon_repl
#endif
#include_next <stdlib.h>
#if __APPLE__
# undef daemon
extern "C" int daemon(int, int);
#endif
