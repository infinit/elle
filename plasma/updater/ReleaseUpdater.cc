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
  _updateDialog(),
  _hasList(false),
  _toUpdate(),
  _infinitHome(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY)),
  _totalDownloadSize(0),
  _downloadedSize(0)
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

extern bool g_dirty_hack;

void ReleaseUpdater::Start()
{
  std::cout << "Checking out " << INFINIT_RELEASE_URI << std::endl;
  assert(_infinitHome.exists());
#ifndef PLASMA_UPDATER_NO_GUI
  this->_updateDialog.show();
#endif

  auto reply = this->_networkManager.get(
      QNetworkRequest(QUrl(INFINIT_RELEASE_URI))
  );

  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );

  this->connect(
      &this->_updateDialog, SIGNAL(cancelled()),
      this, SLOT(_OnCancelled())
  );
}

void ReleaseUpdater::_OnCancelled()
{
  std::cerr << "Cancelled !\n";
  Q_EMIT releaseUpdated(false);
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
      if (this->_toUpdate.size() > 0)
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
      this->_updateDialog.hide();
      Q_EMIT releaseUpdated(success);
    }
}

void ReleaseUpdater::_DownloadNextResource()
{
  assert(this->_toUpdate.size() > 0);
  auto& file = this->_toUpdate.back();
  QString uri = (INFINIT_BASE_URL "/") + file.relpath;


  std::cout << "Checking out " << uri.toStdString() << std::endl;
  this->_updateDialog.setStatus("Downloading " + file.relpath.toStdString() + " ...");
  auto reply = this->_networkManager.get(
      QNetworkRequest(QUrl(uri))
  );
  this->connect(
      reply, SIGNAL(error(QNetworkReply::NetworkError)),
      this, SLOT(_OnDownloadError(QNetworkReply::NetworkError))
  );
  this->connect(
      reply, SIGNAL(downloadProgress(qint64,qint64)),
      this, SLOT(_OnDownloadProgress(qint64, qint64))
  );
}

void ReleaseUpdater::_OnDownloadError(QNetworkReply::NetworkError error)
{
  std::cout << "Got error code = " << error << std::endl;
  //Q_EMIT releaseUpdated(false);
}

void ReleaseUpdater::_OnDownloadProgress(qint64 read, qint64 total)
{
  if (read <= 0 || total <= 0)
    {
      this->_updateDialog.setProgress(0);
      return;
    }
  assert(this->_totalDownloadSize != 0);
  this->_updateDialog.setProgress(
      float(this->_downloadedSize + read) * 100.0f / float(this->_totalDownloadSize)
  );
}

bool ReleaseUpdater::_ProcessResource(QNetworkReply& reply)
{
  assert(this->_hasList);
  assert(this->_toUpdate.size() > 0);
  QDir home_directory(QDir(QDir::homePath()).filePath(INFINIT_HOME_DIRECTORY));
  assert(home_directory.exists());
  auto& src_file = this->_toUpdate.back();
  std::cout << "Just downloaded " << src_file.relpath.toStdString() << std::endl;
  QFile dest_file(home_directory.filePath(src_file.relpath));

  QString dest_dir = QFileInfo(dest_file).dir().path() ;
  std::cout << "Create directory : '" << dest_dir.toStdString() << "'\n";
  if (!QDir().mkpath(dest_dir))
    {
      std::cerr << "Cannot create directory : '" << dest_dir.toStdString() << "'\n";
      return false;
    }
  if (!dest_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
      std::cerr << "Cannot open file : '" << dest_file.fileName().toStdString() << "'\n";
      return false;
    }
  dest_file.write(reply.readAll());
  dest_file.close();
  if (src_file.type == ReleaseReader::File::Type::Executable)
    dest_file.setPermissions(QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner);
  else
    dest_file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
  this->_downloadedSize += src_file.size;
  this->_toUpdate.pop_back();
  return true;
}

bool ReleaseUpdater::_ProcessResourceList(QNetworkReply& reply)
{
  assert(!this->_hasList);
  assert(this->_totalDownloadSize == 0);
  ReleaseReader releaseReader;

  if (!releaseReader.Feed(reply.readAll()))
    {
      std::cerr << "Failed to read release description file\n";
      return false;
    }
  this->_hasList = true;

  auto it = releaseReader.files.begin(),
       end = releaseReader.files.end();
  for (; it != end; ++it)
    {
      QFile dest_file(this->_infinitHome.filePath(it->relpath));
      if (dest_file.open(QIODevice::ReadOnly))
        {
          QByteArray fileData = dest_file.readAll();
          auto array = QCryptographicHash::hash(
              fileData, QCryptographicHash::Md5
          ).toHex();
          std::string hex(array.data(), array.length());
          if (it->md5sum.toStdString() == hex)
            {
              std::cout << "The binary " << dest_file.fileName().toStdString()
                        << " is up to date !\n";
              continue;
            }
        }
      this->_toUpdate.push_back(*it);
      _totalDownloadSize += it->size;
    }

  return true;
}
