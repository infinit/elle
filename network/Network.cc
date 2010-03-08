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
// updated       julien quintard   [sun mar  7 23:26:54 2010]
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
    /// this container holds the list of registered callbacks.
    ///
    Network::Trig::Container		Network::Callbacks;

    ///
    /// this container holds the list of threads waiting for a specific
    /// packet.
    ///
    Network::Wait::Container		Network::Receivers;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes a newly received packet and dispatch it.
    ///
    Status		Network::Dispatch(Context*		context,
					  Header*		header,
					  Data*			data)
    {
      Context*		c = context;
      Header*		h = header;
      Data*		d = data;

      enter(instance(context),
	    instance(header),
	    instance(data));

      // reset the variables that are now tracked.
      context = c;
      header = h;
      data = d;

      printf("Network::Dispatch(%u)\n", header->tag);

      //
      // assign the new context.
      //
      if (Context::Assign(context) == StatusError)
	escape("unable to assign the context");

      // stop tracking context.
      waive(context);

      //
      // first, look at the receivers waiting for this very specific packet.
      //
      {
	Network::Wait::Iterator		iterator;

	// retrieve the condition associated to the header's tag.
	if ((iterator = Network::Receivers.find(header->identifier)) !=
	    Network::Receivers.end())
	  {
	    // set the data pointer.
	    iterator->second->second = data;

	    // wake up the receiver.
	    iterator->second->first.Release();

	    // delete the header.
	    delete header;

	    // stop tracking the header and data.
	    waive(header);
	    waive(data);

	    // delete the entry in the receivers container since
	    // it will no longer be used.
	    //
	    // note that since the entry value has been allocated, it
	    // will not be deleted so that the waited thread will get
	    // the data pointer has expected.
	    Network::Receivers.erase(iterator);

	    leave();
	  }
      }

      //
      // second, call an associated callback.
      //
      {
	Network::Trig::Scoutor		scoutor;

	// retrieve the callback associated to the header's tag.
	if ((scoutor = Network::Callbacks.find(header->tag)) ==
	    Network::Callbacks.end())
	  escape("unable to locate the callback");

	// trigger the callback.
	if (scoutor->second->Call(*data) == StatusError)
	  escape("unable to dispatch the event");

	// delete the header and data.
	delete header;
	delete data;

	// stop tracking the header and data.
	waive(header);
	waive(data);
      }

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
      String			alignment(margin, ' ');
      String			shift(2, ' ');
      Network::Trig::Scoutor	scoutor;

      enter();

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
