//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Author.cc
//
// created       julien quintard   [sun aug 23 15:33:40 2009]
// updated       julien quintard   [wed mar  3 16:05:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Author.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Author::Forge(context::Object*		context)
    {
      enter();

      // if an author exists, release it.
      if (context->author != NULL)
	delete context->author;

      // allocate a new author.
      context->author = new core::Author;

      // if the user the object owner.
      if (context->object->owner.K == agent::Agent::Pair.K)
	{
	  // create an owner author.
	  if (context->author->Create() == StatusError)
	    escape("unable to create the author");

	  leave();
	}
      else
	{
	  // XXX
	  printf("NOT IMPLEMENTED YET\n");
	}

      leave();
    }

  }
}
