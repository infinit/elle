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
    namespace hmac
    {
      /*----------.
      | Functions |
      `----------*/

      template <typename T>
      Digest
      sign(T const& value,
           elle::String const& key,
           Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_DEBUG_FUNCTION(oneway);
        ELLE_DUMP("value: %x", value);

        static_assert(std::is_same<T, Plain>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::Buffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                      "this call should never have occured");

        elle::Buffer _value;
        ELLE_MEASURE("serialize the value")
          _value.writer() << value;

        ELLE_MEASURE_SCOPE("sign the value with the key");

        return (sign(Plain(_value), key, oneway));
      }

      template <typename T>
      elle::Boolean
      verify(Digest const& digest,
             T const& value,
             elle::String const& key,
             Oneway const oneway)
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.hmac");
        ELLE_DEBUG_FUNCTION(oneway);
        ELLE_DUMP("digest: %x", digest);
        ELLE_DUMP("value: %x", value);

        static_assert(std::is_same<T, Plain>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::Buffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                      "this call should never have occured");
        static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                      "this call should never have occured");

        elle::Buffer _value;
        ELLE_MEASURE("serialize the value")
          _value.writer() << value;

        ELLE_MEASURE_SCOPE("verify the value with the key");

        return (verify(digest, Plain(_value), key, oneway));
      }
    }
  }
}

#endif
