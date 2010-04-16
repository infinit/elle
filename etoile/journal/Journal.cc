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
// updated       julien quintard   [fri apr 16 14:32:47 2010]
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

      // first, remove the context from the container so that no application
      // can use it anymore. this only applies to external contexts i.e
      // contexts with valid identifiers.
      if (context->identifier != context::Identifier::Null)
	{
	  // remove the context.
	  if (context::Context::Remove(context) == StatusError)
	    escape("unable to remove the context");
	}

      // XXX easy temporary version, just publish everything.

      // XXX
      if (context->bucket.container.empty() == false)
      {
	context->Dump();
      }
      // XXX

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

      // finally, delete external contexts i.e contexts with valid identifiers.
      if (context->identifier != context::Identifier::Null)
	{
	  // delete it.
	  delete context;
	}

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
