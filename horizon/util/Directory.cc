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
// updated       julien quintard   [tue aug  4 14:04:55 2009]
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
    Status		Directory::Create(etoile::core::Directory& directory,
					  const elle::crypto::KeyPair& user)
    {
      // create the directory.
      if (directory.Create(user) == StatusError)
	escape("unable to create the directory object");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Destroy(etoile::core::Directory& directory)
    {
      // XXX[obviously, this is a simplified one-data-block version]
      // if there is a data block, destroy it as well.
      if (directory.data.references != Address::Null)
	{
	  // XXX[though in the real DHT, CHBs are not destroyed but expire]
	  // destroy the data block.
	  if (Hole::Destroy(directory.data.references) == StatusError)
	    escape("unable to destroy the directory data block");
	}

      // seal the object to obtain its address.
      if (directory.Seal() == StatusError)
	escape("unable to seal the directory");

      // destroy the object block.
      if (Hole::Destroy(directory.address) == StatusError)
	escape("unable to destroy the directory object block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Store(etoile::core::Directory& directory)
    {
      // seal the directory.
      if (directory.Seal() == StatusError)
	escape("unable to seal the directory");

      // store the block.
      if (Hole::Put(directory.address, directory) == StatusError)
	escape("unable to store the directory block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Size(const
					  etoile::core::Directory& directory,
					Natural32&		size)
    {
      Catalog			catalog;

      // check if there is a linked catalog.
      if (directory.data.references == Address::Null)
	{
	  size = 0;
	}
      else
	{
	  // load the catalog.
	  if (Hole::Get(directory.data.references, catalog) == StatusError)
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
    Status		Directory::Add(etoile::core::Directory&	directory,
				       const String&		name,
				       const Address&		address,
				       const PrivateKey&	k)
    {
      Catalog			catalog;

      // check if there is a linked catalog. if not, the default local
      // catalog is used.
      if (directory.data.references != Address::Null)
	{
	  // load the catalog.
	  if (Hole::Get(directory.data.references, catalog) == StatusError)
	    escape("unable to load the catalog");

	  // XXX[though, in the real DHT, data blocks expire but
	  //     are never deleted]
	  // destroy it since no longer used.
	  if (Hole::Destroy(directory.data.references) == StatusError)
	    escape("unable to destroy the data block");
	}

      // add the entry in the catalog.
      if (catalog.Add(name, address) == StatusError)
	escape("unable to add the entry in the catalog");

      // seal the catalog.
      if (catalog.Seal() == StatusError)
	escape("unable to seal the catalog");

      // store the new catalog.
      if (Hole::Put(catalog.address, catalog) == StatusError)
	escape("unable to store the catalog");

      // set the new catalog.
      if (directory.Update(k, catalog.address) == StatusError)
	escape("unable to update the directory");

      // XXX[wrong but just to set a size]
      // set a directory size.
      directory.meta.status.size += 1;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Remove(etoile::core::Directory& directory,
					  const String&		name,
					  const PrivateKey&	k)
    {
      Catalog			catalog;
      Natural32			size;
      Address			address;

      // check if there is a linked catalog.
      if (directory.data.references == Address::Null)
	escape("no catalog found");

      // load the catalog.
      if (Hole::Get(directory.data.references, catalog) == StatusError)
	escape("unable to load the catalog");

      // XXX[though, in the real DHT, data blocks expire but are never deleted]
      // destroy it since no longer used.
      if (Hole::Destroy(directory.data.references) == StatusError)
	escape("unable to destroy the data block");

      // remove the entry from the catalog.
      if (catalog.Remove(name) == StatusError)
	escape("unable to remove the entry from the catalog");

      // check the size.
      if (catalog.Size(size) == StatusError)
	escape("unable to retrieve the catalog size");

      if (size != 0)
	{
	  // seal the catalog.
	  if (catalog.Seal() == StatusError)
	    escape("unable to seal the catalog");

	  // XXX[though in the real DHT, CHBs are not destroyed but expire]
	  // store the new catalog.
	  if (Hole::Put(catalog.address, catalog) == StatusError)
	    escape("unable to store the catalog");

	  // set the address to the catalog's.
	  address = catalog.address;
	}
      else
	{
	  // no need for a catalog since no entry.
	  address = Address::Null;
	}

      // set the new catalog.
      if (directory.Update(k, catalog.address) == StatusError)
	escape("unable to update the directory");

      // XXX[wrong but just to set a size]
      // set a directory size.
      directory.meta.status.size -= 1;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Directory::Lookup(const
					    etoile::core::Directory& directory,
					  const String&		name,
					  Address&		address)
    {
      Catalog			catalog;

      // check if there is a linked catalog.
      if (directory.data.references == Address::Null)
	escape("no directory entry found");

      // load the catalog.
      if (Hole::Get(directory.data.references, catalog) == StatusError)
	escape("unable to load the catalog");

      // lookup in the catalog.
      if (catalog.Lookup(name, address) == StatusError)
	escape("unable to locate the given name in the catalog");

      leave();
    }

  }
}
