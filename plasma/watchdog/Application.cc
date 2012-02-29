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
  this->_server = new QLocalServer(this);
}

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

