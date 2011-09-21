//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 22:14:08 2011]
//

#ifndef NUCLEUS_PROTON_SEAM_HXX
#define NUCLEUS_PROTON_SEAM_HXX

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
    Seam<V>::Entry::Entry(const typename V::K&			key,
			  const Address&			address):
      key(key),
      address(address),
      nodule(NULL)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::Entry::~Entry()
    {
      // delete the nodule, if present.
      if (this->nodule != NULL)
	delete this->nodule;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::Seam():
      Nodule<V>(Nodule<V>::TypeSeam),

      size(0)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::Seam(const elle::Callback<
		    elle::Status,
		    elle::Parameters<
		      const Address&,
		      Nodule<V>*&
		      >
		    >&						load,
		  const elle::Callback<
		    elle::Status,
		    elle::Parameters<
		      const Address&,
		      const Nodule<V>*
		      >
		    >&						unload):
      Nodule<V>(Nodule<V>::TypeSeam),

      size(0),

      load(load),
      unload(unload)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::~Seam()
    {
      Seam<V>::Scoutor	scoutor;

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Seam<V>::Entry*	entry = *scoutor;

	  // delete the entry.
	  delete entry;
	}

      // clear the container.
      this->container.clear();
    }

//
// ---------- nodule ----------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Lookup(const typename V::K&	key,
					Quill<V>*&		quill)
    {
      Seam<V>::Scoutor	scoutor;

      enter();

      // go through the container.
      for (scoutor = this->container.begin();
	   scoutor != this->container.end();
	   scoutor++)
	{
	  Seam<V>::Entry*	entry = *scoutor;

	  // check if this entry is responsible for the given key or
	  // the end of the seam has been reached.
	  if ((key <= entry->key) ||
	      (entry == this->container.back()))
	    {
	      // load the child, if necessary
	      if (entry->nodule == NULL)
		{
		  // load the child nodule.
		  if (this->load.Call(entry->address,
				      entry->nodule) == elle::StatusError)
		    escape("unable to load the child nodule");
		}

	      // lookup in this nodule.
	      if (entry->nodule->Lookup(key, quill) == elle::StatusError)
		escape("unable to locate the quill for the given key");

	      break;
	    }
	}

      leave();
    }

  }
}

#endif
