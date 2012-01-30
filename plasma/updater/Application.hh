//
// ---------- header ----------------------------------------------------------
//
// project       plasma/updater
//
// license       infinit
//
// author        Raphaël Londeix   [Fri 27 Jan 2012 11:54:35 AM CET]
//

#ifndef PLASMA_UPDATER_APPLICATION_HH
#define PLASMA_UPDATER_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <string>
#include <queue>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QApplication>

#include "ReleaseReader.hh"

namespace plasma {
  namespace updater {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// This class is in charge to download a file list.
    /// The file list uri is given to the ctor and downloaded. The downloader
    /// compares each file with its md5.
    /// XXX: add RSA signature instead of md5 hash ?
    ///
    class Application : public QApplication
    {
      Q_OBJECT

    private:
      struct File
      {
        std::string filename;
        std::string directory;
      };

    private:
      std::queue<File>        _to_download;
      QNetworkAccessManager*  _network_access_manager;
      bool                    _has_list;
      ReleaseReader           _release_reader;

    public:
      Application(int ac, char** av);
      virtual ~Application();
      int Exec();
    private slots:
      void _OnDownloadFinished(QNetworkReply* reply);
      void _OnDownloadError(QNetworkReply::NetworkError error);
    private:
      bool _ProcessResource(QNetworkReply& reply);
      bool _ProcessResourceList(QNetworkReply& reply);
    };

  }
} //!plasma::updater

#endif /* ! DOWNLOADER_HH */

