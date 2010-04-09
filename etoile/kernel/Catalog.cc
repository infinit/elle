//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Catalog.cc
//
// created       julien quintard   [wed mar 11 16:55:36 2009]
// updated       julien quintard   [wed apr  7 20:56:36 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/kernel/Catalog.hh>

namespace etoile
{
  namespace kernel
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the given entry to the catalog.
    ///
    Status		Catalog::Add(Entry*			entry)
    {
      enter();

      // add the entry in the set.
      if (this->set.Add(entry) == StatusError)
	escape("unable to add the entry in the set");

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns true if the name exists in the catalog.
    ///
    Status		Catalog::Exist(const path::Slice&	name)
    {
      enter();

      // look in the set.
      if (this->set.Exist(name) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method returns the entry corresponding to the given name.
    ///
    Status		Catalog::Lookup(const path::Slice&	name,
					Entry*&			entry)
    {
      enter();

      // look in the set.
      if (this->set.Lookup(name, entry) == StatusError)
	escape("unable to retrieve the entry");

      leave();
    }

    ///
    /// this method returns a subset of the catalog delimited by the given
    /// index and size.
    ///
    Status		Catalog::Consult(const Index&		index,
					 const Size&		size,
					 Set&			set)
      const
    {
      Set::Scoutor	scoutor;
      Index		i;

      enter();

      // go through the catalog entries.
      for (scoutor = this->set.container.begin(), i = 0;
	   scoutor != this->set.container.end();
	   scoutor++, i++)
	{
	  Entry*	entry = *scoutor;

	  // if this entry lies in the selected set [index, index + size[
	  if ((i >= index) && (i < (index + size)))
	    {
	      Entry*	e;

	      enter(instance(e));

	      // allocate a new entry.
	      e = new Entry(*entry);

	      // add the new entry to the set.
	      if (set.Add(e) == StatusError)
		escape("unable to add the name to the container");

	      // waive.
	      waive(e);

	      release();
	    }
	}

      leave();
    }

    ///
    /// this method removes the given entry.
    ///
    Status		Catalog::Remove(const path::Slice&	name)
    {
      enter();

      // remove the entry from the set.
      if (this->set.Remove(name) == StatusError)
	escape("unable to remove the entry");

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method renames an entry.
    ///
    Status		Catalog::Rename(const path::Slice&	from,
					const path::Slice&	to)
    {
      Entry*		entry;

      enter();

      // look in the set.
      if (this->set.Lookup(from, entry) == StatusError)
	escape("unable to retrieve the entry");

      // modify the name.
      entry->name = to;

      // set the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns the size of the catalog.
    ///
    Status		Catalog::Capacity(Size&			size) const
    {
      enter();

      // look at the size of the set.
      if (this->set.Capacity(size) == StatusError)
	escape("unable to retrieve the set size");

      leave();
    }

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the entity.
    ///
    embed(Entity, Catalog);

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the catalog.
    ///
    Status		Catalog::Dump(Natural32			margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Catalog]" << std::endl;

      // dump the state.
      std::cout << alignment << Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the set.
      if (this->set.Dump(margin + 2) == StatusError)
	escape("unable to dump the set");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the catalog object.
    ///
    Status		Catalog::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the set.
      if (archive.Serialize(this->set) == StatusError)
	escape("unable to serialize the set");

      leave();
    }

    ///
    /// this method extracts the catalog object.
    ///
    Status		Catalog::Extract(Archive&		archive)
    {
      enter();

      // extract the set.
      if (archive.Extract(this->set) == StatusError)
	escape("unable to extract the set");

      leave();
    }

  }
}
