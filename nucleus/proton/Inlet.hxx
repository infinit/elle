#ifndef NUCLEUS_PROTON_INLET_HXX
# define NUCLEUS_PROTON_INLET_HXX

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename K>
    Inlet<K>::Inlet():
      _capacity(0),
      _state(State::clean),
      _footprint(0)
    {
    }

    template <typename K>
    Inlet<K>::Inlet(K const& k,
                    Handle const& v):
      _key(k),
      _value(v),
      _capacity(0),
      _state(State::clean),
      _footprint(0)
    {
      // Compute the footprint of the inlet.
      //
      // Note that this footprint is final as nothing should change,
      // not even the _value_ whose footprint should also be static.
      this->_footprint = elle::serialize::footprint(*this);
    }

    /*---------.
    | Dumpable |
    `---------*/

    template <typename K>
    elle::Status
    Inlet<K>::Dump(const elle::Natural32    margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Inlet] " << this << std::endl;

      // dump the key.
      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Key] " << this->_key << std::endl;

      // dump the value.
      if (this->_value.Dump(margin + 2) == elle::Status::Error)
        throw Exception("unable to dump the value");

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

    /*----------.
    | Printable |
    `----------*/

    template <typename T>
    void
    Inlet<T>::print(std::ostream& stream) const
    {
      stream << this->_key << ", " << this->_value;
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

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
