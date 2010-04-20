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
// updated       julien quintard   [tue apr 20 08:08:40 2010]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Catalog::Catalog():
      state(StateClean)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds the given entry to the catalog.
    ///
    Status		Catalog::Add(Entry*			entry)
    {
      enter();

      // add the entry in the range.
      if (this->range.Add(entry) == StatusError)
	escape("unable to add the entry in the range");

      // range the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method tests if the given name exists.
    ///
    Status		Catalog::Exist(const path::Slice&	name)
    {
      enter();

      // test.
      if (this->range.Exist(name) == false)
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

      // look in the range.
      if (this->range.Lookup(name, entry) == StatusError)
	escape("unable to retrieve the entry");

      leave();
    }

    ///
    /// this method returns a subrange of the catalog delimited by the given
    /// index and size.
    ///
    Status		Catalog::Consult(const Index&		index,
					 const Size&		size,
					 Range<Entry>&		range)
      const
    {
      Range<Entry>::Scoutor	scoutor;
      Index			i;

      enter();

      // first detach the data from the range.
      if (range.Detach() == StatusError)
	escape("unable to detach the data from the range");

      // go through the catalog entries.
      for (scoutor = this->range.container.begin(), i = 0;
	   scoutor != this->range.container.end();
	   scoutor++, i++)
	{
	  Entry*	entry = *scoutor;

	  // if this entry lies in the selected range [index, index + size[
	  if ((i >= index) && (i < (index + size)))
	    {
	      // add the entry to the range.
	      if (range.Add(entry) == StatusError)
		escape("unable to add the entry to the range");
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

      // remove the entry from the range.
      if (this->range.Remove(name) == StatusError)
	escape("unable to remove the entry");

      // range the object as dirty.
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

      // look in the range.
      if (this->range.Lookup(from, entry) == StatusError)
	escape("unable to retrieve the entry");

      // modify the name.
      entry->name = to;

      // range the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method returns the size of the catalog.
    ///
    Status		Catalog::Capacity(Size&			size) const
    {
      enter();

      // look at the size of the range.
      if (this->range.Capacity(size) == StatusError)
	escape("unable to retrieve the range size");

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

      // dump the range.
      if (this->range.Dump(margin + 2) == StatusError)
	escape("unable to dump the range");

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

      // serialize the range.
      if (archive.Serialize(this->range) == StatusError)
	escape("unable to serialize the range");

      leave();
    }

    ///
    /// this method extracts the catalog object.
    ///
    Status		Catalog::Extract(Archive&		archive)
    {
      enter();

      // extract the range.
      if (archive.Extract(this->range) == StatusError)
	escape("unable to extract the range");

      leave();
    }

  }
}
