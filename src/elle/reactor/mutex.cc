#include <elle/reactor/mutex.hh>
#include <elle/assert.hh>

namespace elle
{
  namespace reactor
  {
    Mutex::Mutex()
      : _locked(false)
    {}

    Mutex::~Mutex()
    = default;

    bool
    Mutex::release()
    {
      ELLE_ASSERT(this->_locked);
      this->_locked = false;
      this->_signal();
      return false;
    }

    bool
    Mutex::acquire()
    {
      if (this->_locked)
        return false;
      this->_locked = true;
      return true;
    }

    bool
    Mutex::_wait(Thread* thread, Waker const& waker)
    {
      if (this->_locked)
      {
        this->Waitable::_wait(thread, waker);
        this->_locked = true;
        return true;
      }
      else
      {
        return false;
      }
    }
  }
}
