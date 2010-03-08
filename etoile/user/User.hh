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
// updated       julien quintard   [fri mar  5 10:36:23 2010]
//

#ifndef ETOILE_USER_USER_HH
#define ETOILE_USER_USER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/user/Individual.hh>

#include <map>
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
      struct Data
      {
	typedef std::map<PublicKey, Individual*>	Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      struct Temporary
      {
	typedef Door*					Value;
	typedef std::list<Value>			Container;
	typedef Container::iterator			Iterator;
	typedef Container::const_iterator		Scoutor;
      };

      //
      // static methods
      //
      static Status	Record(Door*);
      static Status	Register(Individual*);

      //
      // static attributes
      //
      static Data::Container		Bank;
      static Temporary::Container	Queue;
    };

  }
}

//
// ---------- manifests -------------------------------------------------------
//

///
/// definitions of the messages expected to authenticate an individual.
///

#endif
