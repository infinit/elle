#ifndef ELLE_NETWORK_BUNDLE_HXX
#define ELLE_NETWORK_BUNDLE_HXX

namespace elle
{
  using namespace radix;

  namespace network
  {

//
// ---------- inputs bundle ---------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <const Tag G,
              typename... T>
    Bundle::Inputs< G,
                    Parameters<T...> >::Inputs(const T&...      objects):
      tag(G),
      arguments(objects...)
    {
    }

    ///
    /// a specific constructor for ensembles i.e Arguments or Variables
    /// for instance.
    ///
    template <const Tag G,
              typename... T>
    template <template <typename...> class E,
              typename... U>
    Bundle::Inputs< G,
                    Parameters<T...> >::Inputs(
                                               E<
                                                 Parameters<
                                                   U...
                                                   >
                                                 >&             ensemble):
      tag(G),
      arguments(ensemble)
    {
    }

    ///
    /// this method dumps the bundle.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Inputs< G,
                    Parameters<T...> >::Dump(const Natural32    margin)
      const
    {
      String    alignment(margin, ' ');

      std::cout << alignment << "[Bundle] Inputs" << std::endl;

      // dump the tag.
      std::cout << alignment << io::Dumpable::Shift
                << "[Tag] " << static_cast<Natural32>(this->tag) << std::endl;

      // dump the arguments.
      if (this->arguments.Dump() == Status::Error)
        escape("unable to dump the arguments");

      return Status::Ok;
    }

//
// ---------- outputs bundle --------------------------------------------------
//

    ///
    /// default constructor.
    ///
    template <const Tag G,
              typename... T>
    Bundle::Outputs< G,
                     Parameters<T...> >::Outputs(T&...          objects):
      tag(G),
      arguments(objects...)
    {
    }

    ///
    /// a specific constructor for ensembles i.e Arguments or Variables
    /// for instance.
    ///
    template <const Tag G,
              typename... T>
    template <template <typename...> class E,
              typename... U>
    Bundle::Outputs< G,
                     Parameters<T...> >::Outputs(
                                           E<
                                             Parameters<
                                               U...
                                               >
                                             >&                 ensemble):
      tag(G),
      arguments(ensemble)
    {
    }

    ///
    /// this method dumps the bundle.
    ///
    template <const Tag G,
              typename... T>
    Status
    Bundle::Outputs< G,
                     Parameters<T...> >::Dump(const Natural32 margin)
      const
    {
      String    alignment(margin, ' ');

      std::cout << alignment << "[Bundle] Outputs" << std::endl;

      // dump the tag.
      std::cout << alignment << io::Dumpable::Shift
                << "[Tag] " << static_cast<Natural32>(this->tag) << std::endl;

      // dump the arguments.
      if (this->arguments.Dump() == Status::Error)
        escape("unable to dump the arguments");

      return Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

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
          _ELLE_SERIALIZE_LOG_ACTION(
            _(elle::network::Bundle::Inputs<G, elle::radix::Parameters<T...>>),
            version, Archive::mode, value
          )
          {
            ar & value.arguments;
          }
        }
    };


    template<
        elle::network::Tag const G
      , typename... T
    >
    struct StoreClassVersion<
        elle::network::Bundle::Inputs<
            G
          , elle::radix::Parameters<T...>
        >
    >
    {
      static bool const value = false;
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
          _ELLE_SERIALIZE_LOG_ACTION(
            _(elle::network::Bundle::Outputs<G, elle::radix::Parameters<T...>>),
            version, Archive::mode, value
          )
            {
              ar & value.arguments;
            }
        }
    };

    template<
        elle::network::Tag const G
      , typename... T
    >
    struct StoreClassVersion<
        elle::network::Bundle::Outputs<
            G
          , elle::radix::Parameters<T...>
        >
    >
    {
      static bool const value = false;
    };

  }
}

#endif
