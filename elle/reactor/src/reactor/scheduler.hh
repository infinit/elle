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
  /** Scheduler
   *
   */
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
      bool step();

    /*-------------------.
    | Threads management |
    `-------------------*/
    public:
      typedef std::set<Thread*> Threads;
      Thread* current() const;
    private:
      void _freeze(Thread& thread);
      void _thread_register(Thread& thread);
      void _unfreeze(Thread& thread);
    private:
      Thread* _current;
      Threads _starting;
      boost::mutex _starting_mtx;
      Threads _running;
      Threads _frozen;

    /*-------.
    | Status |
    `-------*/
    public:
      void Dump(std::ostream&);

    /*----------------.
    | Multithread API |
    `----------------*/
    public:
      template <typename R>
      R
      mt_run(const std::string&                 name,
             const boost::function<R ()>&       action);

    /*-----.
    | Asio |
    `-----*/
    public:
      boost::asio::io_service& io_service();
    private:
      boost::asio::io_service _io_service;
      boost::asio::io_service::work _io_service_work;

    /*--------.
    | Details |
    `--------*/
    private:
      friend class Thread;
      backend::Manager _manager;
  };
}

# include <reactor/scheduler.hxx>

#endif
