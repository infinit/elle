#pragma once

#include <boost/asio.hpp>

#include <elle/Printable.hh>
#include <elle/reactor/fwd.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/Thread.hh>

namespace elle
{
  namespace reactor
  {
    /// Timer will execute given action after delay has elapsed.
    ///
    /// The action is run in a new Thread, managed by the Timer itself.
    class Timer:
      public Waitable
    {
    public:
      using Action = Thread::Action;
      Timer(const Timer& b) = delete;
      /// Construct a Timer.
      ///
      /// \param scheduler The Scheduler to run the Thread in.
      /// \param name A descriptive name used for debugging.
      /// \param d The duration before executing the action.
      /// \param action The Action to execute.
      Timer(Scheduler& s,
            std::string name,
            Duration d,
            Action action);
      /// Construct a Timer.
      ///
      /// \param name A descriptive name used for debugging.
      /// \param d The duration before executing the action.
      /// \param action The Action to execute.
      Timer(std::string const& name,
            Duration d,
            Action const& action);
      /// Cancel timer (if action not already started).
      void
      cancel();
      /// Cancel timer, terminate action.
      void
      terminate();
      /// Cancel if possible, wait for action if already started.
      void
      cancel_now();
      /// Cancel timer, terminate action and wait for termination.
      void
      terminate_now(bool suicide = true);
      /// Cancel timer if possible or wait for action termination.
      ~Timer();
    protected:
      virtual
      bool
      _wait(Thread* thread, Waker const& waker) override;
    private:
      void
      _on_timer(const boost::system::error_code& erc);

      Scheduler& _scheduler;
      std::string _name;
      Action _action;
      std::unique_ptr<Thread> _thread;
      boost::asio::deadline_timer _timer;
      bool _finished;
    };
  }
}
