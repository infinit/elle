#ifndef NUCLEUS_PROTON_PORCUPINE_HXX
# define NUCLEUS_PROTON_PORCUPINE_HXX

# include <nucleus/Exception.hh>
# include <nucleus/proton/Ambit.hh>
# include <nucleus/proton/Seam.hh>
# include <nucleus/proton/Quill.hh>
# include <nucleus/proton/Nest.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/proton/Statistics.hh>
# include <nucleus/proton/Tree.hh>

# include <elle/assert.hh>
# include <elle/log.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Porcupine<T>::Porcupine(Nest& nest):
      _strategy(Strategy::none),
      _nest(nest),
      _state(State::clean)
    {
    }

    template <typename T>
    Porcupine<T>::Porcupine(/* XXX Network const& network,
                               cryptography::PublicKey const& agent_K,*/
                            Radix const& radix,
                            cryptography::SecretKey const& secret,
                            Nest& nest):
      _strategy(radix.strategy()),
      /* XXX
      _network(network),
      _agent_K(agent_K),
      */
      _nest(nest),
      _state(State::clean)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");

      // Manually set all the union pointers to null so as to make sure all
      // the cases are handled.
      this->_value = nullptr;
      this->_handle = nullptr;
      this->_tree = nullptr;

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return;
          }
        case Strategy::value:
          {
            ELLE_TRACE("decrypting the radix' cipher with the secret key");

            this->_value = new T{
              secret.decrypt<T>(radix.value())};

            return;
          }
        case Strategy::block:
          {
            this->_handle = new Handle{radix.block(), secret};

            return;
          }
        case Strategy::tree:
          {
            this->_tree = new Tree<T>{radix.tree(), secret, this->_nest};

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }
    }

    template <typename T>
    Porcupine<T>::~Porcupine()
    {
      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return;
          }
        case Strategy::value:
          {
            delete this->_value;

            return;
          }
        case Strategy::block:
          {
            delete this->_handle;

            return;
          }
        case Strategy::tree:
          {
            delete this->_tree;

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
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

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return (false);
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (this->_value->exist(k));
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            if (value().exist(k) == false)
              return (false);

            value.unload();

            return (true);
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            return (this->_tree->exist(k));
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    Door<T>
    Porcupine<T>::lookup(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("strategy: %s", this->_strategy);

      // Note that this method assumes that a single value or block
      // is responsible for any valid key _k_.

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup in a value.
            ELLE_ASSERT(this->_strategy == Strategy::value);
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (Door<T>(this->_value));
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            return (Door<T>(*this->_handle, this->_nest));
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup in the tree and transform the returned handle into a door.
            return (Door<T>(this->_tree->lookup(k), this->_nest));
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
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

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup.
            ELLE_ASSERT(this->_strategy == Strategy::value);
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Verify the validity of the target index.
            // XXX[si le test chier sur >= bien y reflechir car head()/tail()
            //     utilise ca]
            if (target >= this->_value->capacity())
              throw Exception("the given target index '%s' exceeds the "
                              "capacity '%s' of the value",
                              target, this->_value->capacity());

            // Return a door to the value along with a capacity index of
            // zero since this is the first and only value.
            Door<T> door{this->_value};

            return (std::pair<Door<T>, Capacity>{std::move(door), 0});
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            // Verify the validity of the target index.
            // XXX[si le test chier sur >= bien y reflechir car head()/tail()
            //     utilise ca]
            if (target >= value().capacity())
              throw Exception("the given target index '%s' exceeds the "
                              "capacity '%s' of the value",
                              target, value().capacity());

            value.unload();

            // Return a door to the value block along with a capacity index of
            // zero since this is the first and only value.
            Door<T> door{*this->_handle, this->_nest};

            return (std::pair<Door<T>, Capacity>{std::move(door), 0});
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Seek the target in the tree.
            auto pair = this->_tree->seek(target);

            // Return a door to the value block responsible for the given
            // capacity index along with the base capacity index of this value.
            Door<T> door{pair.first, this->_nest};

            return (std::pair<Door<T>, Capacity>{std::move(door), pair.second});
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    template <typename E>
    void
    Porcupine<T>::insert(typename T::K const& k,
                         E* e)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k, e);

      ELLE_ASSERT(e != nullptr);

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            // Create a value.
            this->_create();

            // Do not break and proceed with the lookup.
            ELLE_ASSERT(this->_strategy == Strategy::value);
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->insert(e);

            // Update the porcupine state.
            this->_state = this->_value->state();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            value().insert(e);

            // Update the porcupine state.
            this->_state = value().state();

            value.unload();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup the handle of the value block responsible for the key.
            Handle handle{this->_tree->lookup(k)};

            Ambit<T> value(this->_nest, handle);

            value.load();

            // Insert the tuple key/element in the value block.
            value().insert(e);

            value.unload();

            // Specify the tree that a value has been modified. This way,
            // the tree will be able to adapt itself to these modifications.
            this->_tree->update(k);

            // Update the porcupine state.
            this->_state = this->_tree->state();

            // Finally, try to optimize the porcupine.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::erase(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          throw Exception("unable to erase the '%s' element from an empty "
                          "porcupine", k);
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->erase(k);

            // Update the porcuine state.
            this->_state = this->_value->state();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            value().erase(k);

            // Update the porcupine state.
            this->_state = value().state();

            value.unload();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Lookup the handle of the value block responsible for the key.
            Handle handle{this->_tree->lookup(k)};

            Ambit<T> value(this->_nest, handle);

            value.load();

            // Erase the tuple key/element from the value block.
            value().erase(k);

            value.unload();

            // Specify the tree that a value has been modified. This way,
            // the tree will be able to adapt itself to these modifications.
            this->_tree->update(k);

            // Update the porcupine state.
            this->_state = this->_tree->state();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(k);

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          throw Exception("unable to update an empty porcupine");
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Update the porcupine state.
            this->_state = this->_value->state();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            // Update the porcupine state.
            this->_state = value().state();

            value.unload();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Update the tree.
            this->_tree->update(k);

            // Update the porcupine state.
            this->_state = this->_tree->state();

            // Try to optimize the porcupine.
            this->_optimize();

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    Door<T>
    Porcupine<T>::head()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          throw Exception("unable to trim an empty porcupine");
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Return a door to the value.
            return (Door<T>{this->_value});
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            // Return a door to the value block.
            return (Door<T>{*this->_handle, this->_nest});
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Return a door on the node responsible for the lowest keys.
            return (Door<T>{this->_tree->head(), this->_nest});
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    Door<T>
    Porcupine<T>::tail()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          throw Exception("unable to trim an empty porcupine");
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Return a door to the value.
            return (Door<T>{this->_value});
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            // Return a door to the value block.
            return (Door<T>{*this->_handle, this->_nest});
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Return a door on the node responsible for the highest keys.
            return (Door<T>{this->_tree->tail(), this->_nest});
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    elle::Size
    Porcupine<T>::size() const
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return (0);
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            return (static_cast<elle::Size>(this->_value->size()));
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            elle::Size size{static_cast<elle::Size>(value().size())};

            value.unload();

            return (size);
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            elle::Size size{static_cast<elle::Size>(this->_tree->capacity())};

            return (size);
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    void
    Porcupine<T>::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(flags);

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return;
          }
        case Strategy::value:
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

            // Check the state.
            if (flags & flags::state)
              {
                if (this->_state != this->_value->state())
                  throw Exception("the porcupine state does not match the "
                                  "value's: porcupine(%s) versus value(%s)",
                                  this->_state, this->_value->state());
              }

            return;
          }
        case Strategy::block:
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
                                  this->_handle->address(), address);
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

            // Check the state.
            if (flags & flags::state)
              {
                if (this->_state != value().state())
                  throw Exception("the porcupine state does not match the "
                                  "value block's: porcupine(%s) versus "
                                  "block(%s)",
                                  this->_state, value().state());
              }

            value.unload();

            return;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            // Check the validity of the tree.
            this->_tree->check(flags);

            // Check the state.
            if (flags & flags::state)
              {
                if (this->_state != this->_tree->state())
                  throw Exception("the porcupine state does not match the "
                                  "tree: porcupine(%s) versus tree(%s)",
                                  this->_state, this->_tree->state());
              }

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    Statistics
    Porcupine<T>::statistics()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
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
        case Strategy::value:
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
        case Strategy::block:
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
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            return (this->_tree->statistics());
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
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

      std::cout << alignment << "[Porcupine] " << this->_strategy << std::endl;

      std::cout << alignment << shift << "[State] "
                << this->_state << std::endl;

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return;
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            this->_value->Dump(2);

            return;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            Ambit<T> value(this->_nest, *this->_handle);

            value.load();

            value.contents().Dump(2);

            value.unload();

            return;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            this->_tree->dump(2);

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    template <typename T>
    Radix
    Porcupine<T>::seal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD(secret);

      ELLE_TRACE("state: %s", this->_state);

      switch (this->_state)
        {
        case State::clean:
          throw Exception("unable to seal a clean porcupine");
        case State::dirty:
          {
            ELLE_TRACE("strategy: %s", this->_strategy);

            switch (this->_strategy)
              {
              case Strategy::none:
                throw Exception("unable to seal an empty porcupine");
              case Strategy::value:
                {
                  ELLE_ASSERT(this->_value != nullptr);

                  // Encrypt the value.
                  cryptography::Cipher cipher{
                    secret.encrypt(*this->_value)};

                  // Update the value state.
                  this->_value->state(State::consistent);

                  // Update the porcupine state.
                  this->_state = this->_value->state();

                  return (Radix(cipher));
                }
              case Strategy::block:
                {
                  ELLE_ASSERT(this->_handle != nullptr);

                  Ambit<T> value(this->_nest, *this->_handle);

                  value.load();

                  // Encrypt and bind the value block.
                  value.contents().encrypt(secret);
                  Address address{value.contents().bind()};

                  // Update the node and block states.
                  value().state(State::consistent);
                  value.contents().state(State::consistent);

                  // Update the porcupine state.
                  this->_state = value().state();

                  value.unload();

                  return (Radix(address));
                }
              case Strategy::tree:
                {
                  ELLE_ASSERT(this->_tree != nullptr);

                  // Seal the tree and return its root-based radix.
                  Radix radix{this->_tree->seal(secret)};

                  // Update the porcupine state.
                  this->_state = this->_tree->state();

                  return (radix);
                }
              default:
                throw Exception("unknown strategy: '%s'",
                                static_cast<int>(this->_strategy));
              }

            elle::unreachable();
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
    Porcupine<T>::destroy()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_TRACE_METHOD("");

      ELLE_TRACE("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            delete this->_value;
            this->_value = nullptr;

            break;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            // Detach the value block, leading to the block's
            // destruction.
            this->_nest.detach(*this->_handle);

            delete this->_handle;
            this->_handle = nullptr;

            break;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            this->_tree->destroy();

            delete this->_tree;
            this->_tree = nullptr;

            break;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      // Set the strategy to none.
      this->_strategy = Strategy::none;
    }

    template <typename T>
    T const&
    Porcupine<T>::value() const
    {
      ELLE_ASSERT(this->_strategy == Strategy::value);
      ELLE_ASSERT(this->_value != nullptr);

      return (*this->_value);
    }

    template <typename T>
    T&
    Porcupine<T>::value()
    {
      ELLE_ASSERT(this->_strategy == Strategy::value);
      ELLE_ASSERT(this->_value != nullptr);

      return (*this->_value);
    }

    template <typename T>
    Handle const&
    Porcupine<T>::block() const
    {
      ELLE_ASSERT(this->_strategy == Strategy::block);
      ELLE_ASSERT(this->_handle != nullptr);

      return (*this->_handle);
    }

    template <typename T>
    Tree<T> const&
    Porcupine<T>::tree() const
    {
      ELLE_ASSERT(this->_strategy == Strategy::tree);
      ELLE_ASSERT(this->_tree != nullptr);

      return (*this->_tree);
    }

    template <typename T>
    Tree<T>&
    Porcupine<T>::tree()
    {
      ELLE_ASSERT(this->_strategy == Strategy::tree);
      ELLE_ASSERT(this->_tree != nullptr);

      return (*this->_tree);
    }

    template <typename T>
    void
    Porcupine<T>::_create()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_DEBUG_METHOD("");

      // Make sure the porcupine is none.
      ELLE_ASSERT(this->_strategy == Strategy::none);

      // Create a T and set it as the value on which the porcupine
      // is operating.
      this->_value = new T;
      this->_strategy = Strategy::value;

      // Note that the porcupine's state is not set to dirty
      // because nothing has been added/removed/updated yet.
    }

    template <typename T>
    void
    Porcupine<T>::_optimize()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Porcupine");
      ELLE_DEBUG_METHOD("");

      ELLE_DEBUG("strategy: %s", this->_strategy);

      switch (this->_strategy)
        {
        case Strategy::none:
          throw Exception("unable to optimize an empty porcupine");
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            // Check if the value has become too large to be embedeed. If
            // so, the value is converted into a standalone value block so
            // as to lighten the containing block.
            if ((this->_value->footprint()) > this->_nest.limits().extent())
              {
                ELLE_DEBUG("value's high limit reached: %s > %s",
                           this->_value->footprint(),
                           this->_nest.limits().extent());

                // In this case, the value is embedeed in a Contents block.
                //
                // Finally, the contents is attached to the nest and is set
                // as the block which the porcupine is operating on.
                //
                // Note that the value's ownership has been transferred to
                // the Contents block. This is why the value is not deleted.
                Contents* contents{new Contents{this->_value}};
                this->_handle = new Handle{this->_nest.attach(contents)};

                // And set the new strategy.
                this->_strategy = Strategy::block;

                // Now that the porcupine is in a valid state, the _optimize()
                // method is called again. Indeed, transforming a value to a
                // block is not enough as the resulting block may also violate
                // the block limits.
                this->_optimize();
              }
            else if ((this->_value->empty() == true) ||
                     (this->_value->footprint() <=
                      (this->_nest.limits().extent() *
                       this->_nest.limits().balancing())))
              {
                ELLE_DEBUG("value's low limit reached: %s < %s",
                           this->_value->footprint(),
                           this->_nest.limits().extent() *
                           this->_nest.limits().balancing());

                // Check if the value is empty.
                if (this->_value->empty() == true)
                  {
                    ELLE_DEBUG("the value is empty");

                    // Delete the value and set the strategy to none.
                    delete this->_value;
                    this->_strategy = Strategy::none;

                    // Update the state.
                    this->_state = State::dirty;
                  }
              }
            else
              {
                ELLE_DEBUG("no optimization to perform");
              }

            return;
          }
        case Strategy::block:
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
                ELLE_DEBUG("value block extent high limit reached: %s > %s",
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
                Tree<T>* tree = new Tree<T>(this->_nest);

                ELLE_FINALLY_ACTION_DELETE(tree);

                tree->add(mayor, value.handle());

                // Force the tree to optimize itself by specifying the
                // tree that the value block responsible for the key _mayor_
                // has been modified.
                tree->update(mayor);

                // Delete the value block handle since this handle has been
                // copied when the block has been added to the tree.
                delete this->_handle;

                // Set the new strategy and the reference to the tree.
                this->_strategy = Strategy::tree;
                this->_tree = tree;

                ELLE_FINALLY_ABORT(tree);

                // Update the porcupine state.
                this->_state = this->_tree->state();
              }
            else if ((value().empty() == true) ||
                     (value().footprint() <=
                      (this->_nest.limits().extent() *
                       this->_nest.limits().balancing())))
              {
                ELLE_DEBUG("value block extent low limit reached: %s < %s",
                           value().footprint(),
                           this->_nest.limits().extent() *
                           this->_nest.limits().balancing());

                // Check if the block has become small enough for its value
                // to be embedded directly in its parent block.
                if (value().footprint() < 1024) // XXX need limits: low/high - same as above
                  {
                    ELLE_DEBUG("the block's value could be embedded");

                    // Ask the value block to cede the ownership on the value
                    // node.
                    Node* node = value.contents().cede();

                    ELLE_ASSERT(dynamic_cast<T*>(node) != nullptr);
                    T* _value = static_cast<T*>(node);

                    ELLE_FINALLY_ACTION_DELETE(_value);

                    // Detach the value block, leading to the block's
                    // destruction.
                    this->_nest.detach(*this->_handle);

                    value.unload();

                    // Set the embedded value along with the new strategy,
                    // taking care to delete the handle which is no longer used.
                    this->_strategy = Strategy::value;

                    delete this->_handle;
                    this->_value = _value;

                    ELLE_FINALLY_ABORT(_value);

                    // Update the porcupine state.
                    this->_state = State::dirty;
                  }
                else
                  {
                    ELLE_DEBUG("no optimization to perform");

                    value.unload();
                  }
              }
            else
              {
                ELLE_DEBUG("no optimization to perform");

                value.unload();
              }

            return;
          }
        case Strategy::tree:
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
                ELLE_DEBUG("the tree contains a single level");

                // Manually load the root nodule, which in this case, is a
                // quill.
                Ambit<Quill<T>> quill(this->_nest, this->_tree->root());

                quill.load();

                // Does the quill contain a single value.
                if (quill().single() == true)
                  {
                    ELLE_DEBUG("the root quill contains a single entry");

                    // If the root nodule contains a single block, retrieve
                    // the handle of this block so as to manage it directly
                    // and destroy the tree.

                    typename T::K maiden{quill().maiden()};

                    // Retrieve the handle associated with the maiden key.
                    Handle orphan{quill().locate_handle(maiden)};

                    // Manually erase the inlet from the quill so as to make the
                    // block really an orphan. Otherwise, the block would be
                    // destroyed when the tree is deleted.
                    quill().erase(maiden);

                    quill.unload();

                    // Destroy the blocks constituing the tree. In this case
                    // the quill is the only remaining block.
                    this->_tree->destroy();

                    // Finally, delete the tree data structure.
                    delete this->_tree;

                    // Set the new porcupine value block handle and update the
                    // strategy.
                    this->_handle = new Handle{orphan};
                    this->_strategy = Strategy::block;
                  }
                else
                  {
                    ELLE_DEBUG("the root has too many inlets to be transformed "
                               "back into a single block value");

                    quill.unload();
                  }
              }
            else
              {
                ELLE_DEBUG("the tree has too many levels to be transformed "
                           "back into a single block value");
              }

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Porcupine<T>::print(std::ostream& stream) const
    {
      stream << this->_strategy;

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            return;
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            stream << "(" << *this->_value << ")";

            return;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_handle != nullptr);

            stream << "(" << *this->_handle << ")";

            return;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_value != nullptr);

            stream << "(" << *this->_tree << ")";

            return;
          }
        default:
          throw Exception("unknown strategy: '%s'",
                          static_cast<int>(this->_strategy));
        }

      elle::unreachable();
    }
  }
}

#endif
