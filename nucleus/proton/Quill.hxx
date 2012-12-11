#ifndef NUCLEUS_PROTON_QUILL_HXX
# define NUCLEUS_PROTON_QUILL_HXX

# include <nucleus/Exception.hh>
# include <nucleus/proton/Ambit.hh>
# include <nucleus/proton/Nest.hh>
# include <nucleus/proton/Limits.hh>
# include <nucleus/proton/Statistics.hh>

# include <elle/assert.hh>
# include <elle/log.hh>
# include <elle/finally.hh>

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Constants |
    `----------*/

    template <typename T>
    Nature const Quill<T>::Constants::nature = T::Constants::quill;

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Quill<T>::Quill():
      Nodule<T>::Nodule(Nodule<T>::Type::quill)
    {
      static Footprint const initial = elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    template <typename T>
    Quill<T>::~Quill()
    {
      auto iterator = this->_container.begin();
      auto end = this->_container.end();

      for (; iterator != end; ++iterator)
        delete iterator->second;

      this->_container.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    template <typename T>
    void
    Quill<T>::insert(I* inlet)
    {
      std::pair<typename Quill<T>::Iterator::Forward, elle::Boolean> result;

      // check if this key has already been recorded.
      if (this->_container.find(inlet->key()) != this->_container.end())
        throw Exception("this key '%s' seems to have already been recorded",
                        inlet->key());

      // insert the inlet in the container.
      result =
        this->_container.insert(
          std::pair<const typename T::K,
                    typename Quill<T>::I*>(inlet->key(), inlet));

      // check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the inlet in the container");

      // set the state.
      this->state(State::dirty);

      // add the inlet footprint to the quill's.
      ELLE_ASSERT(this->footprint() != 0);
      ELLE_ASSERT(inlet->footprint() != 0);
      this->footprint(this->footprint() + inlet->footprint());
    }

    template <typename T>
    void
    Quill<T>::insert(typename T::K const& k,
                     Handle const& v)
    {
      // create an inlet.
      auto inlet =
        std::unique_ptr< typename Quill<T>::I >(
          new typename Quill<T>::I(k, v));

      // add the inlet to the quill.
      this->insert(inlet.get());

      // release the tracking.
      inlet.release();
    }

    template <typename T>
    void
    Quill<T>::erase(typename Iterator::Forward& iterator)
    {
      Quill<T>::I* inlet;

      // retrieve the inlet.
      inlet = iterator->second;

      // set the state.
      this->state(State::dirty);

      // substract the inlet footprint to the quill's.
      ELLE_ASSERT(this->footprint() != 0);
      ELLE_ASSERT(inlet->footprint() != 0);
      this->footprint(this->footprint() - inlet->footprint());

      // delete the inlet.
      delete inlet;

      // finally, erase the entry.
      this->_container.erase(iterator);
    }

    template <typename T>
    void
    Quill<T>::erase(typename T::K const& k)
    {
      typename Iterator::Forward iterator;

      // locate the inlet for the given key.
      iterator = this->locate_iterator(k);

      // delete the entry associated with the given iterator.
      this->erase(iterator);
    }

    template <typename T>
    void
    Quill<T>::refresh(typename Iterator::Forward& iterator,
                      typename T::K const& to)
    {
      Quill<T>::I* inlet;

      // retrieve the inlet.
      inlet = iterator->second;

      std::pair<typename Quill<T>::Iterator::Forward,
                elle::Boolean> result;

      // manually erase the entry.
      this->_container.erase(iterator);

      // update the inlet's key.
      inlet->key(to);

      // insert the inlet in the container.
      result = this->_container.insert(
        std::pair<const typename T::K,
                  Quill<T>::I*>(inlet->key(), inlet));

      // check if the insertion was successful.
      if (result.second == false)
        throw Exception("unable to insert the inlet in the container");

      // set the state.
      this->state(State::dirty);
    }

    template <typename T>
    void
    Quill<T>::refresh(typename T::K const& from,
                      typename T::K const& to)
    {
      typename Quill<T>::Iterator::Forward iterator;

      // locate the entry responsible for this key.
      iterator = this->locate_iterator(from);

      this->refresh(iterator, to);
    }

    template <typename T>
    elle::Boolean
    Quill<T>::exist(typename T::K const& k) const
    {
      return (this->_container.find(k) != this->_container.end());
    }

    /// XXX
    /// this method returns a scoutor on the inlet responsible for
    /// the given key.
    ///
    /// note that contrary to Locate(), Lookup() does not look for the
    /// exact key but for the key just greater than the given one.
    ///
    template <typename T>
    typename Quill<T>::Scoutor::Forward
    Quill<T>::lookup_iterator(typename T::K const& k) const
    {
      auto end = this->_container.end();
      auto rbegin = this->_container.rbegin();
      typename Quill<T>::Scoutor::Forward scoutor;

      // go through the container.
      for (scoutor = this->_container.begin(); scoutor != end; ++scoutor)
        {
          Quill<T>::I* inlet = scoutor->second;

          // check if this inlet is responsible for the given key or
          // the end of the seam has been reached.
          if ((k <= scoutor->first) || (inlet == rbegin->second))
            return (scoutor);
        }

      throw Exception("unable to look up the entry responsible for "
                      "the given key: '%s'", k);
    }

    template <typename T>
    typename Quill<T>::Iterator::Forward
    Quill<T>::lookup_iterator(typename T::K const& k)
    {
      typename Iterator::Forward iterator;
      auto end = this->_container.end();
      auto rbegin = this->_container.rbegin();

      // go through the container.
      for (iterator = this->_container.begin(); iterator != end; ++iterator)
        {
          Quill<T>::I* inlet = iterator->second;

          // check if this inlet is responsible for the given key or
          // the end of the seam has been reached.
          if ((k <= iterator->first) || (inlet == rbegin->second))
            return (iterator);
        }

      throw Exception("unable to look up the entry responsible for the "
                      "given key: '%s'", k);
    }

    template <typename T>
    typename Quill<T>::I*
    Quill<T>::lookup_inlet(typename T::K const& k) const
    {
      auto scoutor = this->lookup_iterator(k);

      return (scoutor->second);
    }

    template <typename T>
    Handle
    Quill<T>::lookup_handle(typename T::K const& k) const
    {
      auto inlet = this->lookup_inlet(k);

      return (inlet->value());
    }

    template <typename T>
    typename Quill<T>::Scoutor::Forward
    Quill<T>::locate_iterator(typename T::K const& k) const
    {
      typename Scoutor::Forward scoutor;

      // locate the given key.
      if ((scoutor = this->_container.find(k)) == this->_container.end())
        throw Exception("unable to locate the given key: '%s'", k);

      return (scoutor);
    }

    template <typename T>
    typename Quill<T>::Iterator::Forward
    Quill<T>::locate_iterator(typename T::K const& k)
    {
      typename Iterator::Forward iterator;

      // locate the given key.
      if ((iterator = this->_container.find(k)) == this->_container.end())
        throw Exception("unable to locate the given key: '%s'", k);

      return (iterator);
    }

    template <typename T>
    typename Quill<T>::I*
    Quill<T>::locate_inlet(typename T::K const& k) const
    {
      auto scoutor = this->locate_iterator(k);

      return (scoutor->second);
    }

    template <typename T>
    Handle
    Quill<T>::locate_handle(typename T::K const& k) const
    {
      typename Quill<T>::I* inlet = this->locate_inlet(k);

      return (inlet->value());
    }

    template <typename T>
    typename Quill<T>::Container&
    Quill<T>::container()
    {
      return (this->_container);
    }

//
// ---------- nodule ----------------------------------------------------------
//

    template <typename T>
    void
    Quill<T>::add(typename T::K const& k,
                  Handle const& v)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      // create an inlet.
      auto inlet =
        std::unique_ptr<typename Quill<T>::I>(
          new typename Quill<T>::I(k, v));

      ELLE_TRACE_SCOPE("add(%s, %s)", k, v);

      this->insert(inlet.get());

      // update the inlet's capacity and state.
      Ambit<T> value(this->nest(), inlet->value());

      value.load();

      inlet->capacity(value().capacity());
      inlet->state(value().state());

      // Note that the current quill's state is not set to
      // Dirty because the insert() method took care of it.
      this->capacity(this->capacity() + inlet->capacity());

      value.unload();

      // release the tracking.
      inlet.release();
    }

    template <typename T>
    void
    Quill<T>::remove(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      typename Quill<T>::Iterator::Forward iterator;
      Quill<T>::I* inlet;

      ELLE_TRACE_SCOPE("remove(%s)", k);

      iterator = this->locate_iterator(k);

      // retrieve the inlet.
      inlet = iterator->second;

      //
      // update the capacity.
      //
      {
        // make sure the operation is valid.
        ELLE_ASSERT(inlet->capacity() <= this->capacity());

        // substract the inlet's capacity though should the value block be
        // empty, this capacity may probably be zero.
        //
        // anyway, from the porcupine point of view, this does not matter
        // as it does not know how the capacity is computed.
        this->capacity(this->capacity() - inlet->capacity());
      }

      // delete the inlet from the nodule given its iterator.
      this->erase(iterator);
    }

    template <typename T>
    void
    Quill<T>::update(typename T::K const& k)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      typename Quill<T>::I* inlet;

      ELLE_TRACE_SCOPE("update(%s)", k);

      // look up the iterator associated with the key.
      auto iterator = this->lookup_iterator(k);

      // retrieve the inlet.
      inlet = iterator->second;

      Ambit<T> value(this->nest(), inlet->value());

      // load the value block.
      value.load();

      // Check whether the value has indeed been modified without
      // which one would not need to call update().
      if (value().state() != State::dirty)
        throw Exception("unable to update the nodule based on a non-dirty "
                        "value");

      // update the inlet's and quill's state.
      inlet->state(State::dirty);
      this->state(State::dirty);

      //
      // update the key, if possible.
      //
      if (value().empty() == false)
        {
          // update the quill if necessary.
          if (inlet->key() != value().mayor())
            this->refresh(iterator, value().mayor());
        }

      //
      // update the capacities.
      //
      {
        Capacity variation;

        // depending on the variation.
        if (value().capacity() > inlet->capacity())
          {
            // compute the capacity variation.
            variation = value().capacity() - inlet->capacity();

            // update the both the inlet and quill capacities.
            inlet->capacity(inlet->capacity() + variation);
            ELLE_ASSERT(inlet->capacity() == value().capacity());
            this->capacity(this->capacity() + variation);
          }
        else
          {
            // compute the capacity variation.
            variation = inlet->capacity() - value().capacity();

            // make sure the operation is valid.
            ELLE_ASSERT(variation <= inlet->capacity());

            // update the both the inlet and quill capacities, if necessary.
            inlet->capacity(inlet->capacity() - variation);
            ELLE_ASSERT(inlet->capacity() == value().capacity());
            this->capacity(this->capacity() - variation);
          }
      }

      // unload the value block.
      value.unload();

      // now, let us try to optimise the tree given the fact that its
      // content has been altered.
      Nodule<T>::optimize(*this, inlet->key());
    }

    template <typename T>
    Handle
    Quill<T>::split()
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");
      ELLE_TRACE_METHOD("");

      // Allocate a new quill.
      Contents* contents{new Contents{new Quill<T>}};
      Handle orphan{this->nest().attach(contents)};
      Ambit<Quill<T>> newright{this->nest(), orphan};

      // Load the new right nodule.
      newright.load();

      // Export the inlets from the current quill into the new quill.
      Nodule<T>::transfer_right(*this,
                                newright(),
                                this->nest().limits().extent() *
                                this->nest().limits().contention());

      // Set both nodules' state as dirty.
      this->state(State::dirty);
      newright().state(State::dirty);

      // Unload the new right nodule.
      newright.unload();

      return (orphan);
    }

    template <typename T>
    void
    Quill<T>::merge(Handle& other)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      Ambit<Quill<T>> quill(this->nest(), other);

      ELLE_TRACE_SCOPE("merge(%s)", other);

      // load the given quill.
      quill.load();

      // check which nodule has the lowest keys.
      if (quill().mayor() < this->mayor())
        {
          // in this case, export the lower quill's inlets into the current's.
          Nodule<T>::transfer_right(quill(), *this, 0);
        }
      else
        {
          // otherwise, import the higher quill's inlets into the current's.
          Nodule<T>::transfer_left(*this, quill(), 0);
        }

      // set both nodules' state as dirty.
      this->state(State::dirty);
      quill().state(State::dirty);

      // unload the given quill.
      quill.unload();
    }

    template <typename T>
    elle::Boolean
    Quill<T>::empty() const
    {
      return (this->_container.empty());
    }

    template <typename T>
    elle::Boolean
    Quill<T>::single() const
    {
      return (this->_container.size() == 1);
    }

    /// XXX
    /// this method returns the quill responsible for the given key.
    ///
    /// since the current nodule is a quill, it is its responsability.
    ///
    template <typename T>
    Handle
    Quill<T>::search(typename T::K const&)
    {
      elle::unreachable();
    }

    template <typename T>
    Handle
    Quill<T>::seek(Capacity const target,
                   Capacity& base)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      auto end = this->_container.end();
      auto scoutor = this->_container.begin();
      Handle value;

      ELLE_TRACE_SCOPE("seek(%s)", target);

      // go through the container.
      for (; scoutor != end; ++scoutor)
        {
          Quill<T>::I* inlet = scoutor->second;

          // check whether the target lies in this value.
          if (target < (base + inlet->capacity()))
            {
              // return the value.
              value = inlet->value();

              break;
            }

          // increases the base.
          base += inlet->capacity();
        }

      return (value);
    }

    template <typename T>
    void
    Quill<T>::check(Flags const flags)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      auto scoutor = this->_container.begin();
      auto end = this->_container.end();
      Capacity capacity(0);
      elle::Boolean dirty(false);

      ELLE_DEBUG_SCOPE("check(%s)", flags);

      // go through the container.
      for (; scoutor != end; ++scoutor)
        {
          Quill<T>::I* inlet = scoutor->second;
          Ambit<T> value(this->nest(), inlet->value());

          ELLE_DEBUG_SCOPE("checking inlet %s", inlet);

          // load the value block.
          value.load();

          // check the address, if required.
          if (flags & flags::address)
            {
              ELLE_DEBUG_SCOPE("checking addresses");

              // bind the value block.
              Address address{value.contents().bind()};

              // compare the addresses.
              if (inlet->value().address() != address)
                throw Exception("invalid address: inlet(%s) versus bind(%s)",
                                inlet->value().address(), address);
            }

          // check the keys, if required.
          if (flags & flags::key)
            {
              ELLE_DEBUG_SCOPE("checking keys");

              // check the key.
              if (inlet->key() != scoutor->first)
                throw Exception("invalid key: inlet(%s) versus container(%s)",
                                inlet->key(), scoutor->first);
            }

          // check the capacities, if required.
          if (flags & flags::capacity)
            {
              ELLE_DEBUG_SCOPE("checking capacities");

              if (inlet->capacity() != value().capacity())
                throw Exception("the recorded inlet capacity does not match "
                                "the value's: inlet(%s) versus value(%s)",
                                inlet->capacity(),
                                value().capacity());

              capacity += inlet->capacity();
            }

          // check the footprint.
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

              if (value().footprint() > this->nest().limits().extent())
                throw Exception("the footprint '%s' exceeds the extent '%s'",
                                value().footprint(),
                                this->nest().limits().extent());
            }

          // check the state.
          if (flags & flags::state)
            {
              ELLE_DEBUG_SCOPE("checking states");

              if (inlet->state() != value().state())
                throw Exception("invalid state: inlet(%s) versus value(%s)",
                                inlet->state(), value().state());

              switch (this->state())
                {
                case State::clean:
                  {
                    if (inlet->state() != State::clean)
                      throw Exception("the inlet's state '%s' should "
                                      "be clean", inlet->state());

                    break;
                  }
                case State::dirty:
                  {
                    if (inlet->state() == State::dirty)
                      dirty = true;

                    break;
                  }
                case State::consistent:
                  {
                    if ((inlet->state() != State::clean) &&
                        (inlet->state() != State::consistent))
                      throw Exception("the inlet's state '%s' should "
                                      "be either clean or consistent",
                                      inlet->state());

                    break;
                  }
                }
            }

          // unload the value block.
          value.unload();
        }

      // compare the quill capacity.
      if (flags & flags::capacity)
        {
          ELLE_DEBUG_SCOPE("checking capacities");

          if (this->capacity() != capacity)
            throw Exception("invalid capacity: this(%s) versus inlets(%s)",
                            this->capacity(), capacity);
        }

      // Should the quill be dirty, verify that at least on of its
      // inlet is.
      if (flags & flags::state)
        {
          ELLE_DEBUG_SCOPE("checking states");

          if ((this->state() == State::dirty) && (dirty == false))
            throw Exception("none of the inlet seems to be dirty");
        }
    }

    template <typename T>
    void
    Quill<T>::seal(cryptography::SecretKey const& secret)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      // XXX
      printf("--- QUILL 0\n");
      std::cout << secret << std::endl;
      printf("--- QUILL 0\n");

      auto iterator = this->_container.begin();
      auto end = this->_container.end();

      ELLE_TRACE_SCOPE("seal(%s)", secret);

      // go through the container.
      for (; iterator != end; ++iterator)
        {
          Quill<T>::I* inlet = iterator->second;
          Ambit<T> value(this->nest(), inlet->value());

          // load the value block.
          value.load();

          // ignore nodules which have not been created or modified
          // i.e is not dirty.
          switch (inlet->state())
            {
            case State::clean:
              {
                ELLE_TRACE_SCOPE("State::clean");

                break;
              }
            case State::dirty:
              {
                ELLE_TRACE_SCOPE("State::dirty");

                // Set the secret.
                ELLE_ASSERT(inlet->value().secret() !=
                            cryptography::SecretKey::Null);
                inlet->value().secret(secret);

                // Encrypt and bind the value block.
                value.contents().encrypt(secret);
                Address address{value.contents().bind()};

                value().state(State::consistent);
                value.contents().state(State::consistent);

                inlet->value().address(address);
                inlet->state(State::consistent);

                // set the current quill as dirty.
                this->state(State::dirty);

                break;
              }
            case State::consistent:
              {
                ELLE_TRACE_SCOPE("State::consistent");

                throw Exception("unexpected state '%s'",
                                value().state());
              }
            }

          // XXX
          printf("--- QUILL 1\n");

          // unload the value block.
          value.unload();

          // XXX
          printf("--- QUILL 2\n");
        }
    }

    template <typename T>
    void
    Quill<T>::dump(elle::Natural32 const margin)
    {
      elle::String alignment(margin, ' ');
      auto scoutor = this->_container.begin();
      auto end = this->_container.end();

      std::cout << alignment << "[Quill] " << this << std::endl;

      // dump the parent nodule.
      if (Nodule<T>::Dump(margin + 2) == elle::Status::Error)
        throw Exception("unable to dump the parent nodule");

      // dump the inlets.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Inlets] " << this->_container.size() << std::endl;

      // go through the container.
      for (; scoutor != end; ++scoutor)
        {
          Quill<T>::I* inlet = scoutor->second;
          Ambit<T> value(this->nest(), inlet->value());

          // dump the inlet.
          if (inlet->Dump(margin + 4) == elle::Status::Error)
            throw Exception("unable to dump the inlet");

          // load the value block.
          value.load();

          // dump the value.
          if (value().Dump(margin + 6) == elle::Status::Error)
            throw Exception("unable to dump the value");

          // unload the value block.
          value.unload();
        }
    }

    template <typename T>
    void
    Quill<T>::statistics(Statistics& stats)
    {
      ELLE_LOG_COMPONENT("infinit.nucleus.proton.Quill");

      auto scoutor = this->_container.begin();
      auto end = this->_container.end();

      ELLE_TRACE_SCOPE("statistics()");

      for (; scoutor != end; ++scoutor)
        {
          Quill<T>::I* inlet = scoutor->second;
          Ambit<T> value(this->nest(), inlet->value());

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
        }
    }

    template <typename T>
    typename T::K const&
    Quill<T>::mayor() const
    {
      ELLE_ASSERT(this->_container.empty() == false);

      return (this->_container.rbegin()->first);
    }

    template <typename T>
    typename T::K const&
    Quill<T>::maiden() const
    {
      ELLE_ASSERT(this->_container.size() == 1);

      return (this->_container.begin()->first);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    template <typename T>
    elle::Status
    Quill<T>::Dump(const elle::Natural32 margin) const
    {
      elle::String alignment(margin, ' ');
      auto scoutor = this->_container.begin();
      auto end = this->_container.end();

      std::cout << alignment << "[Quill] " << this << std::endl;

      // dump the parent nodule.
      if (Nodule<T>::Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the parent nodule");

      // dump the inlets.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Inlets] " << this->_container.size() << std::endl;

      // go through the container.
      for (; scoutor != end; ++scoutor)
        {
          // dump the inlet.
          if (scoutor->second->Dump(margin + 4) == elle::Status::Error)
            escape("unable to dump the inlet");
        }

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT_T1(nucleus::proton::Quill);

ELLE_SERIALIZE_SPLIT_T1_LOAD(nucleus::proton::Quill,
                             archive,
                             value,
                             version)
{
  elle::Natural32 size;
  elle::Natural32 i;

  enforce(version == 0);

  archive >> base_class<nucleus::proton::Nodule<T1>>(value);

  archive >> size;

  for (i = 0; i< size; i++)
    {
      auto inlet =
        archive.template Construct<
          typename nucleus::proton::Quill<T1>::I>();

      // Compute the inlet's footprint because the inlet proper
      // constructor has not been called. Instead, the default
      // constructor has been used before deserializing the inlet.
      inlet->footprint(elle::serialize::footprint(*inlet));

      std::pair<typename nucleus::proton::Quill<T1>::Iterator::Forward,
                elle::Boolean> result;

      if (value._container.find(inlet->key()) != value._container.end())
        throw nucleus::Exception("this key '%s' seems to have already been "
                                 "recorded",
                                 inlet->key());

      result =
        value._container.insert(
          std::pair<const typename T1::K,
                    typename nucleus::proton::Quill<T1>::I*>(
                      inlet->key(), inlet.get()));

      if (result.second == false)
        throw nucleus::Exception("unable to insert the inlet in the container");

      // Update the quill's footprint.
      value.footprint(value.footprint() + inlet->footprint());

      inlet.release();
    }
}

ELLE_SERIALIZE_SPLIT_T1_SAVE(nucleus::proton::Quill,
                             archive,
                             value,
                             version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::Nodule<T1>>(value);

  archive << static_cast<elle::Natural32>(value._container.size());

  for (auto& pair: value._container)
    {
      auto& inlet = pair.second;

      archive << *inlet;
    }
}

#endif
