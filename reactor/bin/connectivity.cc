#include <reactor/network/rdv.hh>

#include <elle/network/Interface.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/network/udp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/utp-socket.hh>
#include <reactor/network/upnp.hh>
#include <reactor/scheduler.hh>

static std::vector<std::string> public_ips;

static std::string nated(std::string const& host_port, int local_port=0)
{
  auto pos = host_port.find(':');
  auto host = host_port.substr(0, pos);
  std::string res;
  if (std::find(public_ips.begin(), public_ips.end(), host) != public_ips.end())
  {
    res = "DIRECT " + host;
  }
  else
    res = "NATED " + host;
  if (local_port)
  {
    if (local_port == std::stoi(host_port.substr(pos+1)))
      res += " SAME_PORT";
    else
      res += " PORT_CHANGED";
  }
  return res;
}

static void tcp(std::string const& host, int port)
{
  try
  {
    reactor::network::TCPSocket s(host, port, 5_sec);
    s << "foo" << std::endl;
    std::string line;
    std::getline(s, line);
    auto p = line.find(' ');
    if (line.substr(p+1) != "foo")
      throw std::runtime_error("Unexpected output: " + line);
    std::cerr << "TCP OK " << nated(line.substr(0, p), s.local_endpoint().port()) << std::endl;
  }
  catch (std::exception const& e)
  {
    std::cerr << "TCP FAIL " << e.what() << std::endl;
  }
}

static void udp(std::string const& host, int port)
{
  try
  {
    reactor::network::UDPSocket s;
    s.close();
    s.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
    auto server_ep = reactor::network::resolve_udp(host, std::to_string(port));
    s.send_to(std::string("foo\n"), server_ep);
    boost::asio::ip::udp::endpoint ep;
    char buffer[5000];
    auto sz = s.receive_from(reactor::network::Buffer(buffer, 5000), ep, 5_sec);
    std::string line(buffer, sz);
    auto p = line.find(' ');
    if (line.substr(p+1) != "foo\n")
      throw std::runtime_error("Unexpected output: " + line);
    std::cerr << "UDP OK " << nated(line.substr(0, p), s.local_endpoint().port()) << std::endl;
  }
  catch (std::exception const& e)
  {
    std::cerr << "UDP FAIL " << e.what() << std::endl;
  }
}

static void nat(std::string const& host, int port)
{ // check cone-nat by comparing our endpoints from rdv server and connectivity server
  try
  {
    reactor::network::RDVSocket socket;
    socket.close();
    socket.bind(
      boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
    // we must be polling for rdv_connect to work
    std::string result;
    bool success = true;
    reactor::Thread poller("poll", [&]
    {
      while (true)
      {
        elle::Buffer buf;
        buf.size(5000);
        boost::asio::ip::udp::endpoint ep;
        int sz = socket.receive_from(reactor::network::Buffer(buf.mutable_contents(),
                                                              buf.size()), ep);
        buf.size(sz);
        std::string line(buf.string());
        auto p = line.find(' ');
        if (line.substr(p+1) != "foo\n")
        {
          success = false;
          result = "Unexpected output: " + line;
        }
        else
        {
          bool cone = (line.substr(0, p) == elle::sprintf("%s", socket.public_endpoint()));
          result = "UDP-RDV OK";
          if (cone)
            result += " CONE NAT";
          else
            result += elle::sprintf(" NOT_CONE NAT %s %s", line.substr(0, p),
            socket.public_endpoint());
        }
        break;
      }
    });
    elle::SafeFinally spoll([&] {
        poller.terminate_now();
    });
    socket.rdv_connect("connectivity-client-udp",
      reactor::network::resolve_udp("rdv.infinit.sh", "7890"),
      5_sec);
    auto ep = reactor::network::resolve_udp(host, std::to_string(port));
    socket.send_to(std::string("foo\n"), ep);
    reactor::wait(poller);
    spoll.abort();
    if (!success)
      throw std::runtime_error(result);
    std::cerr << result << std::endl;
  }
  catch (std::exception const& e)
  {
    std::cerr << "UDP-RDV FAIL " << e.what() << std::endl;
  }
}

static void rdv_utp(std::string const& host, int port)
{
  try
  {
    reactor::network::UTPServer server;
    server.listen(0);
    server.rdv_connect("connectivity", "rdv.infinit.sh:7890", 5_sec);
    reactor::network::UTPSocket s(server);
    s.connect("connectivity-server", {}, 5_sec);
    s << "foo" << std::endl;
    elle::Buffer reply = s.read_until("\n", 5_sec);
    std::string line = reply.string();
    auto p = line.find(' ');
    if (line.substr(p+1) != "foo\n")
      throw std::runtime_error("Unexpected output: " + line);
    std::cerr << "UTP-RDV OK " << nated(line.substr(0, p), server.local_endpoint().port()) << std::endl;
  }
  catch (std::exception const& e)
  {
    std::cerr << "UTP-RDV FAIL " << e.what() << std::endl;
  }
}
static void run(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage: connectivity server_host [port]" << std::endl;
    return;
  }
  std::string host = argv[1];
  int port = 5456;
  if (argc >= 3)
    port = std::stoi(argv[2]);

  auto interfaces = elle::network::Interface::get_map(
    elle::network::Interface::Filter::no_loopback);
  std::cerr << "Local IP Addresses:" << std::endl;
  for (auto i: interfaces)
  {
    if (i.second.ipv4_address.empty())
      continue;
    std::cerr << i.second.ipv4_address << std::endl;
    public_ips.push_back(i.second.ipv4_address);
  }

  std::cerr << "\nConnectivity:" << std::endl;
  tcp(host, port);
  udp(host, port);
  nat(host, port);
  rdv_utp(host, port+1);

  std::cerr << "\nUPNP:" << std::endl;
  auto upnp = reactor::network::UPNP::make();
  try
  {
    upnp->initialize();
    std::cerr << "available: " << upnp->available() << std::endl;
    auto ip = upnp->external_ip();
    std::cerr << "external_ip: " << ip << std::endl;
    auto pm = upnp->setup_redirect(reactor::network::Protocol::tcp, 5678);
    std::cerr << "mapping: " << pm.internal_host << ':' << pm.internal_port
      << " -> " << pm.external_host << ':' << pm.external_port << std::endl;
    auto pm2 = upnp->setup_redirect(reactor::network::Protocol::udt, 5679);
    std::cerr << "mapping: " << pm2.internal_host << ':' << pm2.internal_port
      << " -> " << pm2.external_host << ':' << pm2.external_port << std::endl;
  }
  catch (std::exception const& e)
  {
    std::cerr << "UPNP exception: " << e.what() << std::endl;
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
