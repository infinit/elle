//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// author        julien quintard   [sun apr 18 11:02:43 2010]
//

#ifndef LUNE_MAP_HXX
#define LUNE_MAP_HXX

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
        std::cout << alignment << elle::Dumpable::Shift << "[Name] "
                  << entry->name << std::endl;

        // dump the value.
        if (entry->value.Dump(margin + 2) == elle::Status::Error)
          escape("unable to dump the value");
      }

    return elle::Status::Ok;
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes a map.
  ///
  template <typename T>
  elle::Status          Map<T>::Serialize(elle::Archive&        archive) const
  {
    Map<T>::Scoutor     scoutor;

    // serialize the number of entries.
    if (archive.Serialize(
          static_cast<elle::Natural32>(
            this->container.size())) == elle::Status::Error)
      escape("unable to serialize the number of entries");

    // go through the entries.
    for (scoutor = this->container.begin();
         scoutor != this->container.end();
         scoutor++)
      {
        Map<T>::Entry*  entry = *scoutor;

        // serialize the entry.
        if (archive.Serialize(entry->name,
                              entry->value) == elle::Status::Error)
          escape("unable to serialize the entry");
      }

    return elle::Status::Ok;
  }

  ///
  /// this method extracts a map.
  ///
  template <typename T>
  elle::Status          Map<T>::Extract(elle::Archive&          archive)
  {
    elle::Natural32             size;
    elle::Natural32             i;

    // extract the number of entries.
    if (archive.Extract(size) == elle::Status::Error)
      escape("unable to extract the number of entries");

    // until all the entries have been extract.
    for (i = 0; i < size; i++)
      {
        Map<T>::Entry*  entry;

        // allocate a new entry.
        entry = new Map<T>::Entry;

        // extract the entry.
        if (archive.Extract(entry->name, entry->value) == elle::Status::Error)
          escape("unable to extract the entry");

        // add a new entry.
        this->container.push_back(entry);
      }

    return elle::Status::Ok;
  }

}

#endif
