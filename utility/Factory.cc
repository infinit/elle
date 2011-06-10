//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Factory.cc
//
// created       julien quintard   [thu jan 28 19:19:35 2010]
// updated       julien quintard   [thu jun  9 21:48:42 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Factory.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the factory.
    ///
    Status		Factory::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Factory::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Factory]" << std::endl;

      // go through the map.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  std::cout << alignment << Dumpable::Shift
		    << scoutor->first << std::endl;
	}

      leave();
    }

  }
}
