#ifndef INFINIT_REACTOR_SCHEDULER_HH
# define INFINIT_REACTOR_SCHEDULER_HH

# include <set>

# include <boost/thread.hpp>

# include <reactor/asio.hh>
# include <reactor/duration.hh>
# include <reactor/fwd.hh>
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
  private:
    void _step(Thread* t);

  /*-------------------.
  | Threads management |
  `-------------------*/
  public:
    typedef std::set<Thread*> Threads;
    Thread* current() const;
    void terminate();
  private:
    void _freeze(Thread& thread);
    void _thread_register(Thread& thread);
    void _unfreeze(Thread& thread);
  private:
    void _terminate(Thread* thread);
    void _terminate_now(Thread* thread);
    Thread* _current;
    Threads _starting;
    boost::mutex _starting_mtx;
    Threads _running;
    Threads _frozen;

    /*-------.
    | Status |
    `-------*/
    public:
      void dump(std::ostream&);
      void debug();

  /*----------------.
  | Multithread API |
  `----------------*/
  public:
    template <typename R>
    R
    mt_run(const std::string& name,
           const boost::function<R ()>& action);
  private:
    void
    _mt_run_void(const std::string& name,
                 const boost::function<void ()>& action);

  /*----------.
  | Shortcuts |
  `----------*/
  public:
    void CallLater(const boost::function<void ()>&      f,
                   const std::string&                   name,
                   Duration                             delay);
    void Every(const boost::function<void ()>&  f,
               const std::string&               name,
               Duration                         delay);

    /*-----.
    | Asio |
    `-----*/
    public:
      boost::asio::io_service& io_service();
    private:
      boost::asio::io_service _io_service;
      boost::asio::io_service::work* _io_service_work;

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
