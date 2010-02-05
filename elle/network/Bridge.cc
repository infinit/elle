//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bridge.cc
//
// created       julien quintard   [thu feb  4 15:20:31 2010]
// updated       julien quintard   [thu feb  4 16:52:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Bridge.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// definition of the callbacks container.
    ///
    Bridge::Container		Bridge::Callbacks;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default functionoid constructor.
    ///
    Bridge::Functionoid::Functionoid():
      type(Bridge::Functionoid::TypeUnknown)
    {
    }

    ///
    /// specific functionoid constructor.
    ///
    Bridge::Functionoid::Functionoid(Bridge::Functionoid::Type	type):
      type(type)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the table of callbacks.
    ///
    Status		Bridge::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Bridge::Scoutor	scoutor;

      std::cout << alignment << "[Bridge]" << std::endl;

      // dump the callbacks table.
      for (scoutor = Bridge::Callbacks.begin();
	   scoutor != Bridge::Callbacks.end();
	   scoutor++)
	{
	  // dump the functionoid.
	  if (scoutor->second->Dump(margin + 2) == StatusError)
	    escape("unable to dump the functionoid");
	}

      leave();
    }

  }
}
