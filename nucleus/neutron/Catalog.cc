//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Catalog.cc
//
// created       julien quintard   [wed mar 11 16:55:36 2009]
// updated       julien quintard   [wed may  4 23:11:07 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Catalog.hh>

namespace nucleus
{
  namespace neutron
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
    elle::Status	Catalog::Add(Entry*			entry)
    {
      enter();

      // add the entry in the range.
      if (this->range.Add(entry) == elle::StatusError)
	escape("unable to add the entry in the range");

      // range the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method tests if the given name exists.
    ///
    elle::Status	Catalog::Exist(const elle::String&	name)
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
    elle::Status	Catalog::Lookup(const elle::String&	name,
					Entry*&			entry)
    {
      enter();

      // look in the range.
      if (this->range.Lookup(name, entry) == elle::StatusError)
	escape("unable to retrieve the entry");

      leave();
    }

    ///
    /// this method returns a subrange of the catalog delimited by the given
    /// index and size.
    ///
    elle::Status	Catalog::Consult(const Index&		index,
					 const Size&		size,
					 Range<Entry>&		range)
      const
    {
      Range<Entry>::Scoutor	scoutor;
      Index			i;

      enter();

      // first detach the data from the range.
      if (range.Detach() == elle::StatusError)
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
	      if (range.Add(entry) == elle::StatusError)
		escape("unable to add the entry to the range");
	    }
	}

      leave();
    }

    ///
    /// this method removes the given entry.
    ///
    elle::Status	Catalog::Remove(const elle::String&	name)
    {
      enter();

      // remove the entry from the range.
      if (this->range.Remove(name) == elle::StatusError)
	escape("unable to remove the entry");

      // range the object as dirty.
      this->state = StateDirty;

      leave();
    }

    ///
    /// this method renames an entry.
    ///
    elle::Status	Catalog::Rename(const elle::String&	from,
					const elle::String&	to)
    {
      Entry*		entry;

      enter();

      // look in the range.
      if (this->range.Lookup(from, entry) == elle::StatusError)
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
    elle::Status	Catalog::Capacity(Size&			size) const
    {
      enter();

      // look at the size of the range.
      if (this->range.Capacity(size) == elle::StatusError)
	escape("unable to retrieve the range size");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Catalog, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the catalog.
    ///
    elle::Status	Catalog::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Catalog]" << std::endl;

      // dump the state.
      std::cout << alignment << elle::Dumpable::Shift << "[State] "
		<< this->state << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the range");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the catalog object.
    ///
    elle::Status	Catalog::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the range.
      if (archive.Serialize(this->range) == elle::StatusError)
	escape("unable to serialize the range");

      leave();
    }

    ///
    /// this method extracts the catalog object.
    ///
    elle::Status	Catalog::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the range.
      if (archive.Extract(this->range) == elle::StatusError)
	escape("unable to extract the range");

      leave();
    }

  }
}
