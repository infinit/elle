//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/User.hh
//
// created       julien quintard   [thu mar  4 11:56:54 2010]
// updated       julien quintard   [thu mar 11 11:13:38 2010]
//

#ifndef ETOILE_USER_USER_HH
#define ETOILE_USER_USER_HH

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
    /// this class handles users from connections to authentications etc.
    ///
    class User
    {
    public:
      //
      // types
      //
      typedef std::list<Application*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // attributes
      //
      Agent*		agent;
      Container		applications;
    };

  }
}

<PublicKey, User> Users;

<Door*, User*> Access;

#endif
