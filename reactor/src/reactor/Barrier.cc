#include <reactor/Barrier.hh>

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Barrier::Barrier(const std::string& name)
    : Super(name)
    , _opened(false)
    , _inverted(*this)
  {}

  Barrier::~Barrier()
  {
    this->_assert_destructible();
  }

  /*---------.
  | Openness |
  `---------*/

  Barrier::operator bool() const
  {
    return this->opened();
  }

  /*---------.
  | Waitable |
  `---------*/

  void
  Barrier::open()
  {
    if (!this->_opened)
    {
      this->_opened = true;
      this->_changed(this->_opened);
      this->_signal();
    }
    else
      this->_raise(std::exception_ptr());
  }

  void
  Barrier::close()
  {
    if (this->_opened)
    {
      this->_raise(std::exception_ptr());
      this->_opened = false;
      this->_changed(this->_opened);
      this->_inverted._signal();
    }
  }

  bool
  Barrier::_wait(Thread* thread, Waker const& waker)
  {
    if (this->_opened)
      return false;
    else
      return Super::_wait(thread, waker);
  }

  void
  Barrier::raise(std::exception_ptr e)
  {
    this->_raise(e);
    this->open();
    this->_raise(e);
  }

  /*----------.
  | Inversion |
  `----------*/

  Barrier::InvertedBarrier&
  Barrier::operator !()
  {
    return this->_inverted;
  }

  Barrier::InvertedBarrier::InvertedBarrier(Barrier& barrier)
    : _barrier(barrier)
  {}

  bool
  Barrier::InvertedBarrier::_wait(Thread* thread, Waker const& waker)
  {
    if (!this->_barrier._opened)
      return false;
    else
      return Super::_wait(thread, waker);
  }

  Barrier::InvertedBarrier::operator bool() const
  {
    return !this->_barrier._opened;
  }

  /*----------.
  | Printable |
  `----------*/

  void
  Barrier::print(std::ostream& stream) const
  {
    stream << "barrier ";
    if (!this->name().empty())
      stream << this->name();
    else
      stream << this;
  }
}
