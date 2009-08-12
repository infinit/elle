//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/hole/Hole.cc
//
// created       julien quintard   [sun aug  9 16:47:38 2009]
// updated       julien quintard   [mon aug 10 21:28:21 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Hole.hh>

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
    Status		Hole::Put(const Address&		address,
				  const Block&			block)
    {
      Archive		archive;
      String		identity;

      // first, turns the address into a string.
      if (address.Identify(identity) == StatusError)
	escape("unable to identify the address");

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create an archive");

      // serialize the block.
      if (block.Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // XXX[temporary hack emulating a storage layer]
      {
	char		path[4096];
	int		fd;

	sprintf(path, ".device/%s", identity.c_str());

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
    Status		Hole::Get(const Address&		address,
				  Block&			block)
    {
      Archive		archive;
      Region		region;
      String		identity;

      // identify the address.
      if (address.Identify(identity) == StatusError)
        escape("unable to identify the address");

      // XXX[temporary hack for local storage]
      {
	char		path[4096];
	struct stat	stat;
	int		fd;

	sprintf(path, ".device/%s", identity.c_str());

	if (lstat(path, &stat) == -1)
	  escape("unable to retrieve information on the path");

	if (region.Prepare(stat.st_size) == StatusError)
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
      if (region.Detach() == StatusError)
        escape("unable to detach the region");

      // prepare the archive.
      if (archive.Prepare(region) == StatusError)
        escape("unable to prepare the archive");

      // extract the archive.
      if (archive.Extract(block) == StatusError)
        escape("unable to extract the given block");

      // verify the block's validity.
      if (block.Validate(address) != StatusTrue)
        escape("unable to validate the retrieved block");

      leave();
    }

    ///
    /// XXX
    ///
    /// \todo XXX this method should ask the storage nodes to destroy the
    ///   data, whose should challenge our clients, proving that we are
    ///   the owner.
    ///
    Status		Hole::Destroy(const Address&		address)
    {
      // XXX

      leave();
    }

  }
}
