#include <elle/finally.hh>
#include <elle/log.hh>

#include <stdexcept>

namespace elle
{
  ELLE_LOG_COMPONENT("elle.Finally");

  Finally::Finally(std::function<void()> const& action):
    _action(action)
  {}

  Finally::~Finally() noexcept(false)
  {
    if (!this->_action)
      return;
    try
    {
      this->_action();
    }
    catch (...)
    {
      ELLE_ERR("finally failed: %s", elle::exception_string());
      throw;
    }
  }

  void
  Finally::abort()
  {
    this->_action = std::function<void()>();
  }
}
