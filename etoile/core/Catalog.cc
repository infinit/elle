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
// updated       julien quintard   [wed dec 16 17:22:17 2009]
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
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constants defines the first index.
    ///
    const Contents::Offset	Catalog::Index::First =
      Type<Contents::Offset>::Minimum;

    const Contents::Offset	Catalog::Index::Last =
      Type<Contents::Offset>::Maximum;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Catalog::Catalog():
      state(StateClean)
    {
    }

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
    Status		Catalog::Prepare(const SecretKey&	key)
    {
      // set the key which will be used for decrypting the contents.
      this->key = key;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Add(const String&		name,
				     const hole::Address&	address)
    {
      Catalog::Entry*		entry;

      // look for an existing entry.
      if (this->Search(name) == StatusTrue)
	escape("another entry with the same name seems to already exist");

      // allocate a new entry.
      entry = new Catalog::Entry;

      // build the entry.
      entry->name = name;
      entry->address = address;

      // add the entry to the catalog entries.
      this->entries.push_back(entry);

      // mark the catalog as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Remove(const String&		name)
    {
      Catalog::Iterator		iterator;

      // look for an existing entry.
      if (this->Search(name, &iterator) == StatusFalse)
	escape("unable to find the given named entry");

      // delete the entry.
      this->entries.erase(iterator);

      // mark the catalog as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Lookup(const String&		name,
					hole::Address&		address)
    {
      Catalog::Iterator		iterator;

      // look for an existing entry.
      if (this->Search(name, &iterator) == StatusFalse)
	escape("unable to find the given named entry");

      // retrieve the value from the found entry.
      address = (*iterator)->address;

      leave();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Search(const String&		name,
					Catalog::Iterator*	pointer)
    {
      Catalog::Iterator		iterator;

      // go through the entries.
      for (iterator = this->entries.begin();
	   iterator != this->entries.end();
	   iterator++)
	{
	  Catalog::Entry*	entry = *iterator;

	  // check if the name is present.
	  if (name == entry->name)
	    {
	      // return an iterator if requested i.e different from NULL.
	      if (pointer != NULL)
		*pointer = iterator;

	      true();
	    }
	}

      false();
    }

    ///
    /// XXX
    ///
    Status		Catalog::Size(Contents::Offset&		size) const
    {
      // set the size.
      size = this->entries.size();

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an block object.
    ///
    Status		Catalog::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      String		shift(2, ' ');
      Catalog::Scoutor	scoutor;

      std::cout << alignment << "[Catalog]" << std::endl;

      if (ContentHashBlock::Dump(margin + 2) == StatusError)
	escape("unable to dump the underlying block");

      for (scoutor = this->entries.begin();
	   scoutor != this->entries.end();
	   scoutor++)
	{
	  Catalog::Entry*	entry = *scoutor;

	  std::cout << alignment << shift << "[Name] "
		    << entry->name << std::endl;

	  if (entry->address.Dump(margin + 2) == StatusError)
	    escape("unable to dump the address");
	}

      std::cout << alignment << shift << "[Key]" << std::endl;

      if (this->key.Dump(margin + 4) == StatusError)
	escape("unable to dump the key");

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
      Catalog::Scoutor	scoutor;
      Archive		pack;
      Cipher		cipher;

      // call the parent class.
      if (Contents::Serialize(archive) == StatusError)
	escape("unable to serialize the block");
      /*
      // create a sub-archive for containing the entries that will then
      // be encrypted.
      if (pack.Create() == StatusError)
	escape("unable to create a sub-archive");

      // serialize the number of entries.
      if (pack.Serialize((Contents::Offset)this->entries.size()) ==
	  StatusError)
	escape("unable to serialize the catalog size");

      // serialize the list of entries.
      for (scoutor = this->entries.begin();
	   scoutor != this->entries.end();
	   scoutor++)
	{
	  Catalog::Entry*	entry = *scoutor;

	  // serialize an entry.
	  if (pack.Serialize(entry->name,
			     entry->address) == StatusError)
	    escape("unable to serialize the entry");
	}

      // even serialize the key to make it easier to build a valid object back.
      if (pack.Serialize(this->key) == StatusError)
	escape("unable to serialize the key");

      // encrypt the pack with the internal secret key.
      if (this->key.Encrypt(pack, cipher) == StatusError)
	escape("unable to encrypt the packed entries");

      // finally, serialize the encrypted entries i.e the cipher.
      if (archive.Serialize(cipher) == StatusError)
	escape("unable to serialize the cipher");
      */

      leave();
    }

    ///
    /// this method extracts the block object.
    ///
    Status		Catalog::Extract(Archive&		archive)
    {
      Natural32		size;
      Natural32		i;

      // XXX to update with encryption

      // call the parent class.
      if (ContentHashBlock::Extract(archive) == StatusError)
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
