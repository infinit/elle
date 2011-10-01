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
    template <typename V>
    elle::Status	Porcupine<V>::Add(const typename V::K&	key,
					  V*			value)
    {
      Quill<V>*			quill;

      enter();

      // search for the quill responsible for the given key.
      if (this->Search(key, quill) == elle::StatusError)
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
    elle::Status	Porcupine<V>::Locate(const typename V::K& key,
					     V*&		value)
    {
      Quill<V>*			quill;

      enter();

      // search for the quill responsible for the given key.
      if (this->Search(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // locate the value within the quill.
      if (quill->Locate(key, value) == elle::StatusError)
	escape("unable to locate the value");

      leave();
    }

    ///
    /// XXX remove
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Remove(const typename V::K& key)
    {
      Quill<V>*			quill;

      enter();

      // search for the quill responsible for the given key.
      if (this->Search(key, quill) == elle::StatusError)
	escape("unable to locate a quill for this key");

      // delete the key/value tuple from the given quill.
      if (this->Delete(quill, key) == elle::StatusError)
	escape("unable to delete the key tuple");

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
      typename N::I*	inlet;

      enter(instance(inlet));

      // create an inlet here in order to be able to compute its footprint.
      inlet = new typename N::I(key, value);

      // compute the inlet's footprint.
      if (inlet->_footprint.Compute() == elle::StatusError)
	escape("unable to compute the inlet's footprint");

      // check if the future nodule's footprint---i.e should the inlet
      // be inserted---would exceed the extent.
      if ((nodule->_footprint.size +
	   inlet->_footprint.size) > hole::Hole::Descriptor.extent)
	{
	  N*			right;
	  Seam<V>*		parent;
	  typename V::K		major;

	  //
	  // in this case, the inlet is not inserted in order to prevent
	  // the tree from being updated though we know the nodule will
	  // be split.
	  //
	  // thus, the nodule is split first, then the inlet will be inserted.
	  //

	  // split the nodule.
	  if (nodule->Split(right) == elle::StatusError)
	    escape("unable to split the nodule");

	  // retrieve the left nodule's major key.
	  if (nodule->Major(major) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // insert the inlet depending on its key: if its key is lower than
	  // the major, then it falls in the left nodule; otherwise in the
	  // right.
	  if (inlet->key < major)
	    {
	      // insert the inlet to the left nodule.
	      if (nodule->Insert(inlet) == elle::StatusError)
		escape("unable to insert the inlet to the nodule");
	    }
	  else
	    {
	      // insert the inlet to the right nodule.
	      if (right->Insert(inlet) == elle::StatusError)
		escape("unable to insert the inlet to the nodule");
	    }

	  // waive.
	  waive(inlet);

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
	      // load the parent nodule, if necessary.
	      if (nodule->_parent == NULL)
		{
		  // XXX load nodule->parent into nodule->_parent
		}

	      // set the parent.
	      parent = nodule->_parent;
	    }

	  // retrieve the major key.
	  if (right->Major(major) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // insert the right nodule in its new parent.
	  if (this->Insert(parent, major, right) == elle::StatusError)
	    escape("unable to insert the right nodule to its parent");
	}
      else
	{
	  //
	  // in this case, the nodule can accept the new inlet and does
	  // not need to be split.
	  //

	  // insert the inlet to the nodule.
	  if (nodule->Insert(inlet) == elle::StatusError)
	    escape("unable to insert the inlet to the nodule");

	  // waive.
	  waive(inlet);
	}

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    template <typename N>
    elle::Status	Porcupine<V>::Delete(N*			nodule,
					     const typename V::K& key)
    {
      typename V::K	major;

      enter();

      // retrieve the nodule's major.
      if (nodule->Major(major) == elle::StatusError)
	escape("unable to retrieve the nodule's major key");

      // delete the inlet to the nodule.
      if (nodule->Delete(key) == elle::StatusError)
	escape("unable to delete the inlet from the nodule");

      // operate depending on the state of the nodule.
      if (nodule->container.empty() == true)
	{
	  // if a parent exists...
	  if (nodule->parent != Address::Null)
	    {
	      // load the parent nodule, if necessary.
	      if (nodule->_parent == NULL)
		{
		  // XXX load the parent nodule
		}

	      // delete the nodule from its parent by passing the major i.e
	      // the key the parent seam uses to referenced the now-empty
	      // nodule.
	      if (this->Delete(nodule->_parent, major) == elle::StatusError)
		escape("unable to delete the nodule from its parent");
	    }
	  else
	    {
	      // in this case, the nodule is the root nodule: delete it
	      // manually.
	      delete nodule;

	      // reset the root nodule address and pointer.
	      this->root = Address::Null;
	      this->_root = NULL;

	      // decrease the tree's height.
	      this->height--;
	    }
	}
      else if (nodule->_footprint.size < 10)
	{
	  // XXX
	}

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Search(const typename V::K& key,
					     Quill<V>*&		quill)
    {
      enter();

      // if the tree is non-existent, create a first level by making it grow.
      if (this->height == 0)
	{
	  // make the tree grow.
	  if (this->Grow() == elle::StatusError)
	    escape("unable to make the tree grow");
	}

      // load the root nodule as one must be present, if necessary.
      if (this->_root == NULL)
	{
	  // XXX load the root nodule
	}

      // trigger the search method on the root nodule.
      if (this->_root->Search(key, quill) == elle::StatusError)
	escape("unable to locate the quill for this key");

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

      // if the tree does not exist, create a root nodule.
      if (this->height == 0)
      {
	Quill<V>*	root;

	enter(instance(root));

	// at first, allocate an initial root nodule which happens to be
	// a leaf since alone.
	root = new Quill<V>(
	         elle::Callback<>::Infer(&Porcupine::Load, this),
		 elle::Callback<>::Infer(&Porcupine::Unload, this));

	// create the quill.
	if (root->Create() == elle::StatusError)
	  escape("unable to create the quill");

	// set the root nodule address and pointer.
	this->root = Address::Some;
	this->_root = root;

	// waive the tracking.
	waive(root);
      }
      else
	{
	  // load the root nodule, if necessary.
	  if (this->_root == NULL)
	    {
	      // XXX load the root nodule
	    }

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
	  this->_root->parent = Address::Some;
	  this->_root->_parent = seam;

	  // set the new root.
	  this->root = Address::Some;
	  this->_root = seam;

	  // waive seam.
	  waive(seam);
	}

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

      // dump the height.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Height] " << std::dec << this->height << std::endl;

      // dump the root.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Root]" << std::endl;

      if (this->root.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the address");

      // dump the load callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Load]" << std::endl;

      if (this->_load.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the unload callback.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[_Unload]" << std::endl;

      if (this->_unload.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the callback");

      // dump the hiearchy.
      if (this->_root != NULL)
	{
	  // dump the hierarchy, if present.
	  std::cout << alignment << elle::Dumpable::Shift
		    << "[_Root]" << std::endl;

	  if (this->_root->Dump(margin + 4) == elle::StatusError)
	    escape("unable to dump the nodule");
	}
      else
	{
	  std::cout << alignment << elle::Dumpable::Shift
		    << elle::Dumpable::Shift
		    << "[_Root] " << elle::none << std::endl;
	}

      leave();
    }

  }
}

#endif
