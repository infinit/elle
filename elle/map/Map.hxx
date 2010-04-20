//
// ---------- header ----------------------------------------------------------
//
// project       8access
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/map/Map.hxx
//
// created       julien quintard   [sun apr 18 11:02:43 2010]
// updated       julien quintard   [mon apr 19 09:47:08 2010]
//

namespace map
{

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this variable is used to enable the Lookup() method to take a
  /// default argument.
  ///
  template <typename T>
  const T*			Map<T>::Trash;

//
// ---------- static methods --------------------------------------------------
//

  ///
  /// this method loads a map.
  ///
  template <typename T>
  Status		Map<T>::Load(const String&		path,
				     Map&			map)
  {
    Archive		archive;
    Region		region;
    struct ::stat	stat;
    int			fd;

    enter();

    // retrieve information on the map.
    if (lstat(path.c_str(), &stat) == -1)
      {
	//
	// if no map exists, create one, meaning, do nothin :)
	//
      }
    else
      {
	//
	// otherwise, read the map file.
	//

	// prepare the region.
	if (region.Prepare(stat.st_size) == StatusError)
	  escape("unable to prepare the region");

	// adjust the region's size.
	region.size = stat.st_size;

	// open the map.
	if ((fd = open(path.c_str(), O_RDONLY)) == -1)
	  escape("unable to open the file");

	// read the map.
	if (read(fd, region.contents, region.size) == -1)
	  escape("unable to read the region");

	// close the file.
	if (close(fd) == -1)
	  escape("unable to close the file");

	// detach the data from the region to prevent multiple
	// resources release.
	if (region.Detach() == StatusError)
	  escape("unable to detach the region");

	// prepare the archive.
	if (archive.Prepare(region) == StatusError)
	  escape("unable to prepare the archive");

	// extract the map.
	if (archive.Extract(map) == StatusError)
	  escape("unable to extract the map");
      }

    leave();
  }

  ///
  /// this method stores a map.
  ///
  template <typename T>
  Status		Map<T>::Store(const String&		path,
				      const Map&		map)
  {
    Archive		archive;
    int			fd;

    enter();

    // create the archive.
    if (archive.Create() == StatusError)
      escape("unable to create the archive");

    // serialize the map.
    if (archive.Serialize(map) == StatusError)
      escape("unable to serialize the map");

    // open the map.
    if ((fd = open(path.c_str(),
		   O_WRONLY | O_TRUNC | O_CREAT,
		   0600)) == -1)
      escape("unable to open the file");

    // write the map.
    if (write(fd, archive.contents, archive.size) != archive.size)
      escape("unable to write the archive");

    // close the file.
    if (close(fd) == -1)
      escape("unable to close the file");

    leave();
  }

//
// ---------- methods ---------------------------------------------------------
//

  ///
  /// this method adds an entry to the map.
  ///
  template <typename T>
  Status		Map<T>::Add(const String&		name,
				    const T&			value)
  {
    enter();

    // check if the entry exists.
    if (this->Lookup(name) == StatusTrue)
      escape("this name is already registered");

    // add a new entry.
    this->container.push_back(Map<T>::Value(name, value));

    leave();
  }

  ///
  /// this method returns the associated value.
  ///
  template <typename T>
  Status		Map<T>::Lookup(const String&		name,
				       const T*&		value)
  {
    Map<T>::Scoutor	scoutor;

    enter();

    // go through the entries.
    for (scoutor = this->container.begin();
	 scoutor != this->container.end();
	 scoutor++)
      {
	// if the entry is found...
	if (scoutor->first == name)
	  {
	    // return the value.
	    value = &scoutor->second;

	    true();
	  }
      }

    false();
  }

  ///
  /// this method returns the associated name.
  ///
  template <typename T>
  Status		Map<T>::Lookup(const T&			value,
				       const String*&		name)
  {
    Map<T>::Scoutor	scoutor;

    enter();

    // go through the entries.
    for (scoutor = this->container.begin();
	 scoutor != this->container.end();
	 scoutor++)
      {
	// if the entry is found...
	if (scoutor->second == value)
	  {
	    // return the name.
	    name = &scoutor->first;

	    true();
	  }
      }

    false();
  }

  ///
  /// this method removes an entry.
  ///
  template <typename T>
  Status		Map<T>::Remove(const String&		name)
  {
    Map<T>::Iterator	iterator;

    enter();

    // go through the entries.
    for (iterator = this->container.begin();
	 iterator != this->container.end();
	 iterator++)
      {
	// if the entry is found...
	if (iterator->first == name)
	  {
	    // erase the entry.
	    this->container.erase(iterator);

	    leave();
	  }
      }

    escape("unable to locate the given name");
  }

//
// ---------- dumpable --------------------------------------------------------
//

  ///
  /// this method dumps the map.
  ///
  template <typename T>
  Status		Map<T>::Dump(const Natural32		margin) const
  {
    Map<T>::Scoutor	scoutor;
    String		alignment(margin, ' ');

    enter();

    std::cout << alignment << "[Map]" << std::endl;

    // go through the entries.
    for (scoutor = this->container.begin();
	 scoutor != this->container.end();
	 scoutor++)
      {
	// dump the name.
	std::cout << alignment << Dumpable::Shift << "[Name] "
		  << scoutor->first << std::endl;

	// dump the value.
	if (scoutor->second.Dump(margin + 2) == StatusError)
	  escape("unable to dump the value");
      }

    leave();
  }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes a public key object.
  ///
  template <typename T>
  Status		Map<T>::Serialize(Archive&		archive) const
  {
    Map<T>::Scoutor	scoutor;

    enter();

    // serialize the number of entries.
    if (archive.Serialize((Natural32)this->container.size()) == StatusError)
      escape("unable to serialize the number of entries");

    // go through the entries.
    for (scoutor = this->container.begin();
	 scoutor != this->container.end();
	 scoutor++)
      {
	// serialize the entry.
	if (archive.Serialize(scoutor->first, scoutor->second) == StatusError)
	  escape("unable to serialize the entry");
      }

    leave();
  }

  ///
  /// this method extract a public key from the given archive.
  ///
  template <typename T>
  Status		Map<T>::Extract(Archive&		archive)
  {
    Natural32		size;
    Natural32		i;

    enter();

    // extract the number of entries.
    if (archive.Extract(size) == StatusError)
      escape("unable to extract the number of entries");

    // until all the entries have been extract.
    for (i = 0; i < size; i++)
      {
	Map<T>::Value	value;

	// extract the entry.
	if (archive.Extract(value.first, value.second) == StatusError)
	  escape("unable to extract the entry");

	// add a new entry.
	this->container.push_back(value);
      }

    leave();
  }

}
