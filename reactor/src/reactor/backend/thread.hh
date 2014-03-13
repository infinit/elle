#ifndef INFINIT_REACTOR_BACKEND_THREAD_HH
# define INFINIT_REACTOR_BACKEND_THREAD_HH

# include <boost/function.hpp>

namespace reactor
{
  namespace backend
  {
    namespace status
    {
      enum Status
      {
        /// The thread has finished.
        done,
        /// The thread is currently running.
        running,
        /// The thread has been created, but did not run yet.
        starting,
        /// The thread is in a runnable state, but not currently
        /// running.
        waiting,
      };
    }
    using status::Status;

    /// Type of a thread main routine.
    typedef boost::function0<void> Action;
  }
}

# include <reactor/backend/pthread/pthread.hh>
# include <reactor/backend/coro_io/thread.hh>

namespace reactor
{
  namespace backend
  {
    // typedef pthread::Manager Manager;
    // typedef pthread::Thread Thread;
    typedef coro_io::Manager Manager;
    typedef coro_io::Thread Thread;
  }
}

#endif
