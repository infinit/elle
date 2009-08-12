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
// updated       julien quintard   [mon aug 10 21:17:03 2009]
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
// ---------- constructors & destructors --------------------------------------
//

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
    Status		References::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      std::list<References::Entry*>::const_iterator i;

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

      // call the parent class.
      if (ContentHashBlock::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // serialize the number of blocks.
      if (archive.Serialize((Natural32)this->blocks.size()) == StatusError)
	escape("unable to serialize the references size");

      // serialize the list of blocks.
      for (i = this->blocks.begin(); i != this->blocks.end(); i++)
	{
	  References::Entry*	entry = *i;

	  // serialize an entry.
	  if (archive.Serialize(entry->address,
				entry->key,
				entry->size) == StatusError)
	    escape("unable to serialize the entry");
	}

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		References::Extract(Archive&		archive)
    {
      Natural32		size;
      Natural32		i;

      // call the parent class.
      if (ContentHashBlock::Extract(archive) == StatusError)
	escape("unable to extract the block");

      // extract the size.
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the size");

      for (i = 0; i < size; i++)
	{
	  References::Entry*	entry;

	  entry = new References::Entry;

	  // extract an entry.
	  if (archive.Extract(entry->address,
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
