#ifndef  ELLE_NETWORK_BUNDLE_SERIALIZE_HXX
# define ELLE_NETWORK_BUNDLE_SERIALIZE_HXX

# include <elle/radix/ArgumentsSerializer.hxx>

# include <elle/network/Bundle.hh>

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
          assert(version == 0);
          ar & value.tag;
          ar & value.arguments;
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
          assert(version == 0);
          ar & value.tag;
          ar & value.arguments;
        }
    };
  }
}

#endif
