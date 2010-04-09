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
// updated       julien quintard   [thu apr  8 20:27:47 2010]
//

#ifndef ETOILE_USER_USER_HH
#define ETOILE_USER_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  ///
  /// XXX
  ///
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Agent;
    class Application;
    class Client;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for handling users.
    ///
    /// XXX expliquer tout le bordel
    ///
    ///
    class User:
      public Dumpable
    {
    public:
      //
      // enumerations
      //
      enum Type
	{
	  TypeUnknown,
	  TypeAgent,
	  TypeApplication
	};

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Instance(User*&);

      static Status	Govern(const Phase&,
			       Fiber*&);

      //
      // static attribute
      //
      static User*	Current;

      //
      // constructors & destructors
      //
      User();

      //
      // methods
      //
      Status		Create(Client*);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Session*		session;

      Client*		client;

      Type		type;

      union
      {
	Agent*		agent;
	Application*	application;
      };
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>
#include <etoile/user/Application.hh>
#include <etoile/user/Client.hh>
#include <etoile/user/Map.hh>
#include <etoile/user/Guest.hh>

#endif
