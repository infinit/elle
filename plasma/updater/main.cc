
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <QDir>


#include "Application.hh"

static void _initAll();

int     main(int ac, char* av[])
{
  try
    {
      _initAll();
      plasma::updater::Application app(ac, av);
      return app.Exec();
    }
  catch (std::exception const& err)
    {
      std::cerr << "A fatal error occured: " << err.what() << ".\n";
    }
  catch (...)
    {
      std::cerr << "An unknown error was thrown.\n";
    }
  return EXIT_FAILURE;
}

#include "elle/Elle.hh"
#include "lune/Lune.hh"

static void _initAll()
{
  if (elle::Elle::Initialize() == elle::StatusError ||
      lune::Lune::Initialize() == elle::StatusError)
    throw std::runtime_error("Couldn't initialize !");
}
