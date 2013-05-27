#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <curly/curly_sched.hh>

namespace curly
{
  sched_request::sched_request(reactor::Scheduler& sched,
                               curly::request_configuration const& conf):
    Operation{sched},
    _request{sched.io_service(), conf}
  {}

  void
  sched_request::_start()
  {
    auto fn = [&] (int code)
    {
      (void)code;
      this->_signal();
    };
    this->_request.async_perform(fn);
  }

  void
  sched_request::_abort()
  {
    _signal();
  }
} /* curly */
