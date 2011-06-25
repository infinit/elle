//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/proton/ImmutableBlock.cc
//
// created       julien quintard   [sat may 21 12:22:14 2011]
// updated       julien quintard   [fri jun 24 18:07:45 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/ImmutableBlock.hh>

#include <lune/Lune.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructs & destructors ----------------------------------------
//

    ImmutableBlock::ImmutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown)
    {
    }

    ImmutableBlock::ImmutableBlock(const Family			family,
				   const neutron::Component	component):
      Block(family, component)
    {
    }

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads the block.
    ///
    elle::Status	ImmutableBlock::Load(const Network&	network,
					     const Address&	address)
    {
      elle::Path	path;
      elle::Unique	unique;
      elle::Region	region;

      enter();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Block::Immutable) ==
	  elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
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
    elle::Status	ImmutableBlock::Store(const Network&	network,
					      const Address&	address) const
    {
      elle::Path	path;
      elle::Unique	unique;
      elle::Region	region;
      elle::String	string;

      enter();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Block::Immutable) ==
	  elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
	escape("unable to complete the path");

      // encode in hexadecimal.
      if (elle::Hexadecimal::Encode(*this, string) == elle::StatusError)
	escape("unable to encode the object in hexadecimal");

      // wrap the string.
      if (region.Wrap((elle::Byte*)string.c_str(),
		      string.length()) == elle::StatusError)
	escape("unable to wrap the string in a region");

      // write the file's content.
      if (elle::File::Write(path, region) == elle::StatusError)
	escape("unable to write the file's content");

      leave();
    }

    ///
    /// this method erases a block.
    ///
    elle::Status	ImmutableBlock::Erase(const Network&	network,
					      const Address&	address) const
    {
      elle::Path	path;
      elle::Unique	unique;

      enter();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Block::Immutable) ==
	  elle::StatusError)
	escape("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
	escape("unable to complete the path");

      // erase the file.
      if (elle::File::Erase(path) == elle::StatusError)
	escape("unable to erase the file");

      leave();
    }

    ///
    /// this method returns true if the block exists.
    ///
    elle::Status	ImmutableBlock::Exist(const Network&	network,
					      const Address&	address) const
    {
      elle::Path	path;
      elle::Unique	unique;

      enter();

      // first, turn the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	flee("unable to save the address' unique");

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::Block::Immutable) ==
	  elle::StatusError)
	flee("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
	flee("unable to complete the path");

      // XXX
      path.Dump();

      // test the file.
      if (elle::File::Exist(path) == elle::StatusTrue)
	true();

      false();
    }

  }
}
