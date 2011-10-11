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
// ---------- macro-functions -------------------------------------------------
//

///
/// XXX
///
#define PorcupineLoad(_object_, _element_)				\
  if (_object_->_element_ != Address::Null)				\
    {									\
      if (_object_->_ ## _element_ == NULL)				\
	{								\
	  printf("PORCUPINE LOAD\n");					\
	}								\
    }									\
  if (_object_->_ ## _element_ != NULL)

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename V>
    Porcupine<V>::Porcupine():
      height(0),

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
    /// XXX clean with _current, _left and _right
    /// XXX add balancing on insertion
    ///
    template <typename V>
    template <typename N,
	      typename W>
    elle::Status	Porcupine<V>::Insert(N*			nodule,
					     const typename V::K& key,
					     W*			value)
    {
      typename N::I*	inlet;

      enterx(instance(inlet));

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
	  struct
	  {
	    typename V::K	ancient;
	    typename V::K	recent;

	    typename V::K	temporary;
	  }			mayor;

	  // first, retrieve the mayor key of the current nodule.
	  if (nodule->Mayor(mayor.ancient) == elle::StatusError)
	    escape("unable to retrive the mayor");

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

	  //
	  // update the nodule's previous right neighbour.
	  //
	  // this operation is required in order to make the left/right
	  // links consistent by updating the neighbours this way:
	  //
	  //   [old right nodule]->left = [new right nodule]
	  //
	  //   [new right nodule]->left = [current nodule]
	  //   [new right nodule]->right = [old right nodule]
	  //
	  //   [current nodule]->right = [new right nodule]
	  //

	  // load the right nodule, if possible.
	  PorcupineLoad(nodule, right)
	    {
	      // update the old right nodule's neighbour links.
	      nodule->_right->left = Address::Some;
	      nodule->_right->_left = right;
	    }

	  // set the new right nodule's neighbour links.
	  right->left = Address::Some;
	  right->_left = nodule;

	  right->right = nodule->right;
	  right->_right = nodule->_right;

	  // finally, update the current nodule's right neighbour link.
	  nodule->right = Address::Some;
	  nodule->_right = right;

	  // retrieve the left nodule's mayor key.
	  if (nodule->Mayor(mayor.temporary) == elle::StatusError)
	    escape("unable to retrieve the mayor key");

	  // insert the inlet depending on its key: if its key is lower than
	  // the mayor, then it falls in the left nodule; otherwise in the
	  // right.
	  if (inlet->key < mayor.temporary)
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

	      // increment the height.
	      this->height++;

	      // set the parent by casting the root into a seam.
	      parent = static_cast<Seam<V>*>(this->_root);
	    }
	  else
	    {
	      // load the parent nodule.
	      PorcupineLoad(nodule, parent);

	      // set the parent.
	      parent = nodule->_parent;

	      // retrieve the current nodule's new mayor key.
	      if (nodule->Mayor(mayor.recent) == elle::StatusError)
		escape("unable to retrive the mayor");

	      // then, update the parent nodule, should the mayor key have
	      // changed.
	      if (mayor.recent != mayor.ancient)
		{
		  // propagate the update to the parent nodules.
		  if (nodule->_parent->Propagate(
		        mayor.ancient,
			mayor.recent) == elle::StatusError)
		    escape("unable to update the parent nodule");
		}
	    }

	  // retrieve the mayor key.
	  if (right->Mayor(mayor.temporary) == elle::StatusError)
	    escape("unable to retrieve the mayor key");

	  // insert the right nodule in its new parent.
	  if (this->Insert(parent,
			   mayor.temporary,
			   right) == elle::StatusError)
	    escape("unable to insert the right nodule to its parent");
	}
      else
	{
	  //
	  // in this case, the nodule can accept the new inlet and does
	  // not need to be split.
	  //
	  // however, operate depending on the nodule's state i.e whether
	  // inlets have been inserted before.
	  //
	  if (nodule->container.empty() == true)
	    {
	      //
	      // the nodule is empty: insert the inlet.
	      //

	      // insert the inlet to the nodule.
	      if (nodule->Insert(inlet) == elle::StatusError)
		escape("unable to insert the inlet to the nodule");

	      // waive.
	      waive(inlet);

	      // note that a nodule being empty in this case implies
	      // that the nodule is the root. therefore, there is no
	      // need to propagate the mayor key.
	    }
	  else
	    {
	      //
	      // the nodule is not empty: insert the inlet and update
	      // the parent nodule if necessary.
	      //

	      struct
	      {
		typename V::K	ancient;
		typename V::K	recent;
	      }			mayor;

	      // retrieve the current nodule's mayor key.
	      if (nodule->Mayor(mayor.ancient) == elle::StatusError)
		escape("unable to retrive the mayor");

	      // insert the inlet to the nodule.
	      if (nodule->Insert(inlet) == elle::StatusError)
		escape("unable to insert the inlet to the nodule");

	      // waive.
	      waive(inlet);

	      // retrieve the nodule's new mayor key.
	      if (nodule->Mayor(mayor.recent) == elle::StatusError)
		escape("unable to retrive the mayor");

	      // finally, update the parent nodule, should the mayor key have
	      // changed.
	      if ((mayor.recent != mayor.ancient) &&
		  (nodule->parent != Address::Null))
		{
		  // load the parent nodule.
		  PorcupineLoad(nodule, parent);

		  // propagate the change of mayor key throughout the
		  // hiearchy.
		  if (nodule->_parent->Propagate(
		        mayor.ancient,
			mayor.recent) == elle::StatusError)
		    escape("unable to update the parent nodule");
		}
	    }
	}

      leave();
    }

    ///
    /// XXX
    ///
    /// XXX add balancing on three nodules
    ///
    template <typename V>
    template <typename N>
    elle::Status	Porcupine<V>::Delete(N*			nodule,
					     const typename V::K& key)
    {
      struct
      {
	struct
	{
	  typename V::K	ancient;
	  typename V::K	recent;
	}		left;
	struct
	{
	  typename V::K	ancient;
	  typename V::K	recent;
	}		current;
	struct
	{
	  typename V::K	ancient;
	  typename V::K	recent;
	}		right;
      }			mayor;
      N*		_current;
      N*		_left;
      N*		_right;

      enter();

      // set the convenient _current alias.
      _current = nodule;

      // load the left nodule, if possible.
      PorcupineLoad(_current, left)
	{
	  // set the _left alias.
	  _left = static_cast<N*>(_current->_left);
	}
      else
	{
	  // reset the pointer.
	  _left = NULL;
	}

      // load the right nodule, if possible.
      PorcupineLoad(_current, right)
	{
	  // set the _right alias.
	  _right = static_cast<N*>(_current->_right);
	}
      else
	{
	  // reset the pointer.
	  _right = NULL;
	}

      // retrieve the nodule's mayor.
      if (_current->Mayor(mayor.current.ancient) == elle::StatusError)
	escape("unable to retrieve the nodule's mayor key");

      // delete the inlet to the nodule.
      if (_current->Delete(key) == elle::StatusError)
	escape("unable to delete the inlet from the nodule");

      // operate depending on the state of the nodule.
      if (_current->container.empty() == true)
	{
	  //
	  // XXX
	  //

	  // if a parent exists...
	  if (_current->parent != Address::Null)
	    {
	      //
	      // XXX
	      //

	      // load the parent nodule.
	      PorcupineLoad(_current, parent);

	      //
	      // update the neighbour nodules. note that this step is performed
	      // before actually deleting the entry as this would imply
	      // destroying the nodule completely.
	      //
	      // this operation is required in order to make the left/right
	      // links consistent by updating the neighbours this way:
	      //
	      //   [left nodule]->right = [deleted nodule]->right
	      //   [right nodule]->left = [deleted nodule]->left
	      //

	      // load the left neighbour, if possible.
	      PorcupineLoad(_current, left)
		{
		  // update the left nodule's right neighbour.
		  _current->_left->right = _current->right;
		  _current->_left->_right = _current->_right;
		}

	      // load the right neighbour, if possible.
	      PorcupineLoad(_current, right)
		{
		  // update the right nodule's left neighbour.
		  _current->_right->left = _current->left;
		  _current->_right->_left = _current->_left;
		}

	      // finally, delete the nodule from its parent by passing
	      // the mayor i.e the key the parent seam uses to reference
	      // the now-empty nodule.
	      if (this->Delete(_current->_parent,
			       mayor.current.ancient) == elle::StatusError)
		escape("unable to delete the nodule from its parent");
	    }
	  else
	    {
	      //
	      // XXX
	      //

	      // delete the root nodule manually, if necessary.
	      if (this->_root != NULL)
		delete this->_root;

	      // reset the root nodule address and pointer.
	      this->root = Address::Null;
	      this->_root = NULL;

	      // reset the tree's height.
	      this->height = 0;
	    }
	}
      else if (_current->_footprint.size <
	       (hole::Hole::Descriptor.extent *
		hole::Hole::Descriptor.balancing))
	{
	  //
	  // XXX
	  //

	  // load the parent nodule, if possible.
	  PorcupineLoad(_current, parent);

	  // XXX optimiser avec des cas pour _state = Dirty

	  // XXX
	  if (_current->_parent == NULL) // XXXparent
	    {
	      //
	      // in this case, the tree is shrunk if possible i.e if
	      // the root nodule is not alone but most importantly if the
	      // root nodule contains a single inlet; which will become
	      // the new root.
	      //
	      // note that this case is tested first so that the following
	      // ones do not have to test for the parent address to be
	      // null.
	      //

	      // shrink the tree.
	      if (this->Shrink() == elle::StatusError)
		escape("unable to shrink the tree");
	    }
	  else if ((_left != NULL) &&
		   (_left->_state == StateDirty) &&
		   (_left->_parent == _current->_parent) && //XXXparent
		   ((_left->_footprint.size +
		     _current->_footprint.size -
		     N::Footprint) <
		    hole::Hole::Descriptor.extent))
	    {
	      //
	      // this case is a specialization of another one below in
	      // which merging is privileged should the neighbour nodule
	      // be already dirty.
	      //
	      // this scenario is considered first because it can reduce
	      // the number of modified nodules which would then need to
	      // be pushed onto the network.
	      //
	      goto _merge_left;
	    }
	  else if ((_right != NULL) &&
		   (_right->_state == StateDirty) &&
		   (_right->_parent == _current->_parent) && //XXXparent
		   ((_current->_footprint.size -
		     N::Footprint +
		     _right->_footprint.size) <
		    hole::Hole::Descriptor.extent))
	    {
	      //
	      // this case is similar to the previous but for the right
	      // neighbour nodule.
	      //
	      goto _merge_right;
	    }
	  else if ((_left != NULL) &&
		   (_left->_parent == _current->_parent) && //XXXparent
		   ((_left->_footprint.size +
		     _current->_footprint.size -
		     N::Footprint) <
		    hole::Hole::Descriptor.extent))
	    {
	      //
	      // XXX
	      //
	    _merge_left:

	      // retrieve the left mayor key.
	      if (_left->Mayor(
		    mayor.left.ancient) == elle::StatusError)
		escape("unable to retrieve the mayor key");

	      // merge the nodule.
	      if (_left->Merge(_current) == elle::StatusError)
		escape("unable to merge the nodules");

	      // retrieve the left mayor key.
	      if (_left->Mayor(
		    mayor.left.recent) == elle::StatusError)
		escape("unable to retrieve the mayor key");

	      //
	      // make the tree consistent by updating the neighbour links.
	      //

	      // update the left nodule which now contains all the inlets.
	      if (_current->_left != NULL)
		{
		  // update the left nodule's right neighbour.
		  _current->_left->right = _current->right;
		  _current->_left->_right = _current->_right;
		}

	      // update the right nodule which should no longer reference
	      // the current, now empty, nodule.
	      if (_current->_right != NULL)
		{
		  // update the right nodule's left neighbour.
		  _current->_right->left = _current->left;
		  _current->_right->_left = _current->_left;
		}

	      // load the parent nodule.
	      PorcupineLoad(_current, parent);
		{
		  // delete the current nodule in its parent since the
		  // nodule is now empty.
		  if (this->Delete(_current->_parent,
				   mayor.current.ancient) == elle::StatusError)
		    escape("unable to delete the entry from the parent");

		  // load the left nodule's parent. note that this is
		  // necessary because the left nodule may have lost its
		  // parent on the way.
		  //
		  // indeed, for instance, by removing the current nodule
		  // from its parent and so forth up to the root nodule,
		  // the tree may have been shrunk, leading for instance
		  // the _left_ nodule as the new root; in which case
		  // this nodule would no longer have a parent.
		  PorcupineLoad(_left, parent)
		    {
		      // update the parent so that the left nodule gets
		      // referenced with the proper key.
		      //
		      // note however that the mayor key does not need
		      // propagation because the highest index key has not
		      // changed through merging.
		      if (_left->_parent->Update(
			    mayor.left.ancient,
			    mayor.left.recent) == elle::StatusError)
			escape("unable to update the parent nodule");
		    }
		}
	    }
	  else if ((_right != NULL) &&
		   (_right->_parent == _current->_parent) && //XXXparent
		   ((_current->_footprint.size -
		     N::Footprint +
		     _right->_footprint.size) <
		    hole::Hole::Descriptor.extent))
	    {
	      //
	      // XXX
	      //
	    _merge_right:

	      // merge the nodule.
	      if (_right->Merge(_current) == elle::StatusError)
		escape("unable to merge the nodules");

	      //
	      // make the tree consistent by updating the neighbour links.
	      //

	      // update the left nodule which now contains all the inlets.
	      if (_current->_left != NULL)
		{
		  // update the left nodule's right neighbour.
		  _current->_left->right = _current->right;
		  _current->_left->_right = _current->_right;
		}

	      // update the right nodule which should no longer reference
	      // the current, now empty, nodule.
	      if (_current->_right != NULL)
		{
		  // update the right nodule's left neighbour.
		  _current->_right->left = _current->left;
		  _current->_right->_left = _current->_left;
		}

	      // load the parent nodule.
	      PorcupineLoad(_current, parent)
		{
		  // delete the current nodule in its parent since the
		  // nodule is now empty.
		  if (this->Delete(_current->_parent,
				   mayor.current.ancient) == elle::StatusError)
		    escape("unable to delete the entry from the parent");

		  // note that the parent nodule does not need to be updated
		  // regarding the new inlets merged into the right nodule
		  // because the added inlets have lower key compared to
		  // the already existing ones in the right nodule.
		}
	    }
	  else
	    {
	      //
	      // in this case, the nodule is small enough for being merged
	      // or balanced but, unfortunately, the neighbours nodules
	      // are not in a state to accept such operations.
	      //
	      // therefore, the nodule is left as such, taking care however
	      // to update the parent nodules for consistency.
	      //
	      goto _propagate;
	    }
	}
      else
	{
	  //
	  // in this case, the nodule is neither empty nor small enough
	  // to be merged or balanced.
	  //
	  // therefore, nothing special is accomplished but propagating
	  // the update on the parent nodules since the mayor key may
	  // have changed following the deletion.
	  //
	_propagate:

	  // retrieve the nodule's new mayor.
	  if (_current->Mayor(mayor.current.recent) == elle::StatusError)
	    escape("unable to retrieve the nodule's mayor key");

	  // finally, update the parent nodule, should the mayor key have
	  // changed.
	  if ((mayor.current.recent != mayor.current.ancient) &&
	      (_current->parent != Address::Null))
	    {
	      // load the parent nodule.
	      PorcupineLoad(_current, parent);

	      // propagate the change of mayor key throughout the
	      // hiearchy.
	      if (_current->_parent->Propagate(
		    mayor.current.ancient,
		    mayor.current.recent) == elle::StatusError)
		escape("unable to update the parent nodule");
	    }
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

	  // increment the height.
	  this->height++;
	}

      // load the root nodule.
      PorcupineLoad(this, root);

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
      typename V::K	mayor;

      enterx(instance(seam));

      // if the tree does not exist, create a root nodule.
      if (this->height == 0)
	{
	  Quill<V>*	root;

	  enterx(instance(root));

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
	  // load the root nodule.
	  PorcupineLoad(this, root);

	  // retrieve the current root's mayor key.
	  if (this->_root->Mayor(mayor) == elle::StatusError)
	    escape("unable to retrieve the mayor key");

	  // allocate a new seam.
	  seam = new Seam<V>(
		   elle::Callback<>::Infer(&Porcupine::Load, this),
		   elle::Callback<>::Infer(&Porcupine::Unload, this));

	  // create the root seam.
	  if (seam->Create() == elle::StatusError)
	    escape("unable to create the seam");

	  // add the current root nodule in the new root seam.
	  if (seam->Insert(mayor, this->_root) == elle::StatusError)
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

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Shrink()
    {
      typename V::K	maiden;
      Seam<V>*		seam;
      Nodule<V>*	root;

      enter();

      // if the tree is empty or has a root nodule only, ignore this step.
      if (this->height <= 1)
	leave();

      // load the root nodule.
      PorcupineLoad(this, root);

      // retrieve the maiden key.
      if (this->_root->Maiden(maiden) == elle::StatusError)
	escape("unable to retrieve the maiden key");

      // note that since the Shrink() method proceeds only for trees
      // with a two-level heigh and more, the root nodule cannot be anything
      // but a seam.
      seam = static_cast<Seam<V>*>(this->_root);

      // retrieve the nodule associated with the given key.
      if (seam->Locate(maiden, root) == elle::StatusError)
	escape("unable to retrieve the nodule");

      // clear the seam from its entries without deleting anything
      // as we want to keep _root_ alive.
      if (seam->Clear() == elle::StatusError)
	escape("unable to clear the seam");

      // delete the seam.
      delete seam;

      // finally, set the new root nodule.
      this->root = Address::Some;
      this->_root = root;

      // update the new root links.
      this->_root->parent = Address::Null;
      this->_root->_parent = NULL;

      // decrease the tree's height.
      this->height--;

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

      // XXX ne pas recevoir de load/unload dans Porcupine et utiliser
      // XXX directement Journal::Get puis si pas present, Depot::Get

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

      if (this->root.Dump(margin + 4) == elle::StatusError)
	escape("unable to dump the address");

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
