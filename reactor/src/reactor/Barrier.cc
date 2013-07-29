#include <reactor/Barrier.hh>

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Barrier::Barrier(const std::string& name):
    Super(name),
    _opened(false)
  {}

  /*---------.
  | Waitable |
  `---------*/

  void
  Barrier::open()
  {
    this->_opened = true;
    this->_signal();
  }

  void
  Barrier::close()
  {
    this->_opened = false;
  }

  bool
  Barrier::_wait(Thread* thread)
  {
    if (this->_opened)
      return false;
    else
      return Super::_wait(thread);
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
