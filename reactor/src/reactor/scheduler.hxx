#include <condition_variable>

#include <elle/assert.hh>

#include <reactor/exception.hh>
#include <reactor/signal.hh>
#include <reactor/thread.hh>

namespace reactor
{
  /*----------------.
  | Multithread API |
  `----------------*/

  template <typename R>
  static void wrapper(std::mutex& mutex,
                      std::condition_variable& cond,
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
    std::unique_lock<std::mutex> lock(mutex);
    cond.notify_one();
  }

  template <typename R>
  R
  Scheduler::mt_run(const std::string& name,
                    const std::function<R ()>& action)
  {
    ELLE_ASSERT(!this->done());
    R result;
    std::mutex mutex;
    std::condition_variable condition;
    std::exception_ptr exn;
    {
      std::unique_lock<std::mutex> lock(mutex);
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

  template <typename R, typename ... Prototype, typename ... Args>
  std::enable_if_t<
    !std::is_convertible<
      typename elle::meta::List<Args...>::template head<void>::type,
      std::function<void (Prototype ...)>>::value,
    void>
  wait(boost::signals2::signal<R(Prototype...)>& signal, Args const& ... values)
  {
    auto s = reactor::Signal{};
    auto vals = std::make_tuple(values...);
    boost::signals2::scoped_connection connection = signal.connect(
      [&] (Prototype const& ... args)
      {
        if (vals == std::make_tuple(args...))
          s.signal();
      });
    reactor::wait(s);
  }

  template <typename R, typename ... Prototype, typename F>
  std::enable_if_t<
    std::is_convertible<F, std::function<bool (Prototype ...)>>::value,
    void>
  wait(boost::signals2::signal<R(Prototype...)>& signal, F predicate)
  {
    auto s = reactor::Signal{};
    boost::signals2::scoped_connection connection = signal.connect(
      [&] (Prototype const& ... args)
      {
        if (std::function<bool (Prototype ...)>(predicate)(args...))
          s.signal();
      });
    reactor::wait(s);
  }
}
