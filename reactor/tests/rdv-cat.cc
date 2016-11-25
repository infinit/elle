#include <reactor/network/rdv.hh>
#include <reactor/network/rdv-socket.hh>

#include <reactor/network/buffer.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("rdvcat");

using namespace reactor::network;


reactor::network::RDVSocket* rdvsocket = nullptr;
rdv::Endpoint remote;

static void send(std::string const& data)
{
  while (rdvsocket == nullptr)
    reactor::sleep(100_ms);
  rdvsocket->send_to(Buffer(data), remote);
}

static void run(int argc, char**argv)
{
  std::string rdvhost = argv[1];
  int rdvport = 7890;
  std::string id(argv[2]);
  std::string remoteid(argv[3]);
  reactor::network::RDVSocket socket;
  socket.close();
  socket.bind(
    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
  reactor::Thread poller("poll", [&]
    {
      while (true)
      {
        elle::Buffer buf;
        buf.size(5000);
        rdv::Endpoint ep;
        int sz = socket.receive_from(reactor::network::Buffer(buf.mutable_contents(),
                                                              buf.size()), ep);
        std::cout.write((const char*)buf.contents(), sz);
        std::cout << std::endl;
      }
    });
  socket.rdv_connect(id, rdvhost, rdvport);
  rdv::Endpoint target = socket.contact(remoteid);
  rdvsocket = &socket;
  remote = target;
  reactor::sleep();
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
        sched.mt_run<void>("data",
          std::function<void()>([&] { send(buf);}));
      }
  });
  sched.run();
  rs.join();
}
