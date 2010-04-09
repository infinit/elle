//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Contents.hxx
//
// created       julien quintard   [mon apr  5 15:13:38 2010]
// updated       julien quintard   [wed apr  7 21:34:00 2010]
//

#ifndef ETOILE_COMPONENTS_CONTENTS_HXX
#define ETOILE_COMPONENTS_CONTENTS_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Author.hh>

namespace etoile
{
  namespace components
  {

    ///
    /// this method opens a contents i.e loads if if presents or creates a
    /// new empty one.
    ///
    template <typename T>
    Status		Contents::Open(T*			context)
    {
      enter();

      // if the contents is already opened, return.
      if (context->contents != NULL)
	leave();

      // check if there exists a contents. if so, load the block.
      if (context->object->data.contents != hole::Address::Null)
	{
	  // load the block.
	  if (depot::Depot::Get(context->object->data.contents,
				context->contents) == StatusError)
	    escape("unable to load the contents");

	  // determine the rights the current user has on this object.
	  if (Rights::Determine(context) == StatusError)
	    escape("unable to determine the user's rights");

	  // verify that the user has the read permission.
	  if ((context->rights->permissions & kernel::PermissionRead) == 0)
	    escape("the user does not have the right to read the contents");

	  // decrypt the contents i.e the contents.
	  if (context->contents->Decrypt(context->rights->key) == StatusError)
	    escape("unable to decrypt the contents");
	}
      else
	{
	  // otherwise create a new contents.
	  context->contents = new kernel::Contents<typename T::Content>;

	  // create the contents.
	  if (context->contents->Create() == StatusError)
	    escape("unable to create the contents");
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
    Status		Contents::Close(T*			context)
    {
      SecretKey		key;
      kernel::Size	size;

      enter();

      //
      // first, check if operations need to be performed.
      //
      {
	// if there is no loaded contents, then there is nothing to do.
	if (context->contents == NULL)
	  leave();

	// if the contents has not changed, delete it.
	if (context->contents->content->state == kernel::StateClean)
	  {
	    // release the contents's memory.
	    delete context->contents;

	    // reset the pointer.
	    context->contents = NULL;

	    leave();
	  }
      }

      // retrieve the contents's size.
      if (context->contents->content->Capacity(size) == StatusError)
	escape("unable to retrieve the contents's size");

      //
      // at this point, this contents is known to have been modified.
      //
      {
	// forge the author which will be attached to the modified object.
	if (Author::Forge(context) == StatusError)
	  escape("unable to forge an author");

	// modify the object according to the content of the contents.
	if (size == 0)
	  {
	    //
	    // if the contents became empty after removals, the
	    // object should no longer point to any contents block while
	    // the old block should be deleted.
	    //
	    Digest	fingerprint;

	    // record the contents so that it is removed.
	    if (context->bucket.Record(
		  &context->object->data.contents) == StatusError)
	      escape("unable to record the contents block in the bucket");

	    // update the object's data section with the null address.
	    if (context->object->Update(
		  *context->author,
		  hole::Address::Null,
		  0,
		  fingerprint) == StatusError)
	      escape("unable to update the object's data section");

	    // release the contents's memory.
	    delete context->contents;

	    // reset the pointer.
	    context->contents = NULL;
	  }
	else
	  {
	    //
	    // otherwise, the new contents address needs to be computed while
	    // the object must be updated accordingly.
	    //
	    Digest	fingerprint;

	    // generate a secret key.
	    if (key.Generate() == StatusError)
	      escape("unable to generate the secret key");

	    // compute a fingerprint of the key.
	    if (OneWay::Hash(key, fingerprint) == StatusError)
	      escape("unable to compute a fingerprint of the key");

	    // encrypt the contents.
	    if (context->contents->Encrypt(key) == StatusError)
	      escape("unable to encrypt the contents");

	    // bind the contents i.e seal it by computing its address.
	    if (context->contents->Bind() == StatusError)
	      escape("unable to bind the contents");

	    // record the contents so that it is published.
	    if (context->bucket.Record(context->contents) == StatusError)
	      escape("unable to record the contents block in the bucket");

	    // update the object data section.
	    if (context->object->Update(
                  *context->author,
		  context->contents->address,
		  size,
		  fingerprint) == StatusError)
	      escape("unable to update the object data section");

	    // set the contents as clean.
	    context->contents->content->state = kernel::StateClean;

	    //
	    // finally, since the data has been re-encrypted, the key must be
	    // distributed to the users having been granted the read
	    // permission.
	    //

	    // open the access.
	    if (Access::Open(context) == StatusError)
	      escape("unable to open the access");

	    // upgrade the access entries with the new key.
	    if (Access::Upgrade(context, key) == StatusError)
	      escape("unable to upgrade the accesses");
	  }
      }

      leave();
    }

  }
}

#endif
