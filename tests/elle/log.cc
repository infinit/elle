
#include <elle/log.hh>

ELLE_LOG_COMPONENT("test_log");

void g()
{
  ELLE_LOG_TRACE("Me too!");
}

void f()
{
  ELLE_LOG_TRACE("This is usefull!")
  {
    ELLE_LOG_TRACE("This is usefull inner");
    g();
  }
}

int main()
{
  ELLE_LOG_COMPONENT("test_log.biet");
  elle::log::debug("BIET");
  elle::log::info("BIET");
  elle::log::warn("BIET");
  elle::log::error("BIET");
  elle::log::fatal("BIET");
  elle::log::info("Salut", "les", 12, "copains");
  elle::log::default_logger.name("LOGGER_NAME_IS_STUPID");
  elle::log::info("tests done.");
  f();
  return 0;
}
