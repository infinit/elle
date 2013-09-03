#ifndef INFINIT_CRYPTOGRAPHY_ONEWAY_HXX
# define INFINIT_CRYPTOGRAPHY_ONEWAY_HXX

# include <cryptography/Plain.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/Buffer.hh>
# include <elle/log.hh>
# include <elle/Measure.hh>

# include <utility>

namespace infinit
{
  namespace cryptography
  {
    namespace oneway
    {
      /*----------.
      | Functions |
      `----------*/

      template <typename T>
      Digest
      hash(T const& value,
           Algorithm algorithm)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.oneway");
        ELLE_DEBUG_FUNCTION(value, algorithm);

        static_assert(!std::is_same<T, Plain>::value,
                      "this call should never have occured");

        elle::Buffer buffer;
        ELLE_MEASURE("Serialize into the buffer")
          buffer.writer() << value;

        ELLE_MEASURE_SCOPE("Hash the buffer");
        return (hash(Plain(elle::WeakBuffer(buffer)), algorithm));
      }
    }
  }
}

#endif
