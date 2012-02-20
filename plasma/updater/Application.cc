//
// ---------- header ----------------------------------------------------------
//
// project       plasma/updater
//
// license       infinit
//
// author        Raphael Londeix   [Mon 30 Jan 2012 12:41:48 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <cassert>
#include <iostream>

#include <QDir>

#include "plasma/common/resources.hh"

#include "Application.hh"

using namespace plasma::updater;

//
// ---------- constructors & destructors --------------------------------------
//

Application::Application(int ac, char** av) :
  QApplication(ac, av, true),
  _releaseUpdater(*this),
  _identityUpdater(*this)
{}

Application::~Application()
{
}

//
// ---------- methods ---------------------------------------------------------
//

int Application::Exec()
{
  this->_releaseUpdater.Start();
  this->connect(
    &this->_releaseUpdater, SIGNAL(releaseUpdated(bool)),
    this, SLOT(_OnReleaseUpdated(bool))
  );

  return this->exec();
}

void Application::_OnReleaseUpdated(bool)
{
  if (!this->_CheckInfinitHome())
    return;
  this->_identityUpdater.Start();
}

bool Application::_CheckInfinitHome()
{
  QDir home_directory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  if ((!home_directory.exists() && !home_directory.mkpath(".")) ||
      (!home_directory.exists("binaries") && !home_directory.mkpath("binaries")) ||
      (!home_directory.exists("networks") && !home_directory.mkpath("networks")))
    {
      std::cerr << "Failed to prepare infinit home '"
                << home_directory.path().toStdString()
                << "'\n";
      return false;
    }

  return true;
}
