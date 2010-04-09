//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Set.cc
//
// created       julien quintard   [wed mar 31 23:36:12 2010]
// updated       julien quintard   [wed apr  7 20:55:47 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Set.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// destructor.
    ///
    Set::~Set()
    {
      Set::Iterator	i;

      // until the set is empty.
      while (this->container.empty() == false)
	{
	  Entry*	entry = this->container.front();

	  // remove the first element.
	  this->container.pop_front();

	  // delete the entry.
	  delete entry;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds an item to the set.
    ///
    Status		Set::Add(Entry*				entry)
    {
      enter();

      // add the entry to the container.
      this->container.push_back(entry);

      leave();
    }

    ///
    /// this method returns true if the set contains the given name.
    ///
    Status		Set::Exist(const path::Slice&		name)
    {
      enter();

      // try to locate the entry.
      if (this->Locate(name) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method removes the identified entry.
    ///
    /// the method returns true if the entry is found, false otherwise.
    ///
    Status		Set::Lookup(const path::Slice&		name,
				    Entry*&			entry)
    {
      Set::Iterator	iterator;

      enter();

      // locate the entry.
      if (this->Locate(name, &iterator) == false)
	false();

      // return the entry.
      entry = *iterator;

      true();
    }

    ///
    /// this method removes a entry from the set.
    ///
    Status		Set::Remove(const path::Slice&		name)
    {
      Set::Iterator	iterator;

      enter();

      // locate the entry.
      if (this->Locate(name, &iterator) == false)
	escape("this name does not seem to be present in this set");

      // erase the entry.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method returns the number of entries in the set.
    ///
    Status		Set::Capacity(Size&			size) const
    {
      enter();

      // return the size.
      size = this->container.size();

      leave();
    }

    ///
    /// this method returns an iterator on the identified entry. this
    /// method can easily be used to test if a name belong to the set:
    /// Locate(name) returns true or false.
    ///
    /// the method returns true if the entry is found, false otherwise.
    ///
    Status		Set::Locate(const path::Slice&		name,
				    Set::Iterator*		iterator)
    {
      Set::Iterator	i;

      enter();

      // go through the container.
      for (i = this->container.begin();
	   i != this->container.end();
	   i++)
	{
	  Entry*	entry = *i;

	  if (entry->name == name)
	    {
	      // return the iterator, if wanted.
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
    /// this function dumps a set.
    ///
    Status		Set::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');
      Set::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Set]" << std::endl;

      // dump every entry
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Entry*	entry = *scoutor;

	  // dump the entry.
	  if (entry->Dump(margin + 2) == StatusError)
	    escape("unable to dump the entry");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the set object.
    ///
    Status		Set::Serialize(Archive&			archive) const
    {
      Set::Scoutor	scoutor;
      Size		size;

      enter();

      // retrieve the number of entries.
      size = this->container.size();

      // serialize the number of entries.
      if (archive.Serialize(size) == StatusError)
	escape("unable to serialize the number of entries");

      // serialize every entry.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Entry*	entry = *scoutor;

	  // serialize the entry.
	  if (archive.Serialize(*entry) == StatusError)
	    escape("unable to serialize the entry");
	}

      leave();
    }

    ///
    /// this method extracts the set object.
    ///
    Status		Set::Extract(Archive&			archive)
    {
      Size		size;
      Index		i;

      enter();

      // extract the number of entries.
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the number of entry");

      // extract every entry.
      for (i = 0; i < size; i++)
	{
	  Entry*	entry;

	  enter(instance(entry));

	  // allocate a new entry.
	  entry = new Entry;

	  // extract the entry.
	  if (archive.Extract(*entry) == StatusError)
	    escape("unable to extract the entry");

	  // add the entry to the container.
	  this->container.push_back(entry);

	  // stop tracking the entry.
	  waive(entry);

	  release();
	}

      leave();
    }

  }
}
