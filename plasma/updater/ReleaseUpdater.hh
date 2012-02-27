//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        Raphael Londeix   [Fri 17 Feb 2012 04:36:05 PM CET]
//

#ifndef PLASMA_UPDATER_RELEASEUPDATER_HH
#define PLASMA_UPDATER_RELEASEUPDATER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "ReleaseReader.hh"

namespace plasma
{
  namespace updater
  {

//
// ---------- classes ---------------------------------------------------------
//


    ///
    /// Download the release description file, and update
    /// release binaries when needed
    ///
    class ReleaseUpdater :
      public QObject
    {
      Q_OBJECT

    private:
      QNetworkAccessManager   _networkManager;
      bool                    _hasList;
      ReleaseReader           _releaseReader;

    public:
      ReleaseUpdater(QApplication& app);
      ~ReleaseUpdater();

      /// Launch the download (non blocking)
      void Start();

    signals:
      void releaseUpdated(bool);

    private slots:
      void _OnDownloadFinished(QNetworkReply* reply);
      void _OnDownloadError(QNetworkReply::NetworkError error);
    private:
      bool _ProcessResource(QNetworkReply& reply);
      bool _ProcessResourceList(QNetworkReply& reply);
      void _DownloadNextResource();
    };

  }
}



#endif /* ! RELEASEUPDATER_HH */


