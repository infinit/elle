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
      _valid(nullptr)
    {
      this->_valid = new Address::Valid(network, family, component, nullptr);

      // XXX[this could be improved if OneWay::hash() returned a digest]
      this->_valid->digest(new elle::cryptography::Digest);

      // compute the digest based on the parameters. note that most of
      // the components requesting this method pass family and component
      // in the parameters as well. for examples, please refer to
      // ContentHashBlock, PublicKeyBlock etc.
      if (elle::cryptography::OneWay::Hash(
            elle::serialize::make_tuple(this->_valid->network(),
                                        this->_valid->family(),
                                        this->_valid->component(),
                                        parameters...),
            *this->_valid->digest()) == elle::Status::Error)
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

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Address,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._type;
  switch (value._type)
    {
      case nucleus::proton::Address::Type::null:
        {
          break;
        }
      case nucleus::proton::Address::Type::some:
        {
          // XXX[to handle for porcupine]
          elle::abort("invalid address type: some");

          break;
        }
      case nucleus::proton::Address::Type::valid:
        {
          archive & elle::serialize::alive_pointer(value._valid);

          break;
        }
    }
}

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Address::Valid,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._network;
  archive & value._family;
  archive & value._component;
  archive & elle::serialize::alive_pointer(value._digest);
}

#endif
