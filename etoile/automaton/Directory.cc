//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 13:22:27 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/automaton/Directory.hh>
#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Contents.hh>
#include <etoile/automaton/Rights.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a directory object within the given context.
    ///
    elle::Status	Directory::Create(
			  gear::Directory&			context)
    {
      nucleus::Address	address;

      enter();

      // create the directory.
      if (context.object.Create(
	    nucleus::GenreDirectory,
	    agent::Agent::Identity.pair.K) == elle::StatusError)
	escape("unable to create the directory object");

      // bind the object to its address i.e this will never changed.
      if (context.object.Bind(address) == elle::StatusError)
	escape("unable to bind the object");

      // create the context's location with an initial version number.
      if (context.location.Create(address) == elle::StatusError)
	escape("unable to create the location");

      // set the context's state.
      context.state = gear::Context::StateCreated;

      leave();
    }

    ///
    /// this method loads an existing directory object identified by the
    /// given location.
    ///
    elle::Status	Directory::Load(
			  gear::Directory&			context)
    {
      enter();

      // return if the context has already been loaded.
      if (context.state != gear::Context::StateUnknown)
	leave();

      // load the object.
      if (Object::Load(context) == elle::StatusError)
	escape("unable to fetch the object");

      // check that the object is a directory.
      if (context.object.meta.genre != nucleus::GenreDirectory)
	escape("this object does not seem to be a directory");

      // set the context's state.
      context.state = gear::Context::StateLoaded;

      leave();
    }

    ///
    /// this method adds an entry to the directory.
    ///
    elle::Status	Directory::Add(
		          gear::Directory&			context,
			  const path::Slice&			name,
			  const nucleus::Address&		address)
    {
      nucleus::Entry*	entry;
      nucleus::Size	size;

      enterx(instance(entry));

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if ((context.rights.permissions & nucleus::PermissionWrite) !=
	  nucleus::PermissionWrite)
	escape("the user does not seem to have the permission to write "
	       "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // allocate a new directory entry.
      entry = new nucleus::Entry(name, address);

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // add the entry in the directory.
      if (context.contents->content->Add(entry) == elle::StatusError)
	escape("unable to add the entry in the directory");

      // stop tracking since the entry has been properly added to the catalog.
      waive(entry);

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      // update the object data section.
      if (context.object.Update(
	    context.object.author,
	    context.object.data.contents,
	    size) == elle::StatusError)
	escape("unable to update the object data section");

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method returns the directory entry associated with the
    /// given name.
    ///
    elle::Status	Directory::Lookup(
			  gear::Directory&			context,
			  const path::Slice&			name,
			  nucleus::Entry*&			entry)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::PermissionRead) !=
	  nucleus::PermissionRead)
	escape("the user does not seem to have the permission to read "
	       "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // look up the entry.
      if (context.contents->content->Lookup(name,
					    entry) == elle::StatusError)
	escape("unable to find the entry in the directory");

      leave();
    }

    ///
    /// this method returns a subset of the directory entries.
    ///
    /// the subset is represented by the arguments _index_ and _size_.
    ///
    elle::Status	Directory::Consult(
			  gear::Directory&			context,
			  const nucleus::Index&			index,
			  const nucleus::Size&			size,
			  nucleus::Range<nucleus::Entry>&	range)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::PermissionRead) !=
	  nucleus::PermissionRead)
	escape("the user does not seem to have the permission to read "
	       "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // consult the directory catalog.
      if (context.contents->content->Consult(index,
					     size,
					     range) == elle::StatusError)
	escape("unable to consult the directory");

      leave();
    }

    ///
    /// this method renames a directory entry.
    ///
    elle::Status	Directory::Rename(
			  gear::Directory&			context,
			  const path::Slice&			from,
			  const path::Slice&			to)
    {
      nucleus::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::PermissionWrite) !=
	  nucleus::PermissionWrite)
	escape("the user does not seem to have the permission to write "
	       "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // rename the entry.
      if (context.contents->content->Rename(from, to) == elle::StatusError)
	escape("unable to rename the directory's entry");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      // update the object data section though renaming an entry may
      // not impact the object's data size.
      if (context.object.Update(
	    context.object.author,
	    context.object.data.contents,
	    size) == elle::StatusError)
	escape("unable to update the object data section");

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status	Directory::Remove(
			  gear::Directory&			context,
			  const path::Slice&			name)
    {
      nucleus::Size	size;

      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if ((context.rights.permissions & nucleus::PermissionWrite) !=
	  nucleus::PermissionWrite)
	escape("the user does not seem to have the permission to write "
	       "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // check that the content exists: the subject may have lost the
      // read permission between the previous check and the Contents::Open().
      if (context.contents->content == NULL)
	escape("the user does not seem to be able to operate on this "
	       "directory");

      // remove the entry.
      if (context.contents->content->Remove(name) == elle::StatusError)
	escape("unable to remove the directory's entry");

      // retrieve the new contents's size.
      if (context.contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      // update the object data section.
      if (context.object.Update(
	    context.object.author,
	    context.object.data.contents,
	    size) == elle::StatusError)
	escape("unable to update the object data section");

      // set the context's state.
      context.state = gear::Context::StateModified;

      leave();
    }

    ///
    /// this method is called whenever the context is being closed without
    /// any modification having been performed.
    ///
    elle::Status	Directory::Discard(
			  gear::Directory&			context)
    {
      enter();

      // set the context's state.
      context.state = gear::Context::StateDiscarded;

      leave();
    }

    ///
    /// this method destroys the directory by marking all the blocks
    /// as dying for future removal.
    ///
    elle::Status	Directory::Destroy(
			  gear::Directory&			context)
    {
      enter();

      // determine the rights.
      if (Rights::Determine(context) == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (context.rights.role != nucleus::RoleOwner)
	escape("the user does not seem to have the permission to destroy "
	       "this directory");

      // open the contents.
      if (Contents::Open(context) == elle::StatusError)
	escape("unable to open the contents");

      // destroy the contents.
      if (Contents::Destroy(context) == elle::StatusError)
	escape("unable to destroy the contents");

      // destroy the object-related information.
      if (Object::Destroy(context) == elle::StatusError)
	escape("unable to destroy the object");

      // set the context's state.
      context.state = gear::Context::StateDestroyed;

      leave();
    }

    ///
    /// this method terminates the automaton by making the whole directory
    /// consistent according to the set of modifications having been performed.
    ///
    elle::Status	Directory::Store(
			  gear::Directory&			context)
    {
      enter();

      // close the contents.
      if (Contents::Close(context) == elle::StatusError)
	escape("unable to close the contents");

      // store the object-related information.
      if (Object::Store(context) == elle::StatusError)
	escape("unable to store the object");

      // set the context's state.
      context.state = gear::Context::StateStored;

      leave();
    }

  }
}
