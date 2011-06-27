//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Meta.cc
//
// created       julien quintard   [mon apr 26 20:08:34 2010]
// updated       julien quintard   [mon jun 27 07:17:37 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Meta.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Maid.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;

  namespace radix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this contains holds the traces related to every memory
    /// allocation performed.
    ///
    Meta::Container		Meta::Traces;

    ///
    /// this value defines if the meta debugging must be activated.
    ///
    /// \todo XXX it could be good to put this in a configuration module
    ///           very much like Etoile.
    ///
    const Boolean		Meta::Debug = false;

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the meta.
    ///
    Status		Meta::Initialize()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method cleans the meta.
    ///
    Status		Meta::Clean()
    {
      enter();

      // if traces exist, dump them.
      if (Meta::Debug == true)
	Meta::Show();

      // flush the meta container.
      if (Meta::Flush() == StatusError)
	escape("unable to flush the meta container");

      leave();
    }

    ///
    /// this method removes all the traces.
    ///
    Status		Meta::Flush()
    {
      Meta::Scoutor	scoutor;

      enter();

      // release the traces.
      for (scoutor = Meta::Traces.begin();
	   scoutor != Meta::Traces.end();
	   scoutor++)
	{
	  Trace*	trace = *scoutor;

	  // release the trace.
	  ::free(trace);
	}

      leave();
    }

    ///
    /// this method dumps all the traces.
    ///
    Status		Meta::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      Meta::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Meta] "
		<< Meta::Traces.size() << std::endl;

      // go through the traces.
      for (scoutor = Meta::Traces.begin();
	   scoutor != Meta::Traces.end();
	   scoutor++)
	{
	  Trace*	trace = *scoutor;

	  // dump the trace.
	  if (trace->Dump(margin + 2) == StatusError)
	    escape("unable to dump the trace");
	}

      leave();
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator allocates memory.
    ///
    /// note that since new operators cannot return NULL, all the errors
    /// handling is skipped.
    ///
    Void*		Meta::operator new(Natural32		size)
    {
      Void*		address;

      // allocate memory on the heap
      address = ::malloc(size);

      // only proceed if debugging has been activated.
      if (Meta::Debug == true)
	{
	  Trace*	trace;

	  // allocate a new trace
	  trace = new (::malloc(sizeof (Trace))) Trace(address);

	  // generate the trace.
	  trace->Generate();

	  // insert the trace in the container.
	  Meta::Traces.push_back(trace);
	}

      return (address);
    }

    ///
    /// this operator is called whenever an object is allocated in
    /// a given memory area referenced by _address_.
    ///
    Void*		Meta::operator new(Natural32,
					   Void*		address)
    {
      // return the address of the already reserved memory area.
      return (address);
    }

    ///
    /// this method releases previously-reserved memory.
    ///
    Void		Meta::operator delete(Void*		address)
    {
      // only proceed if debugging has been activated.
      if (Meta::Debug == true)
	{
	  Meta::Iterator	iterator;

	  // go through the traces.
	  for (iterator = Meta::Traces.begin();
	       iterator != Meta::Traces.end();
	       iterator++)
	    {
	      Trace*		trace = *iterator;

	      // check if this trace corresponds to the given address.
	      if (trace->address == address)
		{
		  // erase the trace.
		  Meta::Traces.erase(iterator);

		  // release the trace.
		  ::free(trace);

		  // release the memory.
		  ::free(address);

		  return;
		}
	    }

	  // otherwise, the address seems not to be correct, therefore display
	  // a message.
	  std::cerr << "[warning] the address " << address << " seems to be "
		    << "invalid" << std::endl;
	}

      // release the memory anyway.
      ::free(address);
    }

  }
}
