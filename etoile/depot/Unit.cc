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
// updated       julien quintard   [tue may  4 10:43:50 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Unit.hh>
#include <etoile/depot/Repository.hh>

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
    elle::Status	Unit::Load(elle::String			path)
    {
      enter();

      // assign the path.
      this->path = path;

      leave();
    }

    ///
    /// this method sets or updates the unit by serializing the block
    /// and storing its content on the disk.
    ///
    elle::Status	Unit::Set(hole::Block*			block)
    {
      elle::Archive	archive;
      elle::Natural32	fd;

      enter();

      printf("TO REVIEW: this->path n'a pas de sens ici\n");
      escape("XXX");

      // create the path based on the block's address.
      if (block->address.Save(this->path) == elle::StatusError)
	escape("unable to save the address' unique");

      // create an archive.
      if (archive.Create() == elle::StatusError)
	escape("unable to create an archive");

      // serialize the block.
      if (block->Serialize(archive) == elle::StatusError)
	escape("unable to serialize the block");

      // create and open the file.
      if ((fd = ::open(this->path.c_str(),
		       O_CREAT | O_TRUNC | O_WRONLY)) == -1)
	escape("unable to create the unit file");

      // write the block's archive to the file.
      if (::write(fd, archive.contents, archive.size) != archive.size)
	{
	  ::close(fd);

	  escape("unable to write the block's archive");
	}

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
    elle::Status	Unit::Get(hole::Block*			block)
    {
      elle::Archive	archive;
      elle::Region	region;
      struct ::stat	stat;
      elle::Natural32	fd;
      elle::String	identifier;

      enter();

      printf("[XXX] ici utiliser Fileable::Store()\n");
      leave();
      // XXX

      // retrieve information on the path, especially the unit size.
      if (::lstat(this->path.c_str(), &stat) == -1)
	escape("unable to retrieve information on the path");

      // prepare a region to receive the file's content.
      if (region.Prepare(stat.st_size) == elle::StatusError)
	escape("unable to prepare the region");

      region.size = stat.st_size;

      // open the unit file.
      if ((fd = ::open(this->path.c_str(), O_RDONLY)) == -1)
	escape("unable to open the file");

      // read the whole file.
      if (::read(fd, region.contents, region.size) == -1)
	{
	  ::close(fd);

	  escape("unable to read the region");
	}

      // close the file.
      ::close(fd);

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

      leave();
    }

    ///
    /// this method destroys a unit file.
    ///
    elle::Status		Unit::Destroy()
    {
      enter();

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
    elle::Status	Unit::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Unit] " << std::endl;

      std::cout << alignment << elle::Dumpable::Shift << "[Path] "
		<< std::hex << this->path << std::endl;

      leave();
    }

  }
}
