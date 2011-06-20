//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Directory.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [sun jun 19 17:58:28 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Directory.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/depot/Depot.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Directory::Directory():
      Object(NatureDirectory),

      contents(NULL)
    {
    }

    ///
    /// the destructor
    ///
    Directory::~Directory()
    {
      // release the contents.
      if (this->contents != NULL)
	delete this->contents;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a directory object.
    ///
    elle::Status	Directory::Create()
    {
      enter();

      // create the directory.
      if (this->object.Create(nucleus::GenreDirectory,
			      agent::Agent::K) == elle::StatusError)
	escape("unable to create the directory object");

      leave();
    }

    ///
    /// this method fetches the directory object identified by the given
    /// location.
    ///
    elle::Status	Directory::Load(const nucleus::Location& location)
    {
      enter();

      // fetch the object.
      if (Object::Load(location) == elle::StatusError)
	escape("unable to fetch the object");

      // check that the object is a directory.
      if (this->object.meta.genre != nucleus::GenreDirectory)
	escape("this object does not seem to be a directory");

      leave();
    }

    ///
    /// this method opens the contents, allocating it if necessary.
    ///
    elle::Status	Directory::Open()
    {
      enter();

      // if the contents is already opened, return.
      if (this->contents != NULL)
	leave();

      // otherwise create a new contents.
      this->contents = new nucleus::Contents<nucleus::Catalog>;

      // check if there exists a contents. if so, fetch it.
      if (this->object.data.contents != nucleus::Address::Null)
	{
	  // load the contents.
	  if (depot::Depot::Pull(this->object.data.contents,
				 nucleus::Version::Any,
				 *this->contents) == elle::StatusError)
	    escape("unable to load the contents");

	  // determine the rights the current user has over the object.
	  if (this->Rights() == elle::StatusError)
	    escape("unable to determine the user's rights");

	  // verify that the user has the read permission.
	  if (!(this->rights->permissions & nucleus::PermissionRead))
	    escape("the user does not have the right to read the content");

	  // decrypt the contents i.e the catalog.
	  if (this->contents->Decrypt(this->rights->key) == elle::StatusError)
	    escape("unable to decrypt the contents");
	}
      else
	{
	  // create the contents.
	  if (this->contents->Create() == elle::StatusError)
	    escape("unable to create the contents");
	}

      leave();
    }

    ///
    /// this method adds an entry to the directory.
    ///
    elle::Status	Directory::Add(const path::Slice&	name,
				       const nucleus::Address&	address)
    {
      nucleus::Entry*	entry;

      enter(instance(entry));

      // determine the rights.
      if (this->Rights() == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the write the catalog.
      if (!(this->rights->permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (this->Open() == elle::StatusError)
	escape("unable to open the contents");

      // allocate a new entry.
      entry = new nucleus::Entry(name, address);

      // add the entry in the directory.
      if (this->contents->content->Add(entry) == elle::StatusError)
	escape("unable to add the entry in the directory");

      // stop tracking since the entry has been properly added to the catalog.
      waive(entry);

      leave();
    }

    ///
    /// this method returns the address corresponding to the given name.
    ///
    elle::Status	Directory::Lookup(const path::Slice&	name,
					  nucleus::Entry*&	entry)
    {
      enter();

      // determine the rights.
      if (this->Rights() == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(this->rights->permissions & nucleus::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (this->Open() == elle::StatusError)
	escape("unable to open the contents");

      // look up the entry.
      if (this->contents->content->Lookup(name, entry) == elle::StatusError)
	escape("unable to find the entry in the directory");

      leave();
    }

    ///
    /// this method returns a subset of the directory entries.
    ///
    elle::Status	Directory::Consult(
			  const nucleus::Index&			index,
			  const nucleus::Size&			size,
			  nucleus::Range<nucleus::Entry>&	range)
    {
      enter();

      // determine the rights.
      if (this->Rights() == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(this->rights->permissions & nucleus::PermissionRead))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (this->Open() == elle::StatusError)
	escape("unable to open the contents");

      // consult the directory catalog.
      if (this->contents->content->Consult(index,
					   size,
					   range) == elle::StatusError)
	escape("unable to consult the directory");

      leave();
    }

    ///
    /// this method renames a directory entry.
    ///
    elle::Status	Directory::Rename(const path::Slice&	from,
					  const path::Slice&	to)
    {
      enter();

      // determine the rights.
      if (this->Rights() == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(this->rights->permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (this->Open() == elle::StatusError)
	escape("unable to open the contents");

      // rename the entry.
      if (this->contents->content->Rename(from, to) == elle::StatusError)
	escape("unable to rename the directory's entry");

      leave();
    }

    ///
    /// this method removes a directory entry.
    ///
    elle::Status	Directory::Remove(const path::Slice&	name)
    {
      enter();

      // determine the rights.
      if (this->Rights() == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user has the right the read the catalog.
      if (!(this->rights->permissions & nucleus::PermissionWrite))
	escape("unable to perform the operation without the permission");

      // open the contents.
      if (this->Open() == elle::StatusError)
	escape("unable to open the contents");

      // remove the entry.
      if (this->contents->content->Remove(name) == elle::StatusError)
	escape("unable to remove the directory's entry");

      leave();
    }

    ///
    /// XXX mark all blocks as StateDying
    ///
    elle::Status	Directory::Destroy()
    {
      enter();

      // determine the rights.
      if (this->Rights() == elle::StatusError)
	escape("unable to determine the rights");

      // check if the current user is the object owner.
      if (this->rights->role != nucleus::RoleOwner)
	escape("unable to destroy a not-owned object");

      // XXX mark all blocks as dying

      // destroy the object-related information.
      if (Object::Destroy() == elle::StatusError)
	escape("unable to destroy the object");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Directory::Close()
    {
      elle::SecretKey	key;
      nucleus::Size	size;

      enter();

      //
      // first, check if operations need to be performed.
      //
      {
	// if there is no loaded contents, then there is nothing to do.
	if (this->contents == NULL)
	  leave();

	// if the contents has not changed, delete it.
	if (this->contents->content->state == nucleus::StateClean)
	  {
	    // release the contents's memory.
	    delete this->contents;

	    // reset the pointer.
	    this->contents = NULL;

	    leave();
	  }
      }

      // retrieve the contents's size.
      if (this->contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      //
      // at this point, this contents is known to have been modified.
      //
      {
	// create the author which will be embedded within the object.
	if (this->Author() == elle::StatusError)
	  escape("unable to create the author");

	// modify the object according to the content of the contents.
	if (size == 0)
	  {
	    //
	    // if the contents became empty after removals, the
	    // object should no longer point to any contents block while
	    // the old block should be deleted.
	    //

	    /// XXX \todo note that for now nothing is done as every object
	    /// is assumed to make use of history.
	  }
	else
	  {
	    //
	    // otherwise, the new contents address needs to be computed while
	    // the object must be updated accordingly.
	    //
	    nucleus::Address	address;

	    // delete the previous data block, should one exist.
	    if (this->object.data.contents != nucleus::Address::Null)
	      {
		/// XXX \todo note that for now nothing is done as every object
		/// is assumed to make use of history. therefore the old
		/// data block is not marked as dying.
	      }

	    // generate a secret key.
	    if (key.Generate() == elle::StatusError)
	      escape("unable to generate the secret key");

	    // encrypt the contents.
	    if (this->contents->Encrypt(key) == elle::StatusError)
	      escape("unable to encrypt the contents");

	    // bind the contents i.e seal it by computing its address.
	    if (this->contents->Bind(address) == elle::StatusError)
	      escape("unable to bind the contents");

	    // set the contents as updated.
	    this->contents->content->state = nucleus::StateUpdated;

	    // update the object data section.
	    if (this->object.Update(*this->author,
				    address,
				    size) == elle::StatusError)
	      escape("unable to update the object data section");

	    //
	    // finally, since the data has been re-encrypted, the key must be
	    // distributed to the users having been granted the read
	    // permission.
	    //

	    /* XXX
	    // upgrade the access entries with the new key.
	    if (this->Upgrade(key) == elle::StatusError)
	      escape("unable to upgrade the accesses");
	    */
	  }
      }

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Directory::Store()
    {
      enter();

      // close the catalog.
      if (this->Close() == elle::StatusError)
	escape("unable to close the contents");

      // store the object-related information.
      if (this->Store() == elle::StatusError)
	escape("unable to store the object");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents along the the inherited object context.
    ///
    elle::Status	Directory::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Directory]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the inherited object");

      // dump the contents.
      if (this->contents != NULL)
	{
	  if (this->contents->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the contents");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[Contents] " << elle::none << std::endl;
	}

      leave();
    }

//
// ---------- transcribable ---------------------------------------------------
//

    ///
    /// this method records in the given transcript the blocks which should
    /// be added to or removed from the network.
    ///
    elle::Status	Directory::Transcribe(journal::Transcript& transcript)
      const
    {
      enter();

      // XXX
      /* XXX
      transcript.Push(address, block);
      transcript.Wipe(
      */

      leave();
    }

  }
}
