#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HXX
# define ELLE_CRYPTOGRAPHY_ONEWAY_HXX

# include <elle/cryptography/Plain.hh>
# include <elle/cryptography/Digest.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <utility>

namespace elle
{
  namespace cryptography
  {
    namespace oneway
    {
      template <typename T>
      Digest
      hash(T const& value,
           Algorithm algorithm)
      {
        ELLE_LOG_COMPONENT("elle.cryptography.oneway");
        ELLE_TRACE_FUNCTION(value);

        elle::Buffer buffer;
        buffer.writer() << value;

        return (hash(Plain(WeakBuffer(buffer)), algorithm));
      }
    }
  }
}

#endif
