//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit (c)
//
// file          /home/mycure/infinit/pig/hole/Hole.cc
//
// created       julien quintard   [sun aug  2 18:32:19 2009]
// updated       julien quintard   [mon aug 10 21:29:43 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/hole/Hole.hh>

namespace pig
{
  namespace hole
  {

    // XXX
    //   all this stuff is temporary!
    // XXX

//
// ---------- globals ---------------------------------------------------------
//

    // XXX
    const char*		g_device = ".device";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Hole::Put(const Address&		address,
				  const Block&			block)
    {
      Archive		archive;
      String		identity;
      char		path[4096];
      int		fd;

      if (address.Identify(identity) == StatusError)
	escape("unable to identify the address");

      if (archive.Create() == StatusError)
	escape("unable to create an archive");

      if (archive.Serialize(block) == StatusError)
	escape("unable to serialize the given block");

      sprintf(path, "%s/%s", g_device, identity.c_str());

      if ((fd = open(path, O_CREAT | O_WRONLY, 0600)) == -1)
	escape("unable to open the file");

      if (write(fd, archive.contents, archive.size) == -1)
	escape("unable to write the archive");

      if (close(fd) == -1)
	escape("unable to close the file");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Hole::Get(const Address&		address,
				  Block&			block)
    {
      Archive		archive;
      Region		region;
      String		identity;
      char		path[4096];
      struct stat	stat;
      int		fd;

      if (address.Identify(identity) == StatusError)
	escape("unable to identify the address");

      sprintf(path, "%s/%s", g_device, identity.c_str());

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

      if (region.Detach() == StatusError)
	escape("unable to detach the region");

      if (archive.Prepare(region) == StatusError)
	escape("unable to prepare the archive");

      if (archive.Extract(block) == StatusError)
	escape("unable to extract the given block");

      if (block.Validate(address) != StatusTrue)
	escape("unable to validate the retrieved block");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Hole::Destroy(const Address&		address)
    {
      String		identity;
      char		path[4096];

      if (address.Identify(identity) == StatusError)
	escape("unable to identify the address");

      sprintf(path, "%s/%s", g_device, identity.c_str());

      // XXX
      if (unlink(path) != 0)
	escape("unable to delete the file");

      leave();
    }

  }
}
