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
// updated       julien quintard   [sat feb 26 15:03:57 2011]
//

#ifndef ETOILE_USER_CLIENT_HH
#define ETOILE_USER_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/kernel/Subject.hh>

#include <etoile/user/Agent.hh>
#include <etoile/user/Application.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Agent;
    class Applications;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class handles clients from connections to authentications etc.
    ///
    class Client:
      public elle::Entity
    {
    public:
      //
      // structures
      //
      struct A
      {
	//
	// types
	//
	typedef std::list<Application*>		Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;
      };

      struct C
      {
	//
	// types
	//
	typedef std::list<Client*>		Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;
      };

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Add(Client*);
      static elle::Status	Locate(Client*,
				       C::Iterator&);
      static elle::Status	Remove(Client*);

      static elle::Status	Retrieve(const elle::Channel*,
					 Client*&);
      static elle::Status	Retrieve(const elle::PublicKey&,
					 Client*&);
      static elle::Status	Retrieve(const elle::String&,
					 Client*&);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static C::Container	Clients;

      //
      // constructors & destructors
      //
      Client();
      ~Client();

      //
      // methods
      //
      elle::Status		Create();
      elle::Status		Destroy();

      elle::Status		Record(Agent*);

      elle::Status		Add(Application*);
      elle::Status		Locate(Application*,
				       A::Iterator&);
      elle::Status		Retrieve(const elle::Channel*,
					 Application*&);
      elle::Status		Remove(Application*);

      //
      // interfaces
      //

      // dumpable
      elle::Status		Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      // XXX hole::User		user;

      Agent*			agent;
      A::Container		applications;

      elle::String		phrase;

      kernel::Subject		subject;
    };

  }
}

#endif
