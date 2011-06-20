//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/utility/Author.cc
//
// created       julien quintard   [thu jun 16 16:16:28 2011]
// updated       julien quintard   [thu jun 16 17:38:14 2011]
//
 
//
// ---------- includes --------------------------------------------------------
//

#include <etoile/utility/Author.hh>

namespace etoile
{
  namespace utility
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method forges the author required for modifying the associated
    /// object.
    ///
    elle::Status	Author::Create(Rights + Access)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // allocate a new author.
      context->author = new nucleus::Author;

      // build the author object according to the subject's role.
      switch (context->rights->role)
	{
	case nucleus::RoleOwner:
	  {
	    // create an owner author.
	    if (context->author->Create() == elle::StatusError)
	      escape("unable to create the author");

	    break;
	  }
	case nucleus::RoleDelegate:
	  {
	    nucleus::Index	index;

	    // retrieve the record index.
	    if (context->access->Locate(user->client->subject,
					index) == elle::StatusError)
	      escape("unable to locate the record");

	    // create the delegate-specific author.
	    if (context->author->Create(index) == elle::StatusError)
	      escape("unable to create the author");

	    break;
	  }
	default:
	  {
	    // XXX at this point, the user is probably a vassal but
	    // a voucher must be found.
	    printf("[XXX] Author::Forge() NYI\n");

	    break;
	  }
	}

      leave();
    }

  }
}
