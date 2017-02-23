#include <elle/reactor/Barrier.hh>
#include <elle/reactor/network/rdv.hh>
#include <elle/reactor/network/utp-server.hh>
#include <elle/reactor/network/utp-socket.hh>
#include <elle/reactor/scheduler.hh>

#include <elle/os/environ.hh>

ELLE_LOG_COMPONENT("rdvcat");

using namespace elle::reactor::network;


elle::reactor::network::UTPSocket* utpsocket = nullptr;
std::unique_ptr<elle::reactor::network::UTPSocket> hsocket;
std::unique_ptr<elle::reactor::Barrier> exit_request;

static void send(std::string const& data)
{
  while (utpsocket == nullptr)
    elle::reactor::sleep(100_ms);
  ELLE_TRACE("pushing data");
  utpsocket->write(elle::ConstWeakBuffer(data));
}

static void close()
{
  ELLE_TRACE("reset socket...");
  hsocket.reset();
  exit_request->open();
  ELLE_TRACE("...done");
}

static void run(int argc, char**argv)
{
  std::string rdvhost = argv[1];
  int rdvport = 7890;
  std::string id(argv[2]);
  std::string remoteid(argv[3]);
  elle::reactor::network::UTPServer server;
  server.listen(0);
  server.rdv_connect(id, rdvhost + ":" + std::to_string(rdvport));
  if (id < remoteid)
  {
    ELLE_TRACE("accepting...");
    hsocket = server.accept();
    ELLE_TRACE("...accepted");
  }
  else
  {
    hsocket.reset(new elle::reactor::network::UTPSocket(server));
    ELLE_TRACE("connecting...");
    hsocket->connect(remoteid);
    ELLE_TRACE("...connected");
  }
  utpsocket = hsocket.get();
  static bool noread = !elle::os::getenv("NO_READ", "").empty();
  try
  {
    while (hsocket)
    {
      if (noread)
        elle::reactor::wait(*exit_request);
      else
      {
        std::string line;
        ELLE_TRACE("reading data");
        std::getline(*utpsocket, line);
        std::cout << line << std::endl;
      }
    }
  }
  catch (elle::Error const& e)
  {
    ELLE_TRACE("Read loop exited with %s", e);
  }
  utpsocket = nullptr;
  hsocket.reset();
}

int main(int argc, char** argv)
{
  elle::reactor::Scheduler sched;
  exit_request = std::make_unique<elle::reactor::Barrier>();
  elle::reactor::Thread t(sched, "main", [&]
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
          std::function<void()>([&] { send(buf);}));
      }
      ELLE_TRACE("cin EOF, closing");
      sched.mt_run("close",
          std::function<void()>([&] { close();}));
  });
  sched.run();
  ELLE_TRACE("scheduler exited");
  rs.detach();
  ELLE_TRACE("program exit");
}
