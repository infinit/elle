#ifndef NUCLEUS_PROTON_TREE_HXX
# define NUCLEUS_PROTON_TREE_HXX

# include <nucleus/proton/Nest.hh>
# include <nucleus/proton/Seam.hh>
# include <nucleus/proton/Quill.hh>
# include <nucleus/proton/Contents.hh>

# include <cryptography/SecretKey.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Tree<T>::Tree(Nest& nest):
      _root(new Handle{nest.attach(new Contents(new Quill<T>))}),
      _height(1),
      _capacity(0),
      _nest(nest),
      _state(State::dirty)
    {
    }

    template <typename T>
    Tree<T>::Tree(Root const& root,
                  cryptography::SecretKey const& secret,
                  Nest& nest):
      _root(new Handle{root.address(), secret}),
      _height(root.height()),
      _capacity(root.capacity()),
      _nest(nest),
      _state(State::clean)
    {
    }

    template <typename T>
    Tree<T>::~Tree()
    {
      delete this->_root;
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    elle::Boolean
    Tree<T>::exist(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      Handle handle(this->lookup(k));
      Ambit<T> value(this->_nest, handle);

      value.load();

      if (value().exist(k) == false)
        return (false);

      value.unload();

      return (true);
    }

    template <typename T>
    void
    Tree<T>::add(typename T::K const& k,
                 Handle const& v)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k, v);

      // The code below proceeds in inserting the key/value tuple in
      // the hierarchy by calling the virtual method add().
      //
      // Once added, the tree's characteristics are updated. Besides,
      // since the tree's configuration has changed, the tree is optimised,
      // operation which could result in nodules being split etc.

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      root().add(k, v);

      this->_capacity = root().capacity();
      this->_state = root().state();

      root.unload();

      // Try to optimize the tree.
      this->_optimize();
    }

    template <typename T>
    void
    Tree<T>::remove(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Recursively remove the value from the root.
      root().remove(k);

      // Update the capacity and state.
      this->_capacity = root().capacity();
      this->_state = root().state();

      root.unload();

      // Try to optimize the tree.
      this->_optimize();
    }

    template <typename T>
    Handle
    Tree<T>::lookup(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      // Search for the quill responsible for the given key.
      Handle handle{this->_search(k)};

      Ambit<Quill<T>> quill(this->_nest, handle);

      quill.load();

      // Retrieve the handle of the looked up value.
      Handle v{quill().lookup_handle(k)};

      quill.unload();

      return (v);
    }

    template <typename T>
    std::pair<Handle, Capacity>
    Tree<T>::seek(Capacity const target)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(target);

      // XXX[check sur target >= capacity -> error]

      // Initialize the base.
      Capacity base(0);

      ELLE_ASSERT(this->_root != nullptr);
      ELLE_ASSERT(target < this->_capacity);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Seek down the tree.
      Handle v{root().seek(target, base)};

      root.unload();

      return (std::pair<Handle, Capacity>(v, base));
    }

    template <typename T>
    void
    Tree<T>::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Recursively update the tree from the given key.
      root().update(k);

      // Update the capacity and state.
      this->_capacity = root().capacity();
      this->_state = root().state();

      root.unload();

      // Try to optimize the tree.
      this->_optimize();
    }

    template <typename T>
    Handle
    Tree<T>::head()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_root != nullptr);
      ELLE_ASSERT(this->_capacity != 0);

      return (this->seek(0).first);
    }

    template <typename T>
    Handle
    Tree<T>::tail()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_root != nullptr);
      ELLE_ASSERT(this->_capacity != 0);

      return (this->seek(this->_capacity - 1).first);
    }

    template <typename T>
    void
    Tree<T>::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(flags);

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Check the address.
      if (flags & flags::address)
        {
          ELLE_DEBUG_SCOPE("checking addresses");

          // Compute the address of the root block.
          Address address{root.contents().bind()};

          // Compare this address with the one recorded as reference.
          if (this->_root->address() != address)
            throw Exception("invalid address: root(%s) versus bind(%s)",
                            this->_root->address(), address);
        }

      // Recursively check the tree's validity.
      if (flags & flags::recursive)
        root().check(flags);

      // Check the capacity.
      if (flags & flags::capacity)
        {
          ELLE_DEBUG_SCOPE("checking capacities");

          if (this->_capacity != root().capacity())
            throw Exception("invalid capacity: this(%s) versus root(%s)",
                            this->_capacity, root().capacity());
        }

      // Check the footprint.
      if (flags & flags::footprint)
        {
          ELLE_DEBUG_SCOPE("checking footprints");

          if (root().footprint() == 0)
            throw Exception("the footprint is null");

          if (root().footprint() != elle::serialize::footprint(root()))
            throw Exception("the recorded footprint does not match the "
                            "instance's: root(%s) versus footprint(%s)",
                            root().footprint(),
                            elle::serialize::footprint(root()));

          if (root().footprint() > this->_nest.limits().extent())
            throw Exception("the footprint '%s' exceeds the extent '%s'",
                            root().footprint(),
                            this->_nest.limits().extent());
        }

      // Check the state.
      if (flags & flags::state)
        {
          ELLE_DEBUG_SCOPE("checking states");

          if (this->_state != root().state())
            throw Exception("invalid state: this(%s) versus root(%s)",
                            this->_state, root().state());
        }

      root.unload();
    }

    template <typename T>
    Statistics
    Tree<T>::statistics()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      Statistics stats;

      // Recursively collect statistics from the tree.
      root().statistics(stats);

      root.unload();

      return (stats);
    }

    template <typename T>
    void
    Tree<T>::dump(elle::Natural32 const margin)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(margin);

      elle::String alignment(margin, ' ');
      elle::String shift(2, ' ');

      ELLE_ASSERT(this->_root != nullptr);

      std::cout << alignment << "[Tree] "
                << "height(" << this->_height << ") "
                << "capacity(" << this->_capacity << ") "
                << "root(" << *this->_root << ")" << std::endl;

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Recursively dump the tree.
      root().dump(margin + 2);

      root.unload();
    }

    template <typename T>
    Root
    Tree<T>::seal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(secret);

      switch (this->_state)
        {
        case State::clean:
          throw Exception("unable to seal a clean porcupine");
        case State::dirty:
          {
            ELLE_ASSERT(this->_root != nullptr);

            Ambit<Nodule<T>> root(this->_nest, *this->_root);

            root.load();

            // Recursively seal the tree blocks.
            root().seal(secret);

            // Encrypt and bind the root block.
            root.contents().encrypt(secret);
            Address address{root.contents().bind()};

            // Update the node and block.
            root().state(State::consistent);
            root.contents().state(State::consistent);

            // Update the tree state.
            this->_state = root().state();

            root.unload();

            // Reset the handle with the new address and secret.
            this->_root->reset(address, secret);

            // Return the type-independent tree's root.
            return (Root(this->_root->address(),
                         this->_height,
                         this->_capacity));
          }
        case State::consistent:
          throw Exception("unable to seal a consistent porcupine");
        default:
          throw Exception("unknown state: '%s'",
                          static_cast<int>(this->_state));
        }
    }

    template <typename T>
    void
    Tree<T>::destroy()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Recursively destroy the blocks.
      root().destroy();

      // Detach the root nodule from the nest.
      this->_nest.detach(root.handle());

      root.unload();

      delete this->_root;
      this->_root = nullptr;
    }

    template <typename T>
    Handle const&
    Tree<T>::root() const
    {
      ELLE_ASSERT(this->_root != nullptr);

      return (*this->_root);
    }

    template <typename T>
    Handle&
    Tree<T>::root()
    {
      ELLE_ASSERT(this->_root != nullptr);

      return (*this->_root);
    }

    template <typename T>
    Handle
    Tree<T>::_search(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // If the root nodule is a quill, the job is over. Otherwise,
      // explore the tree.
      if (root().type() == Nodule<T>::Type::quill)
        {
          root.unload();

          return (*this->_root);
        }
      else
        {
          // Recursively search for the key until a quill is hit.
          Handle v{root().search(k)};

          root.unload();

          return (v);
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Tree<T>::_optimize()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT(this->_root != nullptr);

      Ambit<Nodule<T>> root(this->_nest, *this->_root);

      root.load();

      // Check if the root nodule's footprint exceeds the limit.
      if ((root().footprint()) > this->_nest.limits().extent())
        {
          ELLE_TRACE("root nodule extent hight limit reached: %s > %s",
                     root().footprint(),
                     this->_nest.limits().extent());

          // In this case, the root nodule needs to be split creating a
          // brother nodule for which a parent nodule (the future root)
          // will have to be created.

          // Split the root nodule.
          Handle orphan{root().split()};

          // At this point, the tree needs to grow in order to welcome
          // the new right nodule.

          // Retrieve the mayor key of the current root nodule.
          //
          // This key will be used for inserting the current root nodule in
          // the new root.
          typename T::K mayor_root{root().mayor()};

          // Retrieve the current root nodule's capacity and state.
          Capacity capacity_root{root().capacity()};
          State state_root{root().state()};

          // Unload the root nodule so as insert it in the new root.
          root.unload();

          // Load the new right nodule (result of the split) so as
          // to retrieve information for inserting it in the new root
          // nodule.
          Ambit<Nodule<T>> newright(this->_nest, orphan);

          newright.load();

          // Retrieve the mayor key, capacity and state of the _newright_
          // nodule.
          typename T::K mayor_newright{newright().mayor()};
          Capacity capacity_newright{newright().capacity()};
          State state_newright{newright().state()};

          newright.unload();

          // At this point, a new root nodule must be created so as to
          // hold its two future children.

          // Save the new _newright_ handle.
          Handle handle_newright{orphan};

          // Copy the current root handle so that no ambit references
          // _this->_root_ since this variable is going to change.
          Handle handle_root{*this->_root};

          // Allocate a new root block: a seam since there was already
          // a root nodule i.e either a quill or a seam.
          Contents* contents(new Contents(new Seam<T>));

          ELLE_FINALLY_ACTION_DELETE(contents);

          // Attach the block to the nest.
          Handle handle_newroot{this->_nest.attach(contents)};

          ELLE_FINALLY_ABORT(contents);

          // Let us now proceed to the insertion of the two child
          // nodules in the new root.

          Ambit<Seam<T>> newroot(this->_nest, handle_newroot);

          newroot.load();

          // The child nodules are manually inserted into the seam
          // so as to optimize the number of calls.

          // Instanciate a new seam inlet, set its capacity and state
          // manually before inserting it the new root.
          typename Seam<T>::I* inlet_left =
            new typename Seam<T>::I{mayor_root, handle_root};

          ELLE_FINALLY_ACTION_DELETE(inlet_left);

          inlet_left->capacity(capacity_root);
          inlet_left->state(state_root);

          newroot().insert(inlet_left);

          ELLE_FINALLY_ABORT(inlet_left);

          // Do the same for the right inlet i.e the split _newright_ nodule.
          typename Seam<T>::I* inlet_right =
            new typename Seam<T>::I{mayor_newright, handle_newright};

          ELLE_FINALLY_ACTION_DELETE(inlet_right);

          inlet_right->capacity(capacity_newright);
          inlet_right->state(state_newright);

          newroot().insert(inlet_right);

          ELLE_FINALLY_ABORT(inlet_right);

          // Compute the capacity of the new root seam. Note that
          // the state does not need to be set explicitely because
          // the insert() methods took care of updating the nodule's
          // state to dirty.
          newroot().capacity(capacity_root + capacity_newright);

          // Update the capacity and state of the tree.
          this->_capacity = newroot().capacity();
          this->_state = newroot().state();

          newroot.unload();

          // Finally, assign the new root handle as the root.
          delete this->_root;
          this->_root = new Handle{handle_newroot};

          // And increment the height.
          this->_height++;

          // At this point, the freshly split _newright_ nodule may need
          // to be optimized further.
          Ambit<Seam<T>> _newroot(this->_nest, handle_newroot);

          _newroot.load();

          Nodule<T>::optimize(_newroot(), mayor_newright);

          _newroot.unload();

          // Finally, following the optimization of the _newright_ nodule
          // which may have been split several times, the new root may
          // now need to be optimized as well.
          this->_optimize();
        }
      else
        {
          ELLE_ASSERT(this->_height != 0);

          // Try to reduce the tree's height.
          if (this->_height > 1)
            {
              ELLE_TRACE("the tree contains several levels of hierarchy");

              // Start by unloading the root nodule so as to reload it as
              // a seam. Indeed, since the tree's height is higher than 1,
              // the root nodule must obviously be a seam.
              root.unload();

              Ambit<Seam<T>> seam(this->_nest, *this->_root);

              seam.load();

              // Check whether the root seam contains a single entry in which
              // case this entry could become the new root.
              if (seam().single() == true)
                {
                  ELLE_TRACE("the root seam contains a single entry");

                  // Retrieve the handle associated with the maiden key.
                  Handle orphan{seam().locate_handle(seam().maiden())};

                  // Detach the root nodule from the porcupine.
                  this->_nest.detach(*this->_root);

                  seam.unload();

                  // Set the new tree's root handle as being the one of the
                  // block which happens to be alone.
                  delete this->_root;
                  this->_root = new Handle{orphan};

                  // Decrease the tree's height.
                  this->_height--;

                  ELLE_TRACE("try to optimize further the tree");

                  // Note that it may be possible to optimize the tree further.
                  // For example, the tree, following a removal for instance,
                  // may have lost a block on every level of its hierarchy,
                  // leaving every level with a single inlet.
                  //
                  // Although the tree has been shrunk by one level, the new
                  // root may in turn have a single inlet and could therefore
                  // be optimized as well.
                  //
                  // The most extreme case could led to successive shrinking
                  // operations leaving the tree with a single block.
                  this->_optimize();
                }
              else
                seam.unload();
            }
          else
            {
              ELLE_TRACE("the tree contains a single level of hierarchy and "
                         "cannot be optimized further");

              root.unload();
            }
        }
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Tree<T>::print(std::ostream& stream) const
    {
      ELLE_ASSERT(this->_root != nullptr);

      stream << "("
             << *this->_root
             << ", "
             << this->_height
             << ", "
             << this->_capacity
             << ")";
    }
  }
}

#endif
