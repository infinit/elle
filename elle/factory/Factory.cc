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
// updated       julien quintard   [thu mar 25 17:38:50 2010]
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

      leave();
    }

    ///
    /// this method cleans the factory.
    ///
    Status		Factory::Clean()
    {
      enter();

      /// XXX \todo remove the types.

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
