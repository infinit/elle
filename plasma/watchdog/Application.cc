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

#include <iostream>

#include "plasma/common/resources.hh"

#include "Application.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Application::Application(int ac, char* av[]) :
  QApplication(ac, av)
{
  std::cout << "ctor\n";
}

Application::~Application()
{
  std::cout << "dtor\n";
}

//
// ---------- methods  --------------------------------------------------------
//

int Application::Exec()
{
  std::cout << "YEAH !\n";
  return this->exec();
  std::cout << "YEAH !(fin)\n";
}

