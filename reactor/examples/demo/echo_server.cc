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

#include <reactor/Scope.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>

int
main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
      return 1;
    }
    // Create a Scheduler, the coroutines operator.
    reactor::Scheduler sched;
    // Properly terminate the scheduler in case of SIGINT.
    sched.signal_handle(SIGINT, [&sched] { sched.terminate(); });
    // Create a coroutine (named reactor::Thread).
    reactor::Thread acceptor(sched, "acceptor", [&]
      {
        reactor::network::TCPServer server;
        server.listen(std::atoi(argv[1]));
        // Scope enable to start tasks and make sure they are terminated upon
        // destruction, elle::With handles nested exceptions.
        elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
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
                catch (reactor::network::ConnectionClosed const&)
                {
                  std::cout << "Connection closed" << std::endl;
                }
              });
          }
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
