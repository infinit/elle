
#include <elle/log.hh>

int main()
{
  elle::log::debug("BIET");
  elle::log::info("BIET");
  elle::log::warn("BIET");
  elle::log::error("BIET");
  elle::log::fatal("BIET");
  elle::log::info("Salut", "les", 12, "copains");
  elle::log::default_logger.name("LOGGER_NAME_IS_STUPID");
  elle::log::info("tests done.");
  return 0;
}
