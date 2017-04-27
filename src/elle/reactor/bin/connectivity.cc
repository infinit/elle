#include <algorithm>
#include <iostream>
#include <vector>

#include <elle/Exception.hh>
#include <elle/algorithm.hh>
#include <elle/network/Interface.hh>

#include <elle/reactor/network/upnp.hh>
#include <elle/reactor/scheduler.hh>

#include <elle/reactor/connectivity/connectivity.hh>

static
std::string
nated(std::vector<std::string> public_ips,
      elle::reactor::connectivity::Result const& res)
{
  std::string output = elle::contains(public_ips, res.host) ? "direct" : "nated";
  if (res.local_port)
  {
    output += " ";
    auto same_port = res.local_port == res.remote_port;
    output += same_port ? "on same port" : "on changed port";
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
  auto const host = argv[1];
  auto const port = argc < 3 ? 5456 : std::stoi(argv[2]);

  auto interfaces = elle::network::Interface::get_map(
    elle::network::Interface::Filter::no_loopback);
  std::cerr << "Local IP Addresses:\n";
  auto public_ips = std::vector<std::string>{};
  for (auto const& i: interfaces)
  {
    elle::push_back(public_ips, i.second.ipv4_address);
    elle::push_back(public_ips, i.second.ipv6_address);
  }
  for (auto const& i: public_ips)
    std::cerr << i << std::endl;

  std::cerr << "\nConnectivity:\n";
  std::cerr.flush();
  using Func
    = std::function<elle::reactor::connectivity::Result(
                       std::string const& host,
                       uint16_t port)>;
  auto report = [&] (std::string name, Func const& func, int deltaport = 0)
  {
    std::cerr << "  " << name << " ";
    try
    {
      elle::reactor::Thread t(name, [&] {
          auto address = func(host, port + deltaport);
          std::cerr << "OK: " << nated(public_ips, address) << std::endl;
      });
      if (!elle::reactor::wait(t, 5_sec))
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
  report("TCP", elle::reactor::connectivity::tcp);
  report("UDP", elle::reactor::connectivity::udp);
  report("UTP-XOR", [](std::string const& host, int port)
         {
           return elle::reactor::connectivity::utp(host, port, 0xFF);
         }, 2);
  report("UTP",  [](std::string const& host, int port)
         {
           return elle::reactor::connectivity::utp(host, port, 0);
         }, 1);
  report("RDV UTP XOR", [](std::string const& host, int port)
         {
           return elle::reactor::connectivity::rdv_utp(host, port, 0xFF);
         }, 2);
  report("RDV UTP", [](std::string const& host, int port)
         {
           return elle::reactor::connectivity::rdv_utp(host, port, 0);
         }, 1);
  std::cerr << "  NAT ";
  try
  {
    std::cerr << elle::reactor::connectivity::nat(host, port) << std::endl;
  }
  catch (std::runtime_error const&)
  {
    std::cerr << elle::exception_string() << std::endl;
  }

  std::cerr << "\nUPNP:\n";
  auto upnp = elle::reactor::network::UPNP::make();
  try
  {
    upnp->initialize();
    std::cerr << "  available: " << upnp->available() << std::endl;
    auto ip = upnp->external_ip();
    std::cerr << "  external_ip: " << ip << std::endl;
    auto pm = upnp->setup_redirect(elle::reactor::network::Protocol::tcp, 5678);
    std::cerr << "  mapping: " << pm.internal_host << ':' << pm.internal_port
      << " -> " << pm.external_host << ':' << pm.external_port << std::endl;
    auto pm2 = upnp->setup_redirect(elle::reactor::network::Protocol::utp, 5679);
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
  elle::reactor::Scheduler sched;
  elle::reactor::Thread t(sched, "main", [&]
                          {
                            run(argc, argv);
                          });
  sched.run();
}
