//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 20:56:15 2011]
//

#ifndef NUCLEUS_PROTON_QUILL_HXX
#define NUCLEUS_PROTON_QUILL_HXX

namespace nucleus
{
  namespace proton
  {

//
// ---------- entry -----------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Entry::Entry(V*					value):
      value(value)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Entry::Entry(const Address&			address):
      address(address),
      value(NULL)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Entry::~Entry()
    {
      // delete the value, if present.
      if (this->value != NULL)
	delete this->value;
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Entry::Dump(const elle::Natural32 margin)
      const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Entry] " << this << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      // dump the value, if present.
      if (this->value != NULL)
	{
	  if (this->value->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the value");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::none << std::endl;
	}

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Quill():
      Nodule<V>(Nodule<V>::TypeQuill)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Quill(const elle::Callback<
		      elle::Status,
		      elle::Parameters<
		        const Address&,
			Nodule<V>*&
			>
		      >&					load,
		    const elle::Callback<
		      elle::Status,
		      elle::Parameters<
			const Address&,
			const Nodule<V>*
			>
		      >&					unload):
      Nodule<V>(Nodule<V>::TypeQuill),

      load(load),
      unload(unload)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Add(const typename V::K&	key,
				      Entry*			entry)
    {
      std::pair<Quill<V>::Iterator, elle::Boolean>	result;

      enter();

      // check if this key has already been recorded.
      if (this->container.find(key) != this->container.end())
	escape("this key seems to have already been recorded");

      // insert the entry in the container.
      result = this->container.insert(
	         std::pair<const typename V::K,
			   Quill<V>::Entry*>(key, entry));

      // check if the insertion was successful.
      if (result.second == false)
	escape("unable to insert the entry in the container");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Lookup(const typename V::K&	key,
					 V*&			value)
      const
    {
      Quill<V>::Scoutor	scoutor;

      enter();

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Quill<V>::Entry*	entry = scoutor->second;

	  // check the key.
	  if (key <= scoutor->first)
	    {
	      // load the value, if necessary.
	      if (entry->value == NULL)
		{
		  // load the value.
		  if (this->load.Call(entry->address,
				      entry->value) == elle::StatusError)
		    escape("unable to load the value");
		}

	      // return the value.
	      value = entry->value;

	      break;
	    }
	}

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Locate(const typename V::K&	key,
					 V*&			value)
      const
    {
      Quill<V>::Scoutor	scoutor;
      Quill<V>::Entry*	entry;

      enter();

      // find the entry associated with the given key.
      if ((scoutor = this->container.find(key)) != this->container.end())
	escape("unable to locate the given key");

      // retrieve the entry.
      entry = scoutor->second;

      // load the value, if necessary.
      if (entry->value == NULL)
	{
	  // load the value.
	  if (this->load.Call(entry->address,
			      entry->value) == elle::StatusError)
	    escape("unable to load the value");
	}

      // return the value.
      value = entry->value;

      leave();
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Lookup(const typename V::K&,
					 Quill<V>*&		quill)
    {
      enter();

      // return this quill.
      quill = this;

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Quill<V>::Dump(const elle::Natural32	margin)
      const
    {
      elle::String			alignment(margin, ' ');
      typename Quill<V>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Quill] " << this << std::endl;

      /* XXX
      // dump the load callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Load]" << std::endl;

      if (this->load.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the unload callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Unload]" << std::endl;

      if (this->unload.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the callback");
      */

      std::cout << alignment << elle::Dumpable::Shift
		<< "[Items]" << std::endl;

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::Dumpable::Shift
		    << "[Item]" << std::endl;

	  // dump the key.
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::Dumpable::Shift << elle::Dumpable::Shift
		    << "[Key] " << scoutor->first << std::endl;

	  // dump the entry.
	  if (scoutor->second->Dump(margin + 6) == elle::StatusError)
	    escape("unable to dump the entry");
	}

      leave();
    }

  }
}

#endif
