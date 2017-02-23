#include <elle/os/environ.hh>

#include <elle/reactor/network/utp-server.hh>
#include <elle/reactor/network/utp-socket.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("utp-chat");

std::unique_ptr<elle::reactor::network::UTPServer> server;
elle::reactor::Scheduler sched;

static
void
proceed(elle::reactor::network::UTPSocket& s)
{
  std::thread rs([&] {
      while (std::cin.good())
      {
        std::string buf;
        std::getline(std::cin, buf);
        buf += '\n';
        sched.mt_run<void>("data",
          std::function<void()>([&] { s.write(buf);}));
      }
      ELLE_TRACE("cin EOF, closing");
      sched.mt_run<void>("close",
          std::function<void()>([&] { s.close();}));
  });
  while (true)
  {
    std::string line;
    std::getline(s, line);
    std::cout << line << std::endl;
  }
}

static void run(int argc, char** argv)
{
  server.reset(new elle::reactor::network::UTPServer);
  if (argc == 2)
  {
    server->listen(std::stoi(argv[1]));
    auto s = server->accept();
    proceed(*s);
  }
  else
  {
    server->listen(0);
    elle::reactor::network::UTPSocket s(*server);
    s.connect(argv[1], std::stoi(argv[2]));
    proceed(s);
  }
}
int main(int argc, char** argv)
{
  elle::reactor::Thread t(sched, "main", [&]
    {
      run(argc, argv);
    });
  sched.run();
}
