#include <elle/log.hh>
#include <elle/Buffer.hh>

ELLE_LOG_COMPONENT("component1");

int
main()
{
  // Report if the log level is set to at least LOG (default).
  ELLE_LOG("Something super relevant with two parameters (%s) and this one (%s)", 42, elle::Buffer("buffer"));
  // Report if the log level is set to at least TRACE.
  ELLE_TRACE("Something relevant");
  // Report if the log level is set to at least DEBUG.
  ELLE_DEBUG("Something less relevant");
  // Report if the log level is set to at least DUMP.
  ELLE_DUMP("Something only relevant for nerdz");
  // XXX.
  ELLE_DEBUG("The log in the following scope will have indented")
  {
    ELLE_DEBUG("< whites spaces");
  }
  // XXX.
  {
    // Nested log component, replacing "component1".
    ELLE_LOG_COMPONENT("component2");
    ELLE_ERR("Something extremely wrong");
    ELLE_WARN("Something wrong");
  }
  return 0;
}
