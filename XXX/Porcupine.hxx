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
      height(0),

      _load(load),
      _unload(unload),
      _root(NULL)
    {
    }

    ///
    /// destructor.
    ///
    template <typename V>
    Porcupine<V>::~Porcupine()
    {
      // delete the root nodule, if present.
      if (this->_root != NULL)
	delete this->_root;
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
      this->_root = quill;

      // waive the tracking.
      waive(quill);

      // increment the height.
      height++;

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

      enter();

      // look up for the quill responsible for the given key.
      if (this->Lookup(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // insert the key/value tuple in the given quill.
      if (this->Insert(quill, key, value) == elle::StatusError)
	escape("unable to insert the key/value tuple");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    template <typename N,
	      typename W>
    elle::Status	Porcupine<V>::Insert(N*			nodule,
					     const typename V::K& key,
					     W*			value)
    {
      enter();

      // add the value to the nodule.
      if (nodule->Insert(key, value) == elle::StatusError)
	escape("unable to add the value to the nodule");

      // check if the nodule's footprint has exceeded the extent.
      if (nodule->_footprint.size > hole::Hole::Descriptor.extent)
	{
	  N*			right;
	  Seam<V>*		parent;
	  typename V::K		major;

	  // split the nodule.
	  if (nodule->Split(right) == elle::StatusError)
	    escape("unable to split the nodule");

	  // create a parent node if necessary i.e if the current nodule
	  // was the root.
	  if (nodule == this->_root)
	    {
	      // make the porcupine grow.
	      if (this->Grow() == elle::StatusError)
		escape("unable to make the porcupine grow");

	      // set the parent by casting the root into a seam.
	      parent = static_cast<Seam<V>*>(this->_root);
	    }
	  else
	    {
	      // set the parent.
	      parent = nodule->_parent;
	    }

	  // retrieve the major key.
	  if (right->Major(major) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // insert the right nodule in its new parent.
	  if (this->Insert(parent, major, right) == elle::StatusError)
	    escape("unable to add the right nodule to its parent");
	}

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
      if (this->_root->Lookup(key, quill) == elle::StatusError)
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
      if (this->_root->Lookup(key, quill) == elle::StatusError)
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
    elle::Status	Porcupine<V>::Grow()
    {
      Seam<V>*		seam;
      typename V::K	major;

      enter(instance(seam));

      // retrieve the current root's major key.
      if (this->_root->Major(major) == elle::StatusError)
	escape("unable to retrieve the major key");

      // allocate a new seam.
      seam = new Seam<V>(
	       elle::Callback<>::Infer(&Porcupine::Load, this),
	       elle::Callback<>::Infer(&Porcupine::Unload, this));

      // create the root seam.
      if (seam->Create() == elle::StatusError)
	escape("unable to create the seam");

      // add the current root nodule in the new root seam.
      if (seam->Insert(major, this->_root) == elle::StatusError)
	escape("unable to add the current root in the new root nodule");

      // link the existing root to the new root.
      this->_root->_parent = seam;

      // set the new root.
      this->_root = seam;

      // waive seam.
      waive(seam);

      // increment the height.
      height++;

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
    elle::Status	Porcupine<V>::Unload(const Address&,
					     const Nodule<V>*)
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

      // dump the height.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Height] " << std::dec << this->height << std::endl;

      // dump the hierarchy, if present.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Data]" << std::endl;

      if (this->_root != NULL)
	{
	  if (this->_root->Dump(margin + 4) == elle::StatusError)
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
