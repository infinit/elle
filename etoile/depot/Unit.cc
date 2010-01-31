//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Unit.cc
//
// created       julien quintard   [tue jan 26 14:23:34 2010]
// updated       julien quintard   [sat jan 30 22:03:04 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Unit.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method is used to load an already existing unit.
    ///
    Status		Unit::Load(String			path)
    {
      // assign the path.
      this->path = path;

      leave();
    }

    ///
    /// this method sets or updates the unit by serializing the block
    /// and storing its content on the disk.
    ///
    Status		Unit::Set(hole::Block*			block)
    {
      Archive		archive;
      Natural32		fd;

      // create the path based on the block's address.
      if (block->address.Identify(this->path) == StatusError)
	escape("unable to retrieve the block's identity");

      // create an archive.
      if (archive.Create() == StatusError)
	escape("unable to create an archive");

      // serialize the block.
      if (block->Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // create and open the file.
      if ((fd = ::open(this->path.c_str(),
		       O_CREAT | O_TRUNC | O_WRONLY)) == -1)
	escape("unable to create the unit file");

      // write the block's archive to the file.
      if (::write(fd, archive.contents, archive.size) != archive.size)
	escape("unable to write the block's archive");

      // close the file.
      ::close(fd);

      // save the size.
      this->size = archive.size;

      // update reserve usage.
      Repository::Reserve::Size += this->size;

      leave();
    }

    ///
    /// this method extracts the on-disk unit and returns a live block.
    ///
    Status		Unit::Get(hole::Block*			block)
    {
      Archive		archive;
      Region		region;
      struct ::stat	stat;
      Natural32		fd;
      String		identifier;

      // retrieve information on the path, especially the unit size.
      if (::lstat(this->path.c_str(), &stat) == -1)
	escape("unable to retrieve information on the path");

      // prepare a region to receive the file's content.
      if (region.Prepare(stat.st_size) == StatusError)
	escape("unable to prepare the region");

      region.size = stat.st_size;

      // open the unit file.
      if ((fd = ::open(this->path.c_str(), O_RDONLY)) == -1)
	escape("unable to open the file");

      // read the whole file.
      if (::read(fd, region.contents, region.size) == -1)
	escape("unable to read the region");

      // close the file.
      ::close(fd);

      // detach the data from the region to prevent multiple resources release.
      if (region.Detach() == StatusError)
        escape("unable to detach the region");

      // prepare the archive.
      if (archive.Prepare(region) == StatusError)
        escape("unable to prepare the archive");

      // extract the component identifier.
      if (archive.Extract(identifier) == StatusError)
        escape("unable to extract the component identifier");

      // build the block according to the component type.
      if (Factory::Build(identifier, block) == StatusError)
	escape("unable to build the block");

      // extract the archive.
      if (block->Extract(archive) == StatusError)
        escape("unable to extract the given block");

      // bind so that the internal address is computed.
      if (block->Bind() == StatusError)
	escape("unable to bind the block");

      leave();
    }

    ///
    /// this method destroys a unit file.
    ///
    Status		Unit::Destroy()
    {
      // destroy the file.
      if (::unlink(this->path.c_str()) != 0)
	escape("unable to remove the unit file");

      // update reserve usage.
      Repository::Reserve::Size -= this->size;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the unit.
    ///
    Status		Unit::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');

      std::cout << alignment << "[Unit] " << std::endl;

      std::cout << alignment << shift << "[Path] "
		<< std::hex << this->path << std::endl;

      leave();
    }

  }
}
