//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Journal.cc
//
// created       julien quintard   [sat jan 30 15:22:54 2010]
// updated       julien quintard   [thu may  5 16:05:32 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <etoile/depot/Depot.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the journal.
    ///
    elle::Status	Journal::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the journal.
    ///
    elle::Status	Journal::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method registers a context as being ready to be processed.
    ///
    elle::Status	Journal::Record(context::Context*	context)
    {
      Bucket::Scoutor	scoutor;

      enter();

      // first, remove the exportation so that the application cannot
      // use this context anymore.
      if (context::Context::Import(context) == elle::StatusError)
	escape("unable to import the context");

      // XXX easy temporary version, just publish everything.
      // go through the blocks and publish/destroy them.
      for (scoutor = context->bucket.container.begin();
	   scoutor != context->bucket.container.end();
	   scoutor++)
	{
	  Item*		item = *scoutor;

	  switch (item->operation)
	    {
	    case OperationPush:
	      {
		if (depot::Depot::Put(item->address,
				      item->block) == elle::StatusError)
		  escape("unable to publish the block");

		break;
	      }
	    case OperationDestroy:
	      {
		if (depot::Depot::Erase(item->address) == elle::StatusError)
		  escape("unable to erase the block through hole");

		break;
	      }
	    }
	}

      // finally, delete the context.
      if (context::Context::Delete(context) == elle::StatusError)
	escape("unable to delete the context");

      leave();
    }

    ///
    /// this method is called by Depot whenever looking for a particular
    /// block.
    ///
    elle::Status	Journal::Get(const nucleus::Address&	address,
				     nucleus::Block*&		block)
    {
      enter();

      printf("[XXX] Journal::Get() NYI\n");

      // XXX

      leave();
    }

  }
}
