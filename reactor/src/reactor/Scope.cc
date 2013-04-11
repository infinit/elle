#include <reactor/scheduler.hh>
#include <reactor/Scope.hh>
#include <reactor/thread.hh>

namespace reactor
{
  Scope::Scope():
    _threads()
  {}

  Scope::~Scope()
  {
    //auto& sched = *Scheduler::scheduler()->current();
    // sched.wait(this->_threads);
    for (auto* t: this->_threads)
    {
      t->terminate_now();
      delete t;
    }
    _threads.clear();
  }

  void
  Scope::run_background(std::string const& name,
                        Thread::Action const& a)
  {
    auto& sched = *Scheduler::scheduler();
    this->_threads.push_back(new Thread(sched, name, a));

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
}
