
#include <unistd.h>
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

Application::Application(int& ac, char** av)
  : QApplication(ac, av, true)
  , _releaseUpdater(*this)
  , _identityUpdater(*this)
  , _licenseDialog()
  , _infinitHome(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY))
{}

Application::~Application()
{
}

//
// ---------- methods ---------------------------------------------------------
//

int Application::Exec()
{
  if (!this->_infinitHome.exists())
    {
      this->_licenseDialog.show();
      this->connect(
          &this->_licenseDialog, SIGNAL(accepted()),
          this, SLOT(_StartUpdate())
      );
      this->connect(
          &this->_licenseDialog, SIGNAL(cancelled()),
          this, SLOT(_CancelUpdate())
      );
    }
  else
    {
      std::cerr << "Infinit home already exists !\n";
      this->_StartUpdate();
    }

  return this->exec();
}

void Application::_StartUpdate()
{
  this->_licenseDialog.hide();
  if (!this->_CheckInfinitHome())
    this->exit(EXIT_FAILURE);

  this->_StopWatchdog();

  if (QFile(_infinitHome.filePath(".dev_no_update")).exists())
    {
      std::cerr << "Skipping update !\n";
      this->_OnReleaseUpdated(true);
    }
  else
    {
      this->_releaseUpdater.Start();
      this->connect(
        &this->_releaseUpdater, SIGNAL(releaseUpdated(bool)),
        this, SLOT(_OnReleaseUpdated(bool))
      );
    }
}

void Application::_CancelUpdate()
{
  std::cerr << "Update cancelled\n";
  this->exit(EXIT_FAILURE);
}

void Application::_OnReleaseUpdated(bool success)
{
  if (!success)
    {
      std::cerr << "Release was not updated correctly\n";
      this->exit(EXIT_FAILURE);
    }
  else
    {
      this->connect(
          &this->_identityUpdater, SIGNAL(identityUpdated(bool)),
          this, SLOT(_OnIdentityUpdated(bool))
      );
      this->_identityUpdater.Start();
    }
}

bool Application::_CheckInfinitHome()
{
  if ((!this->_infinitHome.exists() && !this->_infinitHome.mkpath(".")) ||
      (!this->_infinitHome.exists("bin") && !this->_infinitHome.mkpath("bin")) ||
      (!this->_infinitHome.exists("lib") && !this->_infinitHome.mkpath("lib")) ||
      (!this->_infinitHome.exists("networks") && !this->_infinitHome.mkpath("networks")))
    {
      std::cerr << "Failed to prepare infinit home '"
                << this->_infinitHome.path().toStdString()
                << "'\n";
      return false;
    }

  return true;
}

QString Application::_readWatchdogId()
{
  if (this->_infinitHome.exists("infinit.wtg"))
    {
      QFile f(this->_infinitHome.filePath("infinit.wtg"));
      if (f.open(QIODevice::ReadOnly))
        {
          QString watchdogId = QString{f.readAll()};
          f.close();
          return watchdogId;
        }
    }
  return "";
}

void Application::_StopWatchdog()
{
  QString id = this->_readWatchdogId();
  if (!id.size())
    return;

  // Stop old watchdog
    {
      QLocalSocket conn;
      conn.connectToServer(WATCHDOG_SERVER_NAME);
      if (conn.waitForConnected(2000))
        {
          QByteArray cmd = QString("{"
                "\"command\":\"stop\","
                "\"_id\": \"" + id + "\""
          "}\n").toAscii();
          conn.write(cmd);
          if (!conn.waitForBytesWritten(2000))
              throw std::runtime_error("Couldn't stop the old watchdog instance");
        }
      else
        {
          std::cerr << "UPDATER: Warning: Couldn't connect to the old watchdog instance\n";
          return;
        }
    }

  // Waiting for the old server to be stopped
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

  QString watchdogId = this->_readWatchdogId();

  ///////////////////////////////////////////////////////////////////////////
  // We finaly launch the watchdog
    {
      QString watchdogPath = this->_infinitHome.filePath("bin/8watchdog");
      std::cerr << "watchdog path: " << watchdogPath.toStdString() << std::endl;
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
  QString newWatchdogId;
  tries = 0;
    do {
      if (tries > 0) ::sleep(1);
      newWatchdogId = this->_readWatchdogId();
      if (newWatchdogId.size() && watchdogId != newWatchdogId)
        {
          watchdogId = newWatchdogId; // prevent errors
          std::cerr << "UPDATER: Found new watchdog id "<< newWatchdogId.toStdString() << "\n";
          break;
        }
    } while (++tries < 10);

    if (tries == 10)
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
  this->exit(EXIT_SUCCESS);
}

