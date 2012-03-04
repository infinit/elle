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

# include <QProcess>

# include "plasma/metaclient/MetaClient.hh"

namespace plasma
{
  namespace watchdog
  {

      namespace meta = ::plasma::metaclient;
//
// ---------- classes ---------------------------------------------------------
//

    class InfinitNetwork
    {
    private:
      meta::NetworkResponse   _description;
      QProcess                _process;

    public:
      InfinitNetwork(meta::NetworkResponse const& response);
      void Update(meta::NetworkResponse const& response);
    };

  }
}


//
// ---------- templates -------------------------------------------------------
//

#endif /* ! INFINITNETWORK_HH */


