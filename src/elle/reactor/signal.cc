#include <elle/reactor/signal.hh>

namespace elle
{
  namespace reactor
  {
    Signal::Signal(const std::string& name)
      : Super(name)
    {}

    Signal::~Signal()
    = default;

    bool
    Signal::signal()
    {
      return _signal();
    }

    bool
    Signal::signal_one()
    {
      return _signal_one();
    }

    /*----------.
      | Printable |
      `----------*/
    void
    Signal::print(std::ostream& stream) const
    {
      stream << "signal ";
      if (!this->name().empty())
        stream << this->name();
      else
        stream << this;
    }
  }
}
