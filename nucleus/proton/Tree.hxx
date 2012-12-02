#ifndef NUCLEUS_PROTON_TREE_HXX
# define NUCLEUS_PROTON_TREE_HXX

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
                  Nest& nest):
      _height(root.height()),
      _capacity(root.capacity()),
      _root(root.block()),
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

  }
}

#endif
