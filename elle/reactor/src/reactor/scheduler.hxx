#ifndef INFINIT_REACTOR_SCHEDULER_HXX
# define INFINIT_REACTOR_SCHEDULER_HXX

namespace reactor
{
  /*----------------.
  | Multithread API |
  `----------------*/

  template <typename R>
  static void wrapper(boost::mutex& mutex,
                      boost::condition_variable& cond,
                      const boost::function<R ()>& action,
                      R& res)
  {
    assert(action);
    res = action();
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
    {
      boost::unique_lock<boost::mutex> lock(mutex);
      new reactor::Thread(*this, name,
                          boost::bind(&wrapper<R>,
                                      boost::ref(mutex),
                                      boost::ref(condition),
                                      action, boost::ref(result)), true);
      condition.wait(lock);
    }
    return result;
  }
}

#endif
