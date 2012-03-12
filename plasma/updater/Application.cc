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
#include <stdexcept>

#include <QDir>
#include <QLocalSocket>
#include <QFile>

#include "plasma/common/resources.hh"

#include "Application.hh"

using namespace plasma::updater;

//
// ---------- constructors & destructors --------------------------------------
//

Application::Application(int& ac, char** av) :
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
  if (!this->_CheckInfinitHome())
    return EXIT_FAILURE;

  this->_releaseUpdater.Start();
  this->connect(
    &this->_releaseUpdater, SIGNAL(releaseUpdated(bool)),
    this, SLOT(_OnReleaseUpdated(bool))
  );

  return this->exec();
}

void Application::_OnReleaseUpdated(bool)
{
  this->_identityUpdater.Start();
  this->connect(
    &this->_identityUpdater,
    SIGNAL(identityUpdated(bool)),
    this,
    SLOT(_OnIdentityUpdated(bool))
  );
}

bool Application::_CheckInfinitHome()
{
  QDir home_directory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  if ((!home_directory.exists() && !home_directory.mkpath(".")) ||
      (!home_directory.exists("bin") && !home_directory.mkpath("bin")) ||
      (!home_directory.exists("lib") && !home_directory.mkpath("lib")) ||
      (!home_directory.exists("networks") && !home_directory.mkpath("networks")))
    {
      std::cerr << "Failed to prepare infinit home '"
                << home_directory.path().toStdString()
                << "'\n";
      return false;
    }

  return true;
}


/// XXX The following is ugly, do not read
void Application::_OnIdentityUpdated(bool success)
{
  if (!success)
    {
      std::cerr << "Something failed ...\n";
      this->exit(EXIT_FAILURE);
      return;
    }
  else
    std::cerr << "Starting the watchdog.\n";
  QDir homeDirectory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  QString watchdogId;

  ///////////////////////////////////////////////////////////////////////////
  // Read old watchdog id
  if (homeDirectory.exists("infinit.wtg"))
    {
      QFile f(homeDirectory.filePath("infinit.wtg"));
      if (f.open(QIODevice::ReadOnly))
        {
          watchdogId = QString{f.readAll()};
          f.close();
        }
    }

  ///////////////////////////////////////////////////////////////////////////
  // Trying to stop the watchdog
  if (watchdogId.size() > 0)
    {
      QLocalSocket conn;
      conn.connectToServer(WATCHDOG_SERVER_NAME);
      if (conn.waitForConnected(2000))
        {
          QString token = this->_identityUpdater.api().token();
          QByteArray cmd = QString("{"
                "\"command\":\"stop\","
                "\"_id\": \"" + watchdogId + "\""
          "}\n").toAscii();
          conn.write(cmd);
          if (!conn.waitForBytesWritten(2000))
              throw std::runtime_error("Couldn't stop the old watchdog instance");
        }
      else
        {
          std::cerr << "UPDATER: Warning: Couldn't connect to the old watchdog instance\n";
          watchdogId = "";
        }
    }

  ///////////////////////////////////////////////////////////////////////////
  // Waiting for the old server to be stopped
  if (watchdogId.size() > 0)
    {
      int tries = 1;
      do {
          QLocalSocket conn;
          conn.connectToServer(WATCHDOG_SERVER_NAME);
          if (!conn.waitForConnected(2000))
            break;
          std::cerr << "UPDATER: Waiting for the old watchdog to be stopped ("
                    << tries << ")\n";
          ::sleep(1);
      } while (++tries < 10);

      if (tries >= 10)
          std::cerr << "UPDATER: Warning: The old watchdog instance does not stop !\n";
    }

  ///////////////////////////////////////////////////////////////////////////
  // We finaly launch the watchdog
    {
      QString watchdogPath = homeDirectory.filePath("binaries/8watchdog");
      QProcess p;
      if (!p.startDetached(watchdogPath))
        throw std::runtime_error("Cannot start the watchdog !");
      std::cerr << "UPDATER: Process started\n";
      // XXX Cannot do that
      //if(!p.waitForStarted(2000))
      //  {
      //    std::cerr << "Couln't wait\n";
      //    throw std::runtime_error("Couldn't start the watchdog");
      //  }
      //std::cerr << "process ready\n";
    }

  QLocalSocket conn;

  ///////////////////////////////////////////////////////////////////////////
  // Connect to the new watchdog instance
  int tries = 0;
  while (tries++ < 5)
    {
      conn.connectToServer(WATCHDOG_SERVER_NAME);
      std::cerr << "UPDATER: Trying to connect to the watchdog\n";
      if (conn.waitForConnected(2000))
        break;
      std::cerr << "UPDATER: Retrying to connect (" << tries << ")\n";
      ::sleep(1);
    }
  if (!conn.isValid())
    throw std::runtime_error("Couldn't connect to the new watchdog instance");


  std::cerr << "UPDATER: Connected to the watchdog\n";

  ///////////////////////////////////////////////////////////////////////////
  // Getting the new watchdog id
  // When connected, the watchdog id file should exists
  QFile f(homeDirectory.filePath("infinit.wtg"));
  QString newWatchdogId;
  tries = 0;
    do {
      if(tries > 0) ::sleep(1);
      if (f.open(QIODevice::ReadOnly))
        {
          newWatchdogId = QString{f.readAll()};
          f.close();
          std::cerr << "UPDATER: Found new watchdog id "<< newWatchdogId.toStdString() << "\n";
        }
    } while (newWatchdogId == watchdogId && ++tries < 10);

    if (newWatchdogId == watchdogId)
      throw std::runtime_error("Couldn't open infinit watchdog id file");

  ///////////////////////////////////////////////////////////////////////////
  // calling watchdog run command (which gives the meta token)
  QString token(this->_identityUpdater.token().c_str());
  QString identity(this->_identityUpdater.identity().c_str());
  QByteArray cmd = QString("{"
      "\"command\":"  "\"run\""                       ","
      "\"_id\":"      "\"" + newWatchdogId + "\""     ","
      "\"token\":"    "\"" + token + "\""             ","
      "\"identity\":" "\"" + identity + "\""
  "}\n").toAscii();
  conn.write(cmd);
  if (!conn.waitForBytesWritten(2000))
    throw std::runtime_error("Couldn't run the watchdog");

  std::cerr << "UPDATER: Run command sent\n";
  //this->exit(EXIT_SUCCESS);
}

