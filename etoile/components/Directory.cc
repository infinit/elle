//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Directory.cc
//
// created       julien quintard   [fri aug 14 19:00:57 2009]
// updated       julien quintard   [sun may  8 09:53:34 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Directory.hh>
#include <etoile/components/Rights.hh>
#include <etoile/components/Contents.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/user/User.hh>

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
    elle::Status	Directory::Create(context::Directory*	context)
    {
      user::User*	user;

      enter();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // allocate a new directory object.
      context->object = new nucleus::Object;

      // place the block in the application's network.
      if (context->object->Place(user->application->network) ==
	  elle::StatusError)
	escape("unable to place the object");

      // create the directory.
      if (context->object->Create(nucleus::GenreDirectory,
				  user->client->agent->K) == elle::StatusError)
	escape("unable to create the directory object");

      // bind the object.
      if (context->object->Bind(context->address) == elle::StatusError)
	escape("unable to bind the object");

      leave();
    }

    ///
    /// this method loads the directory object identified by the given
    /// address in the context.
    ///
    elle::Status	Directory::Load(context::Directory*	context,
					const nucleus::Address&	address)
					
    {
      enter();

      // load the object.
      if (Object::Load(context, address) == elle::StatusError)
	escape("unable to load the object");

      // check that the object is a directory.
      if (context->object->meta.genre != nucleus::GenreDirectory)
	escape("this object is not a directory");

      leave();
    }

    ///
    /// this method adds a directory entry to the directory.
    ///
    elle::Status	Directory::Add(context::Directory*	directory,
				       const path::Slice&	name,
				       context::Directory*	subdirectory)
    {
      nucleus::Entry*	entry;

      enter(instance(entry));

      // determine the rights.
      if (Rights::Determine(directory) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(directory->rights->record.permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(directory) == elle::StatusError)
	escape("unable to open the contents");

      // allocate a new entry.
      entry = new nucleus::Entry(name, subdirectory->address);

      // add the entry in the directory.
      if (directory->contents->content->Add(entry) == elle::StatusError)
	escape("unable to add the entry in the directory");

      // stop tracking.
      waive(entry);

      leave();
    }

    ///
    /// this method returns the address corresponding to the given name.
    ///
    elle::Status	Directory::Lookup(context::Directory*	context,
					  const path::Slice&	name,
					  nucleus::Entry*&	entry)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(context->rights->record.permissions & nucleus::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // look up the entry.
      if (context->contents->content->Lookup(name, entry) == elle::StatusError)
	escape("unable to find the entry in the directory");

      leave();
    }

    ///
    /// this method returns a subset of the directory entries.
    ///
    elle::Status	Directory::Consult(context::Directory*	context,
					   const nucleus::Index& index,
					   const nucleus::Size&	size,
					   nucleus::Range<nucleus::Entry>&
					     range)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(context->rights->record.permissions & nucleus::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // consult the directory catalog.
      if (context->contents->content->Consult(index,
					      size,
					      range) == elle::StatusError)
	escape("unable to consult the directory");

      leave();
    }

    ///
    /// this method renames an entry.
    ///
    elle::Status	Directory::Rename(context::Directory*	context,
					  const path::Slice&	from,
					  const path::Slice&	to)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(context->rights->record.permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // rename the entry.
      if (context->contents->content->Rename(from, to) == elle::StatusError)
	escape("unable to rename the directory's entry");

      leave();
    }

    ///
    /// this method removes an entry.
    ///
    elle::Status	Directory::Remove(context::Directory*	context,
					  const path::Slice&	name)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(context->rights->record.permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // remove the entry.
      if (context->contents->content->Remove(name) == elle::StatusError)
	escape("unable to remove the directory's entry");

      leave();
    }

    ///
    /// this store the modifications applied onto the directory context.
    ///
    elle::Status	Directory::Store(context::Directory*	context)
    {
      user::User*	user;

      enter();

      // close the catalog.
      if (Contents::Close(context) == elle::StatusError)
	escape("unable to close the contents");

      // store the object.
      if (Object::Store(context) == elle::StatusError)
	escape("unable to store the object");

      leave();
    }

    ///
    /// this method discards the modifications applied onto the context.
    ///
    elle::Status	Directory::Discard(context::Directory*	context)
    {
      enter();

      // discard the object's modifications.
      if (Object::Discard(context) == elle::StatusError)
	escape("unable to discard the object modifications");

      leave();
    }

    ///
    /// this method removes the object along with the blocks attached to it.
    ///
    /// note that, in theory, the directory object could be destroy although
    /// it contains references to sub-entries. it is therefore the
    /// responsability of the caller to destroy the entries.
    ///
    elle::Status	Directory::Destroy(context::Directory*	context)
    {
      user::User*	user;
      nucleus::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context->rights->role != nucleus::RoleOwner)
	escape("unable to destroy a not-owned object");

      // destroy the contents.
      if (Contents::Destroy(context) == elle::StatusError)
	escape("unable to destroy the contents");

      // destroy the object.
      if (Object::Destroy(context) == elle::StatusError)
	escape("unable to destroy the object");

      leave();
    }

  }
}
