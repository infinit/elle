#ifndef ELLE_CRASHREPORTER_HH
# define ELLE_CRASHREPORTER_HH

# include <cstring>

#include <signal.h>

#define HANDLED_CRASH_MAIN(module)                                             \
                                                                               \
  template<int sig>                                                            \
  void                                                                         \
  signal_callback_handler(int)                                                 \
  {                                                                            \
    elle::report_crash(module, std::to_string(sig));                           \
    exit(sig);                                                                 \
  }                                                                            \
                                                                               \
  int                                                                          \
  main(int argc, char* argv[])                                                 \
  {                                                                            \
    /* Register signals and signal handler */               \
    signal(SIGINT,  signal_callback_handler<SIGINT>);       \
    signal(SIGTERM, signal_callback_handler<SIGTERM>);       \
    signal(SIGSEGV, signal_callback_handler<SIGSEGV>);       \
    signal(SIGKILL, signal_callback_handler<SIGKILL>);       \
                                                             \
    try                                                       \
    {                                                         \
      return _main(argc, argv);                               \
    }                                                         \
    catch(std::exception const& e)                            \
    {                                                         \
      elle::report_crash(module, e.what());                   \
    }                                                         \
    \
  }                                                        \
/**/

namespace elle
{
  bool report_crash(std::string const& module,
                    std::string const& signal);
}

#endif
