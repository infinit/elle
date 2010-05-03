//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Map.hh
//
// created       julien quintard   [fri mar 19 11:59:09 2010]
// updated       julien quintard   [mon may  3 13:48:40 2010]
//

#ifndef ETOILE_USER_MAP_HH
#define ETOILE_USER_MAP_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this forward declares the Client class.
    //
    class Client;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables anyone to retrive a client based on a channel
    /// it is associated with.
    ///
    class Map
    {
    public:
      //
      // types
      //
      typedef std::pair<const elle::Channel*, Client*>	Value;
      typedef std::map<const elle::Channel*, Client*>	Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // static methods
      //
      static elle::Status	Initialize();
      static elle::Status	Clean();

      static elle::Status	Add(const elle::Channel*,
				    Client*);
      static elle::Status	Retrieve(const elle::Channel*,
					 Client*&);
      static elle::Status	Remove(const elle::Channel*);

      static elle::Status	Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static Container	Mappings;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>

#endif
