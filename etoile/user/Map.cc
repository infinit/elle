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
// updated       julien quintard   [mon may  3 18:41:59 2010]
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
    elle::Status	Map::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the map container.
    ///
    elle::Status	Map::Clean()
    {
      enter();

      // remove every remaining mapping.
      while (Map::Mappings.empty() == false)
	{
	  Iterator	iterator = Map::Mappings.begin();

	  if (Map::Remove(iterator->first) == elle::StatusError)
	    escape("unable to remove the mapping");
	}

      leave();
    }

    ///
    /// this method adds a mapping.
    ///
    elle::Status	Map::Add(const elle::Channel*		channel,
				 Client*			client)
    {
      std::pair<Map::Iterator, elle::Boolean>		result;

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
    elle::Status	Map::Retrieve(const elle::Channel*	channel,
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
    elle::Status	Map::Remove(const elle::Channel*	channel)
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
    elle::Status	Map::Show(const elle::Natural32		margin)
    {
      Map::Scoutor	scoutor;
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Map]" << std::endl;

      // explore the container.
      for (scoutor = Map::Mappings.begin();
	   scoutor != Map::Mappings.end();
	   scoutor++)
	{
	  // print the record.
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Channel - Client] "
		    << std::hex << scoutor->first << " - "
		    << std::hex << scoutor->second << std::endl;
	}

      leave();
    }

  }
}
