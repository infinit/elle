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
    this->_opened = true;
    this->_changed(this->_opened);
    this->_signal();
  }

  void
  Barrier::close()
  {
    this->_opened = false;
    this->_changed(this->_opened);
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
