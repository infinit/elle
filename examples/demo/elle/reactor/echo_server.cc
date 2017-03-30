/*
  Create a TCP echo server, which echo you back what you sent.

  How to run:
  $ ./reactor/examples/echo_server 8080

  How to test (using netcat):
  $ nc 127.0.0.1 8080
*/
#include <elle/With.hh>
#include <elle/utility/Move.hh>
#include <elle/Exception.hh>

#include <elle/reactor/Scope.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/tcp-socket.hh>
#include <elle/reactor/network/tcp-server.hh>
#include <elle/reactor/scheduler.hh>

int
main(int argc, char* argv[])
{
  try
  {
    if (argc < 2 || argc > 3)
    {
      std::cerr << "Usage: " << argv[0] << " <port> [local_test]" << std::endl;
      return 1;
    }
    // Create a Scheduler, the coroutines operator.
    elle::reactor::Scheduler sched;
    // Properly terminate the scheduler in case of SIGINT.
    sched.signal_handle(SIGINT, [&sched] { sched.terminate(); });
    auto local_test = false;
    if (argc == 3)
      local_test = std::string{argv[2]} == "true";
    // Create a coroutine (named elle::reactor::Thread).
    elle::reactor::Thread acceptor(sched, "echo server", [&]
      {
        elle::reactor::network::TCPServer server;
        auto port = std::atoi(argv[1]);
        server.listen(port);
        port = server.port();
        // Scope enable to start tasks and make sure they are terminated upon
        // destruction, elle::With handles nested exceptions.
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
        {
          if (local_test)
            scope.run_background("echo me",
                                 [port,&scope]
                                 {
                                   elle::reactor::network::TCPSocket s(
                                     "127.0.0.1", port);
                                   s.write("hello you!\n");
                                   auto res = s.read_some(20);
                                   assert(res.string() == "hello you!\n");
                                   scope.terminate_now();
                                 });
          scope.run_background(
            "acceptor",
            [&]
            {
              while (true)
              {
                // Server::accept yields until it gets a connection.
                auto socket = elle::utility::move_on_copy(server.accept());
                // Scope::run_background spawns a new coroutine.
                scope.run_background(
                  elle::sprintf("client %s", socket),
                  [socket]
                  {
                    std::cout << "New connection" << std::endl;
                    try
                    {
                      while (true)
                      {
                        // Yield until reading "<...>\n".
                        elle::Buffer line = socket->read_until("\n");
                        // Write the line we just get (this yields too).
                        socket->write(line);
                      }
                    }
                    catch (elle::reactor::network::ConnectionClosed const&)
                    {
                      std::cout << "Connection closed" << std::endl;
                    }
                  });
              }
            });
          scope.wait();
        };
      });
    // Run the Scheduler until all coroutines are over or it gets interrupted
    // (by a signal or programmatically).
    sched.run();
  }
  catch (...)
  {
    std::cerr << "fatal error: " << elle::exception_string() << std::endl;
    return 1;
  }
  return 0;
}
