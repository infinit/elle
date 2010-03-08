//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Catalog.cc
//
// created       julien quintard   [mon aug 17 11:46:30 2009]
// updated       julien quintard   [wed mar  3 16:06:26 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Catalog.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method is different from the Load() method found in the
    /// components because its role is not to load a catalog if one exists
    /// or report an error otherwise but to create one if necessary. as such,
    /// if this method returns successfully, a catalog will be ready to be
    /// used, either empty or not.
    ///
    Status		Catalog::Open(context::Directory*	context)
    {
      enter();

      // if the catalog is already opened, return.
      if (context->catalog != NULL)
	leave();

      // check if there exists a catalog. if so, load the block.
      if (context->object->data.contents != hole::Address::Null)
	{
	  // determine the rights the current user has on this object.
	  if (Rights::Determine(context) == StatusError)
	    escape("unable to determine the user's rights");

	  // load the block.
	  if (depot::Depot::Get(context->object->data.contents,
				context->catalog) == StatusError)
	    escape("unable to load the catalog");

	  // decrypt the contents i.e the catalog.
	  if (context->catalog->Decrypt(context->rights->key) == StatusError)
	    escape("unable to decrypt the contents");
	}
      else
	{
	  // otherwise create a new contents.
	  context->catalog = new core::Contents<core::Catalog>;

	  // create the contents.
	  if (context->catalog->Create() == StatusError)
	    escape("unable to create the contents");
	}

      leave();
    }

    ///
    /// this method releases the memory used if the catalog is unchanged
    /// or new but empty. otherwise, the new catalog address is computed
    /// and the directory object is updated.
    ///
    /// \todo on pourrait remplacer dans components Catalog/../.. par
    ///   Contents et fournir des fonctions templatees. par exemple, Close
    ///   sera le meme pour les trois: autant les rassembler.
    ///
    Status		Catalog::Close(context::Directory*	context)
    {
      SecretKey			key;
      core::Offset		size;

      enter();

      //
      // first, the method verifies that a catalog is present and that it
      // has changed so that the modifications need to be published.
      //
      {
	// if there is no loaded catalog, then there is nothing to do.
	if (context->catalog == NULL)
	  leave();

	// retrieve the catalog's size.
	if (context->catalog->content->Size(size) == StatusError)
	  escape("unable to retrieve the catalog's size");

	// if the catalog has not changed, or if the catalog is empty,
	// delete it.
	if ((size == 0) || (context->catalog->content->state == StateClean))
	  {
	    // release the catalog's memory.
	    delete context->catalog;

	    // reset the pointer.
	    context->catalog = NULL;

	    leave();
	  }
      }

      //
      // second, the author is generated according to the current user
      // and her permissions on the object.
      //
      {
	// forge the author.
	if (Author::Forge(context) == StatusError)
	  escape("unable to forge an author");
      }

      //
      // third, a new key is generated, the catalog is re-encrypted with
      // this key while the object data is updated since the catalog has
      // changed.
      //
      {
	Digest			fingerprint;

	// if a new catalog exists, generate a secret key.
	if (key.Generate() == StatusError)
	  escape("unable to generate the secret key");

	// compute a fingerprint of the key.
	if (OneWay::Hash(key, fingerprint) == StatusError)
	  escape("unable to compute a fingerprint of the key");

	// finally, encrypt the contents.
	if (context->catalog->Encrypt(key) == StatusError)
	  escape("unable to encrypt the contents");

	// bind the catalog i.e seal it by computing its address.
	if (context->catalog->Bind() == StatusError)
	  escape("unable to bind the catalog");

	// update the object data section.
	if (context->object->Update(*context->author,
				    context->catalog->address,
				    size,
				    fingerprint) == StatusError)
	  escape("unable to update the object");
      }

      //
      // finally, since the data has been re-encrypted, the key must be
      // distributed to the users having been granted such a permission.
      //
      {
	// open the access.
	if (Access::Open(context) == StatusError)
	  escape("unable to open the access");

	// upgrade the access entries with the new key.
	if (Access::Upgrade(context, key) == StatusError)
	  escape("unable to upgrade the accesses");
      }

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Lookup(context::Directory*	context,
					const String&		name,
					hole::Address&		address)
    {
      enter();

      // look up the entry.
      if (context->catalog->content->Lookup(name, address) == StatusError)
	escape("unable to find the entry in the catalog");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Add(context::Directory*	context,
				     const String&		name,
				     const hole::Address&	address)
    {
      enter();

      // add the entry in the catalog.
      if (context->catalog->content->Add(name, address) == StatusError)
	escape("unable to add the entry in the catalog");

      leave();
    }

  }
}
