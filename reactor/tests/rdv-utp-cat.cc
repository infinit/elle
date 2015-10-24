#include <reactor/network/rdv.hh>
#include <reactor/network/utp-socket.hh>

#include <reactor/network/buffer.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("rdvcat");

using namespace reactor::network;


reactor::network::UTPSocket* utpsocket = nullptr;

static void send(std::string const& data)
{
  while (utpsocket == nullptr)
    reactor::sleep(100_ms);
  ELLE_TRACE("pushing data");
  utpsocket->write(elle::ConstWeakBuffer(data));
}

static void run(int argc, char**argv)
{
  std::string rdvhost = argv[1];
  int rdvport = 7890;
  std::string id(argv[2]);
  std::string remoteid(argv[3]);
  reactor::network::UTPServer server;
  server.listen(0);
  server.rdv_connect(id, rdvhost + ":" + std::to_string(rdvport));
  std::unique_ptr<reactor::network::UTPSocket> socket;
  if (id < remoteid)
  {
    ELLE_TRACE("accepting...");
    socket = server.accept();
    ELLE_TRACE("...accepted");
  }
  else
  {
    socket.reset(new reactor::network::UTPSocket(server));
    ELLE_TRACE("connecting...");
    socket->connect(remoteid);
    ELLE_TRACE("...connected");
  }
  utpsocket = socket.get();
  while (true)
  {
    std::string line;
    ELLE_TRACE("reading data");
    std::getline(*utpsocket, line);
    std::cout << line << std::endl;
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
        buf += '\n';
        sched.mt_run("data",
          boost::function<void()>([&] { send(buf);}));
      }
  });
  sched.run();
  rs.join();
}