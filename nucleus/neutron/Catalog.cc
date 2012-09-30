#include <nucleus/neutron/Catalog.hh>

#include <nucleus/proton/Contents.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant defines the component for the porcupine's seams
    /// associated with Catalogs.
    ///
    const Component                     Catalog::S = ComponentSeamCatalog;

    ///
    /// this constant defines the component for the porcupine's quills
    /// associated with Catalogs.
    ///
    const Component                     Catalog::Q = ComponentQuillCatalog;

//
// ---------- constructors & destructors --------------------------------------
//

    Catalog::Catalog(proton::Contents<Catalog>& other):
      contents(other)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates the catalog.
    ///
    elle::Status        Catalog::Create()
    {
      this->contents.state(proton::StateDirty);

      return elle::Status::Ok;
    }

    ///
    /// this method adds the given entry to the catalog.
    ///
    elle::Status        Catalog::Add(Entry*                     entry)
    {
      // check that the entry is non-empty.
      if (entry->name.empty() == true)
        escape("unable to create an empty-named entry in the catalog");

      // add the entry in the range.
      if (this->range.Add(entry) == elle::Status::Error)
        escape("unable to add the entry in the range");

      // range the object as dirty.
      this->contents.state(proton::StateDirty);

      return elle::Status::Ok;
    }

    ///
    /// this method tests if the given name exists.
    ///
    elle::Status        Catalog::Exist(const elle::String&      name) const
    {
      // test.
      if (this->range.Exist(name) == false)
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// this method returns the entry corresponding to the given name.
    ///
    elle::Status        Catalog::Lookup(const elle::String&     name,
                                        Entry const*& entry) const
    {
      // look in the range.
      if (this->range.Lookup(name, entry) == elle::Status::Error)
        escape("unable to retrieve the entry");

      return elle::Status::Ok;
    }

    ///
    /// this method returns a subrange of the catalog delimited by the given
    /// index and size.
    ///
    elle::Status        Catalog::Consult(const Index&           index,
                                         const Size&            size,
                                         Range<Entry>&          range) const
    {
      Range<Entry>::Scoutor     scoutor;
      Index                     i;

      // first detach the data from the range.
      if (range.Detach() == elle::Status::Error)
        escape("unable to detach the data from the range");

      // go through the catalog entries.
      for (scoutor = this->range.container.begin(), i = 0;
           scoutor != this->range.container.end();
           scoutor++)
        {
          Entry*        entry = *scoutor;

          // if this entry lies in the selected range [index, index + size[
          if ((i >= index) && (i < (index + size)))
            {
              // check if the entry is empty: this should never
              // happen but you never know!
              if (entry->name.empty() == true)
                continue;

              // add the entry to the range.
              if (range.Add(entry) == elle::Status::Error)
                escape("unable to add the entry to the range");
            }

          // increment the index.
          //
          // note that this is done at the end so that empty
          // entries are not taken into account.
          i++;
        }

      return elle::Status::Ok;
    }

    ///
    /// this method removes the given entry.
    ///
    elle::Status        Catalog::Remove(const elle::String&     name)
    {
      // remove the entry from the range.
      if (this->range.Remove(name) == elle::Status::Error)
        escape("unable to remove the entry");

      // range the object as dirty.
      this->contents.state(proton::StateDirty);

      return elle::Status::Ok;
    }

    ///
    /// this method renames an entry.
    ///
    elle::Status        Catalog::Rename(const elle::String&     from,
                                        const elle::String&     to)
    {
      Entry* entry;

      // if _from_ and _to_ are identical, return.
      if (from == to)
        return elle::Status::Ok;

      // check that the entry is non-empty.
      if (to.empty() == true)
        escape("unable to rename to a non empty-named entry in the catalog");

      // check that an entry _to_ does not already exist.
      if (this->range.Exist(to) == elle::Status::True)
        escape("an entry already exists with the to-be-renamed name");

      // look in the range.
      if (this->range.Lookup(from, entry) == elle::Status::Error)
        escape("unable to retrieve the entry");

      // modify the name.
      entry->name = to;

      // range the object as dirty.
      this->contents.state(proton::StateDirty);

      return elle::Status::Ok;
    }

    ///
    /// this method returns the size of the catalog.
    ///
    elle::Status        Catalog::Capacity(Size&                 size) const
    {
      // look at the size of the range.
      if (this->range.Capacity(size) == elle::Status::Error)
        escape("unable to retrieve the range size");

      return elle::Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps the catalog.
    ///
    elle::Status        Catalog::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Catalog]" << std::endl;

      // dump the range.
      if (this->range.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the range");

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Catalog::print(std::ostream& stream) const
    {
      stream << "catalog("
             << this->range
             << ")";
    }

  }
}
