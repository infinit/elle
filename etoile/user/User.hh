//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/User.hh
//
// created       julien quintard   [thu mar 11 16:05:28 2010]
// updated       julien quintard   [sat mar 20 15:38:03 2010]
//

#ifndef ETOILE_USER_USER_HH
#define ETOILE_USER_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/user/Client.hh>
#include <etoile/user/Map.hh>
#include <etoile/user/Guest.hh>

namespace etoile
{
  ///
  /// XXX
  ///
  namespace user
  {

    ///
    /// this class provides functionalities for handling users.
    ///
    /// XXX expliquer tout le bordel
    ///
    class User
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Assign(Client*);
      static Status	Assign();

      //
      // constructors & destructors
      //
      User();

      //
      // attributes
      //
      Client*		client;
    };

//
// ---------- externs ---------------------------------------------------------
//

    extern User			user;

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>
#include <etoile/user/Application.hh>

#endif
