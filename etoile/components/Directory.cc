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
// updated       julien quintard   [tue apr 20 07:56:41 2010]
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
    /// this method creates a directory object.
    ///
    Status		Directory::Create(context::Directory*	context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == StatusError)
	escape("unable to load the user");

      // allocate a new directory object.
      context->object = new kernel::Object;

      // create the irectory.
      if (context->object->Create(kernel::GenreDirectory,
				  user->client->agent->K) == StatusError)
	escape("unable to create the directory object");

      // bind the object.
      if (context->object->Bind() == StatusError)
	escape("unable to bind the object");

      // set the address.
      context->address = context->object->address;

      leave();
    }

    ///
    /// this method loads the directory object identified by the given
    /// address in the context.
    ///
    Status		Directory::Load(context::Directory*	context,
					const hole::Address&	address)
					
    {
      enter();

      // load the object.
      if (Object::Load(context, address) == StatusError)
	escape("unable to load the object");

      // check that the object is a directory.
      if (context->object->meta.genre != kernel::GenreDirectory)
	escape("this object is not a directory");

      leave();
    }

    ///
    /// this method adds a directory entry to the directory.
    ///
    Status		Directory::Add(context::Directory*	directory,
				       const path::Slice&	name,
				       context::Directory*	subdirectory)
    {
      kernel::Entry*	entry;

      enter(instance(entry));

      // determine the rights.
      if (Rights::Determine(directory) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(directory->rights->record.permissions & kernel::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(directory) == StatusError)
	escape("unable to open the contents");

      // allocate a new entry.
      entry = new kernel::Entry(name, subdirectory->object->address);

      // add the entry in the directory.
      if (directory->contents->content->Add(entry) == StatusError)
	escape("unable to add the entry in the directory");

      // stop tracking.
      waive(entry);

      leave();
    }

    ///
    /// this method returns the address corresponding to the given name.
    ///
    Status		Directory::Lookup(context::Directory*	context,
					  const path::Slice&	name,
					  kernel::Entry*&	entry)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(context->rights->record.permissions & kernel::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // look up the entry.
      if (context->contents->content->Lookup(name, entry) == StatusError)
	escape("unable to find the entry in the directory");

      leave();
    }

    ///
    /// this method returns a subset of the directory entries.
    ///
    Status		Directory::Consult(context::Directory*	context,
					   const kernel::Index&	index,
					   const kernel::Size&	size,
					   kernel::Range<kernel::Entry>& range)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(context->rights->record.permissions & kernel::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // consult the directory catalog.
      if (context->contents->content->Consult(index,
					      size,
					      range) == StatusError)
	escape("unable to consult the directory");

      leave();
    }

    ///
    /// this method renames an entry.
    ///
    Status		Directory::Rename(context::Directory*	context,
					const path::Slice&	from,
					const path::Slice&	to)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(context->rights->record.permissions & kernel::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // rename the entry.
      if (context->contents->content->Rename(from, to) == StatusError)
	escape("unable to rename the directory's entry");

      leave();
    }

    ///
    /// this method removes an entry.
    ///
    Status		Directory::Remove(context::Directory*	context,
					  const path::Slice&	name)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(context->rights->record.permissions & kernel::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == StatusError)
	escape("unable to open the contents");

      // remove the entry.
      if (context->contents->content->Remove(name) == StatusError)
	escape("unable to remove the directory's entry");

      leave();
    }

    ///
    /// this store the modifications applied onto the directory context.
    ///
    Status		Directory::Store(context::Directory*	context)
    {
      user::User*	user;

      enter();

      // close the catalog.
      if (Contents::Close(context) == StatusError)
	escape("unable to close the contents");

      // store the object.
      if (Object::Store(context) == StatusError)
	escape("unable to store the object");

      leave();
    }

    ///
    /// this method removes the object along with the blocks attached to it.
    ///
    /// note that, in theory, the directory object could be destroy although
    /// it contains references to sub-entries. it is therefore the
    /// responsability of the caller to destroy the entries.
    ///
    Status		Directory::Destroy(context::Directory*	context)
    {
      user::User*	user;
      kernel::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context->rights->role != kernel::RoleOwner)
	escape("unable to destroy a not-owned object");

      // destroy the contents.
      if (Contents::Destroy(context) == StatusError)
	escape("unable to destroy the contents");

      // destroy the object.
      if (Object::Destroy(context) == StatusError)
	escape("unable to destroy the object");

      leave();
    }

  }
}
