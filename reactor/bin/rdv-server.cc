#include <boost/optional.hpp>

#include <elle/serialization/json.hh>

#include <reactor/network/rdv.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/scheduler.hh>


ELLE_LOG_COMPONENT("rdv.server");

using namespace reactor::network;

std::unordered_map<std::string, rdv::Endpoint> peers;
reactor::network::UDPSocket* server = nullptr;

static void send_with_magik(elle::Buffer const& b, rdv::Endpoint peer)
{
  elle::Buffer data(reactor::network::rdv::rdv_magic, 8);
  data.append(b.contents(), b.size());
  server->send_to(elle::ConstWeakBuffer(data), peer);
}

static void run(int argc, char** argv)
{
  int port = 7890;
  if (argc >1)
    port = std::stoi(argv[1]);
  reactor::network::UDPSocket srv;
  server = &srv;
  srv.close();
  srv.bind(
    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
  elle::Buffer buf;
  while (true)
  {
    buf.size(5000);
    rdv::Endpoint source;
    int sz = srv.receive_from(elle::WeakBuffer(buf), source);
    buf.size(sz);
    if (memcmp(buf.contents(), rdv::rdv_magic, 8) == 0)
    {
      memmove(buf.mutable_contents(), buf.contents()+8, buf.size()-8);
      buf.size(sz - 8);
    }
    rdv::Message req;
    try
    {
      req = elle::serialization::json::deserialize<rdv::Message>(buf, false);

      peers[req.id] = source;
      rdv::Message reply;
      reply.id = req.id;
      reply.source_endpoint = source;
      ELLE_TRACE("Got %s packet from %s", (int)req.command, source);
      switch(req.command)
      {
      case rdv::Command::ping:
        reply.command = rdv::Command::pong;
        break;
      case rdv::Command::pong:
        break;
      case rdv::Command::connect:
        {
          reply.command = rdv::Command::connect;
          ELLE_ASSERT(req.target_address);
          reply.target_address = req.target_address;
          auto peer = peers.find(*req.target_address);
          if (peer != peers.end())
          {
            ELLE_TRACE("Found peer at %s", peer->second);
            reply.target_endpoint = peer->second;
            rdv::Message other;
            other.command = rdv::Command::connect_requested;
            other.id = *req.target_address;
            other.source_endpoint = peer->second;
            other.target_address = req.id;
            other.target_endpoint = source;
            elle::Buffer b = elle::serialization::json::serialize(other, false);
            send_with_magik(b, peer->second);
          }
        }
        break;
      case rdv::Command::connect_requested:
      case rdv::Command::error:
        ELLE_LOG("unexpected connect_requested");
        break;
      }
      elle::Buffer b = elle::serialization::json::serialize(reply, false);
      send_with_magik(b, source);
    }
    catch (elle::Error const& e)
    {
      ELLE_LOG("Exception handling packet: %s", e);
      rdv::Message reply;
      reply.id = req.id;
      reply.command = rdv::Command::error;
      reply.target_address = e.what();
      elle::Buffer b = elle::serialization::json::serialize(reply, false);
      send_with_magik(b, source);
    }
  }
}

int main(int argc, char** argv)
{
  reactor::Scheduler sched;
  reactor::Thread t(sched, "main", [&]
    {
      run(argc, argv);
    });
  sched.run();
}
