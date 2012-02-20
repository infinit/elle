//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        Raphael Londeix   [Fri 17 Feb 2012 04:41:27 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include <cassert>
#include <iostream>

#include <QCryptographicHash>
#include <QDir>
#include <QNetworkRequest>
#include <QUrl>

#include "plasma/common/resources.hh"

#include "ReleaseReader.hh"
#include "ReleaseUpdater.hh"

using namespace plasma::updater;

//
// ---------- contructors & descructors ---------------------------------------
//

ReleaseUpdater::ReleaseUpdater(QApplication& app) :
  _networkManager(&app),
  _hasList(false),
  _releaseReader()
{
  this->connect(
      &this->_networkManager,
      SIGNAL(finished(QNetworkReply*)),
      SLOT(_OnDownloadFinished(QNetworkReply*))
  );
}

ReleaseUpdater::~ReleaseUpdater()
{}

//
// ---------- methods  --------------------------------------------------------
//

void ReleaseUpdater::Start()
{
  std::cout << "Checking out " << INFINIT_RELEASE_URI << std::endl;

  auto reply = this->_networkManager.get(
      QNetworkRequest(QUrl(INFINIT_RELEASE_URI))
  );

  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );
}


void ReleaseUpdater::_OnDownloadFinished(QNetworkReply* reply)
{
  bool success = true;
  bool isDone = false;

  if (reply != nullptr)
    {
      if (reply->error() == QNetworkReply::NoError)
        {
          if (this->_hasList)
            success = this->_ProcessResource(*reply);
          else
            success = this->_ProcessResourceList(*reply);
        }
      else
        {
          std::cerr << "An error occured, ignoring further replies\n";
          success = false;
        }
      reply->deleteLater();
    }

  if (success)
    {
      if (this->_releaseReader.files.size() > 0)
        {
          this->_DownloadNextResource();
        }
      else
        {
          std::cout << "All release files downloaded successfully\n";
          isDone = true;
        }
    }

  if (isDone || !success)
    {
      emit releaseUpdated(success);
    }
}

void ReleaseUpdater::_DownloadNextResource()
{
  assert(this->_releaseReader.files.size() > 0);
  auto& file = this->_releaseReader.files.back();
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
          std::cout << "The binary " << dest_file.fileName().toStdString()
                    << " is up to date !\n";
          this->_releaseReader.files.pop_back();
          emit _OnDownloadFinished(nullptr);
          return;
        }
    }

  std::cout << "Checking out " << uri.toStdString() << std::endl;
  auto reply = this->_networkManager.get(
      QNetworkRequest(QUrl(uri))
  );
  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );
}

void ReleaseUpdater::_OnDownloadError(QNetworkReply::NetworkError error)
{
  std::cout << "Got error code = " << error << std::endl;
  emit releaseUpdated(false);
}

bool ReleaseUpdater::_ProcessResource(QNetworkReply& reply)
{
  assert(this->_hasList);
  assert(this->_releaseReader.files.size() > 0);
  QDir home_directory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  assert(home_directory.exists());
  auto& src_file = this->_releaseReader.files.back();
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
  this->_releaseReader.files.pop_back();
  return true;
}

bool ReleaseUpdater::_ProcessResourceList(QNetworkReply& reply)
{
  assert(!this->_hasList);
  if (!this->_releaseReader.Feed(reply.readAll()))
    {
      std::cerr << "Failed to read release description file\n";
      return false;
    }
  this->_hasList = true;
  return true;
}
