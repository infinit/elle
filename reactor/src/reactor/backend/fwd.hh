#ifndef REACTOR_BACKEND_FWD_HH
# define REACTOR_BACKEND_FWD_HH

# include <functional>

namespace reactor
{
  namespace backend
  {
    /// An action run by a thread.
    typedef std::function<void ()> Action;
    class Backend;
    class Thread;
  }
}

#endif
