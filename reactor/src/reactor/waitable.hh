#ifndef INFINIT_REACTOR_WAITABLE_HH
# define INFINIT_REACTOR_WAITABLE_HH

# include <set>

# include <boost/function.hpp>
# include <boost/noncopyable.hpp>

# include <elle/Exception.hh>
# include <elle/Printable.hh>

# include <reactor/duration.hh>
# include <reactor/fwd.hh>

namespace reactor
{
  class Waitable:
    public boost::noncopyable,
    public elle::Printable
  {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      /// Self type.
      typedef Waitable Self;
      /// Collection of threads blocked on this.
      typedef std::set<Thread*> Waiters;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Construct a Waitable.
    Waitable(const std::string& name = std::string());
    /// Move a Waitable.
    Waitable(Waitable&& source);
  protected:
    /// Destroy a Waitable.
    ~Waitable();

    /*-------.
    | Status |
    `-------*/
    public:
      /// Pretty name.
      const std::string& name() const;
      /// Threads blocked on this.
      const Waiters& waiters() const;
    private:
      std::string _name;

  /*--------.
  | Waiting |
  `--------*/
  public:
    bool
    wait(DurationOpt timeout = DurationOpt());

    protected:
      /** Wake a thread when we are done, if we must be waited.
       *
       * If true is returned, the thread will block until we call its
       * wake method. Otherwise it will carry on, and its wake method
       * should not be called.
       *
       * The default implementation always returns true and registers
       * the thread for wake. By overriding this method, one can
       * create a waitable that does not always block by either
       * returning false, either calling the parent method and
       * returning true.
       *
       * @param thread The thread to wake up when we're done.
       * @return Whether the thread needs to wait for us.
       */
      virtual bool _wait(Thread* thread);
      /** Abort waking a thread when we are done.
       *
       * Forget about a thread previously successfully registered with
       * _wait. This happen in the case of timeouts, for
       * instance. Overriding this method can be useful to cancel an
       * operation if nobody waits on it anymore, for instance. The
       * parent method must still be called, to avoid trying to wake
       * up the thread erroneously when we're done.
       */
      virtual void _unwait(Thread* thread);
      /// Wake waiting threads.
      int  _signal();
      virtual Thread* _signal_one();
      /** Register an exception waiting thread should throw when woken.
       *
       * @param e The exception threads must throw.
       */
      template <typename Exception, typename... Args>
      void _raise(Args&&... args);
      void _raise(std::exception_ptr e);

    private:
      /// Let threads register/unregister themselves.
      friend class Thread;
      /// Waiting threads.
      Waiters _threads;
      /// Exception woken thread must throw.
      ELLE_ATTRIBUTE_R(std::exception_ptr, exception);

  /*----------.
  | Printable |
  `----------*/
  public:
    virtual
    void
    print(std::ostream& stream) const override;
  };

  /// Add a Waitable into Waitables.
  Waitables& operator << (Waitables& waitables, Waitable& s);
  /// Add a Waitable into Waitables.
  Waitables& operator << (Waitables& waitables, Waitable* s);
  /// Pretty print Waitables
  std::ostream& operator << (std::ostream& stream, Waitables const& waitables);
}

# include <reactor/waitable.hxx>

#endif
