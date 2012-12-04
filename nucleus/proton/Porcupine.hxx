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
            this->_handle = new Handle{radix.block(), secret};

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
            delete this->_handle;

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
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

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
            ELLE_ASSERT(this->_handle != nullptr);

            return (Door<T>(*this->_handle, this->_nest));
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
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

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
              Door<T>(*this->_handle, this->_nest), 0});
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

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            value()->insert(k, e);

            value.unload();

            // Try to optimize the porcupine.
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

            // Specify the tree that a value has been modified. This way,
            // the tree will be able to adapt itself to these modifications.
            this->_tree->update(k);

            // Finally, try to optimize the porcupine.
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

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            value()->erase(k);

            value.unload();

            // Try to optimize the porcupine.
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

            // Specify the tree that a value has been modified. This way,
            // the tree will be able to adapt itself to these modifications.
            this->_tree->update(k);

            // Try to optimize the porcupine.
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

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            // Update the capacity state.
            this->_state = value().state();

            value.unload();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Update the tree.
            this->_tree->update(k);

            // Try to optimize the porcupine.
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
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

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
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            // Check the address.
            if (flags & flags::address)
              {
                ELLE_DEBUG_SCOPE("checking addresses");

                // Compute the block's address.
                Address address{value.contents().bind()};

                // Compare it with the one used as a reference.
                if (this->_handle->address() != address)
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
            ELLE_ASSERT(this->_handle != nullptr);

            Statistics stats;
            Ambit<T> value(this->_nest, *this->_handle);

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
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

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
                  ELLE_ASSERt(this->_handle != nullptr);

                  Ambit<T> value(this->_nest, *this->_handle);

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

                  return (Radix(this->_handle.address()));
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
          throw Exception("unable to optimize an empty porcupine");
        case Nature::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Check if the value has become too large to be embedeed. If
            // so, the value is converted into a standalone value block so
            // as to lighten the containing block.
            if ((this->_value->footprint()) > this->_nest.limits().extent())
              {
                ELLE_TRACE("value's high limit reached: %s > %s",
                           this->_value->footprint(),
                           this->_nest.limits().extent());

                // In this case, the value is embedeed in a Contents block.
                Contents contents{new Contents{this->_value}};

                ELLE_FINALLY_ACTION_DELETE(contents);

                // Finally, the contents is attached to the nest and is set
                // as the block which the porcupine is operating on.
                //
                // Note that the value's ownership has been transferred to
                // the Contents block. This is why the value is not deleted.
                this->_handle = new Handle{this->_nest.attach(contents)};

                ELLE_FINALLY_ABORT(contents);

                // And set the new nature.
                this->_nature = Nature::block;

                // Now that the porcupine is in a valid state, the _optimize()
                // method is called again. Indeed, transforming a value to a
                // block is not enough as the resulting block may also violate
                // the block limits.
                this->_optimize();
              }
            else if (this->_value->footprint() <
                     (this->_nest.limits().extent() *
                      this->_nest.limits().balancing()))
              {
                ELLE_TRACE("value's low limit reached: %s < %s",
                           this->_value->footprint(),
                           this->_nest.limits().extent() *
                           this->_nest.limits().balancing());

                // Check if the value is empty.
                if (this->_value().empty() == true)
                  {
                    ELLE_TRACE("the value is empty");

                    // Delete the value and set the nature to empty.
                    delete this->_value;
                    this->_nature = Nature::empty;

                    // Update the state.
                    this->_state = State::dirty;
                  }
              }
            else
              {
                ELLE_TRACE("no optimization to perform");
              }

            return;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            // Check if the value block has exceeded the extent limit
            // in which case this block would need to be split, inevitably
            // leading to the creation of a tree with a root block
            // referencing two value blocks.
            if ((value().footprint()) > this->_nest.limits().extent())
              {
                ELLE_TRACE("value block extent high limit reached: %s > %s",
                           value().footprint(),
                           this->_nest.limits().extent());

                // In this case, the value needs to be split. However,
                // in order to factorize the code as much as possible,
                // the following steps are performed. First, a tree is
                // created. Then, the value block is added to the tree.
                // Finally, since the tree cannot imagine that the added
                // block violates the limits, a call to update() is performed
                // so as to force the optimization.

                // Retrieve the mayor key from the value block so as to
                // pinpoint the location where the optimization should take
                // place.
                typename T::K mayor{value().mayor()};

                value.unload();

                // Create the tree and add the initial value block.
                Tree<T>* tree(this->_nest);

                ELLE_FINALLY_ACTION_DELETE(tree);

                tree->add(mayor, value.handle());

                // Force the tree to optimize itself by specifying the
                // tree that the value block responsible for the key _mayor_
                // has been modified.
                tree->update(mayor);

                // Delete the value block handle since this handle has been
                // copied when the block has been added to the tree.
                delete this->_handle;

                // Set the new nature and the reference to the tree.
                this->_nature = Nature::tree;
                this->_tree = tree;

                ELLE_FINALLY_ABORT(tree);

                // Update the porcupine state.
                this->_state = this->_tree->state();
              }
            else if (value().footprint() <
                     (this->_nest.limits().extent() *
                      this->_nest.limits().balancing()))
              {
                ELLE_TRACE("value block extent low limit reached: %s < %s",
                           value().footprint(),
                           this->_nest.limits().extent() *
                           this->_nest.limits().balancing());

                // Check if the block has become small enough for its value
                // to be embedded directly in its parent block.
                if (value().footprint() < 1024) // XXX need limits: low/hight
                  {
                    ELLE_TRACE("the block's value could be embedded");

                    // Ask the value block to cede the ownership on the value
                    // node.
                    ELLE_ASSERT(dynamic_cast<T*>(value().cede()) != nullptr);
                    T* _value{static_cast<T*>(value().cede())};

                    ELLE_FINALLY_ACTION_DELETE(_value);

                    // Detach the value block, leading to the block's
                    // destruction.
                    this->_nest.detach(*this->_handle);

                    value.unload();

                    // Set the embedded value along with the new nature,
                    // taking care to delete the handle which is no longer used.
                    this->_nature = Nature::empty;

                    delete this->_handle;
                    this->_value = _value;

                    ELLE_FINALLY_ABORT(_value);

                    // Update the porcupine state.
                    this->_state = State::dirty;
                  }
                else
                  {
                    ELLE_TRACE("no optimization to perform");

                    value.unload();
                  }
              }
            else
              {
                ELLE_TRACE("no optimization to perform");

                value.unload();
              }

            return;
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // At this point, either the tree has been modified through
            // an operation, in which case it would have been optimizing itself
            // or it has not changed and there is nothing to do.
            //
            // The following checks if the tree has become small enough to
            // fall back to an easier structure i.e a single block.

            // Check if the tree could be transformed back into a single block.
            //
            // For that, the tree must have a single level (height of one) and
            // the root nodule (a quill in this case) should contain a reference
            // to a single block.
            if (this->_tree->height() == 1)
              {
                ELLE_TRACE("the tree contains a single level");

                // Manually load the root nodule, which in this case, is a
                // quill.
                Ambit<Quill<T>> quill(this->_nest, this->_tree->root());

                quill.load();

                // Does the quill contain a single value.
                if (quill().single() == true)
                  {
                    ELLE_TRACE("the root quill contains a single entry");

                    // If the root nodule contains a single block, retrieve
                    // the handle of this block so as to manage it directly
                    // and destroy the tree.

                    typename T::K maiden{quill().maiden()};

                    // Retrieve the handle associated with the maiden key.
                    Handle orphan{quill().locate_handle(maiden)};

                    // Erase the inlet from the quill so as to make the block
                    // really an orphan. Otherwise, the block would be destroyed
                    // when the tree is deleted.
                    quill().erase(maiden);

                    quill.unload();

                    // Destroy the tree and delete it.
                    delete this->_tree;

                    // Set the new porcupine value block handle and update the
                    // nature.
                    this->_handle = new Handle{orphan};
                    this->_nature = Nature::block;
                  }
                else
                  {
                    ELLE_TRACE("the tree has too many levels to be transformed "
                               "back into a single block value");

                    quill.unload();
                  }

            return;
          }
        default:
          throw Exception("unknown nature: '%s'",
                          static_cast<int>(this->_nature));
        }

      elle::unreachable();
    }

    /*----------.
    | Printable |
    `----------*/

    // XXX
  }
}

#endif
