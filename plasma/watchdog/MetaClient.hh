//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphael Londeix   [Wed 01 Feb 2012 07:12:06 PM CET]
//

#ifndef PLASMA_WATCHDOG_METACLIENT_HH
#define PLASMA_WATCHDOG_METACLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <string>
#include <functional>
#include <memory>
#include <list>

#include <boost/noncopyable.hpp>

#include <QObject>
#include <QNetworkAccessManager>

#include "Network.hh"

namespace plasma
{
  namespace watchdog
  {

    struct Network;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// This class communicate with the meta web server
    /// through a full REST api
    ///
    class MetaClient :
      public QObject,
      private boost::noncopyable
    {
      Q_OBJECT

    public:
      typedef std::unique_ptr<Network> NetworkPtr;
      typedef std::function<void(std::list<NetworkPtr>)> NetworksCallback;
    private:
      std::string             _server_address;
      QNetworkAccessManager  _network_access_manager;

    public:
      MetaClient(std::string const& server_address);
      ~MetaClient();

      void GetNetworks(NetworksCallback callback);
    };

  }
}

#endif /* ! METACLIENT_HH */


