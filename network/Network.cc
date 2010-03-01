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
// updated       julien quintard   [thu feb 25 14:01:03 2010]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes an archive, extract its identifier and trigger
    /// the callback associated with this tag by re-constructing the
    /// live objects.
    ///
    Status		Network::Dispatch(Packet&		packet)
    {
      Network::Scoutor	scoutor;
      Header		header;
      Data		data;

      // first, extract the header which contain the metadata required
      // to build the parameters, such as the tag.
      if (packet.Extract(header) == StatusError)
	escape("unable to extract the header");

      /// XXX \todo here header.size can be used to chop the packet
      /// and return what has not been used.

      // extract the data.
      if (packet.Extract(data) == StatusError)
	escape("unable to extract the data");

      // retrieve the callback associated to the header's tag.
      if ((scoutor = Network::Callbacks.find(header.tag)) ==
	  Network::Callbacks.end())
	escape("unable to locate the callback");

      // trigger the callback.
      if (scoutor->second->Dispatch(data) == StatusError)
	escape("unable to dispatch the event");

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
