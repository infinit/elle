#ifndef NUCLEUS_NEUTRON_RANGE_HXX
# define NUCLEUS_NEUTRON_RANGE_HXX

# include <nucleus/neutron/Size.hh>

# include <elle/standalone/Log.hh>
# include <elle/standalone/Report.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// default constructor.
    ///
    template <typename T>
    Range<T>::Range():
      options(Range<T>::OptionNone)
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename T>
    Range<T>::Range(const Range<T>&                             element):
      Object(element),

      options(element.options)
    {
      Range<T>::Scoutor         scoutor;

      // go through the container.
      for (scoutor = element.container.begin();
           scoutor != element.container.end();
           scoutor++)
        {
          T*                    item;

          // copy the item depending on the options.
          switch (this->options)
            {
            case Range<T>::OptionNone:
              {
                // in this case, the data must be duplicated.
                item = new T(**scoutor);

                break;
              }
            case Range<T>::OptionDetach:
              {
                // in this case, the memory is not handled by this instance,
                // hence just copy the pointer.
                item = *scoutor;

                break;
              }
            }

          // add the item to the container.
          if (this->Add(item) == elle::Status::Error)
            fail("unable to add the item to the container");
        }
    }

    ///
    /// destructor.
    ///
    template <typename T>
    Range<T>::~Range()
    {
      Range<T>::Iterator        i;

      // until the range is empty.
      while (this->container.empty() == false)
        {
          T*    item = this->container.front();

          // remove the first element.
          this->container.pop_front();

          // delete the item, if necessary.
          if ((this->options & Range<T>::OptionDetach) == 0)
            delete item;
        }
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds an item to the range.
    ///
    template <typename T>
    elle::Status        Range<T>::Add(T*                        item)
    {
      // check if another item exists with this symbol.
      if (this->Exist(item->symbol()) == true)
        escape("an item with this symbol already exist");

      // add the item to the container.
      this->container.push_back(item);

      return elle::Status::Ok;
    }

    template <typename T>
    elle::Status
    Range<T>::Add(Range<T> const& other)
    {
      if (this->options != other.options)
        escape("unable to merge ranges with different options");

      for (T* item: other.container)
        {
          switch (this->options)
            {
            case Range<T>::OptionNone:
              {
                if (this->Add(new T(*item)) == elle::Status::Error)
                  escape("unable to add the item");

                break;
              }
            case Range<T>::OptionDetach:
              {
                if (this->Add(item) == elle::Status::Error)
                  escape("unable to add the item");

                break;
              }
            }
        }

      return elle::Status::Ok;
    }

    ///
    /// this method returns true if an item for the given symbol exists.
    ///
    template <typename T>
    elle::Status        Range<T>::Exist(const Range<T>::Symbol& symbol) const
    {
      Range<T>::Scoutor scoutor;

      // try to locate the entry.
      if (this->Locate(symbol, scoutor) != elle::Status::True)
        return elle::Status::False;

      return elle::Status::True;
    }

    ///
    /// this method looks up the range.
    ///
    /// the method returns true if the item is found, false otherwise.
    ///
    template <typename T>
    elle::Status        Range<T>::Lookup(const Range<T>::Symbol& symbol,
                                         T const*& item) const
    {
      Range<T>::Scoutor scoutor;

      // initialize the pointer to null.
      item = nullptr;

      // try to locate the item.
      if (this->Locate(symbol, scoutor) == false)
        return elle::Status::False;

      // return the item.
      item = *scoutor;

      return elle::Status::True;
    }

    template <typename T>
    elle::Status        Range<T>::Lookup(const Range<T>::Symbol& symbol,
                                         T*& item) const
    {
      Range<T>::Scoutor scoutor;

      // initialize the pointer to null.
      item = nullptr;

      // try to locate the item.
      if (this->Locate(symbol, scoutor) == false)
        return elle::Status::False;

      // return the item.
      item = *scoutor;

      return elle::Status::True;
    }

    ///
    /// this method removes an item from the range.
    ///
    template <typename T>
    elle::Status        Range<T>::Remove(const Range<T>::Symbol& symbol)
    {
      Range<T>::Iterator        iterator;

      // locate the item.
      if (this->Locate(symbol, iterator) == false)
        escape("this symbol does not seem to be present in this range");

      // delete the item.
      delete *iterator;

      // erase the item from the container.
      this->container.erase(iterator);

      return elle::Status::Ok;
    }

    ///
    /// this method returns the number of items in the range.
    ///
    template <typename T>
    elle::Status        Range<T>::Capacity(Size&                size) const
    {
      // return the size.
      size = this->container.size();

      return elle::Status::Ok;
    }

    ///
    /// this method returns a scoutor on the identified item.
    ///
    template <typename T>
    elle::Status        Range<T>::Locate(const Range<T>::Symbol& symbol,
                                         Range<T>::Scoutor&     scoutor) const
    {
      Range<T>::Scoutor s;

      // go through the container.
      for (s = this->container.begin();
           s != this->container.end();
           s++)
        {
          T*                    item = *s;

          // if found...
          if (item->symbol() == symbol)
            {
              // return the scoutor.
              scoutor = s;

              return elle::Status::True;
            }
        }

      return elle::Status::False;
    }

    ///
    /// this method returns an iterator on the identified item.
    ///
    /// the method returns true if the item is found, false otherwise.
    ///
    template <typename T>
    elle::Status        Range<T>::Locate(const Range<T>::Symbol& symbol,
                                         Range<T>::Iterator&    iterator)
    {
      Range<T>::Iterator        i;

      // go through the container.
      for (i = this->container.begin();
           i != this->container.end();
           i++)
        {
          T*                    item = *i;

          // if found...
          if (item->symbol() == symbol)
            {
              // return the iterator.
              iterator = i;

              return elle::Status::True;
            }
        }

      return elle::Status::False;
    }

    ///
    /// this method instructs the range to detach the memory management
    /// from the object so that items do not get deleted once the range
    /// dies.
    ///
    template <typename T>
    elle::Status        Range<T>::Detach()
    {
      // activate the option.
      this->options = Range<T>::OptionDetach;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    template <typename T>
    elle::Boolean       Range<T>::operator==(const Range<T>&    element) const
    {
      Range<T>::Scoutor s;
      Range<T>::Scoutor t;

      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the sizes.
      if (this->container.size() != element.container.size())
        return false;

      // go through the elements.
      for (s = this->container.begin(), t = element.container.begin();
           s != this->container.end();
           s++, t++)
        {
          // compare the entries.
          if (*s != *t)
            return false;
        }

      return true;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Range<T>, _(template <typename T>));

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a range.
    ///
    template <typename T>
    elle::Status        Range<T>::Dump(elle::Natural32          margin) const
    {
      elle::String      alignment(margin, ' ');
      Range<T>::Scoutor scoutor;

      std::cout << alignment << "[Range] "
                << std::dec << this->container.size() << std::endl;

      // dump the options.
      std::cout << alignment << elle::io::Dumpable::Shift << "[Options] "
                << this->options << std::endl;

      // dump every item.
      for (scoutor = this->container.begin();
           scoutor != this->container.end();
           scoutor++)
        {
          T*            item = *scoutor;

          // dump the item.
          if (item->Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the item");
        }

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SPLIT_T1(nucleus::neutron::Range);

ELLE_SERIALIZE_SPLIT_T1_LOAD(nucleus::neutron::Range,
                             archive,
                             value,
                             version)
{
  typedef typename Archive::SequenceSizeType SizeType;
  assert(version == 0);

  SizeType size;
  archive >> size;

  for (SizeType i = 0; i < size; ++i)
    value.container.push_back(
        archive.template Construct<T1>().release()
    );
}

ELLE_SERIALIZE_SPLIT_T1_SAVE(nucleus::neutron::Range,
                             archive,
                             value,
                             version)
{
  typedef typename Archive::SequenceSizeType SizeType;
  assert(version == 0);
  archive << static_cast<SizeType>(value.container.size());

  auto it = value.container.begin(),
       end = value.container.end();
  for (; it != end; ++it)
    archive << *(*it);
}

#endif
