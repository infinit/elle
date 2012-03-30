#ifndef INFINIT_REACTOR_SCHEDULER_HH
# define INFINIT_REACTOR_SCHEDULER_HH

# include <set>

# include <boost/thread.hpp>

# include <reactor/asio.hh>
# include <reactor/fwd.hh>
# include <reactor/scheduler.hh>
# include <reactor/backend/thread.hh>

namespace reactor
{
  class Scheduler
  {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Scheduler();

    /*----.
    | Run |
    `----*/
    public:
      void run();

    /*-------------------.
    | Threads management |
    `-------------------*/
    public:
      Thread* current() const;
    private:
      void _freeze(Thread& thread);
      void _thread_register(Thread& thread);
      void _unfreeze(Thread& thread);
      typedef std::set<Thread*> Threads;
      Thread* _current;
      Threads _starting;
      boost::mutex _starting_mtx;
      Threads _running;
      Threads _frozen;

    /*-----.
    | Asio |
    `-----*/
    public:
      boost::asio::io_service& io_service();
    private:
      boost::asio::io_service _io_service;

    /*--------.
    | Details |
    `--------*/
    private:
      friend class Thread;
      backend::Manager _manager;
  };
}

#endif
