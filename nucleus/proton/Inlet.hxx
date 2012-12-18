#ifndef NUCLEUS_PROTON_INLET_HXX
# define NUCLEUS_PROTON_INLET_HXX

# include <nucleus/proton/Porcupine.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    template <typename K>
    Inlet<K>::Inlet():
      _capacity(0),
      _state(State::clean),
      _footprint(0)
    {
    }

    template <typename K>
    Inlet<K>::Inlet(K const& key,
                    Handle const& value):
      _key(key),
      _value(value),
      _capacity(0),
      _state(State::clean),
      _footprint(0)
    {
      this->_footprint = elle::serialize::footprint(*this);
    }

//
// ---------- accessors -------------------------------------------------------
//

    template <typename K>
    K const&
    Inlet<K>::key() const
    {
      return (this->_key);
    }

    template <typename K>
    void
    Inlet<K>::key(K const&          key)
    {
      this->_key = key;
    }

    template <typename K>
    Handle&
    Inlet<K>::value()
    {
      return (this->_value);
    }

    template <typename K>
    Capacity const&
    Inlet<K>::capacity() const
    {
      return (this->_capacity);
    }

    template <typename K>
    void
    Inlet<K>::capacity(elle::Natural64 const capacity)
    {
      this->_capacity = capacity;
    }

    template <typename K>
    State const&
    Inlet<K>::state() const
    {
      return (this->_state);
    }

    template <typename K>
    void
    Inlet<K>::state(State const state)
    {
      this->_state = state;
    }

    template <typename K>
    Footprint const&
    Inlet<K>::footprint() const
    {
      return (this->_footprint);
    }

    template <typename K>
    void
    Inlet<K>::footprint(Footprint const footprint)
    {
      this->_footprint = footprint;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the inlet.
    ///
    template <typename K>
    elle::Status        Inlet<K>::Dump(const elle::Natural32    margin)
      const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Inlet] " << this << std::endl;

      // dump the key.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Key] " << std::dec << this->_key << std::endl; // XXX[remove dec]

      // dump the value.
      if (this->_value.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the value");

      // dump the capacity.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Capacity] " << std::dec << this->_capacity << std::endl;

      // dump the state.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[State] " << std::dec << this->_state << std::endl;

      // dump the footprint.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Footprint] " << std::dec << this->_footprint << std::endl;

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE_T1(nucleus::proton::Inlet,
                         archive,
                         value,
                         version)
{
  enforce(version == 0);

  archive & value._key;
  archive & value._value;
  archive & value._capacity;
}

#endif
