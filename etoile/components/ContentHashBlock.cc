//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       GPL
//
// file          /home/mycure/infinit/core/components/ContentHashBlock.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [thu jul 16 01:13:00 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <core/components/ContentHashBlock.hh>

namespace core
{
  namespace components
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		ContentHashBlock::Class = "ContentHashBlock";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method seals the object.
    ///
    Status		ContentHashBlock::Seal()
    {
      // compute the address.
      if (this->address.Create(*this) == StatusError)
	escape("unable to compute the CHB's address");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		ContentHashBlock::New(ContentHashBlock&)
    {
      leave();
    }

    ///
    /// this method releases and reinitializes.
    ///
    Status		ContentHashBlock::Delete(ContentHashBlock&)
    {
      leave();
    }

    ///
    /// assign the content hash block.
    ///
    ContentHashBlock&	ContentHashBlock::operator=(const ContentHashBlock&	element)
    {
      // self-check.
      if (this == &element)
	return (*this);

      // call the parent class.
      Block::operator=(element);

      // reinitialize the object.
      if ((ContentHashBlock::Delete(*this) == StatusError) ||
	  (ContentHashBlock::New(*this) == StatusError))
	yield("unable to reinitialize the object", *this);

      return (*this);
    }

    ///
    /// this method checks if two objects match.
    ///
    /// note that two objects cannot be compared if they have not been sealed.
    /// indeed, this method starts by calling the parent Block class that
    /// compares the addresses. since these addresses are computed when
    /// the blocks are sealed, the process will failed if the two blocks
    /// have not been sealed.
    ///
    Boolean		ContentHashBlock::operator==(const ContentHashBlock& element)
    {
      // call the parent class.
      return (Block::operator==(element));
    }

    ///
    /// this method checks if two objects dis-match.
    ///
    Boolean		ContentHashBlock::operator!=(const ContentHashBlock& element)
    {
      return (!(*this == element));
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		ContentHashBlock::Dump(Natural32	margin)
    {
      String		alignment(margin, ' ');

      std::cout << alignment << "[ContentHashBlock]" << std::endl;

      if (Block::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		ContentHashBlock::Serialize(Archive&	archive) const
    {
      Archive		ar;

      // call the parent class.
      if (Block::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(ContentHashBlock::Class) == StatusError)
	escape("unable to serialize the class name");

      // record the object archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		ContentHashBlock::Extract(Archive&	archive)
    {
      Archive		ar;
      String		name;

      // call the parent class.
      if (Block::Extract(archive) == StatusError)
	escape("unable to extract the block");

      // extract the block archive object.
      if (archive.Extract(ar) == StatusError)
	escape("unable to extract the block archive object");

      // extract the name.
      if (ar.Extract(name) == StatusError)
	escape("unable to extract the class name");

      // check the name.
      if (ContentHashBlock::Class != name)
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
  /// this function displays a content hash block.
  ///
  std::ostream&		operator<<(std::ostream&		stream,
				   const core::components::ContentHashBlock& element)
  {
    elle::archive::Archive	archive;
    elle::crypto::Digest	digest;

    // prepare the archive.
    if (archive.Create() == elle::misc::StatusError)
      return (stream);

    // serialize the element.
    if (element.Serialize(archive) == elle::misc::StatusError)
      return (stream);

    // digest the archive.
    if (elle::crypto::OneWay::Hash(archive, digest) == elle::misc::StatusError)
      return (stream);

    // put the fingerprint into the stream.
    stream << digest;

    return (stream);
  }

}
