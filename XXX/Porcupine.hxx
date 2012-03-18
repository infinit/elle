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
                                          Handle&               value)
    {
      Handle            leaf;

      // search for the quill responsible for the given key.
      if (this->Search(key, leaf) == elle::StatusError)
        escape("unable to locate a quill for this key");

      // insert the key/value tuple in the given quill.
      if (this->Insert< Quill<V> >(leaf, key, value) == elle::StatusError)
        escape("unable to insert the key/value tuple");

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

      // delete the key/value tuple from the given quill.
      if (this->Delete< Quill<V> >(handle, key) == elle::StatusError)
        escape("unable to delete the key tuple");

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
    template <typename N>
    elle::Status        Porcupine<V>::Insert(Handle&            nodule,
                                             const typename V::K& key,
                                             Handle&            value)
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
      Ambit<N>          current(nodule);

      // load the current block.
      if (current.Load() == elle::StatusError)
        escape("unable to load the block");

      // make sure the current block has been loaded.
      assert(current() != nullptr);

      // create an inlet here in order to be able to compute its footprint.
      auto              inlet =
        std::unique_ptr<typename N::I>(new typename N::I(key, value));

      // compute the inlet's footprint.
      if (inlet.get()->_footprint.Compute() == elle::StatusError)
        escape("unable to compute the inlet's footprint");

      // check if the future nodule's footprint---i.e should the inlet
      // be inserted---would exceed the extent.
      if ((current()->_footprint.size + inlet.get()->_footprint.size) >
          hole::Hole::Descriptor.extent)
        {
          struct
          {
            Handle              right;
            Handle              parent;
          }                     handle;
          N*                    _right;

          // first, retrieve the mayor key of the current nodule.
          if (current()->Mayor(mayor.current.ancient) == elle::StatusError)
            escape("unable to retrive the mayor");

          //
          // in this case, the inlet is not inserted in order to prevent
          // the tree from being updated though we know the nodule will
          // be split.
          //
          // thus, the nodule is split first, then the inlet will be inserted.
          //

          // split the nodule.
          if (current()->Split(_right) == elle::StatusError)
            escape("unable to split the nodule");

          // attach the block to the porcupine.
          if (Porcupine<>::Attach.Call(_right,
                                       handle.right) == elle::StatusError)
            escape("unable to attach the block");

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

          Ambit<N>              newright(handle.right);

          // load the new right nodule.
          if (newright.Load() == elle::StatusError)
            escape("unable to load the block");

          // at this point, the new right nodule must have been loaded.
          assert(newright() != nullptr);

          // update the old right nodule's neighbour links, if required.
          if (current()->right != Handle::Null)
            {
              Ambit<N>          right(current()->right);

              // load the right nodule, if possible.
              if (right.Load() == elle::StatusError)
                escape("unable to load the block");

              // make sure the block got loaded.
              assert(right() != nullptr);

              // upadte the right nodule.
              right()->left = newright.handle;

              // unload the right nodule.
              if (right.Unload() == elle::StatusError)
                escape("unable to unload the block");
            }

          // set the new right nodule's neighbour links.
          newright()->left = current.handle;
          newright()->right = current()->right;

          // finally, update the current nodule's right neighbour link.
          current()->right = newright.handle;

          // retrieve the current nodule's mayor key.
          if (current()->Mayor(mayor.current.temporary) == elle::StatusError)
            escape("unable to retrieve the mayor key");

          // insert the inlet depending on its key: if its key is lower than
          // the mayor, then it falls in the left nodule; otherwise in the
          // right.
          if (inlet.get()->key < mayor.current.temporary)
            {
              // insert the inlet to the left nodule.
              if (current()->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");

              // the new child nodule, referenced through the inserted inlet,
              // needs to be updated so as to reference its new parent i.e
              // the _current_ nodule.
              //
              // note however that should _current_ be a quill, the Link()
              // method would do nothing since the insert inlet referenced
              // a data value which obviously does not embed a parent link.
              if (current()->Link(inlet.get(),
                                  current.handle) == elle::StatusError)
                escape("unable to update the parent link");
            }
          else
            {
              // insert the inlet to the right nodule.
              if (newright()->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");
            }

          // release the tracking.
          inlet.release();

          // now that some child nodules have been migrated to the _newright_,
          // these nodules need to be updated in order to reference their
          // new parent i.e the _newright_.
          if (newright()->Link(newright.handle) == elle::StatusError)
            escape("unable to update the child nodules so as to "
                   "reference their new parent");

          // create a parent node if necessary i.e if the current nodule
          // is the root.
          if (current.handle == this->root)
            {
              // make the porcupine grow.
              if (this->Grow() == elle::StatusError)
                escape("unable to make the porcupine grow");

              // set the parent handle from the new root.
              handle.parent = this->root;
            }
          else
            {
              // set the handle from the parent's.
              handle.parent = current()->parent;

              // retrieve the current nodule's new mayor key.
              if (current()->Mayor(mayor.current.recent) == elle::StatusError)
                escape("unable to retrive the mayor");

              // then, update the parent nodule, should the mayor key have
              // changed.
              if (mayor.current.recent != mayor.current.ancient)
                {
                  Ambit< Seam<V> >      parent(current()->parent);

                  // load the parent block, if possible.
                  if (parent.Load() == elle::StatusError)
                    escape("unable to load the parent block");

                  // make sure the nodule gets loaded.
                  assert(parent() != nullptr);

                  // propagate the update to the parent nodules.
                  if (parent()->Propagate(
                        mayor.current.ancient,
                        mayor.current.recent) == elle::StatusError)
                    escape("unable to update the parent nodule");

                  // unload the parent block.
                  if (parent.Unload() == elle::StatusError)
                    escape("unable to unload the parent block");
                }
            }

          // make sure the a parent nodule is present.
          assert(handle.parent != Handle::Null);

          // retrieve the right nodule's mayor key.
          if (newright()->Mayor(mayor.right.recent) == elle::StatusError)
            escape("unable to retrieve the mayor key");

          // insert the right nodule in its new parent.
          if (this->Insert< Seam<V> >(handle.parent,
                                      mayor.right.recent,
                                      newright.handle) == elle::StatusError)
            escape("unable to insert the right nodule to its parent");

          // unload the new right nodule.
          if (newright.Unload() == elle::StatusError)
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
          if (current()->container.empty() == true)
            {
              //
              // the nodule is empty: insert the inlet.
              //

              // insert the inlet into the nodule.
              if (current()->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");

              // update the nodule referenced by the inlet so as to link
              // its new parent nodule.
              if (current()->Link(inlet.get(),
                                  current.handle) == elle::StatusError)
                escape("unable to link the inlet's value to its "
                       "parent nodule");

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
              if (current()->Mayor(mayor.current.ancient) == elle::StatusError)
                escape("unable to retrive the mayor");

              // insert the inlet to the nodule.
              if (current()->Insert(inlet.get()) == elle::StatusError)
                escape("unable to insert the inlet to the nodule");

              // update the nodule referenced by the inlet so as to link
              // its parent nodule.
              if (current()->Link(inlet.get(),
                                  current.handle) == elle::StatusError)
                escape("unable to link the inlet's value to its "
                       "parent nodule");

              // release the tracking.
              inlet.release();

              // retrieve the nodule's new mayor key.
              if (current()->Mayor(mayor.current.recent) == elle::StatusError)
                escape("unable to retrive the mayor");

              // finally, update the parent nodule, should the mayor key have
              // changed.
              if ((mayor.current.recent != mayor.current.ancient) &&
                  (current()->parent != Handle::Null))
                {
                  Ambit< Seam<V> >      parent(current()->parent);

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

      // unload the current block.
      if (current.Unload() == elle::StatusError)
        escape("unable to unload the block");

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
    elle::Status        Porcupine<V>::Delete(Handle&            nodule,
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
      Ambit<N>          current(nodule);

      // load the current block.
      if (current.Load() == elle::StatusError)
        escape("unable to load the block");

      // make sure the current block has been loaded.
      assert(current() != nullptr);

      // retrieve the nodule's mayor.
      if (current()->Mayor(mayor.current.ancient) == elle::StatusError)
        escape("unable to retrieve the nodule's mayor key");

      // delete the inlet in the nodule.
      if (current()->Delete(key) == elle::StatusError)
        escape("unable to delete the inlet from the nodule");

      // operate depending on the state of the nodule.
      if (current()->container.empty() == true)
        {
          //
          // if the nodule became empty, it can be removed as well.
          //
          // if the nodule is the root one, the tree is now empty, otherwise
          // the nodule must be deleted from its parent nodule.
          //

          // if a parent exists...
          if (current()->parent != Handle::Null)
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

              // update the left nodule's right neighbour, if required.
              if (current()->left != Handle::Null)
                {
                  Ambit<N>              left(current()->left);

                  // load the left nodule.
                  if (left.Load() == elle::StatusError)
                    escape("unable to load the block");

                  // make sure the block got loaded.
                  assert(left() != nullptr);

                  // update the link.
                  left()->right = current()->right;

                  // unload the left nodule.
                  if (left.Unload() == elle::StatusError)
                    escape("unable to unload the block");
                }

              // update the right nodule's left neighbour.
              if (current()->right != Handle::Null)
                {
                  Ambit<N>              right(current()->right);

                  // load the right nodule.
                  if (right.Load() == elle::StatusError)
                    escape("unable to load the block");

                  // make sure the block got loaed.
                  assert(right() != nullptr);

                  // update the link.
                  right()->left = current()->left;

                  // unload the right nodule.
                  if (right.Unload() == elle::StatusError)
                    escape("unable to unload the block");
                }

              // finally, delete the nodule from its parent by passing
              // the mayor i.e the key the parent seam uses to reference
              // the now-empty nodule.
              if (this->Delete< Seam<V> >(
                    current()->parent,
                    mayor.current.ancient) == elle::StatusError)
                escape("unable to delete the nodule from its parent");
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

          // in any case, at this point, current is empty and unreferenced.
          // therefore, detach the block from the porcupine.
          //
          // note that the block is finally unloaded at the end of
          // this method.
          if (Porcupine<>::Detach.Call(current.handle) == elle::StatusError)
            escape("unable to detach the block from the porcupine");
        }
      else if (current()->_footprint.size <
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
          // note that, usually, the handles are compared to know if
          // a nodule references a left/right/parent neighbour. however,
          // in the following code, this is done differently. instead,
          // the parent, left and right nodules are loaded, if possible
          // i.e if they exist. then, the address of the loaded blocks
          // is used to know if a neighbour exists. this is required in
          // order to test in the main conditions if a nodule is eligible
          // for merging i.e is present, is in a correct state and has
          // enough available space.
          //
          Ambit<N>              left(current()->left);
          Ambit<N>              right(current()->right);
          Ambit< Seam<V> >      parent(current()->parent);

          // load the nodule, if present.
          if (left.handle != Handle::Null)
            {
              if (left.Load() == elle::StatusError)
                escape("unable to load the block");
            }

          // load the nodule, if present.
          if (right.handle != Handle::Null)
            {
              if (right.Load() == elle::StatusError)
                escape("unable to load the block");
            }

          // load the nodule, if present.
          if (parent.handle != Handle::Null)
            {
              if (parent.Load() == elle::StatusError)
                escape("unable to load the block");
            }

          // operate depending of several configurations, ranging from
          // the easiest or more convenient to the less optimised.
          //
          // note that the pointer can be compared because the block
          // has been loaded, if present.
          if (parent() != nullptr)
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
                   (left()->parent == current()->parent) &&
                   ((left()->_footprint.size +
                     current()->_footprint.size -
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
                   (right()->parent == current()->parent) &&
                   ((current()->_footprint.size -
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
                   (left()->parent == current()->parent) &&
                   ((left()->_footprint.size +
                     current()->_footprint.size -
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
              // i.e blocks being potentially loaded from the network,
              // modified etc.
              //
            _merge_left:

              // retrieve the left mayor key.
              if (left()->Mayor(
                    mayor.left.ancient) == elle::StatusError)
                escape("unable to retrieve the mayor key");

              // merge the nodule.
              if (left()->Merge(current()) == elle::StatusError)
                escape("unable to merge the nodules");

              // now that the inlets have been merged into _left_,
              // the child nodules need to be updated in order to reference
              // their new parent i.e the _left_.
              if (left()->Link(left.handle) == elle::StatusError)
                escape("unable to update the child nodules so as to "
                       "reference their new parent");

              // retrieve the left mayor key.
              if (left()->Mayor(
                    mayor.left.recent) == elle::StatusError)
                escape("unable to retrieve the mayor key");

              //
              // make the tree consistent by updating the neighbour links.
              //

              // update the left nodule's right neighbour.
              left()->right = current()->right;

              // update the right nodule which should no longer reference
              // the current, now empty, nodule.
              //
              // note that the pointer can be compared because the block
              // has been loaded, if present.
              if (right() != nullptr)
                right()->left = current()->left;

              // at this point, current is empty and unreferenced.
              // therefore, detach the block from the porcupine.
              //
              // note that the block is finally unloaded at the end of
              // this method.
              if (Porcupine<>::Detach.Call(current.handle) ==
                  elle::StatusError)
                escape("unable to detach the block from the porcupine");

              // in this case, the parent nodule must exist and
              // have been loaded.
              assert(parent() != nullptr);

              // delete the current nodule in its parent since the
              // nodule is now empty.
              if (this->Delete< Seam<V> >(
                    parent.handle,
                    mayor.current.ancient) == elle::StatusError)
                escape("unable to delete the entry from the parent");

              //
              // proceed only if the nodule's parent exist. note that this
              // is necessary because the left nodule may have lost its
              // parent on the way i.e through the recusive deletion.
              //
              // indeed, for instance, by removing the current nodule
              // from its parent and so forth up to the root nodule,
              // the tree may have been shrunk, leading for instance
              // the _left_ nodule to become the new root; in which case
              // this nodule would no longer have a parent.
              //
              if (left()->parent != Handle::Null)
                {
                  Ambit< Seam<V> >      newparent(left()->parent);

                  if (newparent.Load() == elle::StatusError)
                    escape("unable to load the block");

                  // make sure the nodule gets loaded.
                  assert(newparent() != nullptr);

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

                  // unload the block.
                  if (newparent.Unload() == elle::StatusError)
                    escape("unable to unload the block");
                }
            }
          else if ((right() != nullptr) &&
                   (right()->parent == current()->parent) &&
                   ((current()->_footprint.size -
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
              if (right()->Merge(current()) == elle::StatusError)
                escape("unable to merge the nodules");

              // now that the inlets have been merged into _right_,
              // the child nodules need to be updated in order to reference
              // their new parent i.e the _right_.
              if (right()->Link(right.handle) == elle::StatusError)
                escape("unable to update the child nodules so as to "
                       "reference their new parent");

              //
              // make the tree consistent by updating the neighbour links.
              //

              // update the left nodule.
              //
              // note that the pointer can be compared because the block
              // has been loaded, if present.
              if (left() != nullptr)
                left()->right = current()->right;

              // update the right nodule which should no longer reference
              // the current, now empty, nodule.
              right()->left = current()->left;

              // at this point, current is empty and unreferenced.
              // therefore, detach the block from the porcupine.
              //
              // note that the block is finally unloaded at the end of
              // this method.
              if (Porcupine<>::Detach.Call(current.handle) ==
                  elle::StatusError)
                escape("unable to detach the block from the porcupine");

              // in this case, the parent nodule must exist and have
              // been loaded.
              assert(parent() != nullptr);

              // delete the current nodule in its parent since the
              // nodule is now empty.
              if (this->Delete< Seam<V> >(
                    parent.handle,
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

          // load the nodule.
          if (parent.Unload() == elle::StatusError)
            escape("unable to unload the block");

          // load the nodule.
          if (right.Unload() == elle::StatusError)
            escape("unable to unload the block");

          // unload the nodule.
          if (left.Unload() == elle::StatusError)
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
          if (current()->Mayor(mayor.current.recent) == elle::StatusError)
            escape("unable to retrieve the nodule's mayor key");

          // finally, update the parent nodule, should the mayor key have
          // changed.
          if ((mayor.current.recent != mayor.current.ancient) &&
              (current()->parent != Handle::Null))
            {
              Ambit< Seam<V> >  parent(current()->parent);

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

      // unload the current block.
      if (current.Unload() == elle::StatusError)
        escape("unable to unload the block");

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
          Handle                handle;

          // create the quill.
          if (root.get()->Create() == elle::StatusError)
            escape("unable to create the quill");

          // attach the block to the porcupine.
          if (Porcupine<>::Attach.Call(root.get(),
                                       handle) == elle::StatusError)
            escape("unable to attach the block");

          // waive the tracking.
          root.release();

          // assign the new root handle.
          this->root = handle;
        }
      else
        {
          //
          // otherwise, create a new seam and add the current root in
          // this new seam which will become the new root.
          //
          struct
          {
            Handle              root;
            Handle              newroot;
          }                     handle;

          // copy the current root handle so that no
          // ambit reference _this->root_ since this
          // variable is going to change.
          handle.root = this->root;

          Ambit< Nodule<V> >    root(handle.root);

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
                                    root.handle) == elle::StatusError)
            escape("unable to add the current root in the new root nodule");

          // attach the block to the porcupine.
          if (Porcupine<>::Attach.Call(newroot.get(),
                                       handle.newroot) == elle::StatusError)
            escape("unable to attach the block");

          // assign the new root handle.
          this->root = handle.newroot;

          // link the existing root to its new parent i.e the new root.
          //
          // note that this is done manually rather than by calling the
          // Link() method.
          root()->parent = this->root;

          // unload the root nodule.
          if (root.Unload() == elle::StatusError)
            escape("unable to unload the root block");

          // at this point the new root must have no parent.
          assert(newroot.get()->parent == Handle::Null);

          // waive the tracking.
          newroot.release();
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

      struct
      {
        Handle                  root; // XXX
        Handle                  newroot;
      }                         handle;

      // copy the current root handle so that no ambit reference
      // _this->root_ since this variable is going to change.
      handle.root = this->root;

      // note that since the Shrink() method proceeds only for trees
      // with a two-level heigh and more, the root nodule cannot be anything
      // but a seam.
      Ambit< Seam<V> >          root(handle.root);

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

      // detach the block from the porcupine.
      if (Porcupine<>::Detach.Call(root.handle) == elle::StatusError)
        escape("unable to detach the block from the porcupine");

      // unload the root nodule.
      if (root.Unload() == elle::StatusError)
        escape("unable to unload the root block");

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
      // proceed if there is a root block.
      if (this->root != Handle::Null)
        {
          Ambit< Nodule<V> >    root(this->root);
          Handle                null = Handle::Null;

          // load the root nodule.
          if (root.Load() == elle::StatusError)
            escape("unable to load the root block");

          // trigger the check method on the root nodule.
          if (root()->Check(null,
                            this->root) == elle::StatusError)
            escape("unable to check the root nodule's consistency");

          // unload the root nodule.
          if (root.Unload() == elle::StatusError)
            escape("unable to unload the root block");
        }

      return elle::StatusOk;
    }

    ///
    /// this method traverses the porcupine.
    ///
    template <typename V>
    elle::Status        Porcupine<V>::Traverse(const elle::Natural32    margin)
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Porcupine]" << std::endl;

      // dump the height.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Height] " << std::dec << this->height << std::endl;

      // proceed if there is a root block.
      if (this->root != Handle::Null)
        {
          Ambit< Nodule<V> >    root(this->root);

          // dump the handle.
          if (this->root.Dump(margin + 4) == elle::StatusError)
            escape("unable to dump the handle");

          // dump the root.
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Root]" << std::endl;

          // load the root nodule.
          if (root.Load() == elle::StatusError)
            escape("unable to load the root block");

          // traverse the nodule.
          if (root()->Traverse(margin + 4) == elle::StatusError)
            escape("unable to traverse the nodule");

          // unload the root nodule.
          if (root.Unload() == elle::StatusError)
            escape("unable to unload the root block");
        }
      else
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Root] " << elle::none << std::endl;
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
