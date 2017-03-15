#include <elle/reactor/BackgroundOperation.hh>

#include <elle/log.hh>
#include <elle/utility/Move.hh>

#include <elle/reactor/scheduler.hh>

namespace elle
{
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
      ELLE_LOG_COMPONENT("elle.reactor.BackgroundOperation");
      auto status = this->_status;
      auto action = this->_action;
      auto& sched = reactor::scheduler();
      sched._run_background(
        [this, status, action, &sched] () -> std::function<void ()>
        {
          try
          {
            ELLE_TRACE_SCOPE("%s: run background operation", sched);
            auto res = elle::utility::move_on_copy(result<T>::call(action));
            ELLE_TRACE("%s: background operation finished", sched);
            return [this, status, &sched, res]
            {
              if (!status->aborted)
              {
                this->_result_set(std::move(*res));
                this->done();
              }
            };
          }
          catch (...)
          {
            ELLE_TRACE("%s: background operation threw: %s",
                       sched, elle::exception_string());
            auto e = std::current_exception();
            return [this, status, &sched, e]
            {
              if (!status->aborted)
              {
                this->_raise(e);
                this->done();
              }
            };
          }
        });
    }

    template <typename T>
    void
    BackgroundOperation<T>::_abort()
    {
      ELLE_LOG_COMPONENT("elle.reactor.BackgroundOperation");
      auto& sched = *Scheduler::scheduler();
      ELLE_TRACE_SCOPE("%s: abort background operation", sched);
      this->_status->aborted = true;
      this->_signal();
      ++sched._background_pool_free;
    }
  }
}
