//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Network.cc
//
// created       julien quintard   [wed feb  3 16:49:46 2010]
// updated       julien quintard   [thu feb  4 15:22:29 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Network.hh>

namespace elle
{
  namespace network
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// XXX
    ///
    Network::Container		Network::Callbacks;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default functionoid constructor.
    ///
    Network::Functionoid::Functionoid():
      type(Network::Functionoid::TypeUnknown)
    {
    }

    ///
    /// specific functionoid constructor.
    ///
    Network::Functionoid::Functionoid(Network::Functionoid::Type	type):
      type(type)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes an archive, extract its identifier and trigger
    /// the callback associated with this tag by constructing a brand
    /// new object.
    ///
    Status		Network::Dispatch(Environment&		environment,
					  Archive&		archive)
    {
      Network::Scoutor	scoutor;
      Tag		tag;

      // extract the tag from the archive.
      if (archive.Extract(tag) == StatusError)
	escape("unable to extract the tag from the archive");

      // retrieve the associated callback.
      if ((scoutor = Network::Callbacks.find(tag)) == Network::Callbacks.end())
	escape("unable to locate the callback");

      // trigger the callback.
      if (scoutor->second->Trigger(environment, archive) == StatusError)
	escape("unable to trigger the callback");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the table of callbacks.
    ///
    Status		Network::Dump(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Network::Scoutor	scoutor;

      std::cout << alignment << "[Network]" << std::endl;

      // dump the callbacks table.
      for (scoutor = Network::Callbacks.begin();
	   scoutor != Network::Callbacks.end();
	   scoutor++)
	{
	  // dump the identifier.
	  std::cout << alignment << shift << "[Identifier] "
		    << std::dec << scoutor->first << std::endl;

	  // dump the functionoid.
	  if (scoutor->second->Dump(margin + 2) == StatusError)
	    escape("unable to dump the functionoid");
	}

      leave();
    }

  }
}
