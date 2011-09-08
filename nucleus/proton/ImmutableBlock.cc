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
// updated       julien quintard   [wed sep  7 18:45:38 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/ImmutableBlock.hh>

#include <lune/Lune.hh>
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
    ImmutableBlock::ImmutableBlock():
      Block(FamilyUnknown, neutron::ComponentUnknown)
    {
    }

    ///
    /// specific constructor.
    ///
    ImmutableBlock::ImmutableBlock(const Family			family,
				   const neutron::Component	component):
      Block(family, component)
    {
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(ImmutableBlock, _());

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
      elle::String	unique;
      elle::Region	region;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::ImmutableBlock::Load(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
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
      if (elle::Hexadecimal::Decode(
	    elle::String(reinterpret_cast<char*>(region.contents),
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
      elle::String	unique;
      elle::Region	region;
      elle::String	string;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::ImmutableBlock::Store(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
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
      if (region.Wrap(reinterpret_cast<const elle::Byte*>(string.c_str()),
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
      elle::String	unique;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	escape("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::ImmutableBlock::Erase(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
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
      elle::String	unique;

      enter();

      // first, turn the block's address into a hexadecimal string.
      if (elle::Hexadecimal::Encode(address.digest->region,
				    unique) == elle::StatusError)
	flee("unable to convert the address in its hexadecimal form");

      // debug.
      if (Infinit::Configuration.debug.nucleus == true)
	printf("[nucleus] proton::ImmutableBlock::Exist(%s)\n",
	       unique.c_str());

      // create the shelter path.
      if (path.Create(lune::Lune::Network::Shelter::ImmutableBlock) ==
	  elle::StatusError)
	flee("unable to create the path");

      // complete the path with the network name.
      if (path.Complete(elle::Piece("%NETWORK%", network.name),
			elle::Piece("%ADDRESS%", unique)) == elle::StatusError)
	flee("unable to complete the path");

      // test the file.
      if (elle::File::Exist(path) == elle::StatusTrue)
	true();

      false();
    }

  }
}
