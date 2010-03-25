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
// updated       julien quintard   [thu mar 25 17:58:16 2010]
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
      typedef std::pair<Channel*, Client*>		Value;
      typedef std::map<Channel*, Client*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Add(Channel*,
			    Client*);
      static Status	Retrieve(Channel*,
				 Client*&);
      static Status	Remove(Channel*);

      static Status	Show(const Natural32 = 0);

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
