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
      ELLE_TRACE("%s: run action", *this)
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
    ELLE_TRACE("%s: abort", *this);
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
    ELLE_TRACE("%s: run action", *this)
      this->_action();
  }

  void
  Finally::abort()
  {
    ELLE_TRACE("%s: abort", *this);
    this->_action = std::function<void()>();
  }
}
