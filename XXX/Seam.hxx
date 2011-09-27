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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::Seam():
      Nodule<V>(Nodule<V>::TypeSeam)
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

      _load(load),
      _unload(unload),
      _footprint(*this)
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
	  Seam<V>::I*	inlet = scoutor->second;

	  // delete the inlet.
	  delete inlet;
	}

      // clear the container.
      this->container.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Create()
    {
      enter();

      // compute the initial footprint from which the Add(), Remove()
      // methods will work in order to adjust it.
      if (this->_footprint.Compute() == elle::StatusError)
	escape("unable to compute the footprint");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Add(const typename V::K&	key,
				     I*				nodule)
    {
      enter();

      // XXX

      leave();
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
	  Seam<V>::I*	inlet = scoutor->second;

	  // check if this inlet is responsible for the given key or
	  // the end of the seam has been reached.
	  if ((key <= scoutor->first) ||
	      (inlet == this->container.rbegin()->second))
	    {
	      // load the child, if necessary
	      if (inlet->_value == NULL)
		{
		  // load the child nodule.
		  if (this->_load.Call(inlet->address,
				       inlet->_value) == elle::StatusError)
		    escape("unable to load the child nodule");
		}

	      // lookup in this nodule.
	      if (inlet->_value->Lookup(key, quill) == elle::StatusError)
		escape("unable to locate the quill for the given key");

	      break;
	    }
	}

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Root(Nodule<V>*&		root) const
    {
      enter();

      // XXX

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Dump(const elle::Natural32	margin)
      const
    {
      elle::String		alignment(margin, ' ');
      typename Seam<V>::Scoutor	scoutor;

      enter();

      std::cout << alignment << "[Seam] " << this << std::endl;

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
		<< "[Items] " << this->container.size() << std::endl;

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

	  // dump the inlet.
	  if (scoutor->second->Dump(margin + 6) == elle::StatusError)
	    escape("unable to dump the inlet");
	}

      // dump the footprint.
      if (this->_footprint.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the footprint");

      leave();
    }

  }
}

#endif
