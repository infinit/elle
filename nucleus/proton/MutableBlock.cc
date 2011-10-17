//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat may 21 12:27:39 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/History.hh>

#include <lune/Lune.hh>
#include <hole/Hole.hh>
#include <Infinit.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructs & destructors ----------------------------------------
//

    ///
    /// default constructor.
    ///
    MutableBlock::MutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown),

      version(Version::First)
    {
    }

    ///
    /// specific constructor.
    ///
    MutableBlock::MutableBlock(const Family			family,
			       const neutron::Component		component):
      Block(family, component),

      version(Version::First)
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(MutableBlock, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    elle::Status	MutableBlock::Dump(const elle::Natural32 margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[MutableBlock]" << std::endl;

      // dump the parent.
      if (Block::Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the parent");

      // dump the version.
      if (this->version.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the version");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    elle::Status	MutableBlock::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the parent class.
      if (Block::Serialize(archive) == elle::StatusError)
	escape("unable to serialize the underlying block");

      // serialize the attributes.
      if (archive.Serialize(this->version) == elle::StatusError)
	escape("unable to serialize the attributess");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    elle::Status	MutableBlock::Extract(elle::Archive&	archive)
    {
      enter();

      // extract the parent class.
      if (Block::Extract(archive) == elle::StatusError)
	escape("unable to extract the underlying block");

      // extracts the attributes.
      if (archive.Extract(this->version) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads the block.
    ///
    elle::Status	MutableBlock::Load(const Network&	network,
					   const Address&	address,
					   const Version&	version)
    {
      elle::Path	path;
      elle::String	unique;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::MutableBlock::Load(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
	{
	  elle::Region	region;
	  elle::Archive	archive;

	  printf("XXX History False\n");

	  // complete the path with the network name.
	  if (path.Complete(elle::Piece("%NETWORK%", network.name),
			    elle::Piece("%ADDRESS%", unique),
			    elle::Piece("%VERSION%", "@")) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // read the file's content.
	  if (elle::File::Read(path, region) == elle::StatusError)
	    escape("unable to read the file's content");

	  // wrap the region into an archive.
	  if (archive.Wrap(region) == elle::StatusError)
	    escape("unable to prepare the archive");

	  // extract from the archive.
	  if (archive.Extract(*this) == elle::StatusError)
	    escape("unable to extract the archive");
	}
      else
	{
	  elle::Region	region;
	  elle::String	number;
	  elle::Archive	archive;

	  // if the requested version is the latest...
	  if (version == Version::Last)
	    {
	      elle::Path	link;
	      elle::Region	r;

	      // copy the path.
	      link = path;

	      // complete the link path.
	      if (link.Complete(elle::Piece("%NETWORK%", network.name),
				elle::Piece("%ADDRESS%", unique),
				elle::Piece("%VERSION%", "@")) ==
		  elle::StatusError)
		escape("unable to complete the path");

	      // read the file's content.
	      if (elle::File::Read(link, r) == elle::StatusError)
		escape("unable to read the file's content");

	      // set the number of the latest version.
	      number = elle::String(reinterpret_cast<const char*>(r.contents),
				    r.size);
	    }
	  else
	    {
	      // convert the version number.
	      if (elle::Variable::Convert(version.number,
					  number) == elle::StatusFalse)
		escape("unable to convert the version number into a string");
	    }

	  // complete the path with the network name.
	  if (path.Complete(elle::Piece("%NETWORK%", network.name),
			    elle::Piece("%ADDRESS%", unique),
			    elle::Piece("%VERSION%", number)) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // read the file's content.
	  if (elle::File::Read(path, region) == elle::StatusError)
	    escape("unable to read the file's content");

	  // wrap the region into an archive.
	  if (archive.Wrap(region) == elle::StatusError)
	    escape("unable to prepare the archive");

	  // extract from the archive.
	  if (archive.Extract(*this) == elle::StatusError)
	    escape("unable to extract the archive");
	}

      printf("XXX /proton::MutableBlock::Load(%s)\n",
	     unique.c_str());

      leave();
    }

    ///
    /// this method stores the block in its file format.
    ///
    elle::Status	MutableBlock::Store(const Network&	network,
					    const Address&	address)
      const
    {
      elle::Path	file;
      elle::String	unique;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::MutableBlock::Store(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (file.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (file.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
	escape("unable to complete the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
	{
	  //
	  // if the history is not supported, store the mutable block
	  // in a file without version number extension.
	  //
	  elle::Archive		archive;
	  elle::Region		region;

	  // complete the file path.
	  if (file.Complete(elle::Piece("%VERSION%", "@")) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // create the archive.
	  if (archive.Create() == elle::StatusError)
	    escape("unable to create the archive");

	  // serialize the object.
	  if (archive.Serialize(*this) == elle::StatusError)
	    escape("unable to serialize the object");

	  // wrap the archive.
	  if (region.Wrap(reinterpret_cast<const elle::Byte*>(
			    archive.contents),
			  archive.size) == elle::StatusError)
	    escape("unable to wrap the archive in a region");

	  // write the file's content.
	  if (elle::File::Write(file, region) == elle::StatusError)
	    escape("unable to write the file's content");
	}
      else
	{
	  //
	  // otherwise, store the block in a file with a name of the
	  // form [identifier]#[version number].blk. besides, a special
	  // file of the form [identifier]#@.blk contains the number
	  // of the latest version of the mutable block.
	  //
	  elle::String		number;
	  elle::Path		link;
	  elle::Region		region;
	  elle::Archive		archive;
	  nucleus::History	history;

	  // convert the version number into a string.
	  if (elle::Variable::Convert(this->version.number,
				      number) == elle::StatusFalse)
	    escape("unable to transform the version number into a string");

	  // duplicate the path.
	  link = file;

	  // complete the file path.
	  if (file.Complete(elle::Piece("%VERSION%", number)) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // create the archive.
	  if (archive.Create() == elle::StatusError)
	    escape("unable to create the archive");

	  // serialize the object.
	  if (archive.Serialize(*this) == elle::StatusError)
	    escape("unable to serialize the object");

	  // wrap the archive.
	  if (region.Wrap(reinterpret_cast<const elle::Byte*>(
			    archive.contents),
			  archive.size) == elle::StatusError)
	    escape("unable to wrap the archive in a region");

	  // write the file's content.
	  if (elle::File::Write(file, region) == elle::StatusError)
	    escape("unable to write the file's content");

	  // complete the link path.
	  if (link.Complete(elle::Piece("%VERSION%", "@")) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // if there is a link, remove it.
	  if (elle::File::Exist(link) == elle::StatusTrue)
	    {
	      // delete the file which references the latest version since a
	      // new version has been created.
	      if (elle::File::Erase(link) == elle::StatusError)
		escape("unable to erase the block link");
	    }

	  // finally, create the block link.
	  if (elle::File::Write(
	        link,
		elle::Region(
		  reinterpret_cast<const elle::Byte*>(number.c_str()),
		  number.length())) == elle::StatusError)
	    escape("unable to create the block link");

	  // if there is a history, load it.
	  if (history.Exist(network, address) == elle::StatusTrue)
	    {
	      // load the history.
	      if (history.Load(network, address) == elle::StatusError)
		escape("unable to load the history");
	    }

	  // register the new version.
	  if (history.Register(version) == elle::StatusError)
	    escape("unable to register the new version");

	  // store the history.
	  if (history.Store(network, address) == elle::StatusError)
	    escape("unable to store the history");
	}

      leave();
    }

    ///
    /// this method erases a block.
    ///
    elle::Status	MutableBlock::Erase(const Network&	network,
					    const Address&	address) const
    {
      elle::Unique	unique;
      elle::Path	path;

      enter();

      // turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::MutableBlock::Erase(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
	{
	  // complete the path with the network name.
	  if (path.Complete(elle::Piece("%NETWORK%", network.name),
			    elle::Piece("%ADDRESS%", unique),
			    elle::Piece("%VERSION%", "@")) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // erase the file.
	  if (elle::File::Erase(path) == elle::StatusError)
	    escape("unable to erase the file");
	}
      else
	{
	  History	history;
	  Version::Type	size;
	  Version::Type	i;

	  // complete the path with the network name.
	  if (path.Complete(elle::Piece("%NETWORK%", network.name),
			    elle::Piece("%ADDRESS%", unique)) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // load the history.
	  if (history.Load(network, address) == elle::StatusError)
	    escape("unable to load the history");

	  // retrieve the number of versions.
	  if (history.Size(size) == elle::StatusError)
	    escape("unable to retrieve the size of the history");

	  // go through the versions.
	  for (i = 0; i < size; i++)
	    {
	      nucleus::Version	version;
	      elle::String	number;
	      elle::Path	file;

	      // select a particular version.
	      if (history.Select(i, version) == elle::StatusError)
		escape("unable to select a particular version");

	      // convert the version number into a string.
	      if (elle::Variable::Convert(version.number,
					  number) == elle::StatusFalse)
		escape("unable to transform the version number into a string");

	      // duplicate the generic path.
	      file = path;

	      // complete the path with the version number.
	      if (file.Complete(elle::Piece("%VERSION%",
					    number)) == elle::StatusError)
		escape("unable to complete the path");

	      // erase the file.
	      if (elle::File::Erase(file) == elle::StatusError)
		escape("unable to erase the file");
	    }

	  // complete the path with the last version pointer.
	  if (path.Complete(elle::Piece("%VERSION%", "@")) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // delete the link which references the latest version.
	  if (elle::File::Erase(path) == elle::StatusError)
	    escape("unable to erase the block link");

	  // erase the history.
	  if (history.Erase(network, address) == elle::StatusError)
	    escape("unable to erase the history");
	}

      leave();
    }

    ///
    /// this method returns true if the block exists.
    ///
    elle::Status	MutableBlock::Exist(const Network&	network,
					    const Address&	address,
					    const Version&	version)
      const
    {
      elle::Path		path;
      elle::String		unique;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	flee("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::MutableBlock::Exist(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	flee("unable to create the path");

      // operate depending on the network's support of history.
      if (hole::Hole::Descriptor.history == false)
	{
	  // complete the path with the network name.
	  if (path.Complete(elle::Piece("%NETWORK%", network.name),
			    elle::Piece("%ADDRESS%", unique),
			    elle::Piece("%VERSION%", "@")) ==
	      elle::StatusError)
	    flee("unable to complete the path");

	  // test the file.
	  if (elle::File::Exist(path) == elle::StatusTrue)
	    true();
	}
      else
	{
	  elle::String	number;

	  // if the requested version is the latest...
	  if (version == Version::Last)
	    {
	      elle::Path	link;
	      elle::Region	r;

	      // copy the path.
	      link = path;

	      // complete the link path.
	      if (link.Complete(elle::Piece("%NETWORK%", network.name),
				elle::Piece("%ADDRESS%", unique),
				elle::Piece("%VERSION%", "@")) ==
		  elle::StatusError)
		flee("unable to complete the path");

	      // read the file's content.
	      if (elle::File::Read(link, r) == elle::StatusError)
		flee("unable to read the file's content");

	      // set the number of the latest version.
	      number = elle::String(reinterpret_cast<const char*>(r.contents),
				    r.size);
	    }
	  else
	    {
	      // convert the version number.
	      if (elle::Variable::Convert(version.number,
					  number) == elle::StatusFalse)
		flee("unable to convert the version number into a string");
	    }

	  // complete the path with the network name.
	  if (path.Complete(elle::Piece("%NETWORK%", network.name),
			    elle::Piece("%ADDRESS%", unique),
			    elle::Piece("%VERSION%", number)) ==
	      elle::StatusError)
	    flee("unable to complete the path");

	  // test the file.
	  if (elle::File::Exist(path) == elle::StatusTrue)
	    true();
	}

      false();
    }

  }
}
