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
// updated       julien quintard   [sun aug 23 16:40:26 2009]
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
    Status		Author::Create(context::Object&		context)
    {
      // if the user the object owner.
      if (context.object->owner.K == agent::Agent::Pair.K)
	{
	  // create an owner author.
	  context.author.Create();

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
