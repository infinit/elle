//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/XXX/Contents.hxx
//
// created       julien quintard   [mon apr  5 15:13:38 2010]
// updated       julien quintard   [wed jun 22 14:33:28 2011]
//

#ifndef ETOILE_COMPONENTS_CONTENTS_HXX
#define ETOILE_COMPONENTS_CONTENTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

#include <etoile/components/Author.hh>
#include <etoile/components/Access.hh>

#include <etoile/depot/Depot.hh>

namespace etoile
{
  namespace components
  {

    ///
    /// this method opens a contents i.e loads if if presents or creates a
    /// new empty one.
    ///
    template <typename T>
    elle::Status	Contents::Open(T*			context)
    {
      user::User*	user;

      enter();

      // if the contents is already opened, return.
      if (context->contents != NULL)
	leave();

      // load the current user.
      if (user::User::Instance(user) == elle::StatusError)
	escape("unable to load the user");

      // otherwise create a new contents.
      context->contents = new nucleus::Contents<typename T::Content>;

      // check if there exists a contents. if so, load the block.
      if (context->object->data.contents != nucleus::Address::Null)
	{
	  // load the block.
	  if (depot::Depot::Pull(user->application->network,
				 context->object->data.contents,
				 nucleus::Version::Last,
				 *context->contents) == elle::StatusError)
	    escape("unable to load the contents");

	  // determine the rights the current user has on this object.
	  if (Rights::Determine(context) == elle::StatusError)
	    escape("unable to determine the user's rights");

	  // verify that the user has the read permission.
	  if ((context->rights->record.permissions &
	       nucleus::PermissionRead) == 0)
	    escape("the user does not have the right to read the contents");

	  // decrypt the contents i.e the contents.
	  if (context->contents->Decrypt(context->rights->key) ==
	      elle::StatusError)
	    escape("unable to decrypt the contents");
	}
      else
	{
	  // place the block in the application's network.
	  if (context->contents->Place(user->application->network) ==
	      elle::StatusError)
	    escape("unable to place the object");

	  // create the contents.
	  if (context->contents->Create() == elle::StatusError)
	    escape("unable to create the contents");
	}

      leave();
    }

    ///
    /// this method destroys the contents.
    ///
    template <typename T>
    elle::Status	Contents::Destroy(T*			context)
    {
      enter();

      // if the block is present.
      if (context->object->data.contents != nucleus::Address::Null)
	{
	  // forge the author which will be attached to the modified object.
	  if (Author::Forge(context) == elle::StatusError)
	    escape("unable to forge an author");

	  // record the block as needed to be removed.
	  if (context->bucket.Wipe(
	        context->object->data.contents) == elle::StatusError)
	    escape("unable to record the block in the bucket");

	  // update the object's data section with the null address.
	  if (context->object->Update(
	        *context->author,
		nucleus::Address::Null,
		0,
		elle::Digest::Null) == elle::StatusError)
	    escape("unable to update the object's data section");

	  // release the contents's memory.
	  delete context->contents;

	  // reset the pointer.
	  context->contents = NULL;

	  //
	  // finally, since the data has changed (is now empty), the tokens
	  // must be reinitialized to null.
	  //

	  // open the access.
	  if (Access::Open(context) == elle::StatusError)
	    escape("unable to open the access");

	  // upgrade the access entries with a null key.
	  if (Access::Upgrade(context, elle::SecretKey::Null) ==
	      elle::StatusError)
	    escape("unable to upgrade the accesses");
	}

      leave();
    }

    ///
    /// this method releases the memory used if the contents is unchanged.
    ///
    /// otherwise, the new contents address is computed and the object
    /// object is updated.
    ///
    template <typename T>
    elle::Status	Contents::Close(T*			context)
    {
      elle::SecretKey	key;
      nucleus::Size	size;

      enter();

      //
      // first, check if operations need to be performed.
      //
      {
	// if there is no loaded contents, then there is nothing to do.
	if (context->contents == NULL)
	  leave();

	// if the contents has not changed, delete it.
	if (context->contents->content->state == nucleus::StateClean)
	  {
	    // release the contents's memory.
	    delete context->contents;

	    // reset the pointer.
	    context->contents = NULL;

	    leave();
	  }
      }

      // retrieve the contents's size.
      if (context->contents->content->Capacity(size) == elle::StatusError)
	escape("unable to retrieve the contents's size");

      //
      // at this point, this contents is known to have been modified.
      //
      {
	// forge the author which will be attached to the modified object.
	if (Author::Forge(context) == elle::StatusError)
	  escape("unable to forge an author");

	// modify the object according to the content of the contents.
	if (size == 0)
	  {
	    //
	    // if the contents became empty after removals, the
	    // object should no longer point to any contents block while
	    // the old block should be deleted.
	    //

	    // destroy the contents block and update the object accordingly.
	    if (Contents::Destroy(context) == elle::StatusError)
	      escape("unable to destroy the contents");
	  }
	else
	  {
	    //
	    // otherwise, the new contents address needs to be computed while
	    // the object must be updated accordingly.
	    //
	    elle::Digest	fingerprint;
	    nucleus::Address	address;

	    // delete the previous data block, should one exist.
	    if (context->object->data.contents != nucleus::Address::Null)
	      {
		// record the contents so that it is destroyed.
		if (context->bucket.Wipe(
		      context->object->data.contents) == elle::StatusError)
		  escape("unable to record the contents block in the bucket");
	      }

	    // generate a secret key.
	    if (key.Generate() == elle::StatusError)
	      escape("unable to generate the secret key");

	    // compute a fingerprint of the key.
	    if (elle::OneWay::Hash(key, fingerprint) == elle::StatusError)
	      escape("unable to compute a fingerprint of the key");

	    // encrypt the contents.
	    if (context->contents->Encrypt(key) == elle::StatusError)
	      escape("unable to encrypt the contents");

	    // bind the contents i.e seal it by computing its address.
	    if (context->contents->Bind(address) == elle::StatusError)
	      escape("unable to bind the contents");

	    // set the contents as clean.
	    context->contents->content->state = nucleus::StateClean;

	    // record the contents so that it is published.
	    if (context->bucket.Push(
		  address,
		  context->contents) == elle::StatusError)
	      escape("unable to record the contents block in the bucket");

	    // update the object data section.
	    if (context->object->Update(
                  *context->author,
		  address,
		  size,
		  fingerprint) == elle::StatusError)
	      escape("unable to update the object data section");

	    //
	    // finally, since the data has been re-encrypted, the key must be
	    // distributed to the users having been granted the read
	    // permission.
	    //

	    // open the access.
	    if (Access::Open(context) == elle::StatusError)
	      escape("unable to open the access");

	    // upgrade the access entries with the new key.
	    if (Access::Upgrade(context, key) == elle::StatusError)
	      escape("unable to upgrade the accesses");
	  }
      }

      leave();
    }

  }
}

#endif
