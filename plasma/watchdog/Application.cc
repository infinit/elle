//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 01 Feb 2012 07:06:01 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "plasma/common/resources.hh"

#include "Application.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Application::Application(int ac, char* av[]) :
  QApplication(ac, av)
{}

Application::~Application()
{
}

//
// ---------- methods  --------------------------------------------------------
//

int Application::Exec()
{
  return this->exec();
}

