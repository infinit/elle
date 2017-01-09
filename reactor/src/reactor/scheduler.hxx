#include <condition_variable>

#include <elle/assert.hh>

#include <reactor/Barrier.hh>
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

  class Waiter
    : public Barrier
  {
  public:
    template <typename ... Prototype, typename F>
    Waiter(
      boost::signals2::signal<void (Prototype...)>& signal, F predicate)
      : _connection(
        signal.connect(
          [&] (Prototype ... args)
          {
            if (std::function<bool (Prototype ...)>(predicate)(args...))
              this->open();
          }))
    {}

    ELLE_ATTRIBUTE(boost::signals2::scoped_connection, connection);
  };

  namespace _details
  {
    template <typename ... Prototype, typename ... Args>
    std::enable_if_t<
      !std::is_convertible<
      typename elle::meta::List<Args...>::template head<void>::type,
        std::function<bool (Prototype ...)>>::value,
      Waiter>
      waiter(boost::signals2::signal<void (Prototype ...)>& signal,
             Args&& ... values)
    {
      return Waiter(
        signal,
        [&, vals = std::tuple<Args const& ...>(values...)]
        (Prototype ... args)
        {
          return vals == std::tuple<Args const& ...>(args...);
        });
    }

    template <typename R, typename ... Prototype, typename F>
    std::enable_if_t<
      std::is_convertible<F, std::function<bool (Prototype ...)>>::value,
      Waiter>
      waiter(boost::signals2::signal<R (Prototype...)>& signal,
             F predicate)
    {
      return Waiter(signal, std::move(predicate));
    }
  }

  template <typename Prototype, typename ... Args>
  Waiter
  waiter(boost::signals2::signal<Prototype>& signal, Args ... args)
  {
    return _details::waiter(signal, std::forward<Args>(args)...);
  }

  template <typename Prototype, typename ... Args>
  void
  wait(boost::signals2::signal<Prototype>& signal, Args&& ... args)
  {
    auto w = waiter(signal, std::forward<Args>(args)...);
    reactor::wait(w);
  }
}
