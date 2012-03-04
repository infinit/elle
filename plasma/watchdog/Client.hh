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

# include <functional>
# include <list>
# include <string>

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

    ///
    /// This class represent a client of the local server.
    /// It allows any authorized process to makes call
    /// to the API meta server, as well as knowing about
    /// local infinit instances.
    ///
    /// See ClientActions class for calls dispatching.
    ///
    class Client : private boost::noncopyable
    {
    public:
      typedef plasma::metaclient::NetworksResponse NetworksResponse;
      typedef std::function<void(std::list<std::string> const&)> NetworksCallback;
    private:
      typedef plasma::metaclient::MetaClient MetaClient;

    private:
      MetaClient& _meta;

    public:
      // ctor & dtor
      Client(MetaClient& meta);
      ~Client();
    };

  }
}

#endif /* ! CLIENT_HH */


