//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed mar 31 23:36:12 2010]
//

#ifndef NUCLEUS_NEUTRON_RANGE_HXX
#define NUCLEUS_NEUTRON_RANGE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/neutron/Index.hh>

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
    template <typename T>
    Range<T>::Range():
      options(Range<T>::OptionNone)
    {
    }

    ///
    /// copy constructor.
    ///
    template <typename T>
    Range<T>::Range(const Range<T>&				element):
      Object(element),

      options(element.options)
    {
      Range<T>::Scoutor		scoutor;

      enter();
 
      // go through the container.
      for (scoutor = element.container.begin();
	   scoutor != element.container.end();
	   scoutor++)
	{
	  T*			item;

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
	  if (this->Add(item) == elle::StatusError)
	    fail("unable to add the item to the container");
	}

      release();
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
    elle::Status	Range<T>::Add(T*			item)
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
    elle::Status	Range<T>::Exist(const Range<T>::S&	symbol)
    {
      enter();

      // try to locate the entry.
      if (this->Locate(symbol) != elle::StatusTrue)
	false();

      true();
    }

    ///
    /// this method looks up the range.
    ///
    /// the method returns true if the item is found, false otherwise.
    ///
    template <typename T>
    elle::Status	Range<T>::Lookup(const Range<T>::S&	symbol,
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
    elle::Status	Range<T>::Remove(const Range<T>::S&	symbol)
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
    elle::Status	Range<T>::Capacity(Size&		size) const
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
    elle::Status	Range<T>::Locate(const Range<T>::S&	symbol,
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
    elle::Status	Range<T>::Detach()
    {
      enter();

      // activate the option.
      this->options = Range<T>::OptionDetach;

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    template <typename T>
    elle::Boolean	Range<T>::operator==(const Range<T>&	element) const
    {
      Range<T>::Scoutor	s;
      Range<T>::Scoutor	t;

      enter();

      // check the address as this may actually be the same object.
      if (this == &element)
	true();

      // compare the sizes.
      if (this->container.size() != element.container.size())
	false();

      // go through the elements.
      for (s = this->container.begin(), t = element.container.begin();
	   s != this->container.end();
	   s++, t++)
	{
	  // compare the entries.
	  if (*s != *t)
	    false();
	}

      true();
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
    elle::Status	Range<T>::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');
      Range<T>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Range] "
		<< std::dec << this->container.size() << std::endl;

      // dump the options.
      std::cout << alignment << elle::Dumpable::Shift << "[Options] "
		<< this->options << std::endl;

      // dump every item.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  T*		item = *scoutor;

	  // dump the item.
	  if (item->Dump(margin + 2) == elle::StatusError)
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
    elle::Status	Range<T>::Serialize(elle::Archive&	archive) const
    {
      Range<T>::Scoutor	scoutor;
      Size		size;

      enter();

      // retrieve the number of items.
      size = this->container.size();

      // serialize the number of items.
      if (archive.Serialize(size) == elle::StatusError)
	escape("unable to serialize the number of items");

      // serialize every item.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  T*		item = *scoutor;

	  // serialize the item.
	  if (archive.Serialize(*item) == elle::StatusError)
	    escape("unable to serialize the item");
	}

      leave();
    }

    ///
    /// this method extracts the range object.
    ///
    template <typename T>
    elle::Status	Range<T>::Extract(elle::Archive&	archive)
    {
      Size		size;
      Index		i;

      enter();

      // extract the number of items.
      if (archive.Extract(size) == elle::StatusError)
	escape("unable to extract the number of items");

      // extract every item.
      for (i = 0; i < size; i++)
	{
	  T*		item;

	  enter(instance(item));

	  // allocate a new item.
	  item = new T;

	  // extract the item.
	  if (archive.Extract(*item) == elle::StatusError)
	    escape("unable to extract the item");

	  // add the item to the container.
	  this->container.push_back(item);

	  // stop tracking the item.
	  waive(item);

	  release();
	}

      leave();
    }

    ///
    /// this method returns the footprint of the object.
    ///
    template <typename T>
    elle::Natural32	Range<T>::Footprint() const
    {
      Range<T>::Scoutor	scoutor;
      elle::Natural32	footprint;

      // initialize the footprint.
      footprint = 0;

      // compute the size's footprint.
      footprint += elle::Archive::Weight<Size>();

      // retrieve every item's footprint
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  T*		item = *scoutor;

	  // compute the item's footprint and add it to the sum.
	  footprint += elle::Archive::Footprint(*item);
	}

      return (footprint);
    }

  }
}

#endif
