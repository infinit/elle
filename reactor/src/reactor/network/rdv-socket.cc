#include <reactor/network/rdv-socket.hh>

#include <reactor/network/resolve.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/rdv.hh>
#include <reactor/network/exception.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {

    using Endpoint = boost::asio::ip::udp::endpoint;
    RDVSocket::RDVSocket()
    {}

    void RDVSocket::rdv_connect(std::string const& id,
                                std::string const& rdv_host, int rdv_port,
                                DurationOpt timeout)
    {
      rdv_connect(id, resolve_udp(rdv_host, std::to_string(rdv_port)), timeout);
    }
    void RDVSocket::rdv_connect(std::string const& id, Endpoint ep, DurationOpt timeout)
    {
      _server_reached.close();
      _server = ep;
      rdv::Request req;
      req.command = rdv::Command::ping;
      req.id = id;
      elle::Buffer buf = elle::serialization::json::serialize(req, false);
      auto now = boost::posix_time::second_clock::universal_time();
      while (true)
      {
        send_to(Buffer(buf), ep);
        if (reactor::wait(_server_reached, 500_ms))
          return;
        if (timeout
          && boost::posix_time::second_clock::universal_time() - now > *timeout)
          throw TimeOut();
      }
    }
    Size RDVSocket::receive_from(Buffer buffer,
        boost::asio::ip::udp::endpoint &endpoint,
        DurationOpt timeout)
    {
      while (true)
      {
        Size sz = UDPSocket::receive_from(buffer, endpoint, timeout);
        if (sz < 8)
          return sz;
        if (endpoint == _server)
          _server_reached.open();
        if (!memcmp(rdv::rdv_magic, buffer.data(), 8))
        {
          rdv::Reply repl = elle::serialization::json::deserialize<rdv::Reply>(
            elle::Buffer(buffer.data()+8, sz - 8), false);
          switch (repl.command)
          {
          case rdv::Command::ping:
            {
              rdv::Reply reply;
              reply.id = _id;
              reply.command = rdv::Command::pong;
              reply.source_endpoint = endpoint;
              elle::Buffer buf = elle::serialization::json::serialize(reply, false);
              send_with_magik(buf, endpoint);
            }
            break;
          case rdv::Command::pong:
            {
              auto it = _contacts.find(repl.id);
              if (it != _contacts.end())
              {
                it->second.result = endpoint;
                it->second.barrier->open();
              }
            }
            break;
          case rdv::Command::connect:
            {
              auto it = _contacts.find(repl.id);
              if (it != _contacts.end())
              {
                if (repl.target_endpoint)
                {
                  // give it a ping
                  rdv::Reply ping;
                  ping.command = rdv::Command::ping;
                  ping.id = _id;
                  ping.source_endpoint = _self;
                  elle::Buffer buf = elle::serialization::json::serialize(ping, false);
                  send_with_magik(buf, endpoint);
                }
                else
                { // nothing to do, contact() will resend periodically
                }
              }
            }
            break;
          case rdv::Command::connect_requested:
            { // send a packet to breach
              rdv::Reply ping;
              ping.command = rdv::Command::ping;
              ping.id = _id;
              ping.source_endpoint = _self;
              elle::Buffer buf = elle::serialization::json::serialize(ping, false);
              send_with_magik(buf, *repl.target_endpoint);
            }
            break;
          }
        }
        else
          return sz;
      }
    }
    Endpoint RDVSocket::contact(std::string const& id,
        std::vector<Endpoint> const& endpoints,
        DurationOpt timeout)
    {
      if (_contacts.find(id) != _contacts.end())
        throw elle::Error("contact already in progress");
      ContactInfo& ci = _contacts[id];
      ci.barrier.reset(new Barrier());
      ci.barrier->close();
      auto now = boost::posix_time::second_clock::universal_time();
      while (true)
      {
        if (!endpoints.empty())
        { // try known endpoints
          rdv::Reply ping;
          ping.command = rdv::Command::ping;
          ping.id = _id;
          ping.source_endpoint = _self;
          elle::Buffer buf = elle::serialization::json::serialize(ping, false);
          for (auto const& ep: endpoints)
            send_with_magik(buf, ep);
        }
        // try establishing link through rdv
        rdv::Request req;
        req.command = rdv::Command::connect;
        req.id = _id;
        req.target = id;
        elle::Buffer buf = elle::serialization::json::serialize(req, false);
        send_to(buf, _server);
        if (reactor::wait(*ci.barrier, 500_ms))
        {
          auto res = _contacts[id].result;
          _contacts.erase(id);
          return res;
        }
        if (timeout
          && boost::posix_time::second_clock::universal_time() - now > *timeout)
          throw TimeOut();
      }
    }

    void RDVSocket::send_with_magik(elle::Buffer const& b, Endpoint peer)
    {
      elle::Buffer data;
      data.append(reactor::network::rdv::rdv_magic, 8);
      data.append(b.contents(), b.size());
      send_to(Buffer(data.contents(), data.size()), peer);
    }
  }
}