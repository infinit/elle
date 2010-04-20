//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Range.hxx
//
// created       julien quintard   [wed mar 31 23:36:12 2010]
// updated       julien quintard   [tue apr 20 06:48:25 2010]
//

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
    template <typename T>
    Range<T>::Range():
      options(Range<T>::OptionNone)
    {
    }

    ///
    /// destructor.
    ///
    template <typename T>
    Range<T>::~Range()
    {
      Range<T>::Iterator	i;

      // until the range is empty.
      while (this->container.empty() == false)
	{
	  T*	item = this->container.front();

	  // remove the first element.
	  this->container.pop_front();

	  // delete the item, if necessary.
	  if ((this->options & Range<T>::OptionDetach) != 0)
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
    Status		Range<T>::Add(T*			item)
    {
      enter();

      // check if another item exists with this symbol.
      if (this->Locate(item->Symbol()) == true)
	escape("an item with this symbol already exist");

      // add the item to the container.
      this->container.push_back(item);

      leave();
    }

    ///
    /// this method returns true if an item for the given symbol exists.
    ///
    template <typename T>
    Status		Range<T>::Exist(const Range<T>::S&	symbol)
    {
      enter();

      // try to locate the entry.
      if (this->Locate(symbol) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method looks up the range.
    ///
    /// the method returns true if the item is found, false otherwise.
    ///
    template <typename T>
    Status		Range<T>::Lookup(const Range<T>::S&	symbol,
					 T*&			item)
    {
      Range<T>::Iterator	iterator;

      enter();

      // initialize the pointer to null.
      item = NULL;

      // try to locate the item.
      if (this->Locate(symbol, &iterator) == false)
	false();

      // return the item.
      item = *iterator;

      true();
    }

    ///
    /// this method removes an item from the range.
    ///
    template <typename T>
    Status		Range<T>::Remove(const Range<T>::S&	symbol)
    {
      Range<T>::Iterator	iterator;

      enter();

      // locate the item.
      if (this->Locate(symbol, &iterator) == false)
	escape("this symbol does not seem to be present in this range");

      // delete the item.
      delete *iterator;

      // erase the item from the container.
      this->container.erase(iterator);

      leave();
    }

    ///
    /// this method returns the number of items in the range.
    ///
    template <typename T>
    Status		Range<T>::Capacity(Size&		size) const
    {
      enter();

      // return the size.
      size = this->container.size();

      leave();
    }

    ///
    /// this method returns an iterator on the identified item.
    ///
    /// the method returns true if the item is found, false otherwise.
    ///
    template <typename T>
    Status		Range<T>::Locate(const Range<T>::S&	symbol,
					 Range<T>::Iterator*	i)
    {
      Range<T>::Iterator	iterator;

      enter();

      // go through the container.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  T*			item = *iterator;

	  // if found...
	  if (item->Symbol() == symbol)
	    {
	      // return the iterator, if wanted.
	      if (i != NULL)
		*i = iterator;

	      true();
	    }
	}

      false();
    }

    ///
    /// this method instructs the range to detach the memory management
    /// from the object so that items do not get deleted once the range
    /// dies.
    ///
    template <typename T>
    Status		Range<T>::Detach()
    {
      enter();

      // activate the option.
      this->options = Range<T>::OptionDetach;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps a range.
    ///
    template <typename T>
    Status		Range<T>::Dump(Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Range<T>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Range]" << std::endl;

      // dump the options.
      std::cout << alignment << Dumpable::Shift << "[Options] "
		<< this->options << std::endl;

      // dump every item.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  T*		item = *scoutor;

	  // dump the item.
	  if (item->Dump(margin + 2) == StatusError)
	    escape("unable to dump the item");
	}

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the range object.
    ///
    template <typename T>
    Status		Range<T>::Serialize(Archive&		archive) const
    {
      Range<T>::Scoutor	scoutor;
      Size		size;

      enter();

      // retrieve the number of items.
      size = this->container.size();

      // serialize the number of items.
      if (archive.Serialize(size) == StatusError)
	escape("unable to serialize the number of items");

      // serialize every item.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  T*		item = *scoutor;

	  // serialize the item.
	  if (archive.Serialize(*item) == StatusError)
	    escape("unable to serialize the item");
	}

      leave();
    }

    ///
    /// this method extracts the range object.
    ///
    template <typename T>
    Status		Range<T>::Extract(Archive&		archive)
    {
      Size		size;
      Index		i;

      enter();

      // extract the number of items.
      if (archive.Extract(size) == StatusError)
	escape("unable to extract the number of items");

      // extract every item.
      for (i = 0; i < size; i++)
	{
	  T*		item;

	  enter(instance(item));

	  // allocate a new item.
	  item = new T;

	  // extract the item.
	  if (archive.Extract(*item) == StatusError)
	    escape("unable to extract the item");

	  // add the item to the container.
	  this->container.push_back(item);

	  // stop tracking the item.
	  waive(item);

	  release();
	}

      leave();
    }

  }
}
