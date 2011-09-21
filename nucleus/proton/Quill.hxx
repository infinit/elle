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
    Quill<V>::Entry::Entry(const typename V::K&			key,
			   const Address&			address):
      key(key),
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

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Quill<V>::Quill():
      Nodule<V>(Nodule<V>::TypeQuill),

      size(0)
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

      size(0),

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
				      V*			value)
    {
      Quill<V>::Iterator	iterator;

      enter();

      // go through the container.
      for (iterator = this->container.begin();
	   iterator != this->container.end();
	   iterator++)
	{
	  Quill<V>::Entry*	entry = *iterator;

	  // check if this key already exists.
	  if (key == entry->key)
	    escape("this key seems to have already been registered");

	  // XXX UTILISER UNE MAP: locate(simple), add(simple), lookup(linear)
	}

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
	  Quill<V>::Entry*	entry = *scoutor;

	  // check the key.
	  if (key <= entry->key)
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

      enter();

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Quill<V>::Entry*	entry = *scoutor;

	  // check the key.
	  if (key == entry->key)
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

  }
}

#endif
