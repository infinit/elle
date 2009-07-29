//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/References.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [wed jul 29 17:03:48 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/References.hh>

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
    const String		References::Class = "References";

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    References::References()
    {
    }

    ///
    /// this method releases and reinitializes.
    ///
    References::~References()
    {
      std::list<References::Entry*>::iterator i;

      for (i = this->blocks.begin(); i != this->blocks.end(); i++)
	delete *i;
    }

//
// ---------- methods ---------------------------------------------------------
//

    // XXX

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		References::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      std::list<References::Entry*>::iterator i;

      std::cout << alignment << "[References]" << std::endl;

      if (ContentHashBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      for (i = this->blocks.begin(); i != this->blocks.end(); i++)
	{
	  References::Entry*	entry = *i;

	  if (entry->address.Dump(margin + 2) == StatusError)
	    escape("unable to dump the address");

	  if (entry->key.Dump(margin + 2) == StatusError)
	    escape("unable to dump the key");

	  std::cout << alignment << shift << "[Size] "
		    << entry->size << std::endl;
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		References::Serialize(Archive&		archive) const
    {
      std::list<References::Entry*>::const_iterator i;
      Archive		ar;

      // call the parent class.
      if (Block::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(References::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the number of blocks.
      if (ar.Serialize((Natural32)this->blocks.size()) == StatusError)
	escape("unable to serialize the references size");

      // serialize the list of blocks.
      for (i = this->blocks.begin(); i != this->blocks.end(); i++)
	{
	  References::Entry*	entry = *i;

	  // serialize an entry.
	  if (ar.Serialize(entry->address,
			   entry->key,
			   entry->size) == StatusError)
	    escape("unable to serialize the entry");
	}

      // record the object archive into the given archive.
      if (archive.Serialize(ar) == StatusError)
	escape("unable to serialize the object archive");

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		References::Extract(Archive&		archive)
    {
      Archive		ar;
      String		name;
      Natural32		size;
      Natural32		i;

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
      if (References::Class != name)
	escape("wrong class name in the extract object");

      // extract the size.
      if (ar.Extract(size) == StatusError)
	escape("unable to extract the size");

      for (i = 0; i < size; i++)
	{
	  References::Entry*	entry;

	  entry = new References::Entry;

	  // extract an entry.
	  if (ar.Extract(entry->address,
			 entry->key,
			 entry->size) == StatusError)
	    escape("unable to extract the entry");

	  // add it to the blocks.
	  this->blocks.push_back(entry);
	}

      leave();
    }

  }
}
