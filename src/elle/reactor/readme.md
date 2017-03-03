# Reactor

The reactor is an asynchronous framework, built around a [coroutine](https://en.wikipedia.org/wiki/Coroutine) scheduler (using [boost::context](http://www.boost.org/doc/libs/1_62_0/libs/context/doc/html/context/overview.html) or [libcoroutine](https://github.com/stevedekorte/coroutine/tree/master) and benefiting from [boost::asio](http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/overview.html)) providing cooperative multitasking on a single system thread, enabling simple and safe imperative style concurrency.

The reactor is part of the [Elle](https://github.com/infinit/elle) set of libraries.

## Motivation

The reactor was designed to suit the needs of I/O bound projects. In this situation, you can benefit from near maximum CPU potential, while other coroutines are waiting on I/O. Furthermore, program behavior is highly predictable compared to parallel programming: context switches trigger when a coroutine yields, synchronization tools give strong guaranties on coroutines execution order and no code is executed in parallel (unless you explicitly ask for it, and the reactor provides tools to make both worlds (single-threading and multi-threading) coexist).

## Example

The main logic of simple echo server implementation would look like:

```cpp
elle::reactor::Thread acceptor(sched, "acceptor", [&]
  {
    elle::reactor::network::TCPServer server;
    server.listen(8080);
    // Scope enable to start tasks and make sure they are terminated upon
    // destruction, elle::With handles nested exceptions.
    elle::With<elle::reactor::Scope>() << [&] (reactor::Scope& scope)
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
            try
            {
              while (true)
              {
                // This coroutine, Yield until reading "<...>\n".
                elle::Buffer line = socket->read_until("\n");
                // Write the line we just get (this yields too).
                socket->write(line);
              }
            }
            catch (elle::reactor::network::ConnectionClosed const&)
            {}
          });
      }
    };
  });
```
See [full example](examples/echo_server.cc).

## How to compile

_See [Elle: How to compile](https://github.com/infinit/elle#how-to-compile)._

```bash
./drake //reactor/build -j 2
```

## Further reading

- [Highly concurrent yet natural programming](http://www.slideshare.net/infinit-one/highly-concurrent-yet-natural-programming)
- [Infinit filesystem, Reactor reloaded](http://www.slideshare.net/infinit-one/infinit-filesystem-reactor-reloaded)

## Maintainers

 * Website: https://infinit.sh/open-source
 * Email: open+elle@infinit.sh
