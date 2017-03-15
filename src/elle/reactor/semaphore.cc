#include <elle/reactor/semaphore.hh>

namespace elle
{
  namespace reactor
  {
    Semaphore::Semaphore(int count)
      : _count(count)
    {}

    bool
    Semaphore::acquire()
    {
      if (this->_count > 0)
      {
        --this->_count;
        return true;
      }
      else
        return false;
    }

    bool
    Semaphore::release()
    {
      ++this->_count;
      return this->_signal();
    }

    bool
    Semaphore::_wait(Thread* thread, Waker const& waker)
    {
      if (this->_count <= 0)
      {
        this->Waitable::_wait(thread, waker);
        return true;
      }
      else
        return false;
    }
  }
}
