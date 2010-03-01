//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/factory/Factory.cc
//
// created       julien quintard   [thu jan 28 19:19:35 2010]
// updated       julien quintard   [sun feb 28 15:19:17 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/factory/Factory.hh>

namespace elle
{
  namespace factory
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this container holds the mappings between a string identifier
    /// and a functionoid capable of generating an object of the
    /// corresponding type.
    ///
    Factory::Container		Factory::Map;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method dumps the factory.
    ///
    Status		Factory::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Factory::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Factory]" << std::endl;

      // go through the map.
      for (scoutor = Factory::Map.begin();
	   scoutor != Factory::Map.end();
	   scoutor++)
	{
	  std::cout << alignment << shift << scoutor->first << std::endl;
	}

      leave();
    }

  }
}
