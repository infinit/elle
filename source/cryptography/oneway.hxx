#ifndef INFINIT_CRYPTOGRAPHY_ONEWAY_HXX
# define INFINIT_CRYPTOGRAPHY_ONEWAY_HXX

# include <cryptography/Plain.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/Buffer.hh>
# include <elle/log.hh>

# include <utility>

namespace infinit
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
        ELLE_LOG_COMPONENT("cryptography.oneway");
        ELLE_TRACE_FUNCTION(value);

        static_assert(!std::is_same<T, Plain>::value,
                      "please use the Plain type for hashin");

        elle::Buffer buffer;
        buffer.writer() << value;

        return (hash(Plain{elle::WeakBuffer{buffer}}, algorithm));
      }
    }
  }
}

#endif
