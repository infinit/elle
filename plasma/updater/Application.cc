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
#include <QCryptographicHash>

#include "plasma/common/resources.hh"

#include "Application.hh"

using namespace plasma::updater;

//
// ---------- constructors & destructors --------------------------------------
//

Application::Application(int ac, char** av) :
  QApplication(ac, av, true),
  _network_access_manager(0),
  _has_list(false),
  _release_reader()
{}

Application::~Application()
{
  delete this->_network_access_manager;
  this->_network_access_manager = 0;
}

//
// ---------- methods ---------------------------------------------------------
//

int Application::Exec()
{
  this->_network_access_manager = new QNetworkAccessManager(this);
  this->connect(
      this->_network_access_manager,
      SIGNAL(finished(QNetworkReply*)),
      SLOT(_OnDownloadFinished(QNetworkReply*))
  );
  std::cout << "Checking out " << INFINIT_RELEASE_URI << std::endl;
  auto reply = this->_network_access_manager->get(
      QNetworkRequest(QUrl(INFINIT_RELEASE_URI))
  );

  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );

  return this->exec();
}

void Application::_OnDownloadFinished(QNetworkReply* reply)
{
  if (reply != nullptr)
    {
      if (reply->error() == QNetworkReply::NoError)
        {
          bool result;

          if (this->_has_list)
            result = this->_ProcessResource(*reply);
          else
            result = this->_ProcessResourceList(*reply);
          if (!result)
            this->quit();
        }
      else
        {
          std::cerr << "An error occured, ignoring further replies\n";
        }
      reply->deleteLater();
    }

  if (this->_release_reader.files.size() > 0)
    this->_DownloadNextResource();
  else
    {
      std::cout << "All release files downloaded successfully\n";
      // end checks here
      this->quit();
    }
}

void Application::_DownloadNextResource()
{
  assert(this->_release_reader.files.size() > 0);
  auto& file = this->_release_reader.files.back();
  QString uri = (INFINIT_BASE_URL "/") + file.relpath;

  QDir home_directory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  QFile dest_file(home_directory.filePath(file.relpath));
  if (dest_file.open(QIODevice::ReadOnly))
    {
      QByteArray fileData = dest_file.readAll();
      auto array = QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex();
      std::string hex(array.data(), array.length());
      if (file.md5sum.toStdString() == hex)
        {
          std::cout << "The binary " << dest_file.fileName().toStdString() << " is up to date !\n";
          this->_release_reader.files.pop_back();
          emit _OnDownloadFinished(nullptr);
          return;
        }
    }

  std::cout << "Checking out " << uri.toStdString() << std::endl;
  auto reply = this->_network_access_manager->get(
      QNetworkRequest(QUrl(uri))
  );
  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );
}

void Application::_OnDownloadError(QNetworkReply::NetworkError error)
{
  std::cout << "Got error code = " << error << std::endl;
  this->exit(1);
}

bool Application::_ProcessResource(QNetworkReply& reply)
{
  assert(this->_has_list);
  assert(this->_release_reader.files.size() > 0);
  QDir home_directory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  assert(home_directory.exists());
  auto& src_file = this->_release_reader.files.back();
  std::cout << "Just downloaded " << src_file.relpath.toStdString() << std::endl;
  QFile dest_file(home_directory.filePath(src_file.relpath));
  if (!dest_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
      std::cerr << "Cannot open file '" << src_file.relpath.toStdString() << "'\n";
      return false;
    }
  dest_file.write(reply.readAll());
  dest_file.close();
  if (src_file.type == ReleaseReader::File::Type::Executable)
    dest_file.setPermissions(QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner);
  else
    dest_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
  this->_release_reader.files.pop_back();
  return true;
}

bool Application::_ProcessResourceList(QNetworkReply& reply)
{
  assert(!this->_has_list);
  if (!this->_release_reader.Feed(reply.readAll()))
    {
      std::cerr << "Failed to read release description file\n";
      return false;
    }
  this->_has_list = true;
  return this->_PrepareInfinitHome();
}

bool Application::_PrepareInfinitHome()
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
