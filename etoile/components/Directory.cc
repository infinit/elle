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
// updated       julien quintard   [sun mar 21 18:11:39 2010]
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
      enter();

      // load the object.
      if (Object::Load(context, address) == StatusError)
	escape("unable to load the object");

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
      enter();

      // open the access.
      if (Access::Open(context) == StatusError)
	escape("unable to open the access");

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(context->rights->permissions & core::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the catalog.
      if (Catalog::Open(context) == StatusError)
	escape("unable to load the catalog");

      // lookup the name.
      if (Catalog::Lookup(context, name, address) == StatusError)
	escape("unable to find the entry");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Add(context::Directory*	context,
				       const String&		name,
				       const hole::Address&	address)
    {
      enter();

      // open the access.
      if (Access::Open(context) == StatusError)
	escape("unable to open the access");

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(context->rights->permissions & core::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the catalog.
      if (Catalog::Open(context) == StatusError)
	escape("unable to open the catalog");

      // add the name.
      if (Catalog::Add(context, name, address) == StatusError)
	escape("unable to add the entry");

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
      enter();

      // close the catalog.
      if (Catalog::Close(context) == StatusError)
	escape("unable to close the catalog");

      // close the access.
      if (Access::Close(context) == StatusError)
	escape("unable to close the access");

      // seal the object.
      if (context->object->Seal(*user::user.client->agent) == StatusError)
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
      enter();

      // XXX

      leave();
    }

  }
}
