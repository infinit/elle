#include "finally.hh"

#include <elle/log.hh>

#include <stdexcept>

namespace elle
{
  ELLE_LOG_COMPONENT("elle.Finally");

  Finally::Finally(std::function<void()> const& action):
    _action(action)
  {}

  Finally::~Finally()
  {
    if (!this->_action)
      return;
    try
    {
      this->_action();
    }
    catch (std::exception const& err)
    {
      ELLE_ERR("Cleaning scope failed: %s", err.what());
    }
    catch (...)
    {
      ELLE_ERR("Cleaning scope failed: Unknown error type");
    }
  }

  void
  Finally::abort()
  {
    this->_action = std::function<void()>();
  }

}
