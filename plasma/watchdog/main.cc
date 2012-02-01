
#include "Application.hh"

int     main(int ac, char* av[])
{
  plasma::watchdog::Application app(ac, av);
  return app.Exec();
}
