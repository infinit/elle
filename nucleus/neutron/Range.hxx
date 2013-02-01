#ifndef NUCLEUS_NEUTRON_RANGE_HXX
# define NUCLEUS_NEUTRON_RANGE_HXX

# include <elle/io/Dumpable.hh>

# include <nucleus/neutron/Size.hh>
# include <nucleus/Exception.hh>

namespace nucleus
{
  namespace neutron
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Range<T>::Range()
    {
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    void
    Range<T>::insert(std::shared_ptr<T const> const& item)
    {
      // Check if another item exists with this symbol.
      if (this->exist(item->symbol()) == true)
        throw Exception("an item with the symbol '%s' already exist",
                        item->symbol());

      // Insert the item to the container.
      this->_container.push_back(item);
    }

    template <typename T>
    void
    Range<T>::add(Range<T> const& other)
    {
      for (auto& item: other._container)
        this->insert(item);
    }

    template <typename T>
    elle::Boolean
    Range<T>::exist(Symbol const& symbol) const
    {
      for (auto& item: this->_container)
        {
          if (item->symbol() == symbol)
            return (true);
        }

      return (false);
    }

    template <typename T>
    T const&
    Range<T>::locate(Symbol const& symbol) const
    {
      Scoutor scoutor{this->_iterator(symbol)};

      return (*scoutor);
    }

    template <typename T>
    void
    Range<T>::erase(Symbol const& symbol)
    {
      Iterator iterator{this->_iterator(symbol)};

      this->_container.erase(iterator);
    }

    template <typename T>
    elle::Size
    Range<T>::size() const
    {
      return (this->_container.size());
    }

    template <typename T>
    typename Range<T>::Scoutor
    Range<T>::_iterator(Symbol const& symbol) const
    {
      Scoutor scoutor;

      for (scoutor = this->_container.begin();
           scoutor != this->_container.end();
           scoutor++)
        {
          auto& item = *scoutor;

          if (item->symbol() == symbol)
            return (scoutor);
        }

      throw Exception("unable to locate the given symbol's item");
    }

    template <typename T>
    typename Range<T>::Iterator
    Range<T>::_iterator(Symbol const& symbol)
    {
      Iterator iterator;

      for (iterator = this->_container.begin();
           iterator != this->_container.end();
           iterator++)
        {
          auto& item = *iterator;

          if (item->symbol() == symbol)
            return (iterator);
        }

      throw Exception("unable to locate the given symbol's item");
    }

    /*---------.
    | Dumpable |
    `---------*/

    template <typename T>
    elle::Status        Range<T>::Dump(elle::Natural32          margin) const
    {
      elle::String      alignment(margin, ' ');
      Scoutor scoutor;

      std::cout << alignment << "[Range] "
                << std::dec << this->_container.size() << std::endl;

      // dump every item.
      for (auto& item: this->_container)
        {
          // dump the item.
          if (item->Dump(margin + 2) == elle::Status::Error)
            throw Exception("unable to dump the item");
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Range<T>::print(std::ostream& stream) const
    {
      stream << "range("
             << "#" << this->_container.size()
             << ")";
    }

    /*---------.
    | Iterable |
    `---------*/

    template <typename T>
    typename Range<T>::Scoutor
    Range<T>::begin() const
    {
      return (this->_container.begin());
    }

    template <typename T>
    typename Range<T>::Scoutor
    Range<T>::end() const
    {
      return (this->_container.end());
    }
  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/finally.hh>
# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT_T1(nucleus::neutron::Range);

ELLE_SERIALIZE_SPLIT_T1_LOAD(nucleus::neutron::Range,
                             archive,
                             value,
                             version)
{
  nucleus::neutron::Size size;
  nucleus::neutron::Size i;

  enforce(version == 0);

  archive >> size;

  for (i = 0; i < size; ++i)
    {
      std::shared_ptr<T1> item{
        archive.template Construct<T1>().release()};

      value._container.push_back(item);
    }
}

ELLE_SERIALIZE_SPLIT_T1_SAVE(nucleus::neutron::Range,
                             archive,
                             value,
                             version)
{
  enforce(version == 0);

  archive << static_cast<nucleus::neutron::Size>(value._container.size());

  for (auto& item: value._container)
    archive << *item;
}

#endif
