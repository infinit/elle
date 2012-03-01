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

#include <cstdlib>
#include <cassert>
#include <ctime>
#include <iostream>

#include <QDir>
#include <QFile>
#include <QLocalSocket>

#include "plasma/common/resources.hh"

#include "LocalServer.hh"
#include "Application.hh"

using namespace plasma::watchdog;

//
// ---------- contructors & descructors ---------------------------------------
//

Application::Application(int ac, char* av[]) :
  QApplication(ac, av)
{
  this->_server = new LocalServer(*this);
}

Application::~Application()
{
  delete this->_server;
  this->_server = nullptr;
}

//
// ---------- methods  --------------------------------------------------------
//


namespace
{

  int initRand() { ::srand(::time(nullptr)+42); return 42; }
  int dummy = initRand();

  /// Returns string of size `len` filled randomly with chars contained in
  /// the string `in'.
  std::string randString(std::string const& in, size_t len)
  {
    assert(in.size() > 0);
    assert(len > 0);

    std::string res;
    res.resize(len);
    size_t in_size = in.size();
    for (size_t i = 0; i < len; ++i)
      res[i] = in[static_cast<size_t>(::rand()) % in_size];
    return res;
  }

  static std::string ASCII{
      "qwertyuiopasdfghjklzxcvbnnm0123456789QWERTYUIOPASDFGHJKJKLZXCVVBNM"
  };

} // !ns anonymous

int Application::Exec()
{
  QDir homeDirectory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));

  std::string watchdogId;

  // Reading any existing watchdog id
  if (homeDirectory.exists("infinit.wtg"))
    {
      QFile f(homeDirectory.filePath("infinit.wtg"));
      if (f.open(QIODevice::ReadOnly))
        {
          watchdogId = QString{f.readAll()}.toStdString();
        }
      f.close();
      if (!f.remove())
        std::cerr << "Warning: Cannot remove file '"
                  << f.fileName().toStdString()
                  << "'\n";
    }

  // Stopping already running watchdog
  if (watchdogId.size())
    {
      QLocalSocket conn;
      conn.connectToServer(WATCHDOG_SERVER_NAME);
      if (conn.isValid() && conn.waitForConnected(2000))
        {
          std::string cmd = "{\"command\":\"STOP\",\"id\": \"" + watchdogId + "\"}\n";
          conn.write(cmd.c_str());
          if (!conn.waitForBytesWritten(2000))
            std::cerr << "Warning: Cannot stop the watchdog...\n";
        }
      else
        std::cerr << "The watchdog " WATCHDOG_SERVER_NAME " is not connected anymore.\n";
    }

  // Generate new watchdog id
  watchdogId = randString(ASCII, 96);
  std::cout << "New watchdog id: " << watchdogId << std::endl;

  // Saving watchdog id
  {
    QFile f(homeDirectory.filePath("infinit.wtg"));
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
      {
        std::cerr << "Cannot open file '"
                  << f.fileName().toStdString()
                  << "'\n";
        return EXIT_FAILURE;
      }
    f.write(watchdogId.c_str());
    f.close();
    f.setPermissions(
        QFile::ReadOwner | QFile::WriteOwner
    );
  }


  this->_server->Start(watchdogId);
  return this->exec();
}

