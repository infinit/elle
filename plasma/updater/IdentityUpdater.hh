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

    namespace meta = ::plasma::metaclient;

    ///
    /// Retreive a token from meta, and update the passport and the identity
    /// file.
    ///
    class IdentityUpdater : public QObject
    {
      Q_OBJECT

    private:

    private:
      meta::MetaClient  _api;
      LoginDialog       _loginDialog;
      std::string       _token;
      std::string       _identity;

    public:
      /// ctor & dtor
      IdentityUpdater(QApplication& app);

      /// properties
      meta::MetaClient&  api()             { return this->_api; }
      std::string const& identity() const  { return this->_identity; }
      std::string const& token() const     { return this->_token; }

      /// methods
      void Start();

    private:
      void _OnLogin(std::string const& password,
                    meta::LoginResponse const& response);
      void _OnError(meta::MetaClient::Error error,
                    std::string const& error_string);
      void _OnDeviceCreated(meta::CreateDeviceResponse const& res);
      void _OnDeviceUpdated(meta::UpdateDeviceResponse const& res);
      void _UpdatePassport();
      std::string _DecryptIdentity(std::string const& password,
                                   std::string const& identity);
      void _StoreIdentity(std::string const& identityString);
      void _CreateDevice();
      void _UpdateDevice();

    private Q_SLOTS:
      void _DoLogin(std::string const& login, std::string const& password);

    Q_SIGNALS:
      void identityUpdated(bool);
    };

  }
}

#endif /* ! IDENTITYUPDATER_HH */


