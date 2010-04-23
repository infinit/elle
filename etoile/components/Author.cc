//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Author.cc
//
// created       julien quintard   [sun aug 23 15:33:40 2009]
// updated       julien quintard   [tue apr  6 12:09:03 2010]
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
    /// this method forges the author object required to modify the
    /// object associated with the given context.
    ///
    Status		Author::Forge(context::Object*		context)
    {
      user::User*	user;

      enter();

      // if an author exists, return.
      if (context->author != NULL)
	leave();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the current user");

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // allocate a new author.
      context->author = new kernel::Author;

      // build the author object according to the subject's role.
      switch (context->rights->role)
	{
	case kernel::RoleOwner:
	  {
	    // create an owner author.
	    if (context->author->Create() == StatusError)
	      escape("unable to create the author");

	    break;
	  }
	case kernel::RoleDelegate:
	  {
	    kernel::Index	index;

	    // retrieve the record index.
	    if (context->access->Locate(user->client->subject,
					index) == StatusError)
	      escape("unable to locate the record");

	    // create the delegate-specific author.
	    if (context->author->Create(index) == StatusError)
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
