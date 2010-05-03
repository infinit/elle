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
// updated       julien quintard   [mon may  3 13:33:01 2010]
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
      public elle::Entity
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
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Instance(User*&);

      static elle::Status	Govern(const elle::Phase&,
				       elle::Fiber*&);

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
      elle::Status	Create(Client*);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      elle::Session*	session;

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
#include <etoile/user/Guest.hh>
#include <etoile/user/Map.hh>

#endif
