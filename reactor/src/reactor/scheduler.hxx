#ifndef INFINIT_REACTOR_SCHEDULER_HXX
# define INFINIT_REACTOR_SCHEDULER_HXX

# include <elle/assert.hh>

# include <reactor/exception.hh>
# include <reactor/signal.hh>
# include <reactor/thread.hh>

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
    catch (Terminate const&)
    {
      // Ignore
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
                    const std::function<R ()>& action)
  {
    ELLE_ASSERT(!this->done());
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
                          const std::function<void ()>& action);

  // A stateless Comparer would be preferable, but GCC 4.8 has a limitation
  // where you can't capture parameter packs in lambdas, forcing a 2-step
  // passing of packs.
  template <typename ... Proto>
  struct Compare
  {
    bool
    compare()
    {
      return true;
    }
  };

  template <typename Head, typename ... Tail>
  struct Compare<Head, Tail...>
    : public Compare<Tail...>
  {
    Compare(Head const& head, Tail const& ... tail)
      : Compare<Tail...>(tail ...)
      , _head(head)
    {}

    bool
    compare(Head const& head, Tail const& ... tail)
    {
      return head == this->_head && this->Compare<Tail...>::compare(tail ...);
    }

    Head const& _head;
  };

  template <typename R, typename ... Prototype, typename ... Args>
  void
  wait(boost::signals2::signal<R(Prototype...)>& signal, Args const& ... values)
  {
    reactor::Signal s;
    Compare<Prototype...> compare(values...);
    boost::signals2::scoped_connection connection = signal.connect(
      [&] (Prototype const& ... args)
      {
        if (compare.compare(args...))
          s.signal();
      });
    reactor::wait(s);
  }
}

#endif
