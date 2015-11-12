#include <reactor/BackgroundOperation.hh>

#include <elle/log.hh>

#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.BackgroundOperation");

namespace reactor
{
  BackgroundOperation::BackgroundOperation(Action const& action)
    : Operation(*Scheduler::scheduler())
    , _action(action)
    , _status(std::make_shared<Status>())
  {
    this->_status->aborted = false;
  }

  void
  BackgroundOperation::_abort()
  {
    auto& sched = *Scheduler::scheduler();
    ELLE_TRACE_SCOPE("%s: abort background operation", sched);
    this->_status->aborted = true;
    this->_signal();
    ++sched._background_pool_free;
  }

  void
  BackgroundOperation::_start()
  {
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
          action();
          ELLE_TRACE("%s: background operation finished", sched);
          sched.io_service().post([this, status, &sched]
                                  {
                                    if (!status->aborted)
                                    {
                                      this->_signal();
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
                                      this->_signal();
                                      ++sched._background_pool_free;
                                    }
                                  });
        }
    });
  }
}
