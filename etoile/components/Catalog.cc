//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /data/mycure/repo...ies/infinit/etoile/components/Catalog.cc
//
// created       julien quintard   [tue feb 17 12:39:45 2009]
// updated       julien quintard   [mon jul 27 14:41:59 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Catalog.hh>

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
    const String		Catalog::Class = "Catalog";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Catalog::Add(String&			name,
				     Address&			address)
    {
      Catalog::Entry*	entry;

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
    Status		Catalog::Remove(String&			name)
    {
      std::list<Catalog::Entry*>::iterator iterator;

      if (this->Search(name, &iterator) == StatusFalse)
	escape("unable to find the given named entry");

      this->entries.erase(iterator);

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Resolve(String&		name,
					 Address&		address)
    {
      std::list<Catalog::Entry*>::iterator iterator;

      if (this->Search(name, &iterator) == StatusFalse)
	escape("unable to find the given named entry");

      address = (*iterator)->address;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Search(String&			name,
					std::list<Catalog::Entry*>::iterator*
  					  iterator)
    {
      std::list<Catalog::Entry*>::iterator i;

      for (i = this->entries.begin(); i != this->entries.end(); i++)
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
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method initializes the object.
    ///
    Status		Catalog::New(Catalog&)
    {
      leave();
    }

    ///
    /// this method releases and reinitializes.
    ///
    Status		Catalog::Delete(Catalog&		catalog)
    {
      std::list<Catalog::Entry*>::iterator i;

      for (i = catalog.entries.begin(); i != catalog.entries.end(); i++)
	delete *i;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Catalog::Dump(Natural32			margin)
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      std::list<Catalog::Entry*>::iterator i;

      std::cout << alignment << "[Catalog]" << std::endl;

      if (ContentHashBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      for (i = this->entries.begin(); i != this->entries.end(); i++)
	{
	  Catalog::Entry*	entry = *i;

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
      Archive		ar;

      // call the parent class.
      if (Block::Serialize(archive) == StatusError)
	escape("unable to serialize the block");

      // prepare the object archive.
      if (ar.Create() == StatusError)
	escape("unable to prepare the object archive");

      // serialize the class name.
      if (ar.Serialize(Catalog::Class) == StatusError)
	escape("unable to serialize the class name");

      // serialize the number of entries.
      if (ar.Serialize((Natural32)this->entries.size()) == StatusError)
	escape("unable to serialize the catalog size");

      // serialize the list of entries.
      for (i = this->entries.begin(); i != this->entries.end(); i++)
	{
	  Catalog::Entry*	entry = *i;

	  // serialize an entry.
	  if (ar.Serialize(entry->name,
			   entry->address) == StatusError)
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
    Status		Catalog::Extract(Archive&		archive)
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
      if (Catalog::Class != name)
	escape("wrong class name in the extract object");

      // extract the size.
      if (ar.Extract(size) == StatusError)
	escape("unable to extract the size");

      for (i = 0; i < size; i++)
	{
	  Catalog::Entry*	entry;

	  entry = new Catalog::Entry;

	  // extract an entry.
	  if (ar.Extract(entry->name,
			 entry->address) == StatusError)
	    escape("unable to extract the entry");

	  // add it to the entries.
	  this->entries.push_back(entry);
	}

      leave();
    }

  }
}
