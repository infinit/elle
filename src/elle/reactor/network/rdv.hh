#include <boost/optional.hpp>
#include <boost/asio.hpp>

#include <elle/serialization/json.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      namespace rdv
      {
        using Endpoint = boost::asio::ip::udp::endpoint;

        constexpr char const* rdv_magic = "RDVMAGIK"; // 8 bytes

        enum class Command
        {
          ping,
          pong,
          connect, // ask for a connect or reply to a connect
          connect_requested, // async connect request from other peer
          error, // error string in target_address
        };

        /// Message to be exchanged during RDV connection.
        struct Message
        {
          Message() {}
          Message(elle::serialization::SerializerIn& sin)
          {
            serialize(sin);
          }
          void serialize(elle::serialization::Serializer& s)
          {
            s.serialize("id", id);
            s.serialize("command", command, elle::serialization::as<int>());
            s.serialize("source_endpoint", source_endpoint);
            s.serialize("target_endpoint", target_endpoint);
            s.serialize("target_address", target_address);
          }
          std::string id;
          Command command;
          boost::optional<Endpoint> source_endpoint;
          boost::optional<Endpoint> target_endpoint;
          boost::optional<std::string>  target_address;
          using serialization_tag = elle::serialization_tag;
        };
      }
    }
  }

  namespace serialization
  {
    /// Conversion API to make reactor::network::rdv::Endpoint serializable.
    template<typename T>
    struct SerializeEndpoint
    {
      using Type = elle::Buffer;

      static
      Type
      convert(T& ep)
      {
        Type res;
        if (ep.address().is_v4())
        {
          auto addr = ep.address().to_v4().to_bytes();
          res.append(addr.data(), addr.size());
        }
        else
        {
          auto addr = ep.address().to_v6().to_bytes();
          res.append(addr.data(), addr.size());
        }
        unsigned short port = ep.port();
        res.append(&port, 2);
        return res;
      }

      static
      T
      convert(elle::Buffer& repr)
      {
        ELLE_ASSERT(repr.size() == 6 || repr.size() == 18);
        if (repr.size() == 6)
        {
          unsigned short port;
          memcpy(&port, &repr[4], 2);
          auto addr = boost::asio::ip::address_v4(
            std::array<unsigned char, 4>{{repr[0], repr[1], repr[2], repr[3]}});
          return T(addr, port);
        }
        else
        {
          unsigned short port;
          memcpy(&port, &repr[16], 2);
          auto addr = boost::asio::ip::address_v6(
            std::array<unsigned char, 16>{{
            repr[0], repr[1], repr[2], repr[3],
            repr[4], repr[5], repr[6], repr[7],
            repr[8], repr[9], repr[10], repr[11],
            repr[12], repr[13], repr[14], repr[15],
            }});
          return T(addr, port);
        }
      }
    };

    template<> struct  Serialize<reactor::network::rdv::Endpoint>
      : public SerializeEndpoint<reactor::network::rdv::Endpoint>
    {};
  }
}
