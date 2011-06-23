//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Author.cc
//
// created       julien quintard   [mon jun 20 14:13:45 2011]
// updated       julien quintard   [tue jun 21 14:12:53 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Author.hh>
#include <etoile/automaton/Rights.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method forges the author objects require in order to prove
    /// the user had the permission to operate upon the object.
    ///
    elle::Status	Author::Forge(gear::Object&		context)
    {
      enter();

      // if an author exists, return.
      if (context.author != nucleus::Author::Null)
	leave();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // build the author object according to the subject's role.
      switch (context.rights.role)
	{
	case nucleus::RoleOwner:
	  {
	    // create an owner author.
	    if (context.author.Create() == elle::StatusError)
	      escape("unable to create the author");

	    break;
	  }
	case nucleus::RoleLord:
	  {
	    /* XXX
	    nucleus::Index	index;

	    // retrieve the record index.
	    if (context->access->Locate(user->client->subject,
					index) == elle::StatusError)
	      escape("unable to locate the record");

	    // create the delegate-specific author.
	    if (context->author->Create(index) == elle::StatusError)
	      escape("unable to create the author");
	    */
	    break;
	  }
	default:
	  {
	    // XXX

	    break;
	  }
	}

      leave();
    }

  }
}
