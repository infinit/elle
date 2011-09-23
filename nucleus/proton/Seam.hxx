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
    Seam<V>::Entry::Entry(const Nodule<V>&			nodule):
      nodule(nodule)
    {
    }

    ///
    /// XXX
    ///
    template <typename V>
    Seam<V>::Entry::Entry(const Address&			address):
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

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Seam<V>::Entry::Dump(const elle::Natural32 margin)
      const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Entry] " << this << std::endl;

      // dump the address.
      if (this->address.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      // dump the nodule, if present.
      if (this->nodule != NULL)
	{
	  if (this->nodule->Dump(margin + 2) == elle::StatusError)
	    escape("unable to dump the nodule");
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
	  Seam<V>::Entry*	entry = scoutor->second;

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
	  Seam<V>::Entry*	entry = scoutor->second;

	  // check if this entry is responsible for the given key or
	  // the end of the seam has been reached.
	  if ((key <= scoutor->first) ||
	      (entry == this->container.rbegin()->second))
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
