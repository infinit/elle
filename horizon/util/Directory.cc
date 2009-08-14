//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/util/Directory.cc
//
// created       julien quintard   [sat aug  1 21:11:57 2009]
// updated       julien quintard   [tue aug 11 00:55:10 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/util/Directory.hh>

namespace pig
{
  namespace util
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Directory::Create(etoile::core::Object& directory,
					  const elle::crypto::KeyPair& user)
    {
      // create the directory.
      if (directory.Create(ComponentDirectory, user) == StatusError)
	escape("unable to create the directory object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Destroy(etoile::core::Object& directory)
    {
      Address		address;

      // retrieve the directory address.
      if (directory.Self(address) == StatusError)
	escape("unable to seal the directory");

      // destroy the object block.
      if (Hole::Destroy(address) == StatusError)
	escape("unable to destroy the directory object block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Store(etoile::core::Object& directory)
    {
      Address		address;

      // retrieve the directory address.
      if (directory.Self(address) == StatusError)
	escape("unable to seal the directory");

      // store the block.
      if (Hole::Put(address, directory) == StatusError)
	escape("unable to store the directory block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Size(const
					  etoile::core::Object& directory,
					Natural32&		size)
    {
      Catalog			catalog;

      // check if there is a linked catalog.
      if (directory.data.contents.type == Contents::TypeNone)
	{
	  size = 0;
	}
      else
	{
	  // load the catalog.
	  if (Hole::Get(directory.data.contents.address,
			catalog) == StatusError)
	    escape("unable to load the catalog");

	  // retrieve the number of entries.
	  if (catalog.Size(size) == StatusError)
	    escape("unable to retrieve the number of catalog entries");
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Add(etoile::core::Object&	directory,
				       const String&		name,
				       const Address&		address,
				       const PrivateKey&	k)
    {
      Catalog			catalog;
      Address			a;

      // check if there is a linked catalog. if not, the default local
      // catalog is used.
      if (directory.data.contents.type != Contents::TypeNone)
	{
	  // load the catalog.
	  if (Hole::Get(directory.data.contents.address,
			catalog) == StatusError)
	    escape("unable to load the catalog");
	}

      // add the entry in the catalog.
      if (catalog.Add(name, address) == StatusError)
	escape("unable to add the entry in the catalog");

      printf("XXX\n");

      catalog.Dump();

      // compute the address.
      if (catalog.Self(a) == StatusError)
	escape("unable to seal the catalog");

      printf("XXX\n");

      // store the new catalog.
      if (Hole::Put(a, catalog) == StatusError)
	escape("unable to store the catalog");

      printf("XXX\n");

      // set the new catalog.
      if (directory.Update(k, a) == StatusError)
	escape("unable to update the directory");

      // XXX[wrong but just to set a size]
      // set a directory size.
      // XXX directory.meta.status.size += 1;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Remove(etoile::core::Object& directory,
					  const String&		name,
					  const PrivateKey&	k)
    {
      Catalog			catalog;
      Natural32			size;
      Address			address;

      // check if there is a linked catalog.
      if (directory.data.contents.type == Contents::TypeNone)
	escape("no catalog found");

      // load the catalog.
      if (Hole::Get(directory.data.contents.address,
		    catalog) == StatusError)
	escape("unable to load the catalog");

      // remove the entry from the catalog.
      if (catalog.Remove(name) == StatusError)
	escape("unable to remove the entry from the catalog");

      // check the size.
      if (catalog.Size(size) == StatusError)
	escape("unable to retrieve the catalog size");

      if (size != 0)
	{
	  Address		self;

	  // seal the catalog.
	  if (catalog.Self(self) == StatusError)
	    escape("unable to seal the catalog");

	  // XXX[though in the real DHT, CHBs are not destroyed but expire]
	  // store the new catalog.
	  if (Hole::Put(self, catalog) == StatusError)
	    escape("unable to store the catalog");

	  // set the address to the catalog's.
	  address = self;
	}
      else
	{
	  // no need for a catalog since no entry.
	  address = Address::Null;
	}

      // set the new catalog.
      if (directory.Update(k, address) == StatusError)
	escape("unable to update the directory");

      // XXX[wrong but just to set a size]
      // set a directory size.
      // XXX directory.meta.status.size -= 1;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Lookup(const
					    etoile::core::Object& directory,
					  const String&		name,
					  Address&		address)
    {
      Catalog			catalog;

      // check if there is a linked catalog.
      if (directory.data.contents.type == Contents::TypeNone)
	escape("no directory entry found");

      // load the catalog.
      if (Hole::Get(directory.data.contents.address,
		    catalog) == StatusError)
	escape("unable to load the catalog");

      // lookup in the catalog.
      if (catalog.Lookup(name, address) == StatusError)
	escape("unable to locate the given name in the catalog");

      leave();
    }

  }
}
