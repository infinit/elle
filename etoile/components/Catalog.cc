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
// updated       julien quintard   [mon aug 17 12:54:20 2009]
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
    Status		Catalog::Open(context::Object&		context)
    {
      // if the catalog is already opened, return.
      if (context.contents.catalog != NULL)
	leave();

      // allocate a new contents.
      context.contents.catalog = new core::Catalog;

      // check if there exists a catalog. if so, load the block, otherwise
      // leave the catalog new.
      if (context.object.data.contents.address != hole::Address::Null)
	{
	  // load the block.
	  if (hole::Hole::Get(context.object.data.contents.address,
			      *context.contents.catalog) == StatusError)
	    escape("unable to load the catalog");
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Close(context::Object&		context)
    {
      // retrieve the address of the catalog, if there is one.
      if (context.contents.catalog != NULL)
	{
	  hole::Address	address;

	  // retrieve the current catalog address.
	  if (context.contents.catalog->Self(address) == StatusError)
	    escape("unable to retrieve the catalog's address");

	  // compare the new address with the old one.
	  if (context.object.data.contents.address == address)
	    {
	      // the catalog has not changed, needless to keep it since
	      // no blocks have to be stored.
	      delete context.contents.catalog;

	      context.contents.catalog = NULL;
	    }
	}

      leave();
    }

  }
}
