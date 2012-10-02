#ifndef NUCLEUS_PROTON_ADDRESS_HXX
# define NUCLEUS_PROTON_ADDRESS_HXX

# include <elle/cryptography/OneWay.hh>
# include <elle/serialize/TupleSerializer.hxx>
# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>

# include <nucleus/Exception.hh>

# include <elle/idiom/Close.hh>
#  include <tuple>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    template <typename... T>
    Address::Address(Network const& network,
                     const Family& family,
                     const neutron::Component& component,
                     const T&... parameters):
      _type(Type::valid),
      _network(network),
      _family(family),
      _component(component)
    {
      // compute the digest based on the parameters. note that most of
      // the components requesting this method pass family and component
      // in the parameters as well. for examples, please refer to
      // ContentHashBlock, PublicKeyBlock etc.
      if (elle::cryptography::OneWay::Hash(
            elle::serialize::make_tuple(this->_network,
                                        this->_family,
                                        this->_component,
                                        parameters...),
            this->_digest) == elle::Status::Error)
        throw Exception("unable to hash the given parameter(s)");
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/idiom/Close.hh>
# include <elle/idiom/Open.hh>

# include <elle/cryptography/Digest.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Address);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  // XXX[to handle for porcupine]
  enforce(value._type != nucleus::proton::Address::Type::some);

  archive & value._type;
  archive & value._network;
  archive & value._family;
  archive & value._component;
  archive & value._digest;
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive & value._type;
  archive & value._network;
  archive & value._family;
  archive & value._component;
  archive & value._digest;

  // A deserialized address can be either valid or null, nothing else.
  enforce(value._type != nucleus::proton::Address::Type::some);
}

#endif
