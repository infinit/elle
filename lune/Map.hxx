#ifndef LUNE_MAP_HXX
# define LUNE_MAP_HXX

# include <elle/standalone/Log.hh>

namespace lune
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// these variables are used to enable the Lookup() methods to take a
  /// default argument.
  ///
  template <typename T>
  elle::String*                 Map<T>::Trash::Name = NULL;

  template <typename T>
  T*                            Map<T>::Trash::Value = NULL;

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// destructor.
  ///
  template <typename T>
  Map<T>::~Map()
  {
    Map<T>::Scoutor     scoutor;

    // release the entries.
    for (scoutor = this->container.begin();
         scoutor != this->container.end();
         scoutor++)
      {
        Map<T>::Entry*  entry = *scoutor;

        // delete the entry.
        delete entry;
      }

    // clear the container.
    this->container.clear();
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method adds an entry to the map.
  ///
  template <typename T>
  elle::Status          Map<T>::Add(const elle::String&         name,
                                    const T&                    value)
  {
    Map<T>::Entry*      entry;

    // check if the entry exists.
    if (this->Lookup(name) == elle::Status::True)
      escape("this name is already registered");

    // allocate a new entry.
    entry = new Map<T>::Entry;

    // create the entry.
    entry->name = name;
    entry->value = value;

    // add a new entry.
    this->container.push_back(entry);

    return elle::Status::Ok;
  }

  ///
  /// this method returns the maped value.
  ///
  template <typename T>
  elle::Status          Map<T>::Lookup(const elle::String&      name,
                                       T*&                      value)
  {
    Map<T>::Scoutor     scoutor;

    // go through the entries.
    for (scoutor = this->container.begin();
         scoutor != this->container.end();
         scoutor++)
      {
        Map<T>::Entry*  entry = *scoutor;

        // if the entry is found...
        if (entry->name == name)
          {
            // return the value.
            value = &entry->value;

            return elle::Status::True;
          }
      }

    return elle::Status::False;
  }

  ///
  /// this method returns the mapped name.
  ///
  template <typename T>
  elle::Status          Map<T>::Lookup(const T&                 value,
                                       elle::String*&           name)
  {
    Map<T>::Scoutor     scoutor;

    // go through the entries.
    for (scoutor = this->container.begin();
         scoutor != this->container.end();
         scoutor++)
      {
        Map<T>::Entry*  entry = *scoutor;

        // if the entry is found...
        if (entry->value == value)
          {
            // return the name.
            name = &entry->name;

            return elle::Status::True;
          }
      }

    return elle::Status::False;
  }

  ///
  /// this method removes an entry.
  ///
  template <typename T>
  elle::Status          Map<T>::Remove(const elle::String&      name)
  {
    Map<T>::Iterator    iterator;

    // go through the entries.
    for (iterator = this->container.begin();
         iterator != this->container.end();
         iterator++)
      {
        Map<T>::Entry*  entry = *iterator;

        // if the entry is found...
        if (entry->name == name)
          {
            // release the memory.
            delete entry;

            // erase the entry.
            this->container.erase(iterator);

            return elle::Status::Ok;
          }
      }

    escape("unable to locate the given name");
  }

//
// ---------- object ----------------------------------------------------------
//

  ///
  /// this macro-function call generates the object.
  ///
  embed(Map<T>, _(template <typename T>));

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the map.
  ///
  template <typename T>
  elle::Status          Map<T>::Dump(const elle::Natural32      margin) const
  {
    Map<T>::Scoutor     scoutor;
    elle::String        alignment(margin, ' ');

    std::cout << alignment << "[Map]" << std::endl;

    // go through the entries.
    for (scoutor = this->container.begin();
         scoutor != this->container.end();
         scoutor++)
      {
        Map<T>::Entry*  entry = *scoutor;

        // dump the name.
        std::cout << alignment << elle::io::Dumpable::Shift << "[Name] "
                  << entry->name << std::endl;

        // dump the value.
        if (entry->value.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the value");
      }

    return elle::Status::Ok;
  }

}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <lune/Map.hh>

ELLE_SERIALIZE_SPLIT_T1(lune::Map);

ELLE_SERIALIZE_SPLIT_T1_SAVE(lune::Map,
                             archive,
                             value,
                             version)
{
  assert(version == 0);

  archive << static_cast<
      typename Archive::SequenceSizeType
  >(value.container.size());

  auto it = value.container.begin(),
       end = value.container.end();
  for (; it != end; ++it)
    {
      typename lune::Map<T1>::Entry* entry = *it;
      archive << entry->name << entry->value;
    }
}

ELLE_SERIALIZE_SPLIT_T1_LOAD(lune::Map,
                             archive,
                             value,
                             version)
{
  assert(version == 0);

  typename Archive::SequenceSizeType size;
  archive >> size;

  for (typename Archive::SequenceSizeType i = 0; i < size; ++i)
    {
      std::unique_ptr<typename lune::Map<T1>::Entry> entry(
          new typename lune::Map<T1>::Entry
      );
      archive >> entry->name >> entry->value;
      value.container.push_back(entry.get());
      entry.release();
    }
}

#endif
