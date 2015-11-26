#include <reactor/BackgroundOperation.hh>

#include <elle/log.hh>
#include <elle/utility/Move.hh>

#include <reactor/scheduler.hh>

namespace reactor
{
  template <typename T>
  BackgroundOperation<T>::BackgroundOperation(Action const& action)
    : Operation(*Scheduler::scheduler())
    , _action(action)
    , _status(std::make_shared<Status>())
  {
    this->_status->aborted = false;
  }

  template <typename T>
  BackgroundOperation<T>::~BackgroundOperation()
  {
    if (this->running())
      this->_abort();
  }

  template <typename T>
  void
  BackgroundOperation<T>::_abort()
  {
    ELLE_LOG_COMPONENT("reactor.BackgroundOperation");
    auto& sched = *Scheduler::scheduler();
    ELLE_TRACE_SCOPE("%s: abort background operation", sched);
    this->_status->aborted = true;
    this->_signal();
    ++sched._background_pool_free;
  }

  template <typename T>
  void
  BackgroundOperationResult<T>::_result_set(T&& v)
  {
    this->_result.emplace(v);
  }

  inline
  void
  BackgroundOperationResult<void>::_result_set(bool)
  {}

  namespace
  {
    template <typename T>
    struct result
    {
      static
      T
      call(std::function<T ()> const& f)
      {
        return f();
      }
    };

    template <>
    struct result<void>
    {
      static
      bool
      call(std::function<void ()> const& f)
      {
        f();
        return true;
      }
    };
  }

  template <typename T>
  void
  BackgroundOperation<T>::_start()
  {
    ELLE_LOG_COMPONENT("reactor.BackgroundOperation");
    auto& sched = *Scheduler::scheduler();
    auto& current = *sched.current();
    if (sched._background_pool_free == 0)
    {
      ELLE_DEBUG("%s: spawn new background thread", sched);
      sched._background_pool.emplace_back([&sched]
                                          {
                                            sched._background_service.run();
                                          });
    }
    else
      --sched._background_pool_free;
    auto status = this->_status;
    auto action = this->_action;
    sched._background_service.post(
      [this, status, action, &current, &sched]
      {
        try
        {
          ELLE_TRACE_SCOPE("%s: run background operation", sched);
          auto res = elle::utility::move_on_copy(result<T>::call(action));
          ELLE_TRACE("%s: background operation finished", sched);
          sched.io_service().post([this, status, &sched, res]
                                  {
                                    if (!status->aborted)
                                    {
                                      this->_result_set(std::move(*res));
                                      this->done();
                                      ++sched._background_pool_free;
                                    }
                                  });
        }
        catch (...)
        {
          ELLE_TRACE("%s: background operation threw: %s",
                     sched, elle::exception_string());
          auto e = std::current_exception();
          sched.io_service().post([this, status, &sched, e]
                                  {
                                    if (!status->aborted)
                                    {
                                      this->_raise(e);
                                      this->done();
                                      ++sched._background_pool_free;
                                    }
                                  });
        }
    });
  }
}
