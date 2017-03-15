#pragma once

#include <elle/reactor/Barrier.hh>
#include <elle/reactor/fwd.hh>
#include <elle/reactor/Thread.hh>
#include <elle/With.hh>

namespace elle
{
  namespace reactor
  {
    /// Manage a collection of Threads.
    ///
    /// A Scope enables to start parallel tasks in Threads and make sure they
    /// are terminated upon destruction.  As a Waitable, a Scope blocks its
    /// waiters until all currently managed Threads are done, enabling to easily
    /// join the group of managed Threads.  If an exception escapes from a
    /// Thread, all other threads are killed and waiting the Scope will re-throw
    /// the exception.
    ///
    /// \code{.cc}
    ///
    /// elle::With<elle::reactor::Scope>() << [] (elle::reactor::Scope& s)
    ///   {
    ///     elle::reactor::Barrier b;
    ///     s.run_background("one",
    ///       [&b]
    ///       {
    ///         std::cout << "[one sleep] "
    ///         elle::reactor::sleep(50_ms);
    ///         b.open();
    ///         std::cout << "[one done] "
    ///       });
    ///     s.run_background("two",
    ///       [&b]
    ///       {
    ///         std::cout << "[two wait] ";
    ///         b.wait();
    ///         std::cout << "[two done] ";
    ///       });
    ///     s.wait(); // Wait until all managed Threads are done.
    ///   };
    /// // Result: [one sleep] [two wait] [one done] [two done]
    ///
    /// \endcode.
    class Scope
      : public reactor::Waitable
    {
    /*-------------.
    | Construction |
    `-------------*/
    private:
      /// Create a scope.
      Scope(std::string name = {});
      /// Destruct a scope, terminating all managed threads now.
      ///
      /// \throw Terminate if interrupted while killing managed threads.
      virtual
      ~Scope() noexcept(false);
      /// Let With manage us.
      friend class elle::With<Scope>;

    /*--------.
    | Threads |
    `--------*/
    public:
      /// Start and manage a thread named \a name executing \a a.
      ///
      /// If an exception escapes \a a, all other threads are killed and waiting
      /// the Scope re-throws the exception.
      ///
      /// \param name The name of the managed thread.
      /// \param a    The action run by the managed thread.
      Thread&
      run_background(std::string const& name,
                     Thread::Action a);
      /// Terminate the Scope by terminating all managed Threads.
      void
      terminate_now();
    private:
      void
      _terminate_now();
      /// Managed threads.
      ELLE_ATTRIBUTE(std::vector<Thread*>, threads);
      /// Number of running threads.
      ELLE_ATTRIBUTE(int, running);
      ELLE_ATTRIBUTE(std::exception_ptr, exception);

    /*---------.
    | Waitable |
    `---------*/
    protected:
      bool
      _wait(Thread* thread, Waker const& waker) override;

    private:
      ELLE_ATTRIBUTE_R(std::string, name);
    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const override;
    };
  }
}
