#include <elle/assert.hh>
#include <elle/log.hh>

#include <iostream>
#include <sstream>

ELLE_LOG_COMPONENT("elle.assert.AssertError");

namespace elle
{
  void
  _assert(bool predicate,
          std::string const& message,
          char const* file,
          int line)
  {
    if (!predicate)
      throw elle::AssertError(message.c_str(), file, line);
  }

  void
  unreachable()
  {
    abort("unreachable code reached");
  }

  void
  abort(std::string const& message)
  {
    // FIXME: argv[0]
    std::cerr << "fatal error: " << message << std::endl;
    std::abort();
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
