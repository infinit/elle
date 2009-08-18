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
// updated       julien quintard   [tue aug 18 16:11:55 2009]
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
    Status		Directory::Load(context::Object&	context,
					const hole::Address&	address)
					
    {
      // load the object.
      if (Object::Load(context, address) == StatusError)
	escape("unable to load the object");

      // set the component.
      context.component = core::ComponentDirectory;

      // initialize the contents.
      context.contents.catalog = NULL;

      // check that the object is a directory.
      if (context.object.meta.status.genre != core::GenreDirectory)
	escape("this object is not a directory");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Store(context::Object&	context)
    {
      // close the catalog.
      if (Catalog::Close(context) == StatusError)
	escape("unable to close the catalog");

      // if there still is a catalog, that means that the system has to
      // store it.
      //
      // XXX \todo here it would be more complicated for multiple-blocks
      //  catalogs.
      /* XXX
      if (context.contents.catalog != NULL)
	{
	  hole::Address	address;

	  // retrieve the block address.
	  if (context.contents.catalog->Self(address) == StatusError)
	    escape("unable to retrieve the catalog's address");

	  // update the object.
	  context.object.data.contents.address = address;
	  /// XXX update size?

	  // store the catalog.
	  // XXX if (hole::Hole::Put(
	  */

      // call the parent store method.
      if (Object::Store(context) == StatusError)
	escape("unable to store the object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Lookup(context::Object&	context,
					  const String&		name,
					  hole::Address&	address)
    {
      // open the catalog.
      if (Catalog::Open(context) == StatusError)
	escape("unable to load the catalog");

      // look up the entry.
      if (context.contents.catalog->Lookup(name, address) == StatusError)
	escape("unable to find the entry in the catalog");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Add(context::Object&		context,
				       const String&		name,
				       const hole::Address&	address)
    {
      // open the catalog.
      if (Catalog::Open(context) == StatusError)
	escape("unable to open the catalog");

      // add the entry in the catalog.
      if (context.contents.catalog->Add(name, address) == StatusError)
	escape("unable to add the entry in the catalog");

      leave();
    }

  }
}
