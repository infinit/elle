#include <algorithm>
#include <iostream>
#include <vector>

#include <elle/Exception.hh>
#include <elle/network/Interface.hh>

#include <reactor/network/upnp.hh>
#include <reactor/scheduler.hh>

#include <reactor/connectivity/connectivity.hh>

static
std::string
nated(std::vector<std::string> public_ips,
      reactor::connectivity::Result const& res)
{
  auto direct = std::find(
    public_ips.begin(), public_ips.end(), res.host) != public_ips.end();
  std::string output = direct ? "direct" : "nated";
  if (res.local_port)
  {
    output += " ";
    if (res.local_port)
    {
      auto same_port = res.local_port == res.remote_port;
      output += same_port ? "on same port" : "on changed port";
    }
  }
  return output;
}

static
void
run(int argc, char** argv)
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
  std::vector<std::string> public_ips;
  for (auto i: interfaces)
  {
    if (i.second.ipv4_address.empty())
      continue;
    std::cerr << "  " << i.second.ipv4_address << std::endl;
    public_ips.push_back(i.second.ipv4_address);
  }

  std::cerr << "\nConnectivity:" << std::endl;
  auto report = [&] (
    std::string name,
    std::function<reactor::connectivity::Result(
                       std::string const& host,
                       uint16_t port)> const& func,
    int deltaport = 0)
  {
    std::cerr << "  " << name << " ";
    try
    {
      reactor::Thread t(name, [&] {
          auto address = func(host, port + deltaport);
          std::cerr << "OK: " << nated(public_ips, address) << std::endl;
      });
      if (!reactor::wait(t, 5_sec))
      {
        t.terminate_now();
        throw std::runtime_error("timeout");
      }
    }
    catch (...)
    {
      std::cerr << "NO: " << elle::exception_string() << std::endl;
    }
  };
  report("TCP", reactor::connectivity::tcp);
  report("UDP", reactor::connectivity::udp);
  report("UTP-XOR", std::bind(reactor::connectivity::utp,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              0xFF), 2);
  report("UTP", std::bind(reactor::connectivity::utp,
                              std::placeholders::_1,
                              std::placeholders::_2,
                              0), 1);
  report("RDV UTP XOR", std::bind(reactor::connectivity::rdv_utp,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  0xFF), 2);
  report("RDV UTP", std::bind(reactor::connectivity::rdv_utp,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  0), 1);
  std::cerr << "  NAT ";
  try
  {
    std::cerr << reactor::connectivity::nat(host, port) << std::endl;
  }
  catch (std::runtime_error const&)
  {
    std::cerr << elle::exception_string() << std::endl;
  }

  std::cerr << std::endl << "UPNP:" << std::endl;
  auto upnp = reactor::network::UPNP::make();
  try
  {
    upnp->initialize();
    std::cerr << "  available: " << upnp->available() << std::endl;
    auto ip = upnp->external_ip();
    std::cerr << "  external_ip: " << ip << std::endl;
    auto pm = upnp->setup_redirect(reactor::network::Protocol::tcp, 5678);
    std::cerr << "  mapping: " << pm.internal_host << ':' << pm.internal_port
      << " -> " << pm.external_host << ':' << pm.external_port << std::endl;
    auto pm2 = upnp->setup_redirect(reactor::network::Protocol::utp, 5679);
    std::cerr << "  mapping: " << pm2.internal_host << ':' << pm2.internal_port
      << " -> " << pm2.external_host << ':' << pm2.external_port << std::endl;
  }
  catch (std::exception const& e)
  {
    std::cerr << "  exception: " << e.what() << std::endl;
  }
}


int
main(int argc, char** argv)
{
  reactor::Scheduler sched;
  reactor::Thread t(sched, "main", [&]
    {
      run(argc, argv);
    });
  sched.run();
}
