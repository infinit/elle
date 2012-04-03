#include <reactor/signal.hh>

namespace reactor
{
  Signal::Signal(const std::string& name)
    : Super(name)
  {}

  void
  Signal::signal()
  {
    _signal();
  }
};
