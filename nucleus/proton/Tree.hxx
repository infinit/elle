#ifndef NUCLEUS_PROTON_TREE_HXX
# define NUCLEUS_PROTON_TREE_HXX

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
      _height(0),
      _capacity(0),
      _root(nest.attach(new Contents(new Quill<T>))),
      _nest(nest),
      _state(State::dirty)
    {
    }

    template <typename T>
    Tree<T>::Tree(Root const& root,
                  cryptography::SecretKey const& secret,
                  Nest& nest):
      _height(root.height()),
      _capacity(root.capacity()),
      _root(root.block(), secret),
      _nest(nest),
      _state(State::clean)
    {
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

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      root().add(k, v);

      this->_capacity = root().capacity();
      this->_state = root().state();

      root.unload();

      // Try to optimize the tree.
      this->optimize();
    }

    template <typename T>
    Tree<T>::remove(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      // Recursively remove the value from the root.
      root().remove(k);

      // Update the capacity and state.
      this->_capacity = root().capacity();
      this->_state = root().state();

      root.unload();

      // Try to optimize the tree.
      this->optimize();
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

      // Initialize the base.
      Capacity base(0);

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      // Seek down the tree.
      Handle v{root().seek(target, base)};

      root.unload();

      return (v, base);
    }

    template <typename T>
    void
    Tree<T>::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      // Recursively update the tree from the given key.
      root().update(k);

      // Update the capacity and state.
      this->_capacity = root().capacity();
      this->_state = root().state();

      root.unload();

      // Try to optimize the tree.
      this->optimize();
    }

    template <typename T>
    void
    Tree<T>::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(flags);

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      // Check the address.
      if (flags & flags::address)
        {
          ELLE_DEBUG_SCOPE("checking addresses");

          // Compute the address of the root block.
          Address address{root.contents().bind()};

          // Compare this address with the one recorded as reference.
          if (this->_root.address() != address)
            throw Exception("invalid address: root(%s) versus bind(%s)",
                            this->_root.address(), address);
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

      Ambit<Nodule<T>> root(this->_nest, this->_root);

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

      std::cout << alignment << "[Tree] "
                << "height(" << this->_height << ") "
                << "capacity(" << this->_capacity << ") "
                << "root(" << this->_root << ")" << std::endl;

      Ambit<Nodule<T>> root(this->_nest, this->_root);

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

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      // If the tree is dirty, so should be the root.
      ELLE_ASSERT(this->_state == root().state());

      // Recursively seal the tree blocks.
      root().seal(secret);

      // Encrypt and bind the root block.
      root.contents().encrypt(secret);
      Address address{root.contents().bind()};

      // Update the node and block.
      root().state(State::consistent);
      root.contents().state(State::consistent);

      root.unload();

      // Update the tree state.
      this->_state = State::consistent;

      // Return the type-independent tree's root.
      return (Root(this->_root.address(), this->_height, this->_capacity));
    }

    template <typename T>
    Handle
    Tree<T>::_search(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Tree");
      ELLE_TRACE_METHOD(k);

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      root.load();

      // If the root nodule is a quill, the job is over. Otherwise,
      // explore the tree.
      if (root().type() == Nodule<T>::Type::quill)
        {
          root.unload();

          return (this->_root);
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
  }
}

#endif
