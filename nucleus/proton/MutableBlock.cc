//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/MutableBlock.cc
//
// created       julien quintard   [sat may 21 12:27:39 2011]
// updated       julien quintard   [wed jul  6 11:23:14 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/MutableBlock.hh>
#include <nucleus/proton/History.hh>

#include <lune/Lune.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructs & destructors ----------------------------------------
//

    MutableBlock::MutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown),

      version(Version::First)
    {
    }

    MutableBlock::MutableBlock(const Family			family,
			       const neutron::Component		component):
      Block(family, component),

      version(Version::First)
    {
    }

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
      elle::Region	region;
      elle::String	number;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // if the requested version is the latest...
      if (version == Version::Last)
	{
	  // this character is used to refer to the latest version.
	  number = "@";
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
			elle::Piece("%VERSION%", number)) == elle::StatusError)
	escape("unable to complete the path");

      // read the file's content.
      if (elle::File::Read(path, region) == elle::StatusError)
	escape("unable to read the file's content");

      // decode and extract the object.
      if (elle::Hexadecimal::Decode(elle::String((char*)region.contents,
						 region.size),
				    *this) == elle::StatusError)
	escape("unable to decode the object");

      leave();
    }

    ///
    /// this method stores the block in its file format.
    ///
    elle::Status	MutableBlock::Store(const Network&	network,
					    const Address&	address)
      const
    {
      elle::Path		file;
      elle::Path		link;
      elle::String		unique;
      elle::Region		region;
      elle::String		string;
      elle::String		number;
      nucleus::History		history;

      enter();

      // convert the version number into a string.
      if (elle::Variable::Convert(this->version.number,
				  number) == elle::StatusFalse)
	escape("unable to transform the version number into a string");

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // create the shelter path.
      if (file.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (file.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
	escape("unable to complete the path");

      // duplicate the path.
      link = file;

      // complete the file path.
      if (file.Complete(elle::Piece("%VERSION%", number)) == elle::StatusError)
	escape("unable to complete the path");

      // encode in hexadecimal.
      if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
	escape("unable to encode the object in hexadecimal");

      // wrap the string.
      if (region.Wrap((elle::Byte*)string.c_str(),
		      string.length()) == elle::StatusError)
	escape("unable to wrap the string in a region");

      // write the file's content.
      if (elle::File::Write(file, region) == elle::StatusError)
	escape("unable to write the file's content");

      // complete the link path.
      if (link.Complete(elle::Piece("%VERSION%", "@")) == elle::StatusError)
	escape("unable to complete the path");

      // if there is a link, remove it.
      if (elle::Link::Exist(link) == elle::StatusTrue)
	{
	  // delete the link which references the latest version since a
	  // new version has been created.
	  if (elle::Link::Erase(link) == elle::StatusError)
	    escape("unable to erase the block link");
	}

      // finally, create the block link.
      if (elle::Link::Create(link, file) == elle::StatusError)
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
      History		history;
      Version::Type	size;
      Version::Type	i;

      enter();

      // turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
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
	  elle::String		number;
	  elle::Path		file;

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
      if (path.Complete(elle::Piece("%VERSION%", "@")) == elle::StatusError)
	escape("unable to complete the path");

      // delete the link which references the latest version.
      if (elle::Link::Erase(path) == elle::StatusError)
	escape("unable to erase the block link");

      // erase the history.
      if (history.Erase(network, address) == elle::StatusError)
	escape("unable to erase the history");

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
      elle::String		number;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::MutableBlock) ==
	  elle::StatusError)
	escape("unable to create the path");

      // if the requested version is the latest...
      if (version == Version::Last)
	{
	  // this character is used to refer to the latest version.
	  number = "@";
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
			elle::Piece("%VERSION%", number)) == elle::StatusError)
	flee("unable to complete the path");

      // test the file.
      if (elle::File::Exist(path) == elle::StatusTrue)
	true();

      false();
    }

  }
}
