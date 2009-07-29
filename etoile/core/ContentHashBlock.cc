//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/ContentHashBlock.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [wed jul 29 16:57:19 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/ContentHashBlock.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// the class name.
    ///
    const String		ContentHashBlock::Class = "ContentHashBlock";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    ContentHashBlock::ContentHashBlock()
    {
    }

    ///
    /// this method releases and reinitializes.
    ///
    ContentHashBlock::~ContentHashBlock()
    {
    }

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
