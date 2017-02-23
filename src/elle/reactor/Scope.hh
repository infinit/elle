#pragma once

#include <elle/reactor/Barrier.hh>
#include <elle/reactor/fwd.hh>
#include <elle/reactor/thread.hh>
#include <elle/With.hh>

namespace elle
{
  namespace reactor
  {
    /// Manage a collection of Threads.
    ///
    /// A Scope enables to start parallel tasks in threads and make sure they
    /// are terminated upon destruction.  As a Waitable, a Scope blocks its
    /// waiters until all currently managed threads are done, enabling to easily
    /// join the group of managed threads.  If an exception escapes from a
    /// thread, all other threads are killed and waiting the Scope will re-throw
    /// the exception.
    class Scope
      : public reactor::Waitable
    {
    /*-------------.
    | Construction |
    `-------------*/
    private:
      /// Create a scope.
      Scope(std::string const& name = std::string{});
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
