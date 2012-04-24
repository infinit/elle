#ifndef  ELLE_NETWORK_BUNDLE_SERIALIZE_HXX
# define ELLE_NETWORK_BUNDLE_SERIALIZE_HXX

# include <elle/network/Bundle.hh>

# include <elle/serialize/ArchiveSerializer.hxx>

namespace elle
{
  namespace serialize
  {

    template<
        elle::network::Tag const G
      , typename... T
    >
    struct ArchiveSerializer<
        elle::network::Bundle::Inputs<
            G
          , elle::radix::Parameters<T...>
        >
    >
    {
      // Alias of the specialized type
      typedef elle::network::Bundle::Inputs<
          G
        , elle::radix::Parameters<T...>
      >                                       Type;

      template<typename Archive>
        static void Serialize(Archive& ar, Type& value, unsigned int version)
        {
          static_assert(false, "biet");
        }
    };


    template<
        elle::network::Tag const G
      , typename... T
    >
    struct ArchiveSerializer<
        elle::network::Bundle::Outputs<
            G
          , elle::radix::Parameters<T...>
        >
    >
    {
      // Alias of the specialized type
      typedef elle::network::Bundle::Outputs<
          G
        , elle::radix::Parameters<T...>
      >                                       Type;

      template<typename Archive>
        static void Serialize(Archive& ar, Type& value, unsigned int version)
        {
          static_assert(false, "biet");
        }
    };
  }
}

#endif
