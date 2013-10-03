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
                     if (!this->_exception)
                     {
                       this->_exception = std::current_exception();
                       this->_raise(std::current_exception());
                       this->_terminate_now();
                     }
                     else
                       ELLE_WARN(
                         "%s: exception already caught, losing exception: %s",
                         *this, elle::exception_string());
                   }
                   if (!--this->_running)
                     this->_signal();
                 }));

    auto it = begin(this->_threads);
    while (it != end(this->_threads))
    {
      auto* t = *it;
      if (t->state() == Thread::State::done)
      {
        delete t;
        it = this->_threads.erase(it);
      }
      else
        ++it;
    }
  }

  void
  Scope::terminate_now()
  {
    this->_terminate_now();
  }

  void
  Scope::_terminate_now()
  {
    ELLE_TRACE_SCOPE("%s: terminate now", *this);
    auto current = reactor::Scheduler::scheduler()->current();
    std::exception_ptr e;
    bool inside = false;
    Waitables join;
    join.reserve(this->_threads.size());
    for (auto* t: this->_threads)
    {
      if (t == current)
      {
        inside = true;
        continue;
      }
      ELLE_DEBUG("%s: terminate %s", *this, *t)
        t->terminate();
      join.push_back(t);
    }
    while (true)
    {
      ELLE_DEBUG_SCOPE("%s: wait for all threads to finish", *this);
      try
      {
        Scheduler::scheduler()->current()->wait(join);
        break;
      }
      catch (...)
      {
        e = std::current_exception();
      }
    }
    for (auto* t: this->_threads)
    {
      if (t == current)
        continue;
      delete t;
    }
    this->_threads.clear();
    if (inside)
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
