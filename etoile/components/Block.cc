//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Block.cc
//
// created       julien quintard   [mon feb 16 17:08:04 2009]
// updated       julien quintard   [sat jul 25 13:30:16 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Block.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		Block::Class = "Block";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method validates the block by comparing the current address
    /// with the given address.
    ///
    /// in practice, this method should be called with the address
    /// used when requesting the block to make sure the block received
    /// matches the one requested.
    ///
    Status		Block::Validate(Address&		address)
    {
      if (this->address != address)
	false();

      true();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		Block::New(Block&)
    {
      leave();
    }

    ///
    /// this method reinitializes the object.
    ///
    Status		Block::Delete(Block&)
    {
      leave();
    }

    ///
    /// assign the block.
    ///
    Block&		Block::operator=(const Block&		element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // reinitialize the object.
      if ((Block::Delete(*this) == StatusError) ||
	  (Block::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      // assign the address.
      this->address = element.address;

      return (*this);
    }

    ///
    /// this method checks if two objects match.
    ///
    Boolean		Block::operator==(const Block&		element)
    {
      // compare the internal addresses.
      return (this->address == element.address);
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		Block::operator!=(const Block&		element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Block::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[Block]" << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		Block::Serialize(Archive&		archive) const
    {
      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Block::Extract(Archive&		archive)
    {
      leave();
    }

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ///
  /// this function displays a block.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const etoile::components::Block& element)
  {
    stream << element.address;

    return (stream);
  }

}
