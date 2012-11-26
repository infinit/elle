#include <cstdlib>
#include <cassert>
#include <ctime>

#include <QDir>
#include <QFile>
#include <QLocalSocket>

#include <elle/log.hh>

#include <common/common.hh>

#include "LocalServer.hh"
#include "Application.hh"

ELLE_LOG_COMPONENT("infinit.plasma.watchdog");

using namespace plasma::watchdog;

static int dummy = 0;

Application::Application(std::string const& user_id) :
  QCoreApplication(dummy, (char**)nullptr),
  _server{new LocalServer(*this, user_id)},
  _user_id{user_id}
{}

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
    (void) dummy;

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

int Application::exec()
{
  // Generate new watchdog id
  std::string watchdogId = randString(ASCII, 42);
  ELLE_DEBUG("New watchdog id: %s", watchdogId);

  // Saving watchdog id
  {
    QFile f(common::watchdog::id_path(this->_user_id).c_str());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
      {
        ELLE_ERR("Cannot open file: %s", f.fileName().toStdString());
        return EXIT_FAILURE;
      }
    f.write(watchdogId.c_str());
    f.flush();
    f.close();
    f.setPermissions(
        QFile::ReadOwner | QFile::WriteOwner
    );
  }


  this->_server->start(watchdogId);
  return this->QCoreApplication::exec();
}

bool
Application::notify(QObject* rec,
                    QEvent* ev)
{
  try
    {
      return QCoreApplication::notify(rec,ev);
    }
  catch(std::exception const& e)
    {
      ELLE_ERR("Handler exception caught: %s", e.what());
    }
  catch(...)
    {
      ELLE_ERR("Handler exception caught an unknown exception.");
    }
  return false;
}
