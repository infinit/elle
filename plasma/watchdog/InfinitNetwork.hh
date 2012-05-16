//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Sun 04 Mar 2012 09:08:04 AM CET]
//

#ifndef INFINITNETWORK_HH
# define INFINITNETWORK_HH

//
// ---------- includes --------------------------------------------------------
//

# include <string>

# include <QDir>
# include <QProcess>

# include "plasma/metaclient/MetaClient.hh"

namespace plasma
{
  namespace watchdog
  {

      class Manager;
      namespace meta = ::plasma::metaclient;
//
// ---------- classes ---------------------------------------------------------
//

    class InfinitNetwork : QObject
    {
      Q_OBJECT

    private:
      meta::NetworkResponse   _description;
      Manager&                _manager;
      QProcess                _process;
      QDir                    _infinitHome;
      QDir                    _home;

    public:
      InfinitNetwork(Manager& manager, meta::NetworkResponse const& response);
      ~InfinitNetwork();
      void Update(meta::NetworkResponse const& response);

    private:
      void _PrepareDirectory();
      void _Update();
      void _CreateNetworkRootBlock();
      void _OnGotDescriptor(meta::UpdateNetworkResponse const& response);
      void _OnAnyError(meta::MetaClient::Error error,
                       std::string const& reason);
      void _RegisterDevice();
      void _OnDeviceRegistered(meta::UpdateNetworkResponse const& response);
      void _OnNetworkNodes(meta::NetworkNodesResponse const& response);
      void _StartProcess();

    private Q_SLOTS:
      void _OnProcessStarted();
      void _OnProcessError(QProcess::ProcessError error);
      void _OnProcessFinished(int exit_code, QProcess::ExitStatus exit_status);
    };

  }
}


//
// ---------- templates -------------------------------------------------------
//

#endif /* ! INFINITNETWORK_HH */


