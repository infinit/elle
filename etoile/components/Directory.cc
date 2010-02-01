//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Directory.cc
//
// created       julien quintard   [fri aug 14 19:00:57 2009]
// updated       julien quintard   [mon feb  1 01:08:51 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Directory.hh>

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
    Status		Directory::Load(context::Directory*	context,
					const hole::Address&	address)
					
    {
      // load the object.
      if (Object::Load(context, address) == StatusError)
	escape("unable to load the object");

      // initialize the contents.
      context->catalog = NULL;

      // check that the object is a directory.
      if (context->object->meta.status.genre != core::GenreDirectory)
	escape("this object is not a directory");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Lookup(context::Directory*	context,
					  const String&		name,
					  hole::Address&	address)
    {
      // open the catalog.
      if (Catalog::Open(context) == StatusError)
	escape("unable to load the catalog");

      // look up the entry.
      if (context->catalog->content->Lookup(name, address) == StatusError)
	escape("unable to find the entry in the catalog");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Add(context::Directory*	context,
				       const String&		name,
				       const hole::Address&	address)
    {
      // open the catalog.
      if (Catalog::Open(context) == StatusError)
	escape("unable to open the catalog");

      // add the entry in the catalog.
      if (context->catalog->content->Add(name, address) == StatusError)
	escape("unable to add the entry in the catalog");

      leave();
    }

    ///
    /// this method first calls the sub-components to remove any
    /// loaded information that has not changed but also to update the link
    /// between the components and sub-components such as the object as a
    /// reference to a potentional catalog.
    ///
    Status		Directory::Commit(context::Directory*	context)
    {
      // close the catalog.
      if (Catalog::Close(context) == StatusError)
	escape("unable tox seal the catalog");

      // close the access.
      // XXX

      // seal the object.
      if (context->object->Seal(agent::Agent::Pair.k) == StatusError)
	escape("unable to seal the object");

      // push the context in the journal.
      if (journal::Journal::Push(context) == StatusError)
	escape("unable to the context");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Close(context::Directory*	context)
    {
      // XXX

      leave();
    }

  }
}
