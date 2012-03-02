//
// ---------- header ----------------------------------------------------------
//
// project       plasma/watchdog
//
// license       infinit
//
// author        Raphaël Londeix  [Thu 01 Mar 2012 11:31:35 AM CET]
//

#ifndef PLASMA_WATCHDOG_CLIENT_HH
# define PLASMA_WATCHDOG_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

# include <boost/noncopyable.hpp>

# include <QApplication>

# include "plasma/metaclient/MetaClient.hh"

namespace plasma
{
  namespace watchdog
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Client : private boost::noncopyable
    {
    private:
      typedef plasma::metaclient::MetaClient MetaClient;
      typedef plasma::metaclient::NetworksResponse NetworksResponse;

    private:
      MetaClient _api;

    public:
      // ctor & dtor
      Client(QApplication& app);
      ~Client();

      // properties
      void token(QString const& token) { this->_api.token(token.toAscii()); }

      // methods
      void Update(std::function<void()> callback);

    private:
      void _OnNetworkList(std::function<void()> cb,
                          NetworksResponse const& response);
    };

  }
}

#endif /* ! CLIENT_HH */


