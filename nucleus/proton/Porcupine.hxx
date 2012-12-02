#ifndef NUCLEUS_PROTON_PORCUPINE_HXX
# define NUCLEUS_PROTON_PORCUPINE_HXX

# include <nucleus/Exception.hh>
# include <nucleus/proton/Ambit.hh>
# include <nucleus/proton/Seam.hh>
# include <nucleus/proton/Quill.hh>
# include <nucleus/proton/Nest.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/proton/Statistics.hh>

# include <elle/assert.hh>
# include <elle/log.hh>

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Constants |
    `----------*/

    template <typename T>
    const elle::Natural32 Porcupine<T>::secret_length = 256;

    /*-----------------.
    | Static Functions |
    `-----------------*/

    template <typename T>
    cryptography::SecretKey
    Porcupine<T>::secret()
    {
      // XXX[use a keylength function of T]
      static cryptography::SecretKey secret{
        cryptography::SecretKey::generate(Porcupine<T>::secret_length)};

      return (secret);
    }

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Porcupine<T>::Porcupine(/* XXX Network const& network,
                               cryptography::PublicKey const& agent_K,*/
                            Radix const& radix,
                            Nest& nest):
      _mode(radix.mode()),
      _value(nullptr),
      /* XXX
      _network(network),
      _agent_K(agent_K),
      */
      _nest(nest)
    {
      switch (this->_mode)
        {
        case Mode::empty:
          {
            break;
          }
        case Mode::value:
          {
            this->_value = &radix.value();

            break;
          }
        case Mode::block:
          {
            this->_block = new Handle{radix.block()};

            break;
          }
        case Mode::tree:
          {
            this->_tree = new Tree{radix.tree(), this->_nest};

            break;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }
    }


    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    elle::Boolean
    Porcupine<T>::empty() const
    {
      return (this->size() == 0);
    }

    template <typename T>
    elle::Boolean
    Porcupine<T>::exist(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            return (false);
          }
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (this->_value->exist(k));
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            if (value().exist(k) == false)
              return (false);

            value.unload();

            return (true);
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            return (this->_tree->exist(k));
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreachable();
    }

    template <typename T>
    Door<T>
    Porcupine<T>::lookup(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("mode: %s", this->_mode);

      // Note that this method assumes that a single value or block
      // is responsible for any valid key _k_.

      switch (this->_mode)
        {
        case Mode::empty:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup in a value.
            ELLE_ASSERT(this->_mode == Mode::block);
          }
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (Door<T>(this->_value));
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            return (Door<T>(*this->_block, this->_nest));
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup in the tree and transform the returned handle into a door.
            return (Door<T>(this->_tree->lookup(k), this->_nest));
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreachable();
    }

    template <typename T>
    std::pair<Door<T>, Capacity>
    Porcupine<T>::seek(Capacity const target)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(target);

      // Note that this method assumes that a single value or block
      // is responsible for any valid key _k_.

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup.
            ELLE_ASSERT(this->_mode == Mode::block);
          }
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Verify the validity of the target index.
            if (target >= this->_value->capacity())
              throw Exception("the given target index '%s' exceeds the "
                              "capacity '%s' of the value",
                              target, this->_value->capacity());

            // Return a door to the value along with a capacity index of
            // zero since this is the first and only value.
            return (std::pair<Door<T>, Capacity>{Door<T>(this->_value), 0});
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            // Verify the validity of the target index.
            if (target >= value()->capacity())
              throw Exception("the given target index '%s' exceeds the "
                              "capacity '%s' of the value",
                              target, value()->capacity());

            value.unload();

            // Return a door to the value block along with a capacity index of
            // zero since this is the first and only value.
            return (std::pair<Door<T>, Capacity>{
              Door<T>(*this->_block, this->_nest), 0});
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Seek the target in the tree.
            auto pair = this->_tree->seek(target);

            // Return a door to the value block responsible for the given
            // capacity index along with the base capacity index of this value.
            return (std::pair<Door<T>, Capacity>{
              Door<T>(pair.first, this->_nest), pair.second});
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreachable();
    }

    template <typename T>
    template <typename >
    void
    Porcupine<T>::insert(typename T::K const& k,
                         E* e)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k, e);

      ELLE_ASSERT(e != nullptr);

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup.
            ELLE_ASSERT(this->_mode == Mode::block);
          }
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->insert(k, e);

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            value()->insert(k, e);

            value.unload();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup the handle of the value block responsible for the key.
            Handle handle{this->_tree->lookup(k)};

            Ambit<T> value(this->_nest, handle);

            value.load();

            // Insert the tuple key/element in the value block.
            value()->insert(k, e);

            value.unload();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::erase(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          throw Exception("unable to erase the '%s' element from an empty "
                          "porcupine", k);
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->erase(k, e);

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            value()->erase(k);

            value.unload();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup the handle of the value block responsible for the key.
            Handle handle{this->_tree->lookup(k)};

            Ambit<T> value(this->_nest, handle);

            value.load();

            // Erase the tuple key/element from the value block.
            value()->erase(k);

            value.unload();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          throw Exception("unable to update an empty porcupine");
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Update the porcupine attribtes.
            this->_state = this->_value->state();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            // Update the capacity state.
            this->_state = value().state();

            value.unload();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Update the tree.
            this->_tree->update(k);

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreacable();
    }

    template <typename T>
    elle::Size
    Porcupine<T>::size() const
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            return (0);
          }
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (static_cast<elle::Size>(this->_value->size()));
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            elle::Size size{static_cast<elle::Size>(value()->size())};

            value.unload();

            return (size);
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            elle::Size size{static_cast<elle::Size>(this->_tree->capacity())};

            return (size);
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      elle::unreacable();
    }

    template <typename T>
    void
    Porcupine<T>::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(flags);

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            break;
          }
        case Mode::block:
          {
            Ambit<T> value(this->_nest, this->_root);

            // load the value nodule.
            value.load();

            // check the address, if required.
            if (flags & flags::address)
              {
                // bind the current block.
                Address address{value.contents().bind()};

                // compare the addresses.
                if (this->_root.address() != address)
                  throw Exception("invalid address: root(%s) versus bind(%s)",
                                  this->_root.address(), address);
              }

            // check the capacity.
            if (flags & flags::capacity)
              {
                if (this->_capacity != value().capacity())
                  throw Exception("invalid capacity: this(%s) versus value(%s)",
                                  this->_capacity, value().capacity());
              }

            // check the footprint.
            if (flags & flags::footprint)
              {
                if (value().footprint() == 0)
                  throw Exception("the footprint is null");

                if (value().footprint() != elle::serialize::footprint(value()))
                  throw Exception("the recorded footprint does not match the "
                                  "instance's: value(%s) versus footprint(%s)",
                                  value().footprint(),
                                  elle::serialize::footprint(value()));

                if (value().footprint() > this->_nest.limits().extent())
                  throw Exception("the footprint '%s' exceeds the extent '%s'",
                                  value().footprint(),
                                  this->_nest.limits().extent());
              }

            // unload the value nodule.
            value.unload();

            break;
          }
        case Mode::tree:
          {
            Ambit<Nodule<T>> root(this->_nest, this->_root);

            // load the root nodule.
            root.load();

            // check the address, if required.
            if (flags & flags::address)
              {
                ELLE_DEBUG_SCOPE("checking addresses");

                // bind the current block.
                Address address{root.contents().bind()};

                // compare the addresses.
                if (this->_root.address() != address)
                  throw Exception("invalid address: root(%s) versus bind(%s)",
                                  this->_root.address(), address);
              }

            // trigger the check method on the root nodule.
            if (flags & flags::recursive)
              root().check(flags);

            // check the capacity, if required.
            if (flags & flags::capacity)
              {
                ELLE_DEBUG_SCOPE("checking capacities");

                if (this->_capacity != root().capacity())
                  throw Exception("invalid capacity: this(%s) versus root(%s)",
                                  this->_capacity, root().capacity());
              }

            // check the footprint.
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

            // check the state.
            if (flags & flags::state)
              {
                ELLE_DEBUG_SCOPE("checking states");

                if (this->_state != root().state())
                  throw Exception("invalid state: this(%s) versus root(%s)",
                                  this->_state, root().state());
              }

            // unload the root nodule.
            root.unload();

            break;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }
    }

    template <typename T>
    void
    Porcupine<T>::walk(elle::Natural32 const margin)
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Porcupine]" << std::endl;

      // dump the mode.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Mode] " << std::dec << this->_mode << std::endl;

      // dump the height.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Height] " << std::dec << this->_height << std::endl;

      // dump the capacity.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity] " << std::dec << this->_capacity << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_state << std::endl;

      // proceed if there is a root block.
      if (this->_root != Handle::Null)
        {
          // dump the value.
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Root]" << std::endl;

          switch (this->_mode)
            {
            case Mode::empty:
              {
                break;
              }
            case Mode::block:
              {
                Ambit<T> value(this->_nest, this->_root);

                // dump the value.
                if (this->_root.Dump(margin + 4) == elle::Status::Error)
                  throw Exception("unable to dump the handle");

                // load the value nodule.
                value.load();

                // dump the value.
                if (value.contents().Dump(margin + 4) == elle::Status::Error)
                  throw Exception("unable to dump the value");

                // unload the value nodule.
                value.unload();

                break;
              }
            case Mode::tree:
              {
                Ambit<Nodule<T>> root(this->_nest, this->_root);

                // dump the root.
                if (this->_root.Dump(margin + 4) == elle::Status::Error)
                  throw Exception("unable to dump the handle");

                // load the root nodule.
                root.load();

                // walk through the nodule.
                root().walk(margin + 4);

                // unload the root nodule.
                root.unload();

                break;
              }
            default:
              throw Exception("unknown mode: '%s'",
                              static_cast<int>(this->_mode));
            }
        }
      else
        {
          std::cout << alignment << elle::io::Dumpable::Shift
                    << "[Root] " << elle::none << std::endl;
        }
    }

    /// XXX
    /// this method goes through the porcupine in order to seal the
    /// blocks having been either created or modified.
    ///
    /// once complete, the porcupine should be consistent so for its
    /// constituent blocks to be stored.
    ///
    template <typename T>
    void
    Porcupine<T>::seal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(secret);

      ELLE_TRACE("state: %s", this->_state);

      switch (this->_state)
        {
        case State::clean:
          {
            break;
          }
        case State::dirty:
          {
            ELLE_TRACE("mode: %s", this->_mode);

            switch (this->_mode)
              {
              case Mode::empty:
                {
                  throw Exception("unable to seal an empty porcupine");
                }
              case Mode::block:
                {
                  Ambit<T> value(this->_nest, this->_root);

                  // load the value nodule.
                  value.load();

                  // if the porcupine is dirty, so should be the value.
                  ELLE_ASSERT(this->_state == value().state());

                  // Encrypt and bind the value.
                  value.contents().encrypt(secret);
                  Address address{value.contents().bind()};

                  value().state(State::consistent);
                  value.contents().state(State::consistent);

                  // unload the value nodule.
                  value.unload();

                  this->_root.address(address);
                  this->_state = State::consistent;

                  break;
                }
              case Mode::tree:
                {
                  Ambit<Nodule<T>> root(this->_nest, this->_root);

                  // load the root nodule.
                  root.load();

                  // if the porcupine is dirty, so should be the root.
                  ELLE_ASSERT(this->_state == root().state());

                  // seal recursively.
                  root().seal(secret);

                  // Encrypt and bind the root block.
                  root.contents().encrypt(secret);
                  Address address{root.contents().bind()};

                  root().state(State::consistent);
                  root.contents().state(State::consistent);

                  // unload the root nodule.
                  root.unload();

                  this->_root.address(address);
                  this->_state = State::consistent;

                  break;
                }
              default:
                throw Exception("unknown mode: '%s'",
                                static_cast<int>(this->_mode));
              }

            break;
          }
        case State::consistent:
          {
            throw Exception("unable to seal a consistent porcupine");
          }
        default:
          throw Exception("unknown state: '%s'",
                          static_cast<int>(this->_state));
        }

      // set the secret key as null.
      this->_root.secret(cryptography::SecretKey::Null);
    }

    template <typename T>
    void
    Porcupine<T>::unseal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(secret);

      this->_root.secret(secret);
    }

    template <typename T>
    Statistics
    Porcupine<T>::statistics()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      Statistics stats;

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            break;
          }
        case Mode::block:
          {
            Ambit<T> value(this->_nest, this->_root);

            value.load();

            stats.blocks_all(stats.blocks_all() + 1);

            switch (value().state())
              {
              case State::clean:
                {
                  stats.blocks_clean(stats.blocks_clean() + 1);

                  break;
                }
              case State::dirty:
                {
                  stats.blocks_dirty(stats.blocks_dirty() + 1);

                  break;
                }
              case State::consistent:
                {
                  stats.blocks_consistent(stats.blocks_consistent() + 1);

                  break;
                }
              }

            Footprint footprint = elle::serialize::footprint(value());
            Capacity capacity = value().capacity();

            stats.footprint_minimum(
              footprint < stats.footprint_minimum() ?
              footprint : stats.footprint_minimum());
            stats.footprint_average(
              (stats.footprint_average() + footprint) / 2);
            stats.footprint_maximum(
              footprint > stats.footprint_maximum() ?
              footprint : stats.footprint_maximum());

            stats.capacity_minimum(
              capacity < stats.capacity_minimum() ?
              capacity : stats.capacity_minimum());
            stats.capacity_average(
              (stats.capacity_average() + capacity) / 2);
            stats.capacity_maximum(
              capacity > stats.capacity_maximum() ?
              capacity : stats.capacity_maximum());

            value.unload();

            break;
          }
        case Mode::tree:
          {
            Ambit<Nodule<T>> root(this->_nest, this->_root);

            // load the root nodule.
            root.load();

            root().statistics(stats);

            // unload the root nodule.
            root.unload();

            break;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }

      return (stats);
    }

    template <typename T>
    void
    Porcupine<T>::_create()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      // Make sure the porcupine is empty.
      ELLE_ASSERT(this->_mode == Mode::empty);

      // Create a value and set it as the value on which the porcupine
      // is operating.

      ELLE_ASSERT(this->_value == nullptr);
      this->_value = new T;

      // Set the porcupine's state and mode.
      this->_state = State::dirty;
      this->_mode = Mode::value;
    }

    template <typename T>
    void
    Porcupine<T>::_optimize()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("mode: %s", this->_mode);

      switch (this->_mode)
        {
        case Mode::empty:
          {
            throw Exception("unable to optimize an empty porcupine");
          }
        case Mode::block:
          {
            Ambit<T> value(this->_nest, this->_root);

            // load the value nodule.
            value.load();

            //
            // XXX
            //
            if ((value().footprint()) > this->_nest.limits().extent())
              {
                ELLE_TRACE("value's extent high limit reached: "
                           "%s > %s",
                           value().footprint(),
                           this->_nest.limits().extent());

                //
                // in this case, the value needs to be split.
                //
                // in order to re-use the code as much as possible, a quill
                // is created in which the value is inserted. then the
                // quill-specific Optimize() method is called so as to
                // split the values.
                //
                typename T::K mayor;

                // retrieve the mayor of the value.
                mayor = value().mayor();

                // unload the value.
                value.unload();

                // allocate a new root quill.
                std::unique_ptr<Contents> contents(new Contents(new Quill<T>));
                Handle orphan(this->_nest.attach(std::move(contents)));
                Ambit<Quill<T>> newroot(this->_nest, orphan);

                // load the newroot.
                newroot.load();

                // add the current value to the new quill root.
                //
                // note that calling add() is valid in this case because
                // _newroot_ is a quill i.e the last nodule of the hierarchy.
                newroot().add(mayor, this->_root);

                // now that the tree is valid, call the Optimize() method because
                // the added value is still too large to remain as such and
                // therefore needs to be split.
                Nodule<T>::optimize(newroot(), mayor);

                // update the capacity and state.
                this->_capacity = newroot().capacity();
                this->_state = newroot().state();

                // unload the newroot.
                newroot.unload();

                // assign the new root handle.
                this->_root = orphan;

                // increment the height.
                this->_height++;

                // set the mode.
                this->_mode = Mode::tree;
              }
            else if (value().footprint() <
                     (this->_nest.limits().extent() *
                      this->_nest.limits().balancing()))
              {
                ELLE_TRACE("value's extent low limit reached: "
                           "%s < %s",
                           value().footprint(),
                           this->_nest.limits().extent() *
                           this->_nest.limits().balancing());

                // XXX
                if (value().empty() == true)
                  {
                    ELLE_TRACE("the value is empty");

                    value.unload();

                    // detache the value block.
                    this->_nest.detach(this->_root);

                    // reset the root.
                    this->_root = Handle::Null;

                    // make sure the tree is empty.
                    ELLE_ASSERT(this->_capacity == 0);

                    // update the state.
                    this->_state = State::dirty;

                    // set the mode.
                    this->_mode = Mode::empty;
                  }
                else
                  value.unload();
              }
            else
              value.unload();

            break;
          }
        case Mode::tree:
          {
            Ambit<Nodule<T>> root(this->_nest, this->_root);

            // load the root nodule.
            root.load();

            ELLE_TRACE("considering the root nodule's footprint: "
                       "%s < %s",
                       root().footprint(),
                       this->_nest.limits().extent());

            //
            // at this point, check whether the add() produced the a nodule
            // whose size exceed the extent.
            //
            // should this happen, the root block is split, creating a new
            // block. thus, a new root block is created in which both nodules
            // are inserted.
            //
            // check if the future nodule's footprint---i.e should the inlet
            // be inserted---would exceed the extent.
            //
            if ((root().footprint()) > this->_nest.limits().extent())
              {
                ELLE_TRACE("root's extent hight limit reached: "
                           "%s > %s",
                           root().footprint(),
                           this->_nest.limits().extent());

                //
                // in this case, the nodule needs to be split.
                //
                Handle orphan;

                // split the current root nodule.
                orphan = root().split();

                //
                // at this point, the tree needs to grow in order to welcome
                // the new right nodule.
                //
                struct
                {
                  typename T::K root;
                  typename T::K newright;
                } mayor;
                struct
                {
                  Capacity root;
                  Capacity newright;
                } capacity;
                struct
                {
                  State root;
                  State  newright;
                } state;

                // retrieve the mayor key of the current root.
                mayor.root = root().mayor();

                // retrieve the capacity and state.
                capacity.root = root().capacity();
                state.root = root().state();

                // unload the root nodule.
                root.unload();

                Ambit<Nodule<T>> newright(this->_nest, orphan);

                // load the new right nodule.
                newright.load();

                // retrieve the mayor key of the newright nodule as this will
                // be required for the future insertion in the current seam.
                mayor.newright = newright().mayor();

                // retrieve the capacity and state.
                capacity.newright = newright().capacity();
                state.newright = newright().state();

                // unload the new right nodule.
                newright.unload();

                //
                // at this point, the new i.e orphan nodule must be inserted
                // into the current seam.
                //
                struct
                {
                  Handle root;
                  Handle newroot;
                  Handle newright;
                } handle;

                // keep the new right handle.
                handle.newright = orphan;

                // copy the current root handle so that no ambit reference
                // _this->_root_ since this variable is going to change.
                handle.root = this->_root;

                // allocate a new root seam.
                std::unique_ptr<Contents> contents(new Contents(new Seam<T>));

                // attach the block to the porcupine.
                handle.newroot = this->_nest.attach(std::move(contents));

                Ambit<Seam<T>> newroot(this->_nest, handle.newroot);

                // load the newroot.
                newroot.load();

                // below are manually inserted the nodules because add()
                // would perform the operation recursively.

                // insert the current root nodule in the new root seam.
                typename Seam<T>::I* inlet;

                newroot().insert(mayor.root, handle.root);
                inlet = newroot().locate_inlet(mayor.root);
                inlet->capacity(capacity.root);
                inlet->state(state.root);

                newroot().insert(mayor.newright, handle.newright);
                inlet = newroot().locate_inlet(mayor.newright);
                inlet->capacity(capacity.newright);
                inlet->state(state.newright);

                // compute the capacity of the new root seam. note that
                // the state does not need to be set explicitely because
                // the insert() methods took care of it.
                newroot().capacity(capacity.root + capacity.newright);

                // update the capacity and state.
                this->_capacity = newroot().capacity();
                this->_state = newroot().state();

                // unload the newroot.
                newroot.unload();

                // assign the new root handle.
                this->_root = handle.newroot;

                // increment the height.
                this->_height++;
              }
            else
              {
                ELLE_TRACE("root's footprint may be low enough to "
                           "reorganize the tree");

                ELLE_ASSERT(this->_height != 0);

                if (this->_height == 1)
                  {
                    ELLE_TRACE("the tree contains a single level");

                    // unload the root nodule.
                    root.unload();

                    Ambit<Quill<T>> quill(this->_nest, this->_root);

                    // load the root nodule.
                    quill.load();

                    // does the quill contain a single value.
                    if (quill().single() == true)
                      {
                        ELLE_TRACE("the root quill contains a single entry");

                        //
                        // if the root nodules contains a single entry, shrink
                        // the tree for this entry to become the new root.
                        //
                        Handle orphan;

                        // retrieve the handle associated with the maiden key.
                        orphan = quill().locate_handle(quill().maiden());

                        // detache the root nodule from the porcupine.
                        this->_nest.detach(quill.handle());

                        // unload the root nodule.
                        quill.unload();

                        // set the new tree's root handle.
                        this->_root = orphan;

                        // decrease the tree's height.
                        this->_height--;

                        // at this point, the tree should have a height of zero.
                        ELLE_ASSERT(this->_height == 0);

                        // therefore, set the mode as value now that the
                        // root handle actually references a value.
                        this->_mode = Mode::block;
                      }
                    else
                      quill.unload();
                  }
                else if (this->_height > 1)
                  {
                    ELLE_TRACE("the tree contains several levels "
                               "of hierarchy.");

                    // unload the root nodule.
                    root.unload();

                    Ambit<Seam<T>> seam(this->_nest, this->_root);

                    // load the root nodule.
                    seam.load();

                    // check whether the root seam contains a single
                    // entry in which case this entry could become
                    // the new root.
                    if (seam().single() == true)
                      {
                        ELLE_TRACE("the root seam contains a single entry.");

                        //
                        // if the root nodules contains a single entry, shrink
                        // the tree for this entry to become the new root.
                        //
                        // XXX here we are sure the root is a seam since height > 1
                        //
                        Handle orphan;

                        // retrieve the handle associated with the maiden key.
                        orphan = seam().locate_handle(seam().maiden());

                        // detache the root nodule from the porcupine.
                        this->_nest.detach(seam.handle());

                        // unload the root nodule.
                        seam.unload();

                        // set the new tree's root handle.
                        this->_root = orphan;

                        // decrease the tree's height.
                        this->_height--;

                        ELLE_TRACE("try to optimize further the tree");

                        this->_optimize();
                      }
                    else
                      seam.unload();
                  }
              }

            break;
          }
        default:
          throw Exception("unknown mode: '%s'", static_cast<int>(this->_mode));
        }
    }

    /// XXX
    /// this method is used for retrieving the handle of the quill
    /// responsible for the given key.
    ///
    /// this method is widely used being for inserting, locating or
    /// deleting entries.
    ///
    template <typename T>
    Handle
    Porcupine<T>::_search(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      Handle v;

      // make sure we are operating in tree mode.
      ELLE_ASSERT(this->_mode == Mode::tree);

      Ambit<Nodule<T>> root(this->_nest, this->_root);

      // load the root nodule.
      root.load();

      // trigger the search method on the root nodule.
      v = root().search(k);

      // unload the root nodule.
      root.unload();

      // first, set the handle as being the root.
      //
      // this is required because, should the root be a quill since the
      // quill search() implementation does nothing.
      //
      // therefore, the whole search() mechanism relies on the fact that
      // the search() method sets the handle temporarily until the child
      // quill does nothing after which the call returns with the temporary
      // handle which happens to reference the quill.
      //
      if (v == Handle::Null)
        v = this->_root;

      return (v);
    }

    /*---------.
    | Dumpable |
    `---------*/

    template <typename T>
    elle::Status
    Porcupine<T>::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');

      std::cout << alignment << "[Porcupine]" << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Radix] " << std::endl;
      this->_radix.Dump(margin + 2);

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_state << std::endl;

      return elle::Status::Ok;
    }

  }
}

#endif
