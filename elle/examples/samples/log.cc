#include <elle/log.hh>
#include <elle/Buffer.hh>

// Declare a log component for the file.
ELLE_LOG_COMPONENT("component1");

int
main()
{
  // Report if the log level is set to at least LOG (default).
  ELLE_LOG("Something super relevant with two parameters (%s) and (%s)",
           42, elle::Buffer("buffer"));
  // Report if the log level is set to at least TRACE.
  ELLE_TRACE("Something relevant");
  // Report if the log level is set to at least DEBUG.
  ELLE_DEBUG("Something less relevant");
  // Report if the log level is set to at least DUMP.
  ELLE_DUMP("Something only relevant for nerdz");
  // Logs in the following block will be indented.
  ELLE_DEBUG("The log in the following scope will be indented")
  {
    ELLE_DEBUG("This log will be prefixed by two white spaces");
  }
  // Create a block with another log component.
  {
    // Nested log component, replacing "component1".
    ELLE_LOG_COMPONENT("component2");
    // Report errors (ELLE_LOG but in red).
    ELLE_ERR("Something extremely wrong happened");
    // Report warning (ELLE_LOG but in yellow).
    ELLE_WARN("Something wrong happened");
  }
  return 0;
}
