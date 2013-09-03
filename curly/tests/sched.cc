#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_IGNORE_SIGCHLD
#define BOOST_TEST_IGNORE_NON_ZERO_CHILD_CODE
#define BOOST_TEST_MODULE curly_sched
#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>

#include <elle/Buffer.hh>
#include <elle/container/vector.hh>
#include <elle/printf.hh>

#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/sleep.hh>
#include <reactor/thread.hh>

#include <curly/curly_sched.cc>

BOOST_AUTO_TEST_CASE(simple_test)
{
  static int const concurrent = 8;
  reactor::Scheduler sched;

  int port;
  reactor::Barrier served;

  reactor::Thread server(
    sched, "server",
    [&]
    {
      auto& sched = *reactor::Scheduler::scheduler();
      reactor::network::TCPServer server(sched);
      server.listen();
      port = server.port();
      served.open();

      int clients = 0;
      reactor::Barrier everybody_is_there;
      reactor::Scope scope;
      while (true)
      {
        std::shared_ptr<reactor::network::TCPSocket> socket(server.accept());
        scope.run_background(
          "request server",
          [&, socket]
          {
            char buffer[1024];
            socket->getline(buffer, sizeof(buffer), '\n');
            buffer[socket->gcount()] = 0;
            BOOST_CHECK_EQUAL(buffer, "GET /some/path HTTP/1.1\r");
            while (std::string(buffer) != "\r")
            {
              socket->getline(buffer, sizeof(buffer), '\n');
              buffer[socket->gcount()] = 0;
            }
            if (++clients == concurrent)
              everybody_is_there.open();
            // Wait until every client made the request, to check they are done
            // concurrently.
            sched.current()->wait(everybody_is_there);
            socket->write(
              "HTTP/1.0 200 OK\r\n"
              "Server: Custom HTTP of doom\r\n"
              "Content-Length: 4\r\n"
              "\r\n"
              "lol\n");
          });
      }
    });

  auto run_test = [&]
  {
    auto& sched = *reactor::Scheduler::scheduler();

    reactor::Thread* current = sched.current();
    auto fn = [&]
    {
      std::stringstream ss;
      auto get = curly::make_get();

      get.output(ss);
      get.option(CURLOPT_VERBOSE, 0);
      get.url(elle::sprintf("http://127.0.0.1:%s/some/path", port));
      curly::sched_request req(sched, std::move(get));
      req.run();
      BOOST_CHECK_EQUAL(ss.str(), "lol\n");
    };

    sched.current()->wait(served);
    std::vector<reactor::Waitable*> threads;
    for (int i = 0; i < concurrent; ++i)
      threads.push_back(
        new reactor::Thread(sched, elle::sprintf("client %s", i), fn));
    current->wait(threads);
    for (auto thread: threads)
      delete thread;
    server.terminate();
  };
  reactor::Thread main(sched, "main", run_test);
  sched.run();
}

BOOST_AUTO_TEST_CASE(timeout)
{
  reactor::Scheduler sched;
  reactor::Signal sig;
  reactor::Signal go;
  int port;

  auto tcp_serv = [&]
  {
    reactor::network::TCPServer serv(sched);

    auto* current = sched.current();
    current->wait(go);
    serv.listen(0);
    port = serv.port();
    sig.signal();
    std::unique_ptr<reactor::network::TCPSocket> socket(serv.accept());
    while (1)
    {
       reactor::Sleep pause(sched, 1_sec);
       pause.run();
    }
  };
  reactor::Thread tcp(sched, "tcp", tcp_serv);

  auto run_test = [&]
  {
    go.signal();
    auto* current = sched.current();
    current->wait(sig);

    auto get = curly::make_get();

    get.option(CURLOPT_VERBOSE, 0);
    get.option(CURLOPT_TIMEOUT, 2); // set timeout to 2sec
    get.url(elle::sprintf("http://127.0.0.1:%d/", port));
    curly::sched_request req(sched, std::move(get));
    BOOST_CHECK_THROW(req.run(), elle::Exception);
    tcp.terminate_now();
  };
  reactor::Thread main(sched, "main", run_test);
  sched.run();
}
