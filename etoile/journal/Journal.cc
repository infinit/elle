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
// updated       julien quintard   [mon feb  1 01:14:38 2010]
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
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Journal::Initialize()
    {
      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Journal::Clean()
    {
      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Journal::Push(context::Context*		context)
    {
      Set::Container	set;
      Set::Scoutor	scoutor;

      // XXX easy temporary version, just publish everything.

      // retrieve the set of blocks to publish.
      if (context->Register(set) == StatusError)
	escape("unable to register the blocks to register");

      // go through the blocks and publish them.
      for (scoutor = set.begin();
	   scoutor != set.end();
	   scoutor++)
	{
	  hole::Block*	block = *scoutor;

	  if (hole::Hole::Put(block->address, block) == StatusError)
	    escape("unable to publish the block through hole");
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Journal::Pop(context::Context*&		context)
    {
      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Journal::Get(const hole::Address&	address,
				     hole::Block*&		block)
    {
      // XXX

      leave();
    }

  }
}
