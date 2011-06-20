//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/XXX/Author.cc
//
// created       julien quintard   [sun aug 23 15:33:40 2009]
// updated       julien quintard   [thu jun 16 16:16:16 2011]
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
    elle::Status	Author::Forge(context::Object*		context)
    {
      user::User*	user;

      enter();

      // if an author exists, return.
      if (context->author != NULL)
	leave();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the current user");

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
