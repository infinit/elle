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
// updated       julien quintard   [fri apr 23 00:38:35 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

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
    Status		Journal::Initialize()
    {
      enter();

      // nothing to do

      leave();
    }

    ///
    /// this method cleans the journal.
    ///
    Status		Journal::Clean()
    {
      enter();

      // nothing to do.

      leave();
    }

    ///
    /// this method registers a context as being ready to be processed.
    ///
    Status		Journal::Record(context::Context*	context)
    {
      Bucket::Scoutor	scoutor;

      enter();

      // first, remove the exportation so that the application cannot
      // use this context anymore.
      if (context::Context::Import(context) == StatusError)
	escape("unable to import the context");

      // XXX
      printf("[XXX] Journal::Record()\n");
      if (dynamic_cast<context::Object*>(context) != NULL)
	dynamic_cast<context::Object*>(context)->route.Dump();

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
		if (hole::Hole::Put(item->address,
				    item->block) == StatusError)
		  escape("unable to publish the block through hole");

		break;
	      }
	    case OperationDestroy:
	      {
		if (hole::Hole::Erase(item->address) == StatusError)
		  escape("unable to erase the block through hole");

		break;
	      }
	    }
	}

      // XXX
      printf("[/XXX] Journal::Record()\n");

      // finally, delete the context.
      if (context::Context::Delete(context) == StatusError)
	escape("unable to delete the context");

      leave();
    }

    ///
    /// this method is called by Depot whenever looking for a particular
    /// block.
    ///
    Status		Journal::Get(const hole::Address&	address,
				     hole::Block*&		block)
    {
      enter();

      printf("[XXX] Journal::Get() NYI\n");

      // XXX

      leave();
    }

  }
}
