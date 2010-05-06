//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Hole.cc
//
// created       julien quintard   [sun aug  9 16:47:38 2009]
// updated       julien quintard   [tue may  4 10:44:03 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Hole.hh>

#include <etoile/kernel/Object.hh>
#include <etoile/kernel/Access.hh>

namespace etoile
{
  namespace hole
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method takes a live block and stores its data into the storage
    /// layer.
    ///
    elle::Status	Hole::Put(const Address&		address,
				  const Block*			block)
    {
      elle::Archive	archive;
      elle::String	identity;

      enter();
      /*
      // verify the block's validity, depending on the block component.
      switch (block->component)
	{
	case ComponentData:
	case ComponentCatalog:
	case ComponentReference:
	case ComponentAccess:
	  {
	    // validate the block.
	    if (block->Validate(address) != elle::StatusTrue)
	      {
		printf("[XXX] unable to validate the retrieved block");
		while (true)
		  ;
	      }

	    break;
	  }
	case ComponentObject:
	  {
	    const kernel::Object*	object =
	      static_cast<const kernel::Object*>(block);
	    kernel::Access*		access;

	    // retrieve the access block, if present.
	    if (object->meta.access != Address::Null)
	      {
		if (Hole::Get(object->meta.access,
			      (Block*&)access) != elle::StatusTrue)
		  {
		    printf("[XXX] unable to retrieve the access block");
		    while (true)
		      ;
		  }
	      }
	    else
	      access = NULL;

	    // validate the block the normal way.
	    if (object->Validate(address, access) != elle::StatusTrue)
	      {
		printf("[XXX] unable to validate the retrieved block");
		while (true)
		  ;
	      }

	    // XXX[debug] retrieve the data block just to make sure it exists.
	    if (object->data.contents != Address::Null)
	      {
		Block*			b;

		if (Hole::Get(object->data.contents, b) != elle::StatusTrue)
		  {
		    printf("unable to retrieve the data block");
		    while (true)
		      ;
		  }

		delete b;
	      }

	    // XXX[delete the temporary stuff]
	    if (access != NULL)
	      delete access;

	    break;
	  }
	}
      */

      // first, turns the address into a string.
      if (address.Save(identity) == elle::StatusError)
	escape("unable to save the address' unique");

      // create an archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create an archive");

      // serialize the block.
      if (block->Serialize(archive) == elle::StatusError)
	escape("unable to serialize the block");

      // XXX[temporary hack emulating a storage layer]
      {
	char		path[4096];
	int		fd;

	printf("[XXX] Hole::Put(%s)\n", identity.c_str());

	sprintf(path, "/home/mycure/.infinit/hole/%s", identity.c_str());

	if ((fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0600)) == -1)
	  escape("unable to open the file");

	if (write(fd, archive.contents, archive.size) == -1)
	  escape("unable to write the archive");

	if (close(fd) == -1)
	  escape("unable to close the file");
      }

