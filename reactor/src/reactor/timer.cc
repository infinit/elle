#include <reactor/timer.hh>

#include <boost/asio.hpp>

#include <elle/finally.hh>
#include <elle/log.hh>

#include <reactor/duration.hh>
#include <reactor/thread.hh>
#include <reactor/scheduler.hh>
#include <reactor/Barrier.hh>


ELLE_LOG_COMPONENT("reactor.Timer")

namespace reactor
{
  Timer::Timer(const std::string& name,
    Duration d,
    const Action& action)
  : Timer(*Scheduler::scheduler(), name, d, action)
  {}

  Timer::Timer(Scheduler& s,
    const std::string& name,
    Duration d,
    const Action& action)
  : _scheduler(s)
  , _name(name)
  , _action(action)
  , _timer(s.io_service())
  , _finished(false)
  {
    _timer.expires_from_now(d);
    ELLE_TRACE_SCOPE("%s: trigger in %s", *this, d);
    _timer.async_wait(std::bind(&Timer::_on_timer, this, std::placeholders::_1));
  }

  Timer::~Timer()
  {
    cancel_now();
  }

  void Timer::_on_timer(const boost::system::error_code& erc)
  {
    ELLE_TRACE_SCOPE("%s: timer reached, canceled: %s", *this, !!erc);
    // Warning, we are not in a Thread!
    if (!erc)
    {
      ELLE_TRACE("%s: start thread", *this);
      _thread.reset(new Thread(_scheduler, _name,
        [this]
        {
          ELLE_TRACE("%s: invoke callback", *this)
            this->_action();
        }));
      _thread->released().connect([this]
        {
          ELLE_TRACE("%s: interrupted or finished, notify", *this);
          this->_finished = true;
          this->_signal();
        });
    }
    else
    {
      this->_finished = true;
      this->_signal();
    }
  }

  void Timer::cancel()
  {
    this->_timer.cancel();
  }

  void Timer::cancel_now()
  {
    ELLE_TRACE_SCOPE("%s: cancel now", *this);
    this->cancel();
    if (!this->_finished)
    {
      ELLE_TRACE("%s: wait", *this);
      this->wait();
    }
    // Waiting on the barrier is not enough as it gets opened from
    // the thread. We must wait for the thread itself.
    if (this->_thread && !this->_thread->done())
      reactor::wait(*this->_thread);
  }

  void Timer::terminate()
  {
    ELLE_TRACE_SCOPE("%s: terminate", *this);
    this->cancel();
    if (this->_thread)
      this->_thread->terminate();
  }

  void Timer::terminate_now(bool suicide)
  {
    ELLE_TRACE_SCOPE("%s: terminate now", *this);
    this->cancel();
    if (this->_thread)
      this->_thread->terminate_now(suicide);
  }

  // waitable interface
  bool Timer::_wait(Thread* thread)
  {
    if (this->_finished)
      return false;
    else
      return Waitable::_wait(thread);
  }
}
