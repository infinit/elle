
#include <cstdlib>

#include <QDir>

#include "Application.hh"

int     main(int ac, char* av[])
{
  plasma::updater::Application app(ac, av);
  return app.Exec();
}
