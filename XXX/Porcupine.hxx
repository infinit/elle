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
	  }			major;

	  // first, retrieve the major key of the current nodule.
	  if (nodule->Major(major.ancient) == elle::StatusError)
	    escape("unable to retrive the major");

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

	  // retrieve the left nodule's major key.
	  if (nodule->Major(major.temporary) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // insert the inlet depending on its key: if its key is lower than
	  // the major, then it falls in the left nodule; otherwise in the
	  // right.
	  if (inlet->key < major.temporary)
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

	      // retrieve the current nodule's new major key.
	      if (nodule->Major(major.recent) == elle::StatusError)
		escape("unable to retrive the major");

	      // then, update the parent nodule, should the major key have
	      // changed.
	      if (major.recent != major.ancient)
		{
		  // propagate the update to the parent nodules.
		  if (nodule->_parent->Propagate(
		        major.ancient,
			major.recent) == elle::StatusError)
		    escape("unable to update the parent nodule");
		}
	    }

	  // retrieve the major key.
	  if (right->Major(major.temporary) == elle::StatusError)
	    escape("unable to retrieve the major key");

	  // insert the right nodule in its new parent.
	  if (this->Insert(parent,
			   major.temporary,
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
	      // need to propagate the major key.
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
	      }			major;

	      // retrieve the current nodule's major key.
	      if (nodule->Major(major.ancient) == elle::StatusError)
		escape("unable to retrive the major");

	      // insert the inlet to the nodule.
	      if (nodule->Insert(inlet) == elle::StatusError)
		escape("unable to insert the inlet to the nodule");

	      // waive.
	      waive(inlet);

	      // retrieve the nodule's new major key.
	      if (nodule->Major(major.recent) == elle::StatusError)
		escape("unable to retrive the major");

	      // finally, update the parent nodule, should the major key have
	      // changed.
	      if ((major.recent != major.ancient) &&
		  (nodule->parent != Address::Null))
		{
		  // load the parent nodule.
		  PorcupineLoad(nodule, parent);

		  // propagate the change of major key throughout the
		  // hiearchy.
		  if (nodule->_parent->Propagate(
		        major.ancient,
			major.recent) == elle::StatusError)
		    escape("unable to update the parent nodule");
		}
	    }
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
      struct
      {
	typename V::K	ancient;
	typename V::K	recent;
      }			major;

      enter();

      // retrieve the nodule's major.
      if (nodule->Major(major.ancient) == elle::StatusError)
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
	      // load the parent nodule.
	      PorcupineLoad(nodule, parent);

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
	      PorcupineLoad(nodule, left)
		{
		  // update the left nodule's right neighbour.
		  nodule->_left->right = nodule->right;
		  nodule->_left->_right = nodule->_right;
		}

	      // load the right neighbour, if possible.
	      PorcupineLoad(nodule, right)
		{
		  // update the right nodule's left neighbour.
		  nodule->_right->left = nodule->left;
		  nodule->_right->_left = nodule->_left;
		}

	      // finally, delete the nodule from its parent by passing
	      // the major i.e the key the parent seam uses to reference
	      // the now-empty nodule.
	      if (this->Delete(nodule->_parent,
			       major.ancient) == elle::StatusError)
		escape("unable to delete the nodule from its parent");
	    }
	  else
	    {
	      // delete the root nodule manually, if necessary.
	      if (this->_root != NULL)
		delete this->_root;

	      // reset the root nodule address and pointer.
	      this->root = Address::Null;
	      this->_root = NULL;

	      // reset the tree's height.
	      this->height--; // XXX = 0;
	    }
	}
      else if (nodule->_footprint.size <
	       (hole::Hole::Descriptor.extent *
		hole::Hole::Descriptor.balancing))
	{
	  //
	  // XXX
	  //

	  struct
	  {
	    N*			nodule;

	    struct
	    {
	      typename V::K	ancient;
	      typename V::K	recent;
	    }			major;
	  }			left;
	  struct
	  {
	    N*			nodule;

	    struct
	    {
	      typename V::K	ancient;
	      typename V::K	recent;
	    }			major;
	  }			right;

	  // load the parent nodule, if possible.
	  PorcupineLoad(nodule, parent);

	  // load the left nodule, if possible.
	  PorcupineLoad(nodule, left)
	    {
	      // set the left.
	      left.nodule = static_cast<N*>(nodule->_left);

	      // retrieve the left major key.
	      if (nodule->_left->Major(
		    left.major.ancient) == elle::StatusError)
		escape("unable to retrieve the major key");
	    }
	  else
	    {
	      left.nodule = NULL;
	    }

	  // load the right nodule, if possible.
	  PorcupineLoad(nodule, right)
	    {
	      // set the right.
	      right.nodule = static_cast<N*>(nodule->_right);

	      // retrieve the right major key.
	      if (nodule->_right->Major(
		    right.major.ancient) == elle::StatusError)
		escape("unable to retrieve the major key");
	    }
	  else
	    {
	      right.nodule = NULL;
	    }

	  // XXX optimiser avec des cas pour _state = Dirty

	  // XXX
	  if (nodule->_parent == NULL) // XXXparent
	    {
	      // XXX root node
	      printf("MERGE ROOT\n");
	    }
	  else if ((nodule->_left != NULL) &&
		   (nodule->_left->_parent == nodule->_parent) && //XXXparent
		   ((nodule->_left->_footprint.size +
		     nodule->_footprint.size -
		     N::Footprint) <
		    hole::Hole::Descriptor.extent))
	    {
	      printf("MERGE LEFT\n");

	      // merge the nodule.
	      if (left.nodule->Merge(nodule) == elle::StatusError)
		escape("unable to merge the nodules");

	      // retrieve the left major key.
	      if (nodule->_left->Major(
		    left.major.recent) == elle::StatusError)
		escape("unable to retrieve the major key");

	      //
	      // make the tree consistent by updating the neighbour links.
	      //

	      // update the left nodule which now contains all the inlets.
	      if (nodule->_left != NULL)
		{
		  // update the left nodule's right neighbour.
		  nodule->_left->right = nodule->right;
		  nodule->_left->_right = nodule->_right;
		}

	      // update the right nodule which should no longer reference
	      // the current, now empty, nodule.
	      if (nodule->_right != NULL)
		{
		  // update the right nodule's left neighbour.
		  nodule->_right->left = nodule->left;
		  nodule->_right->_left = nodule->_left;
		}

	      // load the parent nodule.
	      PorcupineLoad(nodule, parent);
		{
		  // delete the current nodule in its parent since the
		  // nodule is now empty.
		  if (this->Delete(nodule->_parent,
				   major.ancient) == elle::StatusError)
		    escape("unable to delete the entry from the parent");

		  // update the parent so that the left nodule gets referenced
		  // with the proper key.
		  //
		  // note however that the major key does not need propagation
		  // because the highest index key has not changed through
		  // merging.
		  if (left.nodule->_parent->Propagate(
		        left.major.ancient,
			left.major.recent) == elle::StatusError)
		    escape("unable to update the parent nodule");
		}
	    }
	  else if ((nodule->_right != NULL) &&
		   (nodule->_right->_parent == nodule->_parent) && //XXXparent
		   ((nodule->_footprint.size -
		     N::Footprint +
		     nodule->_right->_footprint.size) <
		    hole::Hole::Descriptor.extent))
	    {
	      // XXX
	      printf("MERGE RIGHT\n");

	      // merge the nodule.
	      if (right.nodule->Merge(nodule) == elle::StatusError)
		escape("unable to merge the nodules");

	      //
	      // make the tree consistent by updating the neighbour links.
	      //

	      // update the left nodule which now contains all the inlets.
	      if (nodule->_left != NULL)
		{
		  // update the left nodule's right neighbour.
		  nodule->_left->right = nodule->right;
		  nodule->_left->_right = nodule->_right;
		}

	      // update the right nodule which should no longer reference
	      // the current, now empty, nodule.
	      if (nodule->_right != NULL)
		{
		  // update the right nodule's left neighbour.
		  nodule->_right->left = nodule->left;
		  nodule->_right->_left = nodule->_left;
		}

	      // load the parent nodule.
	      PorcupineLoad(nodule, parent)
		{
		  // delete the current nodule in its parent since the
		  // nodule is now empty.
		  if (this->Delete(nodule->_parent,
				   major.ancient) == elle::StatusError)
		    escape("unable to delete the entry from the parent");

		  // note that the parent nodule does not need to be updated
		  // regarding the new inlets merged into the right nodule
		  // because the added inlets have lower key compared to
		  // the already existing ones in the right nodule.
		}
	    }
	  else
	    {
	      // XXX
	      goto _xyz;
	    }
	}
      else
	{
	  // XXX
	_xyz:

	  // retrieve the nodule's new major.
	  if (nodule->Major(major.recent) == elle::StatusError)
	    escape("unable to retrieve the nodule's major key");

	  // finally, update the parent nodule, should the major key have
	  // changed.
	  if ((major.recent != major.ancient) &&
	      (nodule->parent != Address::Null))
	    {
	      // load the parent nodule.
	      PorcupineLoad(nodule, parent);

	      // propagate the change of major key throughout the
	      // hiearchy.
	      if (nodule->_parent->Propagate(
		    major.ancient,
		    major.recent) == elle::StatusError)
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
      typename V::K	major;

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

      leave();
    }

    ///
    /// XXX
    ///
    template <typename V>
    elle::Status	Porcupine<V>::Shrink()
    {
      Seam<V>*					seam =
	static_cast<Seam<V>*>(this->_root);
      typename Seam<V>::Iterator::Forward	iterator;
      typename Seam<V>::I*			inlet;
      Nodule<V>*				root;

      enter();

      // XXX
      if (this->height <= 1)
	leave();

      // XXX
      PorcupineLoad(this, root);

      printf("SHRINK\n");

      // XXX a clarifier car pas tres beau de tapper dans le container
      // XXX   a la main

      // XXX
      iterator = seam->container.begin();

      // XXX
      inlet = iterator->second;

      // XXX
      root = inlet->_value;

      // XXX
      inlet->_value = NULL;

      // XXX
      delete seam;

      // XXX
      this->_root = root;

      // XXX
      this->_root->parent = Address::Null;
      this->_root->_parent = NULL;

      // XXX
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
