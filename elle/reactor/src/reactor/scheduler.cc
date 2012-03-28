#include <boost/foreach.hpp>

#include <reactor/debug.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Scheduler::Scheduler()
    : _current(0)
    , _running()
    , _frozen()
  {}

  /*----.
  | Run |
  `----*/

  void
  Scheduler::run()
  {
    while (true)
    {
      Threads running(_running);
      INFINIT_REACTOR_DEBUG("Scheduler: new round with "
                            << running.size() << " jobs");
      BOOST_FOREACH (Thread* t, running)
      {
        INFINIT_REACTOR_DEBUG("Scheduler: schedule " << *t);
        _current = t;
        try
        {
          t->_step();
        }
        catch (const std::runtime_error& err)
        {
          std::cerr << "thread " << t->name() << ": "
                    << err.what() << std::endl;
          std::abort();
        }
        catch (...)
        {
          std::cerr << "thread " << t->name() << ": "
                    << "unknown error" << std::endl;
          std::abort();
        }
        _current = 0;
        if (t->state() == Thread::state::done)
        {
          INFINIT_REACTOR_DEBUG("Scheduler: cleanup " << *t);
          _running.erase(t);
        }
      }
      INFINIT_REACTOR_DEBUG("Scheduler: run asio callbacks");
      _io_service.reset();
      _io_service.poll();
      if (_running.empty())
        if (_frozen.empty())
          break;
        else
          while (_running.empty())
          {
            INFINIT_REACTOR_DEBUG("Scheduler: nothing to do, "
                                  "polling asio in a blocking fashion");
            _io_service.reset();
            _io_service.run_one();
          }
    }
    INFINIT_REACTOR_DEBUG("Scheduler: done");
    assert(_frozen.empty());
  }

  /*-------------------.
  | Threads management |
  `-------------------*/

  Thread*
  Scheduler::current() const
  {
    return _current;
  }

  void
  Scheduler::_freeze(Thread& thread)
  {
    assert(thread.state() == Thread::state::running);
    assert(_running.find(&thread) != _running.end());
    _running.erase(&thread);
    _frozen.insert(&thread);
  }

  void
  Scheduler::_thread_register(Thread& thread)
  {
    _running.insert(&thread);
  }

  void
  Scheduler::_unfreeze(Thread& thread)
  {
    assert(thread.state() == Thread::state::frozen);
    _frozen.erase(&thread);
    _running.insert(&thread);
  }

  /*-----.
  | Asio |
  `-----*/

  boost::asio::io_service&
  Scheduler::io_service()
  {
    return _io_service;
  }
}
