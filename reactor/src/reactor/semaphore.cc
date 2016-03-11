#include <reactor/semaphore.hh>

namespace reactor
{
  Semaphore::Semaphore(int count)
    : _count(count)
  {}

  int
  Semaphore::count() const
  {
    return this->_count;
  }

  bool
  Semaphore::release()
  {
    ++this->_count;
    return this->_signal_one();
  }

  bool
  Semaphore::_wait(Thread* thread, Waker const& waker)
  {
    --this->_count;
    if (this->_count < 0)
    {
      this->Waitable::_wait(thread, waker);
      return true;
    }
    else
      return false;
  }
}
