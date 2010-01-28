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
// updated       julien quintard   [wed jan 27 23:45:54 2010]
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
    /// XXX
    ///
    Status		Unit::Load(String			path)
    {
      // assign the path.
      this->path = path;

      leave();
    }

    ///
    /// XXX
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

      // update reserve usage.
      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Unit::Get(hole::Block*			block)
    {
      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    Status		Unit::Destroy()
    {
      // destroy the file.
      if (::unlink(this->path.c_str()) != 0)
	escape("unable to remove the unit file");

      // update reserve usage.
      // XXX

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
