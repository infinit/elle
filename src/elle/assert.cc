#include <elle/assert.hh>

#include <iostream>
#include <sstream>

#include <elle/Backtrace.hh>
#include <elle/log.hh>
#include <elle/os/environ.hh>

ELLE_LOG_COMPONENT("elle.assert");

namespace elle
{
  /*--------------.
  | AssertError.  |
  `--------------*/

  AssertError::AssertError(char const* condition,
                           char const* file, int line) noexcept
    : _bt{std::make_shared<Backtrace>(Backtrace::now)}
  {
    try
    {
      this->_what = elle::sprintf("assertion '%s' failed at %s:%s",
                                  condition, file, line);
      ELLE_ERR("%s", this->_what)
        ELLE_ERR("%s", *this->_bt);
    }
    catch (...)
    {
      ELLE_WARN("could not build the error string for %s at %s:%s",
                condition, file, line);
    }
  }

  const char*
  AssertError::what() const noexcept
  {
    if (_what.empty())
      return "AssertError (could not build the error string)";
    else
      return _what.c_str();
  }

  std::shared_ptr<Backtrace const>
  AssertError::backtrace() const noexcept
  {
    return this->_bt;
  }


  namespace
  {
    auto abort_callbacks = std::vector<AbortCallback>{};
  }

  namespace detail
  {
    void
    on_abort(AbortCallback fun)
    {
      abort_callbacks.emplace_back(std::move(fun));
    }
  }

  void
  unreachable()
  {
    _abort("unreachable code reached", "", 0);
  }

  void
  _abort(std::string const& message, char const* file, int line)
  {
    if (std::uncaught_exception())
    {
      ELLE_WARN(
        "raising an assert (%s at %s:%s) with an exception already in flight",
        message, file, line);
    }
    auto&& error = elle::AssertError(message.c_str(), file, line);
    for (auto&& fun: abort_callbacks)
      fun(error);
    if (elle::os::getenv("ELLE_REAL_ASSERT", false))
    {
      ELLE_ERR("%s: (%s:%s)", message, file, line);
      std::abort();
    }
    else
      throw std::move(error);
  }
}
