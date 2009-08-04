//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Catalog.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [tue aug  4 14:01:56 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/core/Catalog.hh>

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
    Catalog::~Catalog()
    {
      Catalog::Iterator		iterator;

      for (iterator = this->entries.begin();
	   iterator != this->entries.end();
	   iterator++)
	delete *iterator;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Catalog::Size(Natural32&		size)
    {
      size = this->entries.size();

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Add(const String&		name,
				     const Address&		address)
    {
      Catalog::Entry*		entry;

      if (this->Search(name) == StatusTrue)
	escape("another entry with the same name seems to already exist");

      entry = new Catalog::Entry;

      entry->name = name;
      entry->address = address;

      this->entries.push_back(entry);

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Remove(const String&		name)
    {
      Catalog::Iterator		iterator;

      if (this->Search(name, &iterator) == StatusFalse)
	escape("unable to find the given named entry");

      this->entries.erase(iterator);

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Lookup(const String&		name,
					Address&		address)
    {
      Catalog::Iterator		iterator;

      if (this->Search(name, &iterator) == StatusFalse)
	escape("unable to find the given named entry");

      address = (*iterator)->address;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Search(const String&		name,
					Catalog::Iterator*	iterator)
    {
      Catalog::Iterator		i;

      for (i = this->entries.begin();
	   i != this->entries.end();
	   i++)
	{
	  Catalog::Entry*	entry = *i;

	  if (name == entry->name)
	    {
	      if (iterator != NULL)
		*iterator = i;

	      true();
	    }
	}

      false();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Catalog::Dump(Natural32			margin) const
    {
      String			alignment(margin, ' ');
      String			shift(2, ' ');
      Catalog::ConstIterator	iterator;

      std::cout << alignment << "[Catalog]" << std::endl;

      if (ContentHashBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      for (iterator = this->entries.begin();
	   iterator != this->entries.end();
	   iterator++)
	{
	  Catalog::Entry*	entry = *iterator;

	  std::cout << alignment << shift << "[Name] "
		    << entry->name << std::endl;

	  if (entry->address.Dump(margin + 2) == StatusError)
	    escape("unable to dump the address");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the block object.
    ///
    Status		Catalog::Serialize(Archive&		archive) const
    {
      std::list<Catalog::Entry*>::const_iterator i;

      // call the parent class.
      if (Block::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // serialize the number of entries.
      if (archive.Serialize((Natural32)this->entries.size()) == StatusError)
	escape("unable to serialize the catalog size");

      // serialize the list of entries.
      for (i = this->entries.begin(); i != this->entries.end(); i++)
	{
	  Catalog::Entry*	entry = *i;

	  // serialize an entry.
	  if (archive.Serialize(entry->name,
				entry->address) == StatusError)
	    escape("unable to serialize the entry");
	}

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Catalog::Extract(Archive&		archive)
    {
      Natural32		size;
      Natural32		i;

      // call the parent class.
      if (Block::Extract(archive) == StatusError)
	escape("unable to extract the block");

      // extract the size.
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the size");

      for (i = 0; i < size; i++)
	{
	  Catalog::Entry*	entry;

	  entry = new Catalog::Entry;

	  // extract an entry.
	  if (archive.Extract(entry->name,
			      entry->address) == StatusError)
	    escape("unable to extract the entry");

	  // add it to the entries.
	  this->entries.push_back(entry);
	}

      leave();
    }

  }
}
