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
// updated       julien quintard   [fri mar 12 07:10:37 2010]
//

#ifndef ETOILE_USER_USER_HH
#define ETOILE_USER_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/user/Client.hh>

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
    class User
    {
    public:
      //
      // types
      //
      struct Individual
      {
	typedef std::list<Client*>		Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;
      };

      struct Access
      {
	typedef std::pair<Socket*, Client*>	Value;
	typedef std::map<Socket*, Client*>	Container;
	typedef Container::iterator		Iterator;
	typedef Container::const_iterator	Scoutor;
      };

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Record(Socket*);
      static Status	Register(Client*);
      static Status	Locate(Socket*,
			       Client*&);
      static Status	Assign(Client*);

      /// XXX \todo ajouter un callback qui prend les erreur genre disconnected
      /// such une socket et se charge d'updater (possiblement defoncer)
      /// le client correspondant.

      //
      // static attributes
      //
      static Individual::Container	Clients;
      static Access::Container		Sockets;
    };

//
// ---------- externs ---------------------------------------------------------
//

    ///
    /// this variable holds the identity of the current client.
    ///
    extern Client*		client;

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Agent.hh>
#include <etoile/user/Application.hh>

#endif
