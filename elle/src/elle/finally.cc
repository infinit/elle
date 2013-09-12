#include <elle/finally.hh>
#include <elle/log.hh>

#include <stdexcept>

namespace elle
{
  ELLE_LOG_COMPONENT("elle.Finally");

  SafeFinally::SafeFinally():
    _action([] {})
  {}

  SafeFinally::SafeFinally(std::function<void()> const& action):
    _action(action)
  {}

  SafeFinally::~SafeFinally()
  {
    if (!this->_action)
      return;
    try
    {
      this->_action();
    }
    catch (...)
    {
      ELLE_ABORT("safe finally threw: %s", elle::exception_string());
    }
  }

  void
  SafeFinally::abort()
  {
    this->_action = std::function<void()>();
  }

  Finally::Finally():
    _action([] {})
  {}

  Finally::Finally(std::function<void()> const& action):
    _action(action)
  {}

  Finally::~Finally() noexcept(false)
  {
    if (!this->_action)
      return;
    this->_action();
  }

  void
  Finally::abort()
  {
    this->_action = std::function<void()>();
  }
}
