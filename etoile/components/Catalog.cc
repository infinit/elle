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
// updated       julien quintard   [sun aug 23 10:08:37 2009]
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
    Status		Catalog::Open(context::Directory&	context)
    {
      // if the catalog is already opened, return.
      if (context.catalog != NULL)
	leave();

      // allocate a new contents.
      context.catalog = new core::Catalog;

      // check if there exists a catalog. if so, load the block, otherwise
      // leave the catalog new and empty.
      if (context.object->data.contents != hole::Address::Null)
	{
	  // load the block.
	  if (hole::Hole::Get(context.object->data.contents,
			      *context.catalog) == StatusError)
	    escape("unable to load the catalog");
	}

      leave();
    }

    ///
    /// this method releases the memory used if the catalog is unchanged
    /// or new but empty. otherwise, the new catalog address is computed
    /// and the directory object is updated.
    ///
    Status		Catalog::Seal(context::Directory&	context)
    {
      hole::Address	address;
      Natural64		size;
      SecretKey		key;

      // if there is no loaded catalog, then there is nothing to do.
      if (context.catalog == NULL)
	leave();

      // compute the loaded catalog address.
      if (context.catalog->Self(address) == StatusError)
	escape("unable to retrieve the catalog address");

      // retrieve the catalog's size.
      if (context.catalog->Size(size) == StatusError)
	escape("unable to retrieve the catalog's size");

      // if the catalog has not changed, or if the catalog is empty, delete it.
      if ((size == 0) ||
	  (context.object->data.contents == address))
	{
	  // release the catalog's memory.
	  delete context.catalog;

	  // reset the pointer.
	  context.catalog = NULL;

	  leave();
	}

      // if a new catalog exists, generate a secret key.
      if (key.Generate() == StatusError)
	escape("unable to generate the secret key");

      // then seal the catalog.
      //if (context.catalog->Seal(key) == StatusError)
      //escape("unable to seal the catalog");

      // update the object data section.
      // XXX context.object->data

      leave();
    }

  }
}
