#ifndef INFINIT_REACTOR_TIMER_HH
# define INFINIT_REACTOR_TIMER_HH

# include <boost/asio.hpp>

# include <elle/Printable.hh>

# include <reactor/fwd.hh>
# include <reactor/duration.hh>
# include <reactor/thread.hh>
# include <reactor/Barrier.hh>
# include <reactor/backend/thread.hh>

namespace reactor
{
  /** Timer will execute given action after delay has elapsed.
  */
  class Timer:
    public Barrier
  {
  public:
    typedef backend::Action Action;
    Timer(const Timer& b) = delete;
    Timer(Scheduler& s,
      const std::string& name,
      Duration d,
      const Action& action);
    Timer(const std::string& name,
      Duration d,
      const Action& action);
    /// Cancel timer (if action not already started)
    void cancel();
    /// Cancel timer, terminate action
    void terminate();
    /// Cancel if possible, wait for action if already started
    void cancel_now();
    /// Cancel timer, terminate action and wait for termination
    void terminate_now(bool suicide=true);
    /// Cancel timer if possible or wait for action termination
    ~Timer();
  private:
    void _on_timer(const boost::system::error_code& erc);
    Scheduler& _scheduler;
    std::string _name;
    Action _action;
    std::unique_ptr<Thread> _thread;
    boost::asio::deadline_timer _timer;
  };
}
#endif
