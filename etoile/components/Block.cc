//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repositories/infinit/core/components/Block.cc
//
// created       julien quintard   [mon feb 16 17:08:04 2009]
// updated       julien quintard   [thu jul 16 11:37:47 2009]
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

      if (this->address.Dump(margin + 2) == StatusError)
	escape("unable to dump the address");

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
      Archive		ar;

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the elements.
      if (ar.Serialize(Block::Class, this->address) == StatusError)
	escape("unable to serialize the block's elements");

      // record the object archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Block::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;

      // extract the block archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the block archive object");

      // extract the elements.
      if (ar.Extract(name, this->address) == StatusError)
	escape("unable to extract the block's elements");

      // check the name.
      if (Block::Class != name)
	escape("wrong class name in the extract object");

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
