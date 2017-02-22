#ifndef REACTOR_BACKEND_FWD_HH
# define REACTOR_BACKEND_FWD_HH

# include <functional>

namespace reactor
{
  namespace backend
  {
    /// An action run by a thread.
    using Action = std::function<void ()>;
    class Backend;
    class Thread;
  }
}

#endif
