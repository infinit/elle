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
					  const V*		value)
    {
      Quill<V>*		quill;

      enter();

      // look up for the quill responsible for the given key.
      if (this->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // XXX if value->Footprint() > Porcupine<>::Footprint
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
    elle::Status	Porcupine<V>::Load(const Address&	address,
					   Nodule<V>*&		nodule)
    {
      enter();

      // build a block according to the component.
      if (Nucleus::Factory.Build(address.component,
				 nodule) == elle::StatusError)
	escape("unable to build the nodule");

      // call the given load callback.
      if (this->load(address, *nodule) == elle::StatusError)
	escape("unable to load the nodule");

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

  }
}

#endif
