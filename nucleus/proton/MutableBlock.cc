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
// updated       julien quintard   [sun jun 19 22:45:38 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/MutableBlock.hh>

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
      elle::Unique	unique;
      elle::Region	region;
      elle::String	number;

      enter();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Block::Mutable) ==
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
	  std::ostringstream	oss;

	  // transfer the version to the stream.
	  oss << version.number;

	  // assign the number.
	  number = oss.str();
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
      elle::Unique		unique;
      elle::Region		region;
      elle::String		string;
      std::ostringstream	oss;
      elle::String		number;

      enter();

      // transfer the version to the stream.
      oss << this->version.number;

      // assign the number.
      number = oss.str();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // create the shelter path.
      if (file.Create(lune::Lune::Network::Shelter::Block::Mutable) ==
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

      leave();
    }

    ///
    /// this method erases a block.
    ///
    elle::Status	MutableBlock::Erase(const Network&	network,
					    const Address&	address) const
    {
      elle::Unique	unique;
      Version		current;
      Version		i;

      enter();

      // XXX just to remove the warning
      Network n = network;
      Address a = address;

      /* XXX
      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // retrieve the latest version number.
      {
	elle::Path	p;
	elle::Region	r;

	// create the shelter path.
	if (p.Create(lune::Lune::Network::Shelter::Block) ==
	    elle::StatusError)
	  escape("unable to create the path");

	// complete the path with the network name.
	if (p.Complete(elle::Piece("%NETWORK%", network.name),
		       elle::Piece("%ADDRESS%", unique),
		       elle::Piece("%VERSION%", "@")) == elle::StatusError)
	  escape("unable to complete the path");

	// read the file's content.
	if (elle::File::Read(p, r) == elle::StatusError)
	  escape("unable to read the file's content");

	// create a stream in order to extract the number.
	std::istringstream	iss(elle::String((char*)r.contents, r.size));

	// extract the version number which is the latest.
	iss >> current;
      }

      // for every version number down to zero.
      for (i = current; i >= 0; i--)
	{
	  elle::Path		p;
	  std::ostringstream	oss;

	  // transfer the version number into the stream.
	  oss << i;

	  // create the shelter path.
	  if (p.Create(lune::Lune::Network::Shelter::Block) ==
	      elle::StatusError)
	    escape("unable to create the path");

	  // complete the path with the network name.
	  if (p.Complete(elle::Piece("%NETWORK%", network.name),
			 elle::Piece("%ADDRESS%", unique),
			 elle::Piece("%VERSION%", oss.str())) ==
	      elle::StatusError)
	    escape("unable to complete the path");

	  // erase the file.
	  if (elle::File::Erase(p) == elle::StatusError)
	    escape("unable to erase the file");
	}
      */

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
      elle::Unique		unique;
      std::ostringstream	oss;
      elle::String		number;

      enter();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
        flee("unable to save the address' unique");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Block::Mutable) ==
	  elle::StatusError)
	flee("unable to create the path");

      // if the requested version is the latest...
      if (version == Version::Last)
	{
	  // this character is used to refer to the latest version.
	  number = "@";
	}
      else
	{
	  std::ostringstream	oss;

	  // transfer the version to the stream.
	  oss << version.number;

	  // assign the number.
	  number = oss.str();
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
