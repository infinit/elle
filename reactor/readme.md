# Reactor

The reactor is an asynchronous framework, built around a [coroutine](https://en.wikipedia.org/wiki/Coroutine) scheduler (based on [boost::context](http://www.boost.org/doc/libs/1_62_0/libs/context/doc/html/context/overview.html) or [libcoroutine](https://github.com/stevedekorte/coroutine/tree/master) and benefiting from [boost::asio](http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/overview.html)) providing cooperative multitasking on a single thread, enabling simple and safe imperative style concurrency.

The reactor is a part of the [elle](https://github.com/elle) set of libraries.

## Motivation

The reactor has been designed to fit the needs of I/O heavy projects. In this situation, you can benefit of near maximum CPU potential, while your program is waiting on I/O. Furthermore, program behavior is highly predictible compare to parallel programming, the context switches trigger when a coroutine yield, synchronization tools give strong guaranties on coroutines execution order and no code is executed in parallel (unless you explictly ask for it, the reactor provides tools to make both worlds (single-threading and multi-threading) coexist).

## How to compile

_See [elle: How to compile](https://github.com/elle)._

```bash
./drake //reactor/build -j 2
```

## Examples

A simple echo server implementation would look like:

```cpp
#include <reactor/network/exception.hh>
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
    // Create a coroutine (named reactor::Thread).
    reactor::Thread acceptor(sched, "acceptor", [&]
      {
        reactor::network::TCPServer server;
        server.listen(std::atoi(argv[1]));
        // Scope enable to start tasks and make sure they are terminated upon destruction.
        elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
        {
          while (true)
          {
            // Server::accept yields until it gets a connection.
            auto socket = elle::utility::move_on_copy(server.accept());
            scope.run_background(
              elle::sprintf("client %s", socket),
              [socket]
              {
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
                {}
              });
          }
        };
      });
    // Run the Scheduler until all coroutines are over or it gets interrupted (by a signal or programmatically).
    sched.run();
  }
  catch (std::exception const& e)
  {
    std::cerr << "fatal error: " << e.what() << std::endl;
    return 1;
  }
}

```

## Further reading
