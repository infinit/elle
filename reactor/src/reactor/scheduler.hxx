#ifndef INFINIT_REACTOR_SCHEDULER_HXX
# define INFINIT_REACTOR_SCHEDULER_HXX

# include <reactor/thread.hh>

# include <elle/assert.hh>

namespace reactor
{
  /*----------------.
  | Multithread API |
  `----------------*/

  template <typename R>
  static void wrapper(boost::mutex& mutex,
                      boost::condition_variable& cond,
                      const std::function<R ()>& action,
                      std::exception_ptr& exn,
                      R& res)
  {
    try
    {
      res = action();
    }
    catch (...)
    {
      exn = std::current_exception();
    }
    boost::unique_lock<boost::mutex> lock(mutex);
    cond.notify_one();
  }

  template <typename R>
  R
  Scheduler::mt_run(const std::string& name,
                    const boost::function<R ()>& action)
  {
    R result;
    boost::mutex mutex;
    boost::condition_variable condition;
    std::exception_ptr exn;
    {
      boost::unique_lock<boost::mutex> lock(mutex);
      new reactor::Thread(*this, name,
                          std::bind(&wrapper<R>,
                                    std::ref(mutex),
                                    std::ref(condition),
                                    action,
                                    std::ref(exn),
                                    std::ref(result)), true);
      condition.wait(lock);
      if (exn)
        std::rethrow_exception(exn);
    }
    return result;
  }

  template <>
  void
  Scheduler::mt_run<void>(const std::string& name,
                          const boost::function<void ()>& action);
}

#endif
