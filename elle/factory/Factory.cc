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
// updated       julien quintard   [thu may 12 13:24:47 2011]
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
    /// this container holds the mappings between a natural identifier
    /// and a functionoid capable of generating an object of the
    /// corresponding type.
    ///
    Factory::Container		Factory::Map;

    ///
    /// this variable control the access to the factory.
    ///
    Accord			Factory::Control;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the factory.
    ///
    Status		Factory::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the factory.
    ///
    Status		Factory::Clean()
    {
      Factory::Scoutor	scoutor;

      enter();

      // go through the functionoid.
      for (scoutor = Factory::Map.begin();
	   scoutor != Factory::Map.end();
	   scoutor++)
	{
	  Factory::Functionoid*		functionoid = scoutor->second;

	  // delete the functionoid.
	  delete functionoid;
	}

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the factory.
    ///
    Status		Factory::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Factory::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Factory]" << std::endl;

      // lock in reading.
      Factory::Control.Lock(ModeRead);
      {
	// go through the map.
	for (scoutor = Factory::Map.begin();
	     scoutor != Factory::Map.end();
	     scoutor++)
	  {
	    std::cout << alignment << Dumpable::Shift
		      << scoutor->first << std::endl;
	  }
      }
      Factory::Control.Unlock();

      leave();
    }

  }
}
