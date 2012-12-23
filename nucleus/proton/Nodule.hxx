#ifndef NUCLEUS_PROTON_NODULE_HXX
# define NUCLEUS_PROTON_NODULE_HXX

# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/Footprint.hh>

# include <elle/log.hh>
# include <elle/assert.hh>

namespace nucleus
{
  namespace proton
  {
    /*---------------.
    | Static Methods |
    `---------------*/

    template <typename T>
    template <typename X>
    void
    Nodule<T>::transfer_right(X& left,
                              X& right,
                              Extent const size)
    {
      static Footprint const initial = elle::serialize::footprint<X>();

      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Nodule");
      ELLE_TRACE_FUNCTION(left, right, size);

      Extent const extent = left.nest().limits().extent();
      Footprint footprint(initial);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      auto end = left._container.end();
      auto i = left._container.begin();

      // Go through the left nodule's inlets until the future size is reached
      // after which all the remaining inlets will be moved to the right
      // nodule.
      for (; i != end; ++i)
        {
          auto& inlet = i->second;

          // Check whether the container's size has been reached.
          if (footprint >= size)
            break;

          // Otherwise, leave this inlet in the left seam.

          // Note however that another check is performed in order to make
          // sure that adding this inlet will not make the container too large.
          if ((footprint + inlet->footprint()) > extent)
            break;

          // Add the inlet's footprint to the local _footprint_ variable.
          footprint += inlet->footprint();
        }

      // Go through the remaining inlets in order to move them to
      // the right nodule.
      for (auto j = i; j != end; ++j)
        {
          auto& inlet = j->second;

          // Substract the inlet's footprint from the left nodule since
          // it is getting moved to the right one.
          left.footprint(left.footprint() - inlet->footprint());

          // Insert the inlet into the right seam.
          right.insert(inlet);

          // XXX make it like Catalog.cc

          // Make sure the operation is valid.
          ELLE_ASSERT(inlet->capacity() <= left.capacity());

          // adjust the capacities.
          left.capacity(left.capacity() - inlet->capacity());
          right.capacity(right.capacity() + inlet->capacity());
        }

      // remove the moved inlets from the this seam.
      left._container.erase(i, end);
    }

    ///
    /// this method does the opposite of Export(), moving at least _size_
    /// bytes of inlets from the _right_ seam to the _left_ one.
    ///
    /// note that the inlets with the lowest keys are moved in order to
    /// maintain consistency.
    ///
    /// XXX comments -seam +nodule
    ///
    template <typename T>
    template <typename X>
    void
    Nodule<T>::transfer_left(X& left,
                             X& right,
                             Extent const size)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Nodule");

      static Footprint const initial = elle::serialize::footprint<X>();
      auto rend = right._container.rend();
      auto i = right._container.rbegin();
      Extent const extent = left.nest().limits().extent();
      Footprint footprint(initial);

      ELLE_TRACE_SCOPE("transfer_left(%s, %s, %s)",
                       left, right, size);

      ELLE_ASSERT(left.nest().limits().extent() ==
                  right.nest().limits().extent());

      // go through the right seam's inlets until the future size is reached
      // after which all the remaining inlets will be moved to the left seam.
      for (; i != rend; ++i)
        {
          typename X::I* inlet = i->second;

          // check whether the container's size has been reached.
          if (footprint >= size)
            break;

          //
          // otherwise, leave this inlet in the seam.
          //

          // note however that another check is performed in order to make
          // sure that adding this inlet will not make the container too large.
          if ((footprint + inlet->footprint()) > extent)
            break;

          // add the inlet's footprint to the footprint.
          footprint += inlet->footprint();
        }

      // go through the remaining inlets in order to move them to
      // the left seam.
      for (auto j = i; j != rend; ++j)
        {
          typename X::I* inlet = j->second;

          // substract the inlet's footprint from the right seam since
          // it is getting moved to the left one.
          right.footprint(right.footprint() - inlet->footprint());

          // insert the inlet into the left seam.
          left.insert(inlet);

          // make sure the operation is valid.
          ELLE_ASSERT(inlet->capacity() <= right.capacity());

          // adjust the capacities.
          left.capacity(left.capacity() + inlet->capacity());
          right.capacity(right.capacity() - inlet->capacity());
        }

      // remove the moved inlets from the right nodule.
      right._container.erase(right._container.begin(), i.base());
    }

