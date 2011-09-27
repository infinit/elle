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

#include <hole/Hole.hh>

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
      root(NULL)
    {
    }

    ///
    /// destructor.
    ///
    template <typename V>
    Porcupine<V>::~Porcupine()
    {
      // delete the root nodule, if present.
      if (this->root != NULL)
	delete this->root;
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

      // create the quill.
      if (quill->Create() == elle::StatusError)
	escape("unable to create the quill");

      // set the root nodule.
      this->root = quill;

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
      typename Quill<V>::I*	inlet;

      enter(instance(inlet));

      // look up for the quill responsible for the given key.
      if (this->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // create an inlet.
      inlet = new typename Quill<V>::I(key, value);

      // add the value to the quill.
      if (quill->Add(inlet->key, inlet) == elle::StatusError)
	escape("unable to add the value to the quill");

      // waive.
      waive(inlet);

      // check if the quill's footprint has exceeded the extent.
      if (quill->_footprint.size > hole::Hole::Descriptor.extent)
	{
	  // split the quill.
	  if (quill->Split() == elle::StatusError)
	    escape("unable to split the quill");

	  // retrieve the potentially new root.
	  if (quill->Root(this->root) == elle::StatusError)
	    escape("unable to retrieve the root");
	}

      // XXX mettre la taille Extent dans Nucleus ou Block
      // XXX et dans Depot, Journal ou Hole, checker la taille du block.

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
      if (this->root->Lookup(key, quill) == elle::StatusError)
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
      if (this->root->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate the quill for this key");

      // retrieve the value associated with the given key.
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

      /* XXX
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
      */

      // dump the hierarchy, if present.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Data]" << std::endl;

      if (this->root != NULL)
	{
	  if (this->root->Dump(margin + 4) == elle::StatusError)
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
