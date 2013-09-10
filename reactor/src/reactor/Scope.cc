#include <reactor/Scope.hh>
#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Scope::Scope():
    _threads(),
    _running(0)
  {
    this->open();
  }

  Scope::~Scope()
  {
    this->_terminate_now();
  }

  /*--------.
  | Threads |
  `--------*/

  void
  Scope::run_background(std::string const& name,
                        Thread::Action const& a)
  {
    this->close();
    auto& sched = *Scheduler::scheduler();
    this->_threads.push_back(
      new Thread(sched, name,
                 [this, a]
                 {
                   ++this->_running;
                   try
                   {
                     a();
                   }
                   catch (Terminate const&)
                   {
                     if (!--this->_running)
                       this->open();
                     throw;
                   }
                   catch (...)
                   {
                     this->_raise(std::current_exception());
                     this->_terminate_now();
                   }
                   if (!--this->_running)
                     this->open();
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
    auto current = reactor::Scheduler::scheduler()->current();
    for (auto* t: this->_threads)
    {
      if (t == current)
        continue;
      t->terminate_now();
      delete t;
    }
    _threads.clear();
  }
}
