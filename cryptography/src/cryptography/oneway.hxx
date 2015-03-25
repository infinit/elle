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

        static_assert(std::is_same<T, Plain>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::Buffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                      "this call should never have occured");

        elle::Buffer buffer;
        ELLE_MEASURE("Serialize into the buffer")
          buffer.writer() << value;

        ELLE_MEASURE_SCOPE("Hash the buffer");
        return (hash(Plain(buffer), algorithm));
      }

      template <typename T>
      Digest
      hmac(T const& value,
           Digest const& key,
           Algorithm algorithm)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.oneway");
        ELLE_DEBUG_FUNCTION(value, algorithm);

        static_assert(std::is_same<T, Plain>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::Buffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                      "this call should never have occured");

        elle::Buffer buffer;
        ELLE_MEASURE("Serialize into the buffer")
          buffer.writer() << value;

        ELLE_MEASURE_SCOPE("HMAC the buffer");
        return (hmac(Plain(buffer), key, algorithm));
      }
    }
  }
}

#endif
