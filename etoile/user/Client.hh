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
// updated       julien quintard   [fri mar 26 12:39:51 2010]
//

#ifndef ETOILE_USER_CLIENT_HH
#define ETOILE_USER_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Subject.hh>

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
      public Dumpable
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
      static Status		Initialize();
      static Status		Clean();

      static Status		Add(Client*);
      static Status		Locate(Client*,
				       C::Iterator&);
      static Status		Remove(Client*);

      static Status		Show(const Natural32 = 0);

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
      Status			Create();
      Status			Destroy();

      Status			Record(Agent*);

      Status			Add(Application*);
      Status			Locate(Application*,
				       A::Iterator&);
      Status			Remove(Application*);

      //
      // interfaces
      //

      // dumpable
      Status			Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      String			phrase;

      Agent*			agent;
      A::Container		applications;

      core::Subject		subject;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>
#include <etoile/user/Map.hh>

#endif