    ///
    /// XXX si X = Seam<T> alors X::V = Nodule<T>
    ///     si X = Quill<T> alors X::V = T
    ///
    template <typename T>
    template <typename X>
    void
    Nodule<T>::optimize(X& nodule,
                        typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Nodule");

      static Footprint const initial = elle::serialize::footprint<X>();
      typename X::I* inlet;

      ELLE_TRACE_SCOPE("optimize(%s, %s)", nodule, k);

      // look up the entry responsible for this key.
      auto iterator = nodule.lookup_iterator(k);

      // retrieve the inlet.
      inlet = iterator->second;

      Ambit<typename X::V> current(nodule.nest(), inlet->value());

      // load the current child nodule.
      current.load();

      // check if the future nodule's footprint---i.e should the inlet
      // be inserted---would exceed the extent.
      if (current().footprint() > nodule.nest().limits().extent())
        {
          //
          // in this case, the nodule needs to be split.
          //
          struct
          {
            typename T::K current;
            typename T::K newright;
          } mayor;
          struct
          {
            Capacity newright;
          } capacity;
          struct
          {
            State newright;
          } state;

          ELLE_TRACE("nodule's extent high limit reached: "
                     "%s > %s",
                     current().footprint(),
                     nodule.nest().limits().extent());

          // Split the current current nodule.
          Handle orphan{current().split()};

          // load the new right nodule.
          Ambit<typename X::V> newright(nodule.nest(), orphan);

          newright.load();

          //
          // first, retrieve the mayor for both the current and new
          // right nodules.
          //

          mayor.current = current().mayor();
          mayor.newright = newright().mayor();

          // also retrieve the _newright_ capacity and state.
          capacity.newright = newright().capacity();
          state.newright = newright().state();

          // unload the new right nodule.
          newright.unload();

          //
          // now, should the current current mayor have changed,
          // update the current seam so as to reference it properly.
          //
          if (inlet->key() != mayor.current)
            nodule.refresh(iterator, mayor.current);

          // update the current's capacity.
          inlet->capacity(current().capacity());

          // unload the current child nodule.
          current.unload();

          //
          // at this point, the new i.e orphan nodule must be inserted
          // into the current seam.
          //

          // create an inlet.
          auto il = new typename X::I(mayor.newright, orphan);

          ELLE_FINALLY_ACTION_DELETE(il);

          // insert the inlet to the left nodule.
          nodule.insert(il);

          ELLE_FINALLY_ABORT(il);

          // update the inlet with the proper capacity and state.
          il->capacity(capacity.newright);
          il->state(state.newright);

          // Should it be necessary, try to optimize further the
          // freshly split _newright_ node.
          ELLE_TRACE("try to optimize further the new right node");
          Nodule::optimize(nodule, mayor.newright);
        }
      else if ((current().empty() == true) ||
               (current().footprint() <=
                (nodule.nest().limits().extent() *
                 nodule.nest().limits().balancing())))
        {
          ELLE_TRACE("nodule's extent low limit reached: "
                     "%s < %s",
                     current().footprint(),
                     nodule.nest().limits().extent() *
                     nodule.nest().limits().balancing());

          if (current().empty() == true)
            {
              ELLE_TRACE("nodule is empty");

              //
              // if the nodule became empty, it can as well be removed
              // from the current seam.
              //

              // at this point, the capacity of current nodule must
              // be zero.
              ELLE_ASSERT(inlet->capacity() == 0);
              ELLE_ASSERT(current().capacity() == 0);

              // finally, since the current child is now empty and unreferenced,
              // it needs to be detached from the porcupine so its memory
              // gets released.
              //
              // note that the block is unloaded at the end of this method.
              nodule.nest().detach(current.handle());

              // unload the current nodule.
              current.unload();

              // delete the entry from the current seam.
              //
              // note that this must be done after the handle has been
              // used for both detaching the block and unloading the handle.
              nodule.erase(iterator);
            }
          else
            {
              ELLE_TRACE("nodule not empty but could be balanced");

              //
              // the nodule may not be empty but could have reached a size
              // small enough for the tree to be simplified.
              //
              // indeed, neighbour nodes could be balanced in order to better
              // support future insertions and deletions or could even be
              // merged in order to potentially reduce the tree complexity
              // and height.
              //
              typename X::Iterator::Forward previous = iterator;
              typename X::Iterator::Forward next = iterator;

              // move the iterators so as select the previous and next entries
              // within the current seam.
              previous--;
              next++;

              //
              // load the left/right nodules, if the iterators
              // are valid.
              //
              Ambit<typename X::V>* left = nullptr;
              Ambit<typename X::V>* right = nullptr;

              ELLE_FINALLY_ACTION_DELETE(left);
              ELLE_FINALLY_ACTION_DELETE(right);

              if ((iterator != nodule._container.begin()) &&
                  (previous != nodule._container.end()))
                left = new Ambit<typename X::V>(nodule.nest(),
                                                previous->second->value());

              if (next != nodule._container.end())
                right = new Ambit<typename X::V>(nodule.nest(),
                                                 next->second->value());

              // XXX expliquer que la on se base sur le pointeur
              // plutot que le handle.

              if (left != nullptr)
                {
                  ELLE_TRACE("left neighbour present");
                  (*left).load();
                }

              if (right != nullptr)
                {
                  ELLE_TRACE("right neighbour present");
                  (*right).load();
                }

              // XXX expliquer que plus bas on reteste le left/right.handle pour
              // savoir si le neighbour existe et donc a ete loade avant d'essayer
              // d'y acceder.

              //
              // act depending on the context i.e the left node has enough space,
              // or the right exists and has enough space etc.
              //
              // note that, usually, the handles are compared in order to know
              // whether a nodule exists. however, in the present case, the left
              // and right nodules have been loaded anyway so that the pointers
              // can be compared more efficiently.
              //
              if ((left != nullptr) &&
                  ((*left)().state() == State::dirty) &&
                  (((*left)().footprint() + current().footprint() - initial) <
                   nodule.nest().limits().extent()))
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
              else if ((right != nullptr) &&
                       ((*right)().state() == State::dirty) &&
                       ((current().footprint() - initial +
                         (*right)().footprint()) <
                        nodule.nest().limits().extent()))
                {
                  //
                  // this case is similar to the previous but for the right
                  // neighbour nodule.
                  //
                  goto _merge_right;
                }
              else if ((left != nullptr) &&
                       (((*left)().footprint() + current().footprint() -
                         initial) <
                        nodule.nest().limits().extent()))
                {
                  //
                  // in this case the left neighbour nodule seems to be present.
                  //
                _merge_left:

                  ELLE_TRACE("merging with the left neighbour");

                  // merge the nodule.
                  (*left)().merge(current.handle());

                  // update the inlet referencing left.
                  typename X::I* il = previous->second;
                  il->capacity((*left)().capacity());
                  il->state((*left)().state());

                  // at this point, current is empty and unreferenced.
                  // therefore, detach the block from the porcupine.
                  //
                  // note that the block is finally unloaded at the end of
                  // this method.
                  nodule.nest().detach(current.handle());

                  // unload the current nodule.
                  current.unload();

                  // delete the entry from the current seam.
                  //
                  // note that this must be done after the handle has been
                  // used for both detaching the block and unloading the handle.
                  nodule.erase(iterator);

                  // finally, update the current seam so as to reference the new
                  // mayor key for the left nodule since it has received
                  // all the entries of the current child.
                  nodule.refresh(previous, (*left)().mayor());
                }
              else if ((right != nullptr) &&
                       ((current().footprint() - initial +
                         (*right)().footprint()) <
                        nodule.nest().limits().extent()))
                {
                  //
                  // this case is identical to the previous one except that
                  // merging occurs between the current and right nodules.
                  //
                _merge_right:

                  ELLE_TRACE("merging with the right neighbour");

                  // merge the nodule.
                  (*right)().merge(current.handle());

                  // update the inlet referencing right.
                  typename X::I* il = next->second;
                  il->capacity((*right)().capacity());
                  il->state((*right)().state());

                  // at this point, current is empty and unreferenced.
                  // therefore, detach the block from the porcupine.
                  //
                  // note that the block is finally unloaded at the end of
                  // this method.
                  nodule.nest().detach(current.handle());

                  // unload the current nodule.
                  current.unload();

                  // delete the entry from the current seam.
                  //
                  // note that this must be done after the handle has been
                  // used for both detaching the block and unloading the handle.
                  nodule.erase(iterator);

                  // note that the current seam does not need to be updated
                  // regarding the new inlets merged into the right nodule
                  // because the added inlets have lower keys than the already
                  // existing ones in the right nodule.
                }
              else
                {
                  //
                  // in this case, the nodule is small enough for being merged
                  // or balanced but, unfortunately, the neighbours nodules
                  // are not in a state to accept such operations or do not
                  // belong to the same seam.
                  //
                  // therefore, the nodule is left as such, taking care however
                  // to update the current seam for consistency.
                  //

                  // update the current seam so as to reference the new
                  // mayor key.
                  nodule.refresh(iterator, current().mayor());

                  // unload the current nodule.
                  current.unload();
                }

              //
              // unload the left/right nodules, if necessary.
              //

              if (left != nullptr)
                (*left).unload();

              if (right != nullptr)
                (*right).unload();

              delete left;
              delete right;
              ELLE_FINALLY_ABORT(left);
              ELLE_FINALLY_ABORT(right);
            }
        }
      else
        current.unload();
    }

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Nodule<T>::Nodule(Type const type):
      _type(type),
      _capacity(0)
    {
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the nodule.
    ///
    template <typename T>
    elle::Status
    Nodule<T>::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Nodule] " << this->_type << std::endl;

      if (Node::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the node");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity] " << this->_capacity << std::endl;

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Nodule<T>::print(std::ostream& stream) const
    {
      stream << "nodule[" << this->_type << "](" << this->_capacity << ")";
    }
  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE_T1(nucleus::proton::Nodule,
                         archive,
                         value,
                         version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::Node>(value);

  archive & value._capacity;
}

#endif
