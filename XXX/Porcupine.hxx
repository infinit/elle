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

#include <XXX/Ambit.hh>

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
    Porcupine<V>::Porcupine():
      height(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method can be used in order to add a value to the tree given
    /// its key.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Add(const typename V::K&  key,
                                          V*                    value)
    {
      Handle                    handle;

      // search for the quill responsible for the given key.
      if (this->Search(key, handle) == elle::StatusError)
        escape("unable to locate a quill for this key");

      Ambit< Quill<V> >         leaf(handle);

      // load the block.
      if (leaf.Load() == elle::StatusError)
        escape("unable to load the block");

      // the block must have been loaded i.e by Search().
      assert(leaf() != nullptr);

      // insert the key/value tuple in the given quill.
      if (this->Insert(leaf(), key, value) == elle::StatusError)
        escape("unable to insert the key/value tuple");

      // unload the block.
      if (leaf.Unload() == elle::StatusError)
        escape("unable to unload the block");

      return elle::StatusOk;
    }

    ///
    /// this method returns true if a value exists for the given key.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Exist(const typename V::K& key)
    {
      Handle                    handle;

      // search for the quill responsible for the given key.
      if (this->Search(key, handle) == elle::StatusError)
        {
          purge();
          return elle::StatusFalse;
        }

      Ambit< Quill<V> >         leaf(handle);

      // load the block.
      if (leaf.Load() == elle::StatusError) 
        {
          purge();
          return elle::StatusFalse;
        }

      // the block must have been loaded i.e by Search().
      assert(leaf() != nullptr);

      // check whether a value exist for this key.
      if (leaf()->Exist(key) == elle::StatusFalse)
        {
          purge();
          return elle::StatusFalse;
        }

      // unload the block.
      if (leaf.Unload() == elle::StatusError)
        {
          purge();
          return elle::StatusFalse;
        }

      return elle::StatusTrue;
    }

    ///
    /// this method can be used for retrieving a value given its key.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Locate(const typename V::K& key,
                                             Handle&            value)
    {
      Handle                    handle;

      // search for the quill responsible for the given key.
      if (this->Search(key, handle) == elle::StatusError)
        escape("unable to locate a quill for this key");

      Ambit< Quill<V> >         leaf(handle);

      // load the block.
      if (leaf.Load() == elle::StatusError)
        escape("unable to load the block");

      // the block must have been loaded i.e by Search().
      assert(leaf() != nullptr);

      // locate the value within the quill.
      if (leaf()->Locate(key, value) == elle::StatusError)
        escape("unable to locate the value");

      // unload the block.
      if (leaf.Unload() == elle::StatusError)
        escape("unable to unload the block");

      return elle::StatusOk;
    }

    ///
    /// this method removes the value associated with the given key.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Remove(const typename V::K& key)
    {
      Handle                    handle;

      // search for the quill responsible for the given key.
      if (this->Search(key, handle) == elle::StatusError)
        escape("unable to locate a quill for this key");

      Ambit< Quill<V> >         leaf(handle);

      // load the block.
      if (leaf.Load() == elle::StatusError)
        escape("unable to load the block");

      // the block must have been loaded i.e by Search().
      assert(leaf() != nullptr);

      // delete the key/value tuple from the given quill.
      if (this->Delete(leaf(), key) == elle::StatusError)
        escape("unable to delete the key tuple");

      // unload the block.
      if (leaf.Unload() == elle::StatusError)
        escape("unable to unload the block");

      return elle::StatusOk;
    }

    ///
    /// this method is called recursively for inserting the key/value tuple
    /// in the given nodule.
    ///
    /// the recursion is necessary because inserting a tuple may imply
    /// creating a new nodule, after splitting for instance, which would
    /// in turn require inserting the newly created nodule.
    ///
    template <typename V>
    template <typename N,
              typename W>
    elle::Status        Porcupine<V>::Insert(N*                 nodule,
                                             const typename V::K& key,
                                             W*                 value)
    {
      struct
      {
        struct
        {
          typename V::K recent;
        }               right;
        struct
        {
          typename V::K ancient;
          typename V::K temporary;
          typename V::K recent;
        }               current;
      }                 mayor;
      N*                _current;

      // set the convenient _current alias.
      _current = nodule;

      // create an inlet here in order to be able to compute its footprint.
      auto              inlet =
        std::unique_ptr<typename N::I>(new typename N::I(key, value));

      // compute the inlet's footprint.
      if (inlet.get()->_footprint.Compute() == elle::StatusError)
        escape("unable to compute the inlet's footprint");

      // check if the future nodule's footprint---i.e should the inlet
      // be inserted---would exceed the extent.
      if ((_current->_footprint.size + inlet.get()->_footprint.size) >
          hole::Hole::Descriptor.extent)
        {
          N*            _right;

          // first, retrieve the mayor key of the current nodule.
          if (_current->Mayor(mayor.current.ancient) == elle::StatusError)
            escape("unable to retrive the mayor");

          //
          // in this case, the inlet is not inserted in order to prevent
          // the tree from being updated though we know the nodule will
          // be split.
          //
          // thus, the nodule is split first, then the inlet will be inserted.
          //

          // split the nodule.
          if (_current->Split(_right) == elle::StatusError)
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

          Ambit<N>              right(_current->right);

          // load the right nodule, if possible.
          if (right.Load() == elle::StatusError)
            escape("unable to load the block");

          // update the old right nodule's neighbour links, if required.
          if (right() != nullptr)
            right()->left = Handle(_right);

          // set the new right nodule's neighbour links.
          _right->left = Handle(_current);
          _right->right = _current->right;

          // finally, update the current nodule's right neighbour link.
          _current->right = Handle(_right);

          // unload the right nodule.
          if (right.Unload() == elle::StatusError)
            escape("unable to unload the block");

          // retrieve the current nodule's mayor key.
          if (_current->Mayor(mayor.current.temporary) == elle::StatusError)
            escape("unable to retrieve the mayor key");

          // insert the inlet depending on its key: if its key is lower than
          // the mayor, then it falls in the left nodule; otherwise in the
          // right.
          if (inlet.get()->key < mayor.current.temporary)
            {
              // insert the inlet to the left nodule.
              if (_current->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");
            }
          else
            {
              // insert the inlet to the right nodule.
              if (_right->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");
            }

          // release the tracking.
          inlet.release();

          Ambit< Nodule<V> >    root(this->root);
          Handle                handle;

          // load the root nodule, if possible.
          if (root.Load() == elle::StatusError)
            escape("unable to load the root block");

          // create a parent node if necessary i.e if the current nodule
          // is the root.
          if (_current == root()) // XXX[possible de comparer des handles pour ne pas avoir a loader le root? ou juste checker si il y a un parent]
            {
              // make the porcupine grow.
              if (this->Grow() == elle::StatusError)
                escape("unable to make the porcupine grow");

              Ambit< Seam<V> >  newroot(this->root);

              // load the new root nodule.
              if (newroot.Load() == elle::StatusError)
                escape("unable to load the new root block");

              // make sure the nodule gets loaded.
              assert(newroot() != nullptr);

              // set the parent handle from the new root.
              handle = newroot.handle;

              // unload the new root nodule.
              if (newroot.Unload() == elle::StatusError)
                escape("unable to unload the new root block");
            }
          else
            {
              Ambit< Seam<V> >  parent(_current->parent);

              // load the parent block, if possible.
              if (parent.Load() == elle::StatusError)
                escape("unable to load the parent block");

              // make sure the nodule gets loaded.
              assert(parent() != nullptr);

              // set the handle from the parent's.
              handle = parent.handle;

              // retrieve the current nodule's new mayor key.
              if (_current->Mayor(mayor.current.recent) == elle::StatusError)
                escape("unable to retrive the mayor");

              // then, update the parent nodule, should the mayor key have
              // changed.
              if (mayor.current.recent != mayor.current.ancient)
                {
                  // propagate the update to the parent nodules.
                  if (parent()->Propagate(
                        mayor.current.ancient,
                        mayor.current.recent) == elle::StatusError)
                    escape("unable to update the parent nodule");
                }

              // unload the parent block.
              if (parent.Unload() == elle::StatusError)
                escape("unable to unload the parent block");
            }

          // unload the root nodule.
          if (root.Unload() == elle::StatusError)
            escape("unable to unload the block");

          Ambit< Seam<V> >      parent(handle);

          // load the selected parent seam.
          if (parent.Load() == elle::StatusError)
            escape("unable to load the block");

          // make sure the nodule gets loaded.
          assert(parent() != nullptr);

          // retrieve the right nodule's mayor key.
          if (_right->Mayor(mayor.right.recent) == elle::StatusError)
            escape("unable to retrieve the mayor key");

          // insert the right nodule in its new parent.
          if (this->Insert(parent(),
                           mayor.right.recent,
                           _right) == elle::StatusError)
            escape("unable to insert the right nodule to its parent");

          // unload the selected parent seam.
          if (parent.Unload() == elle::StatusError)
            escape("unable to unload the block");
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

          // is the container empty...
          if (_current->container.empty() == true)
            {
              //
              // the nodule is empty: insert the inlet.
              //

              // insert the inlet into the nodule.
              if (_current->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");

              // release the tracking..
              inlet.release();

              // note that a nodule being empty in this case implies
              // that the nodule is the root. therefore, there is no
              // need to propagate the mayor key.
            }
          else
            {
              //
              // the nodule is not empty: insert the inlet and update
              // the parent nodule, if necessary.
              //

              // retrieve the current nodule's mayor key.
              if (_current->Mayor(mayor.current.ancient) == elle::StatusError)
                escape("unable to retrive the mayor");

              // insert the inlet to the nodule.
              if (_current->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");

              // release the tracking.
              inlet.release();

              // retrieve the nodule's new mayor key.
              if (_current->Mayor(mayor.current.recent) == elle::StatusError)
                escape("unable to retrive the mayor");

              // finally, update the parent nodule, should the mayor key have
              // changed.
              if ((mayor.current.recent != mayor.current.ancient) &&
                  (_current->parent != Handle::Null))
                {
                  Ambit< Seam<V> >      parent(_current->parent);

                  // load the parent seam.
                  if (parent.Load() == elle::StatusError)
                    escape("unable to load the parent block");

                  // make sure the nodule gets loaded.
                  assert(parent() != nullptr);

                  // propagate the change of mayor key throughout the
                  // hiearchy.
                  if (parent()->Propagate(
                        mayor.current.ancient,
                        mayor.current.recent) == elle::StatusError)
                    escape("unable to update the parent nodule");

                  // unload the parent seam.
                  if (parent.Unload() == elle::StatusError)
                    escape("unable to unload the block");
                }
            }
        }

      return elle::StatusOk;
    }

    ///
    /// this method deletes the entry referenced by the given key from the
    /// nodule, in a recursive way.
    ///
    /// indeed, as for Insert(), this method may merge nodules together,
    /// leading to the deletion of another nodule, potentially recursively
    /// up to the top of the tree.
    ///
    /// note that the root nodule may also be completely destroyed should
    /// the tree become empty. likewise, the root nodule may be replaced
    /// by another one should it become to small.
    ///
    template <typename V>
    template <typename N>
    elle::Status        Porcupine<V>::Delete(N*                 nodule,
                                             const typename V::K& key)
    {
      struct
      {
        struct
        {
          typename V::K ancient;
          typename V::K recent;
        }               left;
        struct
        {
          typename V::K ancient;
          typename V::K recent;
        }               current;
        struct
        {
          typename V::K ancient;
          typename V::K recent;
        }               right;
      }                 mayor;
      N*                _current;

      // set the convenient _current alias.
      _current = nodule;

      // retrieve the nodule's mayor.
      if (_current->Mayor(mayor.current.ancient) == elle::StatusError)
        escape("unable to retrieve the nodule's mayor key");

      // delete the inlet in the nodule.
      if (_current->Delete(key) == elle::StatusError)
        escape("unable to delete the inlet from the nodule");

      // operate depending on the state of the nodule.
      if (_current->container.empty() == true)
        {
          //
          // if the nodule became empty, it can be removed as well.
          //
          // if the nodule is the root one, the tree is now empty, otherwise
          // the nodule must be deleted from its parent nodule.
          //

          // if a parent exists...
          if (_current->parent != Handle::Null)
            {
              //
              // a parent nodule seems to exist: delete the nodule from
              // it taking care to update the neighbour links.
              //

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

              Ambit<N>                  left(_current->left);

              // load the left nodule.
              if (left.Load() == elle::StatusError)
                escape("unable to load the block");

              // update the left nodule's right neighbour, if required.
              if (left() != nullptr)
                left()->right = _current->right;

              // unload the left nodule.
              if (left.Unload() == elle::StatusError)
                escape("unable to unload the block");

              Ambit<N>                  right(_current->right);

              // load the right nodule.
              if (right.Load() == elle::StatusError)
                escape("unable to load the block");

              // update the right nodule's left neighbour.
              if (right() != nullptr)
                right()->left = _current->left;

              // unload the right nodule.
              if (right.Unload() == elle::StatusError)
                escape("unable to unload the block");

              // finally, delete the nodule from its parent by passing
              // the mayor i.e the key the parent seam uses to reference
              // the now-empty nodule.
              {
                Ambit< Seam<V> >        parent(_current->parent);

                // load the parent nodule.
                if (parent.Load() == elle::StatusError)
                  escape("unable to load the block");

                // make sure the nodule gets loaded.
                assert(parent() != nullptr);

                // delete the nodule.
                if (this->Delete(parent(),
                                 mayor.current.ancient) == elle::StatusError)
                  escape("unable to delete the nodule from its parent");

                // unload the parent nodule.
                if (parent.Unload() == elle::StatusError)
                  escape("unable to unload the block");
              }
            }
          else
            {
              //
              // in this case, there does not seem to be a parent node.
              //
              // in other words, the current nodule is the root one, and
              // is now empty.
              //
              // the tree can therefore be reset.
              //

              // reset the root handle to null.
              this->root = Handle::Null;

              // reset the tree's height.
              this->height = 0;
            }
        }
      else if (_current->_footprint.size <
               (hole::Hole::Descriptor.extent *
                hole::Hole::Descriptor.balancing))
        {
          //
          // the nodule may not be empty but could have reached a size
          // small enough for the tree to be simplified.
          //
          // indeed, neighbour nodes could be balanced in order to better
          // support future insertions and deletions or could even be
          // merged in order to potentially reduced the tree complexity
          // and height.
          //

          Ambit<N>              left(_current->left);
          Ambit<N>              right(_current->right);
          Ambit< Seam<V> >      parent(_current->parent);

          // load the nodule.
          if (left.Load() == elle::StatusError)
            escape("unable to load the block");

          // load the nodule.
          if (right.Load() == elle::StatusError)
            escape("unable to load the block");

          // load the nodule.
          if (parent.Load() == elle::StatusError)
            escape("unable to load the block");

          // operate depending of several configurations, ranging from
          // the easiest or more convenient to the less optimised.
          if (parent() == nullptr)
            {
              //
              // in this case, the nodule does not seem to have a parent i.e
              // is the root nodule. the tree is shrunk, if possible---i.e if
              // the root nodule is not alone but most importantly if the
              // root nodule contains a single inlet---in which case its
              // maiden inlet will become the new root nodule.
              //
              // note that this case is tested first so that the following
              // ones do not have to test for the parent address to be
              // null.
              //

              // shrink the tree.
              if (this->Shrink() == elle::StatusError)
                escape("unable to shrink the tree");
            }
          else if ((left() != nullptr) &&
                   (left()->_state == StateDirty) &&
                   (left()->parent == _current->parent) &&
                   ((left()->_footprint.size +
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
              // the number of modified nodules i.e blocks which would
              // then need to be pushed onto the network.
              //
              goto _merge_left;
            }
          else if ((right() != nullptr) &&
                   (right()->_state == StateDirty) &&
                   (right()->parent == _current->parent) &&
                   ((_current->_footprint.size -
                     N::Footprint +
                     right()->_footprint.size) <
                    hole::Hole::Descriptor.extent))
            {
              //
              // this case is similar to the previous but for the right
              // neighbour nodule.
              //
              goto _merge_right;
            }
          else if ((left() != nullptr) &&
                   (left()->parent == _current->parent) &&
                   ((left()->_footprint.size +
                     _current->_footprint.size -
                     N::Footprint) <
                    hole::Hole::Descriptor.extent))
            {
              //
              // in this case the left neighbour nodule seems to be present.
              //
              // besides, both the current and the left nodules have the same
              // parent. this is important because merging both nodules will
              // imply updating a single parent nodule; the number of updates
              // is reduced implying less computation but also less nodules
              // i.e blocks being potentially modified.
              //
            _merge_left:

              // retrieve the left mayor key.
              if (left()->Mayor(
                    mayor.left.ancient) == elle::StatusError)
                escape("unable to retrieve the mayor key");

              // merge the nodule.
              if (left()->Merge(_current) == elle::StatusError)
                escape("unable to merge the nodules");

              // retrieve the left mayor key.
              if (left()->Mayor(
                    mayor.left.recent) == elle::StatusError)
                escape("unable to retrieve the mayor key");

              //
              // make the tree consistent by updating the neighbour links.
              //

              // update the left nodule's right neighbour.
              left()->right = _current->right;

              // update the right nodule which should no longer reference
              // the current, now empty, nodule.
              if (right() != nullptr)
                right()->left = _current->left;

              // in this case, the parent nodule must exist and
              // have been loaded.
              assert(parent() != nullptr);

              // delete the current nodule in its parent since the
              // nodule is now empty.
              if (this->Delete(parent(),
                               mayor.current.ancient) == elle::StatusError)
                escape("unable to delete the entry from the parent");

              Ambit< Seam<V> >          newparent(left()->parent);

              // load the left nodule's parent. note that this is
              // necessary because the left nodule may have lost its
              // parent on the way i.e through the recusive deletion.
              //
              // indeed, for instance, by removing the current nodule
              // from its parent and so forth up to the root nodule,
              // the tree may have been shrunk, leading for instance
              // the _left_ nodule to become the new root; in which case
              // this nodule would no longer have a parent.
              if (newparent.Load() == elle::StatusError)
                escape("unable to load the block");

              // make sure the nodule gets loaded.
              assert(newparent() != nullptr);

              // if a parent is present...
              if (newparent() != nullptr)
                {
                  // update the parent so that the left nodule gets
                  // referenced with the proper key.
                  //
                  // note that although the mayor key would theoretically
                  // would not need propagation because the highest index
                  // key has not changed through merging, the fact that
                  // that parent Delete()ing has been performed above
                  // may have updated the parent key to a small one :(
                  //
                  // thus propagation is actually required for ensuring
                  // the tree's consistency.
                  if (newparent()->Propagate(
                        mayor.left.ancient,
                        mayor.left.recent) == elle::StatusError)
                    escape("unable to update the parent nodule");
                }

              // unload the block.
              if (newparent.Unload() == elle::StatusError)
                escape("unable to unload the block");
            }
          else if ((right() != nullptr) &&
                   (right()->parent == _current->parent) &&
                   ((_current->_footprint.size -
                     N::Footprint +
                     right()->_footprint.size) <
                    hole::Hole::Descriptor.extent))
            {
              //
              // this case is identical to the previous one except that
              // merging occurs between the current and right nodules.
              //
            _merge_right:

              // merge the nodule.
              if (right()->Merge(_current) == elle::StatusError)
                escape("unable to merge the nodules");

              //
              // make the tree consistent by updating the neighbour links.
              //

              // update the left nodule.
              if (left() != nullptr)
                left()->right = _current->right;

              // update the right nodule which should no longer reference
              // the current, now empty, nodule.
              right()->left = _current->left;

              // in this case, the parent nodule must exist and have
              // been loaded.
              assert(parent() != nullptr);

              // delete the current nodule in its parent since the
              // nodule is now empty.
              if (this->Delete(parent(),
                               mayor.current.ancient) == elle::StatusError)
                escape("unable to delete the entry from the parent");

              // note that the parent nodule does not need to be updated
              // regarding the new inlets merged into the right nodule
              // because the added inlets have lower keys compared to
              // the already existing ones in the right nodule.
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
              goto _normal;
            }

          // unload the nodule.
          if (left.Unload() == elle::StatusError)
            escape("unable to unload the block");

          // load the nodule.
          if (right.Unload() == elle::StatusError)
            escape("unable to unload the block");
        }
      else
        {
          //
          // in this case, the nodule is neither empty nor small enough
          // to be merged or balanced.
          //
          // therefore, nothing special is accomplished but propagating
          // the update on the parent nodule since the mayor key may
          // have changed following the deletion.
          //
        _normal:

          // retrieve the nodule's new mayor.
          if (_current->Mayor(mayor.current.recent) == elle::StatusError)
            escape("unable to retrieve the nodule's mayor key");

          // finally, update the parent nodule, should the mayor key have
          // changed.
          if ((mayor.current.recent != mayor.current.ancient) &&
              (_current->parent != Handle::Null))
            {
              Ambit< Seam<V> >  parent(_current->parent);

              // load the nodule.
              if (parent.Load() == elle::StatusError)
                escape("unable to load the block");

              // make sure the nodule gets loaded.
              assert(parent() != nullptr);

              // propagate the change of mayor key throughout the
              // hiearchy.
              if (parent()->Propagate(
                    mayor.current.ancient,
                    mayor.current.recent) == elle::StatusError)
                escape("unable to update the parent nodule");

              // unload the nodule.
              if (parent.Unload() == elle::StatusError)
                escape("unable to unload the block");
            }
        }

      return elle::StatusOk;
    }

    ///
    /// this method make the tree grow, adding a level to the hiearchy.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Grow()
    {
      typename V::K     mayor;

      // depending on the current height.
      if (this->height == 0)
        {
          //
          // if the tree does not exist, create a root nodule.
          //

          // at first, allocate an initial root nodule which happens to be
          // a leaf since alone.
          auto                  root =
            std::unique_ptr< Quill<V> >(new Quill<V>);

          // create the quill.
          if (root.get()->Create() == elle::StatusError)
            escape("unable to create the quill");

          // create the root handle.
          this->root = Handle(root.get());

          // waive the tracking.
          root.release();

          // register the new block.
          // XXX NestRegister(this->root);
          // XXX unload? ou plutot besoin de faire load pour l'utiliser proprement apres register.
        }
      else
        {
          //
          // otherwise, create a new seam and add the current root in
          // this new seam which will become the new root.
          //
          Ambit< Nodule<V> >    root(this->root);

          // load the root nodule.
          if (root.Load() == elle::StatusError)
            escape("unable to load the root block");

          // make sure the block gets loaded.
          assert(root() != nullptr);

          // retrieve the current root's mayor key.
          if (root()->Mayor(mayor) == elle::StatusError)
            escape("unable to retrieve the mayor key");

          // allocate a new root seam.
          auto                  newroot =
            std::unique_ptr< Seam<V> >(new Seam<V>);

          // create the root seam.
          if (newroot.get()->Create() == elle::StatusError)
            escape("unable to create the seam");

          // add the current root nodule in the new root seam.
          if (newroot.get()->Insert(mayor,
                                    root()) == elle::StatusError)
            escape("unable to add the current root in the new root nodule");

          // link the existing root to its new parent i.e the new root.
          root()->parent = Handle(newroot.get());

          // set the new root.
          this->root = root()->parent;

          // at this point the new root must have no parent.
          assert(newroot.get()->parent == Handle::Null);

          // waive the tracking.
          newroot.release();

          // unload the root nodule.
          if (root.Unload() == elle::StatusError)
            escape("unable to unload the root block");
        }

      // increment the height.
      this->height++;

      return elle::StatusOk;
    }

    ///
    /// this method shrinks the tree, removing the current root nodule
    /// by its maiden child nodule.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Shrink()
    {
      typename V::K             maiden;
      typename Seam<V>::I*      inlet;

      // if the tree is empty or has a root nodule only, ignore this step.
      if (this->height <= 1)
        return elle::StatusOk;
      // XXX[perhaps we should completely wipe out the tree so that all the
      //     blocks get deleted. this way, removing a file's content will
      //     result in an empty tree which would then no longer be referenced.]
      // XXX[in other words, make sure when we get to this point the root
      //     block will be deleted if remaining and empty.]

      // note that since the Shrink() method proceeds only for trees
      // with a two-level heigh and more, the root nodule cannot be anything
      // but a seam.
      Ambit< Seam<V> >          root(this->root);

      // load the root nodule.
      if (root.Load() == elle::StatusError)
        escape("unable to load the root block");

      // make sure the block gets loaded.
      assert(root() != nullptr);

      // retrieve the maiden key.
      if (root()->Maiden(maiden) == elle::StatusError)
        escape("unable to retrieve the maiden key");

      // retrieve the inlet associated with the given key.
      if (root()->Locate(maiden, inlet) == elle::StatusError)
        escape("unable to retrieve the nodule");

      // set the new root nodule.
      this->root = inlet->value;

      // delete the root.
      delete root();

      // XXX[unregister the block]

      Ambit< Seam<V> >          newroot(this->root);

      // load the root nodule.
      if (newroot.Load() == elle::StatusError)
        escape("unable to load the root block");

      // make sure the block gets loaded.
      assert(newroot() != nullptr);

      // update the new root links.
      newroot()->parent = Handle::Null;

      // unload the root nodule.
      if (newroot.Unload() == elle::StatusError)
        escape("unable to unload the root block");

      // decrease the tree's height.
      this->height--;

      return elle::StatusOk;
    }

    ///
    /// this method is used for retrieving the handle of the quill
    /// responsible for the given key.
    ///
    /// this method is widely used being for inserting, locating or
    /// deleting entries.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Search(const typename V::K& key,
                                             Handle&            handle)
    {
      // if the tree is non-existent, create a first level by making it grow.
      //
      // note that this may occur at the very beginning but also after
      // the tree has been emptied.
      if (this->height == 0)
        {
          // make the tree grow.
          if (this->Grow() == elle::StatusError)
            escape("unable to make the tree grow");
        }

      // first, set the handle as being the root.
      //
      // this is required because, should the root be a quill since the
      // quill Search() implementation does nothing.
      //
      // therefore, the whole Search() mechanism relies on the fact that
      // the Search() method sets the handle temporarily until the child
      // quill does nothing after which the call returns with the temporary
      // handle which happens to reference the quill.
      //
      handle = this->root;

      Ambit< Nodule<V> >        root(this->root);

      // XXX
      root.Dump();

      // load the root nodule.
      if (root.Load() == elle::StatusError)
        escape("unable to load the root block");

      // make sure the block gets loaded.
      assert(root() != nullptr);

      // trigger the search method on the root nodule.
      if (root()->Search(key, handle) == elle::StatusError)
        escape("unable to locate the quill for this key");

      // unload the root nodule.
      if (root.Unload() == elle::StatusError)
        escape("unable to unload the root block");

      return elle::StatusOk;
    }

    ///
    /// this method is used for checking the tree's consistency by verifying
    /// that every inlet's reference key is equivalent to the child nodule's
    /// mayor key.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Check()
    {
      Ambit< Nodule<V> >        root(this->root);

      // load the root nodule.
      if (root.Load() == elle::StatusError)
        escape("unable to load the root block");

      // proceed if there is a root block.
      if (root() != nullptr)
        {
          // trigger the check method on the root nodule.
          if (root()->Check() == elle::StatusError)
            escape("unable to check the root nodule's consistency");
        }

      return elle::StatusOk;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the porcupine.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Dump(const elle::Natural32 margin)
      const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Porcupine]" << std::endl;

      // dump the height.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Height] " << std::dec << this->height << std::endl;

      // dump the root.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Root]" << std::endl;

      if (this->root.Dump(margin + 4) == elle::StatusError)
        escape("unable to dump the handle");

      return elle::StatusOk;
    }

  }
}

#endif
