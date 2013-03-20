#include <reactor/signal.hh>

namespace reactor
{
  Signal::Signal(const std::string& name)
    : Super(name)
  {}

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
};
