#include <reactor/network/rdv.hh>
#include <boost/optional.hpp>

#include <elle/serialization/json.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/scheduler.hh>


ELLE_LOG_COMPONENT("rdv-server");
    
std::unordered_map<std::string, Endpoint> peers;

static void run(int argc, char** argv)
{
  int port = 7890;
  if (argc >1)
    port = std::stoi(argv[1]);
  reactor::network::UDPSocket srv;
  srv.close();
  srv.bind(
    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
  elle::Buffer buf;
  while (true)
  {
    buf.size(5000);
    Endpoint source;
    int sz = srv.receive_from(reactor::network::Buffer(buf.mutable_contents(), buf.size()),
                                   source);
    buf.size(sz);
    elle::IOStream ios(buf.istreambuf());
    elle::serialization::json::SerializerIn sin(ios, false);
    Request req;
    sin.serialize_forward(req);
    peers[req.id] = source;
    Reply reply;
    reply.id = req.id;
    reply.source_endpoint = source;
    ELLE_TRACE("Got %s packet from %s", (int)req.command, source);
    switch(req.command)
    {
    case Command::ping:
      reply.command = Command::pong;
      break;
    case Command::pong:
      break;
    case Command::connect:
      {
        reply.command = Command::connect;
        reply.target_address = req.target;
        auto peer = peers.find(*req.target);
        if (peer != peers.end())
        {
          ELLE_TRACE("Found peer at %s", peer->second);
          reply.target_endpoint = peer->second;
          Reply other;
          other.command = Command::connect_requested;
          other.id = *req.target;
          other.source_endpoint = peer->second;
          other.target_address = req.id;
          other.target_endpoint = source;
          elle::Buffer b;
          {
            elle::IOStream ios(b.ostreambuf());
            elle::serialization::json::SerializerOut sout(ios, false);
            sout.serialize_forward(other);
          }
          srv.send_to(reactor::network::Buffer(b.contents(), b.size()), peer->second);
        }
      }
      break;
    case Command::connect_requested:
      ELLE_LOG("unexpected connect_requested");
      break;
    }
    elle::Buffer b;
    {
      elle::IOStream ios(b.ostreambuf());
      elle::serialization::json::SerializerOut sout(ios, false);
      sout.serialize_forward(reply);
    }
    srv.send_to(reactor::network::Buffer(b.contents(), b.size()), source);
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