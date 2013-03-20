#include <reactor/semaphore.hh>

namespace reactor
{
  Semaphore::Semaphore(int count)
    : _count(count)
  {}

  int
  Semaphore::count() const
  {
    return _count;
  }

  bool
  Semaphore::release()
  {
    ++_count;
    return _signal_one();
  }

  bool
  Semaphore::_wait(Thread* thread)
  {
    --_count;
    if (_count < 0)
    {
      Waitable::_wait(thread);
      return true;
    }
    else
      return false;
  }
}
