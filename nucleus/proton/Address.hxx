#ifndef NUCLEUS_PROTON_ADDRESS_HXX
# define NUCLEUS_PROTON_ADDRESS_HXX

# include <elle/cryptography/OneWay.hh>

# include <elle/serialize/TupleSerializer.hxx>

# include <elle/idiom/Close.hh>
#  include <tuple>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// create the address based on an object by serializing it before
    /// hashing it.
    ///
    template <typename... T>
    elle::Status        Address::Create(Network const& network,
                                        const Family&           family,
                                        const
                                          neutron::Component&   component,
                                        const T&...            parameters)
    {
      this->_network = network;
      this->_family = family;
      this->_component = component;

      delete this->_digest;
      this->_digest = new elle::cryptography::Digest;

      // compute the digest based on the parameters. note that most of
      // the components requesting this method pass family and component
      // in the parameters as well. for examples, please refer to
      // ContentHashBlock, PublicKeyBlock etc.
      if (elle::cryptography::OneWay::Hash(
            elle::serialize::make_tuple(this->_network,
                                        this->_family,
                                        this->_component,
                                        parameters...),
            *this->_digest) == elle::Status::Error)
        escape("unable to hash the given parameter(s)");

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/idiom/Close.hh>
#  include <cassert>
# include <elle/idiom/Open.hh>

# include <elle/cryptography/Digest.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Address);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  enforce(value._digest != nullptr);

  archive << value._network
          << value._family
          << value._component
          << *(value._digest);
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  delete value._digest;
  value._digest = nullptr;

  archive >> value._network;
  archive >> value._family;
  archive >> value._component;
  value._digest = archive.template Construct<elle::cryptography::Digest>().release();
}

#endif
