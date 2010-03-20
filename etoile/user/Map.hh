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
// updated       julien quintard   [fri mar 19 20:13:19 2010]
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
    /// this class enables anyone to retrive a client based on a link
    /// it is associated with.
    ///
    class Map
    {
    public:
      //
      // types
      //
      typedef std::pair<Link*, Client*>		Value;
      typedef std::map<Link*, Client*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Add(Link*,
			    Client*);
      static Status	Retrieve(Link*,
				 Client*&);
      static Status	Remove(Link*);

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
