//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Map.cc
//
// created       julien quintard   [fri mar 19 13:37:42 2010]
// updated       julien quintard   [fri mar 19 19:47:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Map.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the links in use in order to speed up the
    /// process of retrieving a client from a link.
    ///
    Map::Container			Map::Mappings;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the map.
    ///
    Status		Map::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the map container.
    ///
    Status		Map::Clean()
    {
      enter();

      // remove every remaining mapping.
      while (Map::Mappings.empty() == false)
	{
	  Iterator	iterator = Map::Mappings.begin();

	  if (Map::Remove(iterator->first) == StatusError)
	    escape("unable to remove the mapping");
	}

      leave();
    }

    ///
    /// this method adds a mapping.
    ///
    Status		Map::Add(Link*				link,
				 Client*			client)
    {
      std::pair<Map::Iterator, Boolean>		result;

      enter();

      // insert the new mapping.
      result = Map::Mappings.insert(Map::Value(link, client));

      // check the result.
      if (result.second != true)
	escape("unable to insert the mapping");

      leave();
    }

    ///
    /// this method retrieves a client from its associated link.
    ///
    Status		Map::Retrieve(Link*			link,
				      Client*&			client)
    {
      Map::Iterator	iterator;

      enter();

      // find the entry.
      iterator = Map::Mappings.find(link);

      // check the result.
      if (iterator == Map::Mappings.end())
	escape("unable to locate the given link");

      // set the client.
      client = iterator->second;

      leave();
    }

    ///
    /// this method removes a mapping.
    ///
    Status		Map::Remove(Link*			link)
    {
      enter();

      // remove the entry.
      if (Map::Mappings.erase(link) != 1)
	escape("unable to remove the given link");

      leave();
    }

    ///
    /// this method dumps the map.
    ///
    Status		Map::Show(const Natural32		margin)
    {
      Map::Scoutor	scoutor;
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      enter();

      std::cout << alignment << "[Map]" << std::endl;

      // explore the container.
      for (scoutor = Map::Mappings.begin();
	   scoutor != Map::Mappings.end();
	   scoutor++)
	{
	  // print the record.
	  std::cout << alignment << shift << "[Link/Client] "
		    << std::hex << scoutor->first << " / "
		    << std::hex << scoutor->second << std::endl;
	}

      leave();
    }

  }
}
