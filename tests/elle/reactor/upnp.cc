#include <iostream>

#include <elle/log.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/Thread.hh>
#include <elle/reactor/network/upnp.hh>

ELLE_LOG_COMPONENT("elle.reactor.Scheduler");

int main(int argc, char** argv)
{
  int lport = 0;
  int eport;
  if (argc < 3 || argc > 4 || argv[1][0] == '-')
  {
    std::cerr << "Usage: " << argv[0] << " (tcp|utp) local_port [external_port_hint]" << std::endl;
    exit(0);
  }
  elle::reactor::Barrier signal;
  elle::reactor::network::Protocol p = elle::reactor::network::Protocol::tcp;
  if (argv[1] == std::string("utp"))
    p = elle::reactor::network::Protocol::utp;
  lport = boost::lexical_cast<int>(argv[2]);
  if (argc > 3)
    eport = boost::lexical_cast<int>(argv[3]);
  (void)eport;
  elle::reactor::Scheduler sched;
  elle::reactor::Thread t(sched, "upnp_start", [&]() {
    try
    {
      ELLE_LOG("requesting mapping....");
      auto upnp = elle::reactor::network::UPNP::make();
      upnp->initialize();
      try
      {
        ELLE_LOG("external ip: %s", upnp->external_ip());
      }
      catch(...)
      {
        ELLE_WARN("External IP unavailable: %s", elle::exception_string());
      }
      elle::reactor::network::PortMapping mapping = upnp->setup_redirect(p, lport);
      ELLE_LOG("got redirection: %s", mapping);
      ELLE_LOG("Mapping acquired, hit ^c to terminate");
      signal.wait();
      ELLE_LOG("Closing mapping");
    }
    catch(std::exception const& e)
    {
      ELLE_ERR("Redirect failure: %s", e.what());
    }
  });
  sched.signal_handle(SIGTERM, [&]() {std::cerr<<"terminating..."<<std::endl;new elle::reactor::Thread(sched,"signal", [&]() { std::cerr<<"pan"<<std::endl;signal.open();}, true); });
  sched.signal_handle(SIGINT, [&]()  {std::cerr<<"terminating..."<<std::endl;new elle::reactor::Thread(sched,"signal", [&]() { std::cerr<<"pan"<<std::endl;signal.open();}, true); });
  sched.run();
}
