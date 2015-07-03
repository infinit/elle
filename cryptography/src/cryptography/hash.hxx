#ifndef INFINIT_CRYPTOGRAPHY_HASH_HXX
# define INFINIT_CRYPTOGRAPHY_HASH_HXX

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
/*-------.
| Legacy |
`-------*/

#  include <cryptography/Plain.hh>
#  include <cryptography/serialization.hh>

#  include <elle/Buffer.hh>
#  include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    template <typename T>
    Digest
    hash(T const& value,
         Oneway oneway)
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.hash");
      ELLE_TRACE_FUNCTION(oneway);
      ELLE_DUMP("value: %x", value);

      elle::Buffer archive = cryptography::serialize(value);

      elle::IOStream _archive(archive.istreambuf());

      return (Digest(hash(static_cast<std::istream&>(_archive), oneway)));
    }
  }
}
# endif

#endif
