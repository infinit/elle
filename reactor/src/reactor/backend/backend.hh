#ifndef REACTOR_BACKEND_BACKEND_HH
# define REACTOR_BACKEND_BACKEND_HH

# include <functional>
# include <memory>
# include <string>

# include <elle/attribute.hh>

# include <reactor/backend/fwd.hh>

namespace reactor
{
  namespace backend
  {
    class Thread;

    /** Pool of thread that can switch execution.
     *
     * All thread are affiliated with a Manager, and can only switch
     * execution to threads in the same manager. The context which
     * instantiated the Manager is a valid thread (the root thread).
     */
    class Backend
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Backend Self;
      virtual
      ~Backend();

    /*--------.
    | Threads |
    `--------*/
    public:
      /// Create a new thread.
      virtual
      std::unique_ptr<backend::Thread>
      make_thread(const std::string& name,
                  Action action) = 0;
      /// The currently running thread.
      virtual
      Thread*
      current() const = 0;
    };

    class Thread
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      /// Ourselves.
      typedef Thread Self;
      enum class Status
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

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Thread(const std::string& name, Action action);
      virtual
      ~Thread();

    /*------.
    | State |
    `------*/
    public:
      /// Run action.
      ELLE_ATTRIBUTE_R(Action, action);
      /// Pretty name.
      ELLE_ATTRIBUTE_R(std::string, name);
      /// Current status.
      ELLE_ATTRIBUTE_R(Status, status);
      ELLE_ATTRIBUTE_R(bool, unwinding, protected);
      ELLE_ATTRIBUTE_R(std::exception_ptr, exception, protected); // stored when yielding
      ELLE_ATTRIBUTE_R(void*, exception_storage, protected);
    protected:
      void
      status(Status status);

    /*----------.
    | Switching |
    `----------*/
    public:
      /** Start or resume execution.
       *
       * Start execution by running the action or resume it at the
       * point where yield was called. May only be called on a
       * waiting or starting thread. Switch status to
       * running. Make this thread the current thread.
       */
      virtual
      void
      step() = 0;
      /** Give execution back to our caller.
       *
       * Suspend our execution and give it back to the thread that
       * called our step method. May only be called on the current
       * thread (whose status is thus running). Switch status to
       * waiting. Make the caller the current thread.
       */
      virtual
      void
      yield() = 0;
    };
  }
}

#endif
