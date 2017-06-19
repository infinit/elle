#include <elle/reactor/network/rdv-socket.hh>
#include <elle/reactor/network/rdv.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("rdvcat");

using namespace elle::reactor::network;


elle::reactor::network::RDVSocket* rdvsocket = nullptr;
rdv::Endpoint remote;

static void send(std::string const& data)
{
  while (rdvsocket == nullptr)
    elle::reactor::sleep(100_ms);
  rdvsocket->send_to(elle::ConstWeakBuffer(data), remote);
}

static void run(int argc, char**argv)
{
  std::string rdvhost = argv[1];
  int rdvport = 7890;
  std::string id(argv[2]);
  std::string remoteid(argv[3]);
  elle::reactor::network::RDVSocket socket;
  socket.close();
  socket.bind(
    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
  elle::reactor::Thread poller("poll", [&]
    {
      while (true)
      {
        auto buf = elle::Buffer{};
        buf.size(5000);
        rdv::Endpoint ep;
        int sz = socket.receive_from(elle::WeakBuffer(buf), ep);
        std::cout.write((const char*)buf.contents(), sz);
        std::cout << std::endl;
      }
    });
  socket.rdv_connect(id, rdvhost, rdvport);
  rdv::Endpoint target = socket.contact(remoteid);
  rdvsocket = &socket;
  remote = target;
  elle::reactor::sleep();
}

int main(int argc, char** argv)
{
  elle::reactor::Scheduler sched;
  elle::reactor::Thread t(sched, "main", [&]
    {
      run(argc, argv);
    });
  std::thread rs([&] {
      while (std::cin.good())
      {
        std::string buf;
        std::getline(std::cin, buf);
        sched.mt_run<void>("data",
          std::function<void()>([&] { send(buf);}));
      }
  });
  sched.run();
  rs.join();
}
