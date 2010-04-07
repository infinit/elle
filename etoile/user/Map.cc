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
// updated       julien quintard   [mon mar 29 15:07:34 2010]
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
    /// this container holds the channels in use in order to speed up the
    /// process of retrieving a client from a channel.
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
    Status		Map::Add(const Channel*			channel,
				 Client*			client)
    {
      std::pair<Map::Iterator, Boolean>		result;

      enter();

      // insert the new mapping.
      result = Map::Mappings.insert(Map::Value(channel, client));

      // check the result.
      if (result.second != true)
	escape("unable to insert the mapping");

      leave();
    }

    ///
    /// this method retrieves a client from its associated channel.
    ///
    Status		Map::Retrieve(const Channel*		channel,
				      Client*&			client)
    {
      Map::Iterator	iterator;

      enter();

      // find the entry.
      iterator = Map::Mappings.find(channel);

      // check the result.
      if (iterator == Map::Mappings.end())
	false();

      // set the client.
      client = iterator->second;

      true();
    }

    ///
    /// this method removes a mapping.
    ///
    Status		Map::Remove(const Channel*		channel)
    {
      enter();

      // remove the entry.
      if (Map::Mappings.erase(channel) != 1)
	escape("unable to remove the given channel");

      leave();
    }

    ///
    /// this method dumps the map.
    ///
    Status		Map::Show(const Natural32		margin)
    {
      Map::Scoutor	scoutor;
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Map]" << std::endl;

      // explore the container.
      for (scoutor = Map::Mappings.begin();
	   scoutor != Map::Mappings.end();
	   scoutor++)
	{
	  // print the record.
	  std::cout << alignment << Dumpable::Shift << "[Channel/Client] "
		    << std::hex << scoutor->first << " / "
		    << std::hex << scoutor->second << std::endl;
	}

      leave();
    }

  }
}
