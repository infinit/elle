//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        Raphael Londeix   [Fri 17 Feb 2012 05:43:55 PM CET]
//

#ifndef IDENTITYUPDATER_HH
#define IDENTITYUPDATER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QObject>

#include "plasma/metaclient/MetaClient.hh"

#include "LoginDialog.hh"

namespace plasma
{
  namespace updater
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// Retreive a token from meta, and update the passport and the identity
    /// file.
    ///
    class IdentityUpdater : public QObject
    {
      Q_OBJECT

    private:
      plasma::metaclient::MetaClient  _api;
      LoginDialog                     _loginDialog;

    public:
      IdentityUpdater(QApplication& app);
      void Start();
    private:
      void _OnLogin(plasma::metaclient::LoginResponse const& response);
      void _OnError(plasma::metaclient::MetaClient::Error error);
    private slots:
      void _DoLogin();

    signals:
      void identityUpdated(bool);
    };

  }
}

#endif /* ! IDENTITYUPDATER_HH */


