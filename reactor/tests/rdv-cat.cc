#include <reactor/network/rdv.hh>

#include <elle/serialization/json.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("rdvcat");

reactor::network::UDPSocket* sock = nullptr;
Endpoint remote;

static void send(std::string const& buf)
{
  while (!sock)
    usleep(100000);
  sock->send_to(reactor::network::Buffer(buf), remote);
}

static void run(int argc, char** argv)
{
  std::string rdvhost = argv[1];
  int rdvport = 7890;
  std::string id(argv[2]);
  std::string remoteid(argv[3]);
  Endpoint server_ep(boost::asio::ip::address::from_string(rdvhost), rdvport);
  reactor::network::UDPSocket srv;
  srv.close();
  srv.bind(
    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
  Request req;
  req.id = id;
  req.command = Command::connect;
  req.target = remoteid;
  elle::Buffer buf = elle::serialization::json::serialize(req, false);
  ELLE_TRACE("sending connect request to %s", server_ep);
  srv.send_to(reactor::network::Buffer(buf.contents(), buf.size()), server_ep);

  while (true)
  {
    elle::Buffer buf;
    buf.size(5000);
    Endpoint ep;
    int sz = srv.receive_from(reactor::network::Buffer(buf.mutable_contents(),
                                                       buf.size()), ep);
    buf.size(sz);
    if (ep.port() == 7890)
    { // rdv packet
      Reply reply;
      reply = elle::serialization::json::deserialize<Reply>(buf, false);
      switch (reply.command)
      {
      case Command::ping:
      case Command::pong:
        break;
      case Command::connect:
      case Command::connect_requested:
        
        if (reply.target_endpoint)
        {
          ELLE_TRACE("got connect ep=%s async=%s",
                     *reply.target_endpoint, reply.command == Command::connect_requested);
          remote = *reply.target_endpoint;
          sock = &srv;
          srv.send_to(reactor::network::Buffer("RDV PING"), remote);
        }
        else
          ELLE_TRACE("got connect without endpoint");
        break;
      }
    }
    else if (buf == "RDV PING")
    {
      ELLE_TRACE("Got ping packet");
    }
    else
    { // chat packet
      std::cout.write((const char*)buf.contents(), buf.size());
      std::cout << std::endl;
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
  std::thread rs([&] {
      while (std::cin.good())
      {
        std::string buf;
        std::getline(std::cin, buf);
        sched.mt_run("data",
          boost::function<void()>([&] { send(buf);}));
      }
  });
  sched.run();
  rs.join();
}