#ifndef INFINIT_REACTOR_DEBUG_HH
# define INFINIT_REACTOR_DEBUG_HH

# include <boost/thread.hpp>

# ifdef INFINIT_REACTOR_ENABLE_DEBUG
#  include <iostream>
# define INFINIT_REACTOR_DEBUG(Arg)                             \
  do                                                            \
  {                                                             \
    boost::unique_lock<boost::mutex> lock(debug_mutex);         \
    std::cerr << Arg << std::endl;                              \
  } while (false)
#else
# define INFINIT_REACTOR_DEBUG(Arg)
#endif

namespace reactor
{
  extern boost::mutex debug_mutex;
}

#endif
