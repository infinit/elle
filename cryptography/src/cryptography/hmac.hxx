#ifndef INFINIT_CRYPTOGRAPHY_HMAC_HXX
# define INFINIT_CRYPTOGRAPHY_HMAC_HXX

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
    /*----------.
    | Functions |
    `----------*/

    template <typename T>
    Digest
    hmac(T const& value,
         Digest const& key,
         Oneway oneway)
    {
      ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
      ELLE_DEBUG_FUNCTION(value, oneway);

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

      ELLE_MEASURE_SCOPE("hmac the buffer");

      return (hmac(Plain(buffer), key, oneway));
    }
  }
}

#endif
