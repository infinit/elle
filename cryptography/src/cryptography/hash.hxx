#ifndef INFINIT_CRYPTOGRAPHY_HASH_HXX
# define INFINIT_CRYPTOGRAPHY_HASH_HXX

# include <cryptography/Plain.hh>
# include <cryptography/serialization.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/Buffer.hh>
# include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    template <typename T>
    Digest
    hash(T const& value,
         Oneway oneway)
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.hash");
      ELLE_TRACE_FUNCTION(value, oneway);

      elle::ConstWeakBuffer archive = cryptography::serialize(value);

      return (hash(Plain(archive), oneway));
    }
  }
}

#endif
