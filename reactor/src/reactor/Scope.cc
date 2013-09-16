#include <elle/log.hh>

#include <reactor/Scope.hh>
#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.Scope");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Scope::Scope():
    _threads(),
    _running(0)
  {}

  Scope::~Scope() noexcept(false)
  {
    ELLE_TRACE_SCOPE("%s: destroy with %s threads",
                     *this, this->_threads.size());
    this->_terminate_now();
  }

  /*--------.
  | Threads |
  `--------*/

  void
  Scope::run_background(std::string const& name,
                        Thread::Action const& a)
  {
    ELLE_TRACE_SCOPE("%s: register background job %s", *this, name);
    auto& sched = *Scheduler::scheduler();
    ++this->_running;
    this->_threads.push_back(
      new Thread(sched, name,
                 [this, a, name]
                 {
                   try
                   {
                     ELLE_TRACE("%s: background job %s starts",
                                *this, name)
                       a();
                     ELLE_TRACE("%s: background job %s finished",
                                *this, name);
                   }
                   catch (Terminate const&)
                   {
                     ELLE_TRACE("%s: background job %s terminated",
                                *this, name);
                   }
                   catch (...)
                   {
                     ELLE_TRACE_SCOPE("%s: background job %s threw: %s",
                                      *this, name, elle::exception_string());
                     this->_raise(std::current_exception());
                     this->_terminate_now();
                   }
                   if (!--this->_running)
                     this->_signal();
                 }));

    auto it = begin(this->_threads);
    for (; it != end(this->_threads); ++it)
    {
      auto* t = *it;

      if (t->state() == Thread::State::done)
      {
        delete t;
        it = this->_threads.erase(it);
      }
    }
  }

  void
  Scope::_terminate_now()
  {
    ELLE_TRACE_SCOPE("%s: terminate now", *this);
    auto current = reactor::Scheduler::scheduler()->current();
    std::exception_ptr e;
    for (auto* t: this->_threads)
    {
      if (t == current)
        continue;
      try
      {
        ELLE_DEBUG_SCOPE("%s: terminate %s now", *this, *t);
        t->terminate_now();
      }
      catch (...)
      {
        e = std::current_exception();
      }
      delete t;
    }
    this->_threads.clear();
    this->_threads.push_back(current);
    if (e)
      std::rethrow_exception(e);
  }

  /*---------.
  | Waitable |
  `---------*/

  bool
  Scope::_wait(Thread* thread)
  {
    if (this->_running == 0)
      return false;
    else
      return Waitable::_wait(thread);
  }
}
