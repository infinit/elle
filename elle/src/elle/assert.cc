#include <elle/assert.hh>
#include <elle/Backtrace.hh>
#include <elle/log.hh>

#include <iostream>
#include <sstream>

ELLE_LOG_COMPONENT("elle.assert");

namespace elle
{
  void
  _assert(bool predicate,
          std::string const& message,
          char const* file,
          int line)
  {
    if (!predicate)
      _abort(message, file, line);
  }

  void
  unreachable()
  {
    _abort("unreachable code reached", "", 0);
  }

  void
  _abort(std::string const& message,
         char const* file,
         int line)
  {
    throw elle::AssertError(message.c_str(), file, line);
  }

  // XXX should fill a backtrace.
  AssertError::AssertError(char const* condition,
                           char const* file,
                           Size line) throw()
  {
    try
      {
        std::stringstream ss;
        ss << "assertion '" << condition << "' failed at "
           << file << ':' << line;
        this->_what = ss.str();
        ELLE_ERR("%s", this->_what)
        {
          for (auto& sf: Backtrace::current())
            ELLE_ERR("%s", sf);
        }

      }
    catch (...)
      {
        ELLE_WARN("could not build the error string for %s at %s:%s",
                  condition,
                  file,
                  line);
      }
  }

  const char*
  AssertError::what() const throw()
  {
    if (_what.size())
      return _what.c_str();
    else
      return "AssertError (could not build the error string)";
  }

}
