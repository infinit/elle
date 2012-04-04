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
# define PLASMA_UPDATER_RELEASEUPDATER_HH

//
// ---------- includes --------------------------------------------------------
//

# include <QApplication>
# include <QDir>
# include <QNetworkAccessManager>
# include <QNetworkReply>

# include "ReleaseReader.hh"
# include "UpdateDialog.hh"

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
      typedef std::vector<ReleaseReader::File> FileList;
    private:
      QNetworkAccessManager   _networkManager;
      UpdateDialog            _updateDialog;
      bool                    _hasList;
      FileList                _toUpdate;
      QDir                    _infinitHome;
      size_t                  _totalDownloadSize;
      size_t                  _downloadedSize;

    public:
      ReleaseUpdater(QApplication& app);
      ~ReleaseUpdater();

      /// Launch the download (non blocking)
      void Start();

    Q_SIGNALS:
      void releaseUpdated(bool);

    private Q_SLOTS:
      void _OnDownloadFinished(QNetworkReply* reply);
      void _OnDownloadError(QNetworkReply::NetworkError error);
      void _OnDownloadProgress(qint64 read, qint64 total);
      void _OnCancelled();

    private:
      bool _ProcessResource(QNetworkReply& reply);
      bool _ProcessResourceList(QNetworkReply& reply);
      void _DownloadNextResource();
    };

  }
}



#endif /* ! RELEASEUPDATER_HH */