      leave();
    }

    ///
    /// this method retrieves data from the storage layer and extracts it
    /// into a live block.
    ///
    /// \todo XXX this method should call Gather() for mutable blocks and
    ///    handle the protocol such that the DHT does not care about
    ///    things such as mutable/immutable distinctions etc.
    /// \todo XXX note that although it would be nice to make the DHT not care
    ///    about the types of blocks and their validity its going to be
    ///    difficult. for example this method verifies the validaity at the
    ///    end. if this test fails, this method would have to re-ask the
    ///    DHT. however, since the DHT has no context for this operation
    ///    failure, it is likely to return the exact same block :(
    /// \todo XXX therefore the simplest way would be for the storage layer
    ///    to actually do everything that is here meaning extracting and
    ///    verifying. the storage layer (Hole) should be able to understand
    ///    Address and Block!
    ///
    elle::Status	Hole::Get(const Address&		address,
				  Block*&			block)
    {
      elle::Archive	archive;
      elle::Region	region;
      elle::String	identity;
      elle::String	identifier;

      enter();

      // identify the address.
      if (address.Save(identity) == elle::StatusError)
	escape("unable to save the address' unique");

      // XXX[temporary hack for local storage]
      {
	char		path[4096];
	struct stat	stat;
	int		fd;

	printf("[XXX] Hole::Get(%s)\n", identity.c_str());

	sprintf(path, "/home/mycure/.infinit/hole/%s", identity.c_str());

	if (lstat(path, &stat) == -1)
	  false();

	if (region.Prepare(stat.st_size) == elle::StatusError)
	  escape("unable to prepare the region");

	region.size = stat.st_size;

	if ((fd = open(path, O_RDONLY)) == -1)
	  escape("unable to open the file");

	if (read(fd, region.contents, region.size) == -1)
	  escape("unable to read the region");

	if (close(fd) == -1)
	  escape("unable to close the file");
      }

      // detach the data from the region to prevent multiple resources release.
      if (region.Detach() == elle::StatusError)
        escape("unable to detach the region");

      // prepare the archive.
      if (archive.Prepare(region) == elle::StatusError)
        escape("unable to prepare the archive");

      // extract the component identifier.
      if (archive.Extract(identifier) == elle::StatusError)
        escape("unable to extract the component identifier");

      // build the block according to the component type.
      if (elle::Factory::Build(identifier, block) == elle::StatusError)
	escape("unable to build the block");

      // extract the archive.
      if (block->Extract(archive) == elle::StatusError)
        escape("unable to extract the given block");

      // bind so that the internal address is computed.
      if (block->Bind() == elle::StatusError)
	escape("unable to bind the block");
      /*
      // verify the block's validity, depending on the block component.
      switch (block->component)
	{
	case ComponentData:
	case ComponentCatalog:
	case ComponentReference:
	case ComponentAccess:
	  {
	    // validate the block.
	    if (block->Validate(address) != elle::StatusTrue)
	      {
		printf("[XXX] unable to validate the retrieved block");
		while (true)
		  ;
	      }

	    break;
	  }
	case ComponentObject:
	  {
	    kernel::Object*	object = static_cast<kernel::Object*>(block);
	    kernel::Access*	access;

	    // retrieve the access block, if present.
	    if (object->meta.access != Address::Null)
	      {
		if (Hole::Get(object->meta.access,
			      (Block*&)access) != elle::StatusTrue)
		  {
		    printf("[XXX] unable to retrieve the access block");
		    while (true)
		      ;
		  }
	      }
	    else
	      access = NULL;

	    // validate the block the normal way.
	    if (object->Validate(address, access) != elle::StatusTrue)
	      {
		printf("unable to validate the retrieved block");
		while (true)
		  ;
	      }

	    // XXX[debug] retrieve the data block just to make sure it exists.
	    if (object->data.contents != Address::Null)
	      {
		Block*			b;

		if (Hole::Get(object->data.contents, b) != elle::StatusTrue)
		  {
		    printf("unable to retrieve the data block");
		    while (true)
		      ;
		  }

		delete b;
	      }

	    // XXX[delete the temporary stuff]
	    if (access != NULL)
	      delete access;

	    break;
	  }
	}
      */
      true();
    }

    ///
    /// XXX
    ///
    /// \todo XXX this method should ask the storage nodes to destroy the
    ///   data, whose should challenge our clients, proving that we are
    ///   the owner.
    ///
    elle::Status	Hole::Erase(const Address&		address)
    {
      elle::String	identity;

      enter();

      // identify the address.
      if (address.Save(identity) == elle::StatusError)
	escape("unable to save the address' unique");

      // XXX[temporary hack for local storage]
      {
	char		path[4096];

	printf("[XXX] Hole::Destroy(%s)\n", identity.c_str());

	sprintf(path, "/home/mycure/.infinit/hole/%s", identity.c_str());

	::unlink(path);
      }

      leave();
    }

  }
}
