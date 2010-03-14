//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Client.hh
//
// created       julien quintard   [thu mar  4 11:56:54 2010]
// updated       julien quintard   [thu mar 11 17:13:04 2010]
//

#ifndef ETOILE_USER_CLIENT_HH
#define ETOILE_USER_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/user/Agent.hh>
#include <etoile/user/Application.hh>

#include <list>

namespace etoile
{
  namespace user
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class handles clients from connections to authentications etc.
    ///
    class Client
    {
    public:
      //
      // types
      //
      typedef std::list<Application*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Client();
      ~Client();

      //
      // methods
      //
      // XXX Add/Remove

      //
      // attributes
      //
      Agent*		agent;
      Container		applications;
    };

  }
}

#endif
