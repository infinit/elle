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
#include "MetaClient.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Application::Application(int ac, char* av[]) :
  QApplication(ac, av),
  _meta(nullptr)
{}

Application::~Application()
{
  delete this->_meta;
  this->_meta = nullptr;
}

//
// ---------- methods  --------------------------------------------------------
//

int Application::Exec()
{
  this->_meta = new MetaClient(INFINIT_META_URL);
  return this->exec();
}

