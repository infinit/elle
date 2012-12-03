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
                            cryptography::SecretKey const& secret,
                            Nest& nest):
      _nature(radix.nature()),
      /* XXX
      _network(network),
      _agent_K(agent_K),
      */
      _nest(nest)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      switch (this->_nature)
        {
        case Nature::empty:
          {
            break;
          }
        case Nature::value:
          {
            ELLE_TRACE("decrypting the radix' cipher with the secret key");

            this->_value = new T;

            if (secret.Decrypt(radix.cipher(),
                               *this->_value) == elle::Status::Error)
              throw Exception("unable to decrypt the cipher");

            break;
          }
        case Nature::block:
          {
            this->_block = new Handle{radix.block(), secret};

            break;
          }
        case Nature::tree:
          {
            this->_tree = new Tree{radix.tree(), secret, this->_nest};

            break;
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }
    }

    template <typename T>
    Porcupine<T>::~Porcupine()
    {
      switch (this->_nature)
        {
        case Nature::empty:
        case Nature::value:
          {
            delete this->_value;

            break;
          }
        case Nature::block:
          {
            delete this->_block;

            break;
          }
        case Nature::tree:
          {
            delete this->_tree;

            break;
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }
    }

    /*--------.
    | Methods |
    `--------*/

    template <typename T>
    elle::Boolean
    Porcupine<T>::empty() const
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      return (this->size() == 0);
    }

    template <typename T>
    elle::Boolean
    Porcupine<T>::exist(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            return (false);
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (this->_value->exist(k));
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            if (value().exist(k) == false)
              return (false);

            value.unload();

            return (true);
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            return (this->_tree->exist(k));
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreachable();
    }

    template <typename T>
    Door<T>
    Porcupine<T>::lookup(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("nature: %s", this->_nature);

      // Note that this method assumes that a single value or block
      // is responsible for any valid key _k_.

      switch (this->_nature)
        {
        case Nature::empty:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup in a value.
            ELLE_ASSERT(this->_nature == Nature::block);
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (Door<T>(this->_value));
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            return (Door<T>(*this->_block, this->_nest));
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup in the tree and transform the returned handle into a door.
            return (Door<T>(this->_tree->lookup(k), this->_nest));
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
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

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup.
            ELLE_ASSERT(this->_nature == Nature::block);
          }
        case Nature::value:
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
        case Nature::block:
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
        case Nature::tree:
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
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
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

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup.
            ELLE_ASSERT(this->_nature == Nature::block);
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->insert(k, e);

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Nature::block:
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
        case Nature::tree:
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
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::erase(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          throw Exception("unable to erase the '%s' element from an empty "
                          "porcupine", k);
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->erase(k, e);

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Nature::block:
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
        case Nature::tree:
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
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          throw Exception("unable to update an empty porcupine");
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Update the porcupine attribtes.
            this->_state = this->_value->state();

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        case Nature::block:
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
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Update the tree.
            this->_tree->update(k);

            // Try to optimize the tree.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreacable();
    }

    template <typename T>
    elle::Size
    Porcupine<T>::size() const
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            return (0);
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (static_cast<elle::Size>(this->_value->size()));
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            elle::Size size{static_cast<elle::Size>(value()->size())};

            value.unload();

            return (size);
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            elle::Size size{static_cast<elle::Size>(this->_tree->capacity())};

            return (size);
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreacable();
    }

    template <typename T>
    void
    Porcupine<T>::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(flags);

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            return;
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Check the footprint.
            if (flags & flags::footprint)
              {
                ELLE_DEBUG_SCOPE("checking footprints");

                if (this->_value->footprint() == 0)
                  throw Exception("the footprint is null");

                if (this->_value->footprint() !=
                    elle::serialize::footprint(*this->_value))
                  throw Exception("the recorded footprint does not match the "
                                  "instance's: value(%s) versus footprint(%s)",
                                  this->_value->footprint(),
                                  elle::serialize::footprint(*this->_value));

                if (this->_value->footprint() > this->_nest.limits().extent())
                  throw Exception("the footprint '%s' exceeds the extent '%s'",
                                  this->_value->footprint(),
                                  this->_nest.limits().extent());
              }

            return;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            // Check the address.
            if (flags & flags::address)
              {
                ELLE_DEBUG_SCOPE("checking addresses");

                // Compute the block's address.
                Address address{value.contents().bind()};

                // Compare it with the one used as a reference.
                if (this->_block->address() != address)
                  throw Exception("invalid address: root(%s) versus bind(%s)",
                                  this->_root.address(), address);
              }

            // Check the footprint.
            if (flags & flags::footprint)
              {
                ELLE_DEBUG_SCOPE("checking footprints");

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

            value.unload();

            return;
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Check the validity of the tree.
            this->_tree->check(flags);

            return;
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreacahable();
    }

    template <typename T>
    Statistics
    Porcupine<T>::statistics()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            Statistics stats;

            // There is no data, hence no statistics.
            stats.footprint_minimum(0);
            stats.footprint_average(0);
            stats.footprint_maximum(0);
            stats.capacity_minimum(0);
            stats.capacity_average(0);
            stats.capacity_maximum(0);
            stats.blocks_clean(0);
            stats.blocks_dirty(0);
            stats.blocks_consistent(0);
            stats.blocks_all(0);

            return (stats);
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            Statistics stats;

            Footprint footprint = elle::serialize::footprint(*this->_value);
            Capacity capacity = this->_value->capacity();

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

            stats.blocks_clean(0);
            stats.blocks_dirty(0);
            stats.blocks_consistent(0);
            stats.blocks_all(0);

            return (stats);
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Statistics stats;
            Ambit<T> value(this->_nest, *this->_block);

            value.load();

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

            stats.blocks_all(1);

            switch (value().state())
              {
              case State::clean:
                {
                  stats.blocks_clean(1);

                  break;
                }
              case State::dirty:
                {
                  stats.blocks_dirty(1);

                  break;
                }
              case State::consistent:
                {
                  stats.blocks_consistent(1);

                  break;
                }
              default:
                throw Exception("unknown state: '%s'",
                                static_cast<int>(value().state()));
              }

            value.unload();

            return (stats);
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            return (this->_tree->statistics());
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::dump(elle::Natural32 const margin)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(margin);

      elle::String alignment(margin, ' ');
      elle::String shift(2, ' ');

      std::cout << alignment << "[Porcupine] " << this->_nature << std::endl;

      switch (this->_nature)
        {
        case Nature::empty:
          {
            break;
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value.Dump(2);

            break;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_block != nullptr);

            Ambit<T> value(this->_nest, *this->_block);

            value.load();

            value.contents().Dump(2);

            value.unload();

            break;
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            this->_tree->dump(2);

            break;
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      std::cout << alignment << shift << "[State] "
                << this->_state << std::endl;

      elle::unreachable();
    }

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
            // Nothing to do since the porcupine is clean i.e nothing has
            // changed.
            return;
          }
        case State::dirty:
          {
            ELLE_TRACE("nature: %s", this->_nature);

            switch (this->_nature)
              {
              case Nature::empty:
                throw Exception("unable to seal an empty porcupine");
              case Nature::value:
                {
                  ELLE_ASSERT(this->_value != nullptr);

                  cryptography::Cipher cipher;

                  if (secret.Encrypt(*this->_value,
                                     cipher) == elle::Status::Error)
                    throw Exception("unable to encrypt the value");

                  return (Radix(cipher));
                }
              case Nature::block:
                {
                  ELLE_ASSERt(this->_block != nullptr);

                  Ambit<T> value(this->_nest, *this->_block);

                  value.load();

                  // If the porcupine is dirty, so should be the value.
                  ELLE_ASSERT(this->_state == value().state());

                  // Encrypt and bind the value block.
                  value.contents().encrypt(secret);
                  Address address{value.contents().bind()};

                  // Update the node and block states.
                  value().state(State::consistent);
                  value.contents().state(State::consistent);

                  value.unload();

                  // Update the porcupine state.
                  this->_state = State::consistent;

                  return (Radix(this->_block.address()));
                }
              case Nature::tree:
                {
                  ELLE_ASSERT(this->_tree != nullptr);

                  // Seal the tree and return its root-based radix.
                  return (Radix(this->_tree->seal(secret)));
                }
              default:
                throw Exception("unknown nature: '%s'",
                                static_cast<int>(this->_nature));
              }

            break;
          }
        case State::consistent:
          throw Exception("unable to seal a consistent porcupine");
        default:
          throw Exception("unknown state: '%s'",
                          static_cast<int>(this->_state));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::_create()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      // Make sure the porcupine is empty.
      ELLE_ASSERT(this->_nature == Nature::empty);

      // Create a T and set it as the value on which the porcupine
      // is operating.
      this->_value = new T;
      this->_nature = Nature::value;

      // Set the porcupine's state.
      this->_state = State::dirty;
    }

    template <typename T>
    void
    Porcupine<T>::_optimize()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("nature: %s", this->_nature);

      switch (this->_nature)
        {
        case Nature::empty:
          {
            throw Exception("unable to optimize an empty porcupine");
          }
        case Nature::block:
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

                // set the nature.
                this->_nature = Nature::tree;
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

                    // set the nature.
                    this->_nature = Nature::empty;
                  }
                else
                  value.unload();
              }
            else
              value.unload();

            break;
          }
        case Nature::tree:
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

                        // therefore, set the nature as value now that the
                        // root handle actually references a value.
                        this->_nature = Nature::block;
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
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }
    }

    /*----------.
    | Printable |
    `----------*/

    // XXX
  }
}

#endif
