#ifndef NUCLEUS_PROTON_ADDRESS_HXX
# define NUCLEUS_PROTON_ADDRESS_HXX

# include <tuple>

# include <elle/cryptography/OneWay.hxx>
# include <elle/serialize/TupleSerializer.hxx>

# include <nucleus/proton/Address.hh>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// create the address based on an object by serializing it before
    /// hashing it.
    ///
    template <typename T,
              typename... TT>
    elle::Status        Address::Create(const Family&           family,
                                        const
                                          neutron::Component&   component,
                                        const T&                parameter,
                                        const TT&...            parameters)
    {
      // set the family.
      this->family = family;

      // set the component.
      this->component = component;

      // release the previous digest.
      if (this->digest != NULL)
        delete this->digest;

      // allocate the digest object.
      this->digest = new elle::cryptography::Digest;

      // compute the digest based on the parameters. note that most of
      // the components requesting this method pass family and component
      // in the parameters as well. for examples, please refer to
      // ContentHashBlock, PublicKeyBlock etc.
      if (elle::cryptography::OneWay::Hash(
            elle::serialize::make_tuple(parameter, parameters...),
            *this->digest) == elle::Status::Error)
        escape("unable to hash the given parameter(s)");

      return elle::Status::Ok;
    }

  }
}

# include <elle/idiom/Close.hh>
#  include <cassert>
# include <elle/idiom/Open.hh>

# include <elle/cryptography/Digest.hh>

# include <nucleus/proton/Address.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Address);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  if (value.digest != nullptr)
    {
      archive << true
              << value.family
              << value.component
              << *(value.digest);
    }
  else
    {
      archive << false;
    }
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Address,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  delete value.digest;
  value.digest = nullptr;

  bool has_digest;
  archive >> has_digest;

  if (!has_digest)
    return; // XXX reset family & component ?

  archive >> value.family >> value.component;
  value.digest = archive.template Construct<elle::cryptography::Digest>().release();
}

#endif
