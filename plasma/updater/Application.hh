#ifndef  PLASMA_UPDATER_APPLICATION_HH
# define PLASMA_UPDATER_APPLICATION_HH

# include <string>

# include <QApplication>
# include <QDir>
# include <QProcess>

# include "IdentityUpdater.hh"
# include "ReleaseUpdater.hh"
# include "LicenseDialog.hh"

namespace plasma {
  namespace updater {

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
      ReleaseUpdater  _releaseUpdater;
      IdentityUpdater _identityUpdater;
      LicenseDialog   _licenseDialog;
      QDir            _infinitHome;

    public:
      Application(int& ac, char** av);
      virtual ~Application();

      int Exec();

    private:
      bool      _CheckInfinitHome();
      void      _StopWatchdog();
      QString   _readWatchdogId();

    private Q_SLOTS:
      void _StartUpdate();
      void _CancelUpdate();
      void _OnReleaseUpdated(bool);
      void _OnIdentityUpdated(bool success);
    };

  }
} //!plasma::updater

#endif /* ! DOWNLOADER_HH */

