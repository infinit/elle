//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri sep 16 13:33:45 2011]
//

#ifndef NUCLEUS_PROTON_PORCUPINE_HXX
#define NUCLEUS_PROTON_PORCUPINE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/Nucleus.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename V>
    Porcupine<V>::Porcupine(const elle::Callback<
			      elle::Status,
			      elle::Parameters<
				const Address&,
				Block&
				>
			      >&				load,
			    const elle::Callback<
			      elle::Status,
			      elle::Parameters<
				const Address&,
				const Block&
				>
			      >&				unload):
      load(load),
      unload(unload),
      nodule(NULL)
    {
    }

    ///
    /// destructor.
    ///
    template <typename V>
    Porcupine<V>::~Porcupine()
    {
      // delete the root nodule, if present.
      if (this->nodule != NULL)
	delete this->nodule;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    /// XXX remove this?
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Create()
    {
      Quill<V>*		quill;

      enter(instance(quill));

      // at first, allocate an initial root nodule which happens to be
      // a leaf since alone.
      quill = new Quill<V>(
	        elle::Callback<>::Infer(&Porcupine::Load, this),
		elle::Callback<>::Infer(&Porcupine::Unload, this));

      // set the root nodule.
      this->nodule = quill;

      // waive the tracking.
      waive(quill);

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Add(const typename V::K&	key,
					  V*			value)
    {
      Quill<V>*			quill;
      typename Quill<V>::Entry*	entry;

      enter(instance(entry));

      // look up for the quill responsible for the given key.
      if (this->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // create an entry.
      entry = new typename Quill<V>::Entry(value);

      // add the value to the quill.
      if (quill->Add(key, entry) == elle::StatusError)
	escape("unable to add the value to the quill");

      waive(entry);

      // XXX peut etre Footprint etc. return Status?

      // XXX
      // check & split

      // XXX gerer a la main le fait de pas recalculer footprint

      // XXX est-ce qu'on ajoute un Footprint
      // XXX if quill->Footprint() > Porcupine<>::Footprint
      // XXX   -> split quill: allocate new quill, quill->Balance(q)
      // XXX     -> new root? else update the hierarchy
      // XXX else
      // XXX   -> insert

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Lookup(const typename V::K& key,
					     Quill<V>*&		quill)
      const
    {
      enter();

      // trigger the lookup method on the root nodule.
      if (this->nodule->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate the quill for this key");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Locate(const typename V::K& key,
					     Quill<V>*&		quill,
					     V*&		value)
      const
    {
      enter();

      // trigger the lookup method on the root nodule.
      if (this->nodule->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate the quill for this key");

      // locate the entry with the exact given key.
      if (quill->Locate(key, value) == elle::StatusError)
	escape("unable to locate the value");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Load(const Address&	address,
					   Nodule<V>*&		nodule)
    {
      enter();

      // build a block according to the component.
      if (Nucleus::Factory.Build(address.component,
				 nodule) == elle::StatusError)
	escape("unable to build the nodule");

      /* XXX
      // call the given load callback.
      if (this->load(address, *nodule) == elle::StatusError)
	escape("unable to load the nodule");
      */

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Unload(const Address&	address,
					     const Nodule<V>*	nodule)
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
    elle::Status	Porcupine<V>::Dump(const elle::Natural32 margin)
      const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Porcupine]" << std::endl;

      // dump the load callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Load]" << std::endl;

      if (this->load.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the unload callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Unload]" << std::endl;

      if (this->unload.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the hierarchy, if present.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Data]" << std::endl;

      if (this->nodule != NULL)
	{
	  if (this->nodule->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the nodule");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::Dumpable::Shift << elle::none << std::endl;
	}

      leave();
    }

  }
}

#endif
