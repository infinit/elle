#ifndef INFINIT_REACTOR_THREAD_HXX
# define INFINIT_REACTOR_THREAD_HXX

namespace reactor
{
  template <typename R>
  static void vthread_catcher(const typename VThread<R>::Action& action,
                       R& result)
  {
    result = action();
  }

  template <typename R>
  VThread<R>::VThread(Scheduler& scheduler,
                      const std::string& name,
                      const Action& action)
    : Thread(scheduler, name, boost::bind(vthread_catcher<R>, action,
                                          boost::ref(_result)))
    , _result()
  {}

  template <typename R>
  const R&
  VThread<R>::result() const
  {
    if (state() != Thread::state::done)
      throw Exception("tried to fetched the result of an unfinished thread");
    return _result;
  }
}

#endif
