
#include <cstdio>
#include <iostream>
//#include <thread>

#include "Application.hh"
//#include "Agent.hh"
#include "WatchdogInterface.hh"

#define BUF_SIZE 4096

using namespace plasma::watchdog;

static void _initAll();

int     main(int ac, char* av[])
{
  //typedef std::unique_ptr<WatchdogInterface> WatchdogInterfacePtr;

  plasma::watchdog::Application app(ac, av);
  //auto agent = Agent(
  //    WatchdogInterfacePtr(new WatchdogInterface(app))
  //);
  try
    {
      _initAll();
      //std::thread agentThread(std::bind(&Agent::Run, &agent));
      auto res = app.Exec();
      //agentThread.join();
      return res;
    }
  catch (std::exception const& err)
    {
      std::cout << "An exception occured: " << err.what() << std::endl;
    }
  catch (...)
    {
      std::cout << "Uncatched exception\n";
    }
  return EXIT_FAILURE;
}


#include "elle/Elle.hh"
#include "lune/Lune.hh"
#include "nucleus/Nucleus.hh"

static void _initAll()
{
  if (elle::Elle::Initialize() == elle::StatusError ||
      lune::Lune::Initialize() == elle::StatusError ||
      nucleus::Nucleus::Initialize() == elle::StatusError)
    throw std::runtime_error("Couldn't initialize !");
}
