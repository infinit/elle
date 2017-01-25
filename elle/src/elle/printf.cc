#include <elle/printf-fwd.hh>

#include <sstream>

#include <elle/Exception.hh>
#include <elle/log.hh>

namespace elle
{
  void
  format_error(std::string const& fmt, boost::io::format_error const& e)
  {
    ELLE_LOG_COMPONENT("elle.printf");
    // Don't use elle::printf/sprintf to handle elle::printf/sprintf
    // fatal errors.
    std::stringstream ss;
    ss << "format error with \"" << fmt << "\": " << e.what();
    ELLE_ERR("%s", ss.str());
    // We throw an elle::Exception rather than an elle::Error as it
    // allows to tell the difference btw an elle::err that succeeded
    // (raised an elle::Error) and one that failed (elle::Exception).
    throw elle::Exception(ss.str());
  }
}
