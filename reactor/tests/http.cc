#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <elle/With.hh>
#include <elle/test.hh>
#include <elle/utility/Move.hh>

#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/http/Client.hh>
#include <reactor/http/EscapedString.hh>
#include <reactor/http/Request.hh>
#include <reactor/http/exceptions.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>
#include <reactor/semaphore.hh>
#include <reactor/signal.hh>

#include <http_server.hh>

ELLE_LOG_COMPONENT("reactor.http.test");

typedef reactor::http::tests::Server HTTPServer;
#define PERSIST_CHECK_EQUAL(a, b)                          \
  do                                                       \
  {                                                        \
  for (unsigned i=0; i<20 && !((a) == (b)); ++i)           \
    reactor::sleep(boost::posix_time::milliseconds(50));   \
  BOOST_CHECK_EQUAL(a, b);                                 \
  }                                                        \
  while(0)

ELLE_TEST_SCHEDULED(simple)
{
  HTTPServer server;
  server.register_route("/simple", reactor::http::Method::GET,
                        [&] (HTTPServer::Headers const&,
                             HTTPServer::Cookies const&,
                             HTTPServer::Parameters const&,
                             elle::Buffer const&) -> std::string
                          {
                            return "/simple";
                          });
  auto url = server.url("simple");
  ELLE_LOG("Get %s", url);
  auto page = reactor::http::get(url);
  BOOST_CHECK_EQUAL(page, elle::ConstWeakBuffer("/simple"));
}

ELLE_TEST_SCHEDULED(complex)
{
  HTTPServer server;
  server.register_route("/complex", reactor::http::Method::GET,
                        [&] (HTTPServer::Headers const&,
                             HTTPServer::Cookies const&,
                             HTTPServer::Parameters const&,
                             elle::Buffer const&) -> std::string
                          {
                            return "/complex";
                          });
  auto url = server.url("complex");
  ELLE_LOG("Get %s", url);
  reactor::http::Request r(url);
  r.wait();
  BOOST_CHECK_EQUAL(r.status(), reactor::http::StatusCode::OK);
  std::string content;
  r >> content;
  BOOST_CHECK_EQUAL(content, "/complex");
  BOOST_CHECK(r.eof());
  BOOST_CHECK_EQUAL(r.headers().at("Server"), "Custom HTTP of doom");
  BOOST_CHECK_EQUAL(r.progress(), (reactor::http::Request::Progress{8,8,0,0}));
}

ELLE_TEST_SCHEDULED(not_found)
{
  HTTPServer server;
  auto url = server.url("404");
  reactor::http::Request r(url);
  ELLE_LOG("Get %s", url);
  BOOST_CHECK_EQUAL(r.status(), reactor::http::StatusCode::Not_Found);
  // 404 error can have payload, which counts as downloaded data.
  BOOST_CHECK_EQUAL(r.progress(), (reactor::http::Request::Progress{35,35,0,0}));
}

ELLE_TEST_SCHEDULED(bad_request)
{
  HTTPServer server;
  server.register_route("/400", reactor::http::Method::GET,
                        [] (HTTPServer::Headers const&,
                            HTTPServer::Cookies const&,
                            HTTPServer::Parameters const&,
                            elle::Buffer const&) -> std::string
                        {
                          throw reactor::http::tests::Server::Exception(
                            "", reactor::http::StatusCode::Bad_Request);
                        });

  auto url = server.url("400");
  reactor::http::Request r(url);
  ELLE_LOG("Get %s", url);
  BOOST_CHECK_EQUAL(r.status(), reactor::http::StatusCode::Bad_Request);
}

class SilentHttpServer:
  public HTTPServer
{
  virtual
  void
  _response(reactor::network::Socket&,
            reactor::http::StatusCode,
            elle::ConstWeakBuffer,
            HTTPServer::Cookies const&) override
  {}
};

ELLE_TEST_SCHEDULED(no_answer)
{
  SilentHttpServer server;
  auto url = server.url("no_answer");
  ELLE_LOG("Get %s", url);
  BOOST_CHECK_THROW(reactor::http::get(url), reactor::http::EmptyResponse);
}

class PartialHttpServer:
  public HTTPServer
{
  virtual
  void
  _response(reactor::network::Socket& socket,
            reactor::http::StatusCode,
            elle::ConstWeakBuffer,
            HTTPServer::Cookies const&) override
  {
    std::string answer(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain; charset=UTF-8\r\n"
      "Content-Length: 42"
      "Connection: Close\r\n"
      "\r\n"
      "crap");
    socket.write(elle::ConstWeakBuffer(answer));
  }
};

ELLE_TEST_SCHEDULED(partial_answer)
{
  PartialHttpServer server;
  auto url = server.url("partial");
  reactor::http::Request r(url);
  BOOST_CHECK_THROW(reactor::wait(r), reactor::http::RequestError);
  BOOST_CHECK_EQUAL(r.progress(), (reactor::http::Request::Progress{4,42,0,0}));
}

class FuckOffHttpServer:
  public HTTPServer
{
  virtual
  void
  _serve(std::unique_ptr<reactor::network::Socket>) override
  {}
};

ELLE_TEST_SCHEDULED(connection_reset)
{
  FuckOffHttpServer server;
  auto url = server.url("connection_reset");
  ELLE_LOG("Get %s", url);
  BOOST_CHECK_THROW(reactor::http::get(url), reactor::http::RequestError);
}

static
void
concurrent()
{
  static int const concurrent = 8;
  static size_t const message_size = 1000;
  reactor::Scheduler sched;

  int port;
  reactor::Barrier served;

  reactor::Thread server(
    sched, "server",
    [&]
    {
      auto& sched = *reactor::Scheduler::scheduler();
      reactor::network::TCPServer server{};
      server.listen();
      port = server.port();
      ELLE_LOG("listen on port %s", port);
      served.open();

      int clients = 0;
      reactor::Barrier everybody_is_there;
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        while (true)
        {
          std::shared_ptr<reactor::network::Socket> socket(server.accept());
          ELLE_LOG("accept connection from %s", socket->peer());
          scope.run_background(
            elle::sprintf("request %s", socket->peer()),
            [&, socket]
            {
              char buffer[1024];
              socket->getline(buffer, sizeof(buffer), '\n');
              buffer[socket->gcount()] = 0;
              ELLE_LOG("got request: %s", buffer);
              BOOST_CHECK_EQUAL(buffer, "GET /some/path HTTP/1.1\r");
              while (std::string(buffer) != "\r")
              {
                ELLE_LOG("got header from: %s", buffer);
                socket->getline(buffer, sizeof(buffer), '\n');
                buffer[socket->gcount()] = 0;
              }
              if (++clients == concurrent)
                everybody_is_there.open();
              // Wait until every client made the request, to check they are done
              // concurrently.
              sched.current()->wait(everybody_is_there);
              std::string answer(
                "HTTP/1.1 200 OK\r\n"
                "Server: Custom HTTP of doom\r\n"
                "Content-Length: " + std::to_string(message_size) + "\r\n"
                "\r\n");
              ELLE_LOG("send response: %s", answer);
              socket->write(elle::ConstWeakBuffer(answer));
              std::string chunk = "lol\n";
              assert(message_size % chunk.size() == 0);
              size_t sent = 0;
              while (sent + chunk.size() <= message_size)
              {
                socket->write(elle::ConstWeakBuffer(chunk));
                sent += chunk.size();
              }
              assert(sent == message_size);
            });
        }
      };
    });

  auto run_test = [&]
  {
    auto& sched = *reactor::Scheduler::scheduler();

    reactor::Thread* current = sched.current();
    auto fn = [&]
    {
      auto url = elle::sprintf("http://127.0.0.1:%s/some/path", port);
      auto res = reactor::http::get(
        url, reactor::http::Request::Configuration(100_sec));
      BOOST_CHECK_EQUAL(res.string().substr(0, 4), "lol\n");
    };

    sched.current()->wait(served);
    std::vector<reactor::Thread*> threads;
    for (int i = 0; i < concurrent; ++i)
      threads.push_back(
        new reactor::Thread(sched, elle::sprintf("client %s", i), fn));
    current->wait(reactor::Waitables(begin(threads), end(threads)));
    for (auto thread: threads)
      delete thread;
    server.terminate();
  };
  reactor::Thread main(sched, "main", run_test);
  sched.run();
}

static
void
timeout()
{
  reactor::Scheduler sched;
  reactor::Signal sig;
  reactor::Signal go;
  int port;
  auto tcp_serv = [&]
  {
    reactor::network::TCPServer serv{};
    auto* current = sched.current();
    current->wait(go);
    serv.listen(0);
    port = serv.port();
    sig.signal();
    std::unique_ptr<reactor::network::Socket> socket(serv.accept());
    while (1)
      reactor::sleep(1_sec);
  };
  reactor::Thread tcp(sched, "tcp", tcp_serv);
  auto run_test = [&]
  {
    go.signal();
    auto* current = sched.current();
    current->wait(sig);
    auto url = elle::sprintf("http://127.0.0.1:%d/", port);
    reactor::http::Request::Configuration conf(500_ms);
    BOOST_CHECK_THROW(reactor::http::get(url, conf), reactor::http::Timeout);
    tcp.terminate_now();
  };
  reactor::Thread main(sched, "main", run_test);
  sched.run();
}

static
void
post(reactor::http::Request::Configuration conf,
     std::string const& version,
     reactor::http::Method method,
     bool expect,
     bool chunked,
     bool body = true)
{
  HTTPServer server;
  server.register_route(elle::sprintf("/%s", method), method,
                        [&] (HTTPServer::Headers const&,
                             HTTPServer::Cookies const&,
                             HTTPServer::Parameters const&,
                             elle::Buffer const& body) -> std::string
                        {
                          ELLE_LOG("body: %s", body);
                          return body.string();
                        });

  server.check_version(
    [&] (std::string const& v)
    {
      BOOST_CHECK_EQUAL(v, version);
    });
  server.check_method(
    [&] (std::string const& m)
    {
      BOOST_CHECK_EQUAL(m, boost::lexical_cast<std::string>(method));
    });
  server.check_expect_continue(
    [&] (bool e)
    {
      BOOST_CHECK_EQUAL(e, expect);
    });
  server.check_chunked(
    [&] (bool c)
    {
      BOOST_CHECK_EQUAL(c, chunked);
    });
  if (body)
  {
    std::string json("{"
                     "  \"ploum\": \"ploum\","
                     "  \"tra\": \"lala\""
                     "}");
    reactor::http::Request r(
      elle::sprintf("http://127.0.0.1:%s/%s", server.port(), method),
      method,
      "application/json",
      conf);
    r << json;
    BOOST_CHECK_EQUAL(r.response(), json);
    BOOST_CHECK_EQUAL(r.pause_count(), 0);
    int64_t l = json.length();
    int64_t lup = l;
    if (chunked)
      lup += 11;
    BOOST_CHECK_EQUAL(r.progress(),
                      (reactor::http::Request::Progress{l,l,lup, 0}));
  }
  else
  {
    reactor::http::Request r(
      elle::sprintf("http://127.0.0.1:%s/%s", server.port(), method),
      method,
      conf);
    r.wait();
    BOOST_CHECK_EQUAL(r.pause_count(), 0);
  }
}

ELLE_TEST_SCHEDULED(post_no_body)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::POST, false, false, false);
}

ELLE_TEST_SCHEDULED(post_10)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::POST, false, false);
}

ELLE_TEST_SCHEDULED(post_11)
{
  reactor::http::Request::Configuration conf;
  post(conf, "HTTP/1.1", reactor::http::Method::POST, true, false);
}

ELLE_TEST_SCHEDULED(post_11_chunked)
{
  reactor::http::Request::Configuration conf;
  conf.chunked_transfers(true);
  post(conf, "HTTP/1.1", reactor::http::Method::POST, true, true);
}

ELLE_TEST_SCHEDULED(put_no_body)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::PUT, false, false, false);
}

ELLE_TEST_SCHEDULED(put_10)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::PUT, false, false);
}

ELLE_TEST_SCHEDULED(put_11)
{
  reactor::http::Request::Configuration conf;
  post(conf, "HTTP/1.1", reactor::http::Method::PUT, true, false);
}

ELLE_TEST_SCHEDULED(put_11_chunked)
{
  reactor::http::Request::Configuration conf;
  conf.chunked_transfers(true);
  post(conf, "HTTP/1.1", reactor::http::Method::PUT, true, true);
}

ELLE_TEST_SCHEDULED(cookies)
{
  HTTPServer server;
  server.register_route(
    "/cookies", reactor::http::Method::GET,
    [&] (HTTPServer::Headers const&,
         HTTPServer::Cookies const& cookies,
         HTTPServer::Parameters const&,
         elle::Buffer const&) -> std::string
    {
      std::string response;
      for (auto const& cookie: cookies)
      {
        auto line = elle::sprintf("%s: %s\n", cookie.first, cookie.second);
        response += line;
      }
      return response + "/cookies";
    });
  server.headers()["Set-Cookie"] = "we=got";
  {
    BOOST_CHECK_EQUAL(reactor::http::get(server.url("cookies")).string(),
                      "/cookies");
  }
  {
    reactor::http::Client client;
    auto r = client.request(server.url("cookies"), reactor::http::Method::GET);
    BOOST_CHECK_EQUAL(r.response(), "/cookies");
    auto check_cookies =
      [&] (reactor::http::Request::Configuration::Cookies const& cookies)
      {
        BOOST_CHECK_EQUAL(cookies.size(), 1);
        BOOST_CHECK(cookies.find("we") != cookies.end());
        BOOST_CHECK_EQUAL(cookies.find("we")->second, "got");
      };
    check_cookies(r.cookies());
    check_cookies(client.cookies());
    BOOST_CHECK_EQUAL(client.get(server.url("cookies")).string(),
                      "we: got\n/cookies");
  }

  {
    reactor::http::Request::Configuration conf;
    conf.cookies()["shitload"] = "of";
    reactor::http::Request r(server.url("cookies"),
                             reactor::http::Method::GET,
                             conf);
    BOOST_CHECK_EQUAL(r.response(), "shitload: of\n/cookies");
  }
}

ELLE_TEST_SCHEDULED(request_move)
{
  HTTPServer server;

  server.register_route("/move", reactor::http::Method::POST,
                        [&] (HTTPServer::Headers const&,
                             HTTPServer::Cookies const&,
                             HTTPServer::Parameters const&,
                             elle::Buffer const& body) -> std::string
                          {
                            return body.string();
                          });
  reactor::http::Request a(
    elle::sprintf("http://127.0.0.1:%s/move", server.port()),
    reactor::http::Method::POST,
    "application/json");
  reactor::http::Request b(std::move(a));
  b << "{}";
  reactor::http::Request c(std::move(b));
  c.finalize();
  reactor::http::Request d(std::move(c));
  reactor::wait(d);
  reactor::http::Request e(std::move(d));
  BOOST_CHECK_EQUAL(e.response(), "{}");
  BOOST_CHECK_EQUAL(e.status(), reactor::http::StatusCode::OK);
  BOOST_CHECK_EQUAL(e.progress(),
                    (reactor::http::Request::Progress{2, 2, 2, 0}));
}

class ScheduledSilentHttpServer:
  public HTTPServer
{
protected:
  virtual
  void
  _serve(std::unique_ptr<reactor::network::Socket>) override
  {
    reactor::sleep(3_sec);
  }
};

ELLE_TEST_SCHEDULED(interrupted)
{
  ScheduledSilentHttpServer server;

  server.register_route("/move", reactor::http::Method::POST,
                        [&] (HTTPServer::Headers const&,
                             HTTPServer::Cookies const&,
                             HTTPServer::Parameters const&,
                             elle::Buffer const& body) -> std::string
                          {
                            return body.string();
                          });

  {
    reactor::http::Request r(
      elle::sprintf("http://127.0.0.1:%s/move", server.port()),
      reactor::http::Method::POST,
      "application/json");
    r << "{}";
    r.finalize();
    reactor::sleep(100_ms); // XXX: wait for curl to read.
  }
}

ELLE_TEST_SCHEDULED(escaped_string)
{
  std::string request_str("Action=GetFederationToken&DurationSeconds=3600&Name=52fa46dbe9a23c0716e77249&Policy={\"Statement\": [{\"Effect\": \"Allow\",\"Resource\": \"arn:aws:s3:::cloud_buffer/5307257ce9a23c1bb5e7724d/*\",\"Action\": [\"s3:PutObject\"]}],\"Version\": \"2012-10-17\"}&Version=2011-06-15&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIPTEKRYOSJORHQMA/20140224/us-east-1/sts/aws4_request&X-Amz-Date=20140224T171359Z&X-Amz-SignedHeaders=content-type;host;x-amz-date&X-Amz-Signature=dc39af31663a25fc09e01f777e1907ef1405e2c94d3dd7c46dcf1022de08b9b6");
  std::string correst_res("Action%3DGetFederationToken%26DurationSeconds%3D3600%26Name%3D52fa46dbe9a23c0716e77249%26Policy%3D%7B%22Statement%22%3A%20%5B%7B%22Effect%22%3A%20%22Allow%22%2C%22Resource%22%3A%20%22arn%3Aaws%3As3%3A%3A%3Acloud_buffer%2F5307257ce9a23c1bb5e7724d%2F%2A%22%2C%22Action%22%3A%20%5B%22s3%3APutObject%22%5D%7D%5D%2C%22Version%22%3A%20%222012-10-17%22%7D%26Version%3D2011-06-15%26X-Amz-Algorithm%3DAWS4-HMAC-SHA256%26X-Amz-Credential%3DAKIAIPTEKRYOSJORHQMA%2F20140224%2Fus-east-1%2Fsts%2Faws4_request%26X-Amz-Date%3D20140224T171359Z%26X-Amz-SignedHeaders%3Dcontent-type%3Bhost%3Bx-amz-date%26X-Amz-Signature%3Ddc39af31663a25fc09e01f777e1907ef1405e2c94d3dd7c46dcf1022de08b9b6");
  reactor::http::EscapedString request_res(request_str);
  BOOST_CHECK_EQUAL(request_res.string(), correst_res);
}

class NoHeaderHttpServer:
  public HTTPServer
{
  virtual
  void
  _response(reactor::network::Socket& socket,
           reactor::http::StatusCode,
            elle::ConstWeakBuffer,
            HTTPServer::Cookies const&) override
  {
    std::string answer(
      "<!DOCTYPE HTML>\n"
      );
    socket.write(elle::ConstWeakBuffer(answer));
  }
};

ELLE_TEST_SCHEDULED(no_header_answer)
{
  NoHeaderHttpServer server;
  server.register_route("/no_header", reactor::http::Method::GET,
                        [&] (HTTPServer::Headers const&,
                             HTTPServer::Cookies const&,
                             HTTPServer::Parameters const&,
                             elle::Buffer const& body) -> std::string
                          {
                            return body.string();
                          });
  auto url = server.url("no_header");
  BOOST_CHECK_THROW(reactor::http::get(url), reactor::http::RequestError);
}

/// Wait on a barrier between each send of a reply chunk
class SlowHttpServer:
  public HTTPServer
{
public:
  SlowHttpServer(std::string reply, int chunk,
                 bool wait_sem = true, reactor::DurationOpt delay=reactor::DurationOpt())
    : _reply(reply)
    , _chunk(chunk)
    , _wait_sem(wait_sem)
    , _delay(delay)
  {}

  ~SlowHttpServer()
  {
    this->_finalize();
  }

  reactor::Semaphore sem;
  std::string _reply;
  int _chunk;
  bool _wait_sem;
  reactor::DurationOpt _delay;

protected:
  virtual
  void
  _serve(std::unique_ptr<reactor::network::Socket> s) override
  {
    unsigned int p = 0;
    do
    {
      if (_wait_sem)
        sem.wait();
      if (_delay)
        reactor::sleep(*_delay);
      s->write(_reply.substr(p, _chunk));
      p += _chunk;
    }
    while (p <  _reply.size());
  }
};


ELLE_TEST_SCHEDULED(download_progress)
{
  using reactor::http::Request;
  auto delay = []
  { // Lets not make too strong hypothesis about sched implementation details.
    for (unsigned i=0; i<10; ++i)
      reactor::yield();
    reactor::sleep(boost::posix_time::milliseconds(100));
    for (unsigned i=0; i<10; ++i)
      reactor::yield();
  };
  const int payload_length = 12;
  std::string header("HTTP/1.1 200 OK\r\nContent-Length: "
                     + boost::lexical_cast<std::string>(payload_length)
                     + "\r\n\r\n");
  std::string payload(payload_length, 'a');
  SlowHttpServer server(header + payload, 1);
  reactor::http::Request r(server.url("whatever"));
  r.finalize();
  reactor::Thread t("progress", [&]
    {
      for (unsigned i=0; i< header.size(); ++i)
        server.sem.release();
      delay();
      PERSIST_CHECK_EQUAL(r.progress(), (Request::Progress{0,payload_length,0,0}));
      for (unsigned i=0; i < payload_length; ++i)
      {
        server.sem.release();
        delay();
        PERSIST_CHECK_EQUAL(r.progress(), (Request::Progress{i+1, payload_length,0,0}));
      }
    });
  reactor::wait(r);
  PERSIST_CHECK_EQUAL(r.progress(), (Request::Progress{payload_length, payload_length,0,0}));
  reactor::wait(t);
}

ELLE_TEST_SCHEDULED(download_stall)
{
  const int payload_length = 2000;
  std::string header("HTTP/1.1 200 OK\r\nContent-Length: "
                     + boost::lexical_cast<std::string>(payload_length)
                     + "\r\n\r\n");
  std::string payload(payload_length, 'a');
  SlowHttpServer server(header + payload, 10);
  for (unsigned i=0; i<100 + header.size() / 10; ++i)
    server.sem.release();
  // Careful, stall timeout has only second resolution.
  reactor::http::Request::Configuration conf(reactor::DurationOpt(), 1_sec);
  reactor::http::Request r(server.url("whatever"), reactor::http::Method::GET,
                           conf);
  r.finalize();
  // CURL takes some time to trigger the stall detection timeout, take margins.
  BOOST_CHECK_THROW(reactor::wait(r), reactor::http::Timeout);
  // magic 999 is because we did not take header length into consideration.
  BOOST_CHECK_EQUAL(r.progress(),
                    (reactor::http::Request::Progress{999, 2000, 0, 0}));
  // Unstuck the server. Request disconnected so it will terminate fast.
  server.sem.release();
  server.sem.release();
}

ELLE_TEST_SCHEDULED(query_string)
{
  HTTPServer server;
  reactor::Signal hit;
  server.register_route(
    "/query",
    reactor::http::Method::GET,
    [&] (HTTPServer::Headers const&,
         HTTPServer::Cookies const&,
         HTTPServer::Parameters const& params,
         elle::Buffer const& body) -> std::string
    {
      BOOST_CHECK_EQUAL(params.at("string%20space"), "something-dash");
      BOOST_CHECK_EQUAL(params.at("simple"), "stuffs");
      hit.signal();
      return "coucou";
    });
  reactor::http::Request r(server.url("query"), reactor::http::Method::GET);
  std::unordered_map<std::string, std::string> query_dict;
  query_dict["string space"] = "something-dash";
  query_dict["simple"] = "stuffs";
  r.query_string(query_dict);
  BOOST_CHECK_EQUAL(r.query_string(),
                    "simple=stuffs&string%20space=something-dash");
  r.finalize();
  reactor::wait(hit);
}

ELLE_TEST_SCHEDULED(keep_alive)
{
  HTTPServer server;
  server.register_route(
    "/alive", reactor::http::Method::GET,
    [&] (HTTPServer::Headers const& headers,
         HTTPServer::Cookies const&,
         HTTPServer::Parameters const&,
         elle::Buffer const&) -> std::string
    {
      BOOST_CHECK_EQUAL(headers.at("Connection"), "keep-alive");
      return "alive";
    });
  server.register_route(
    "/dead", reactor::http::Method::GET,
    [&](HTTPServer::Headers const& headers,
         HTTPServer::Cookies const&,
         HTTPServer::Parameters const&,
         elle::Buffer const&) -> std::string
      {
        BOOST_CHECK_THROW(headers.at("Connection"), std::out_of_range);
        return "dead";
      });
  reactor::http::Request r_keep_alive(server.url("alive"));
  r_keep_alive.wait();
  std::string content;
  r_keep_alive >> content;
  BOOST_CHECK_EQUAL(content, "alive");
  reactor::http::Request::Configuration conf(30_sec,
                                             reactor::DurationOpt(),
                                             reactor::http::Version::v11,
                                             false);
  reactor::http::Request r_close(server.url("dead"),
                                 reactor::http::Method::GET,
                                 conf);
  r_close.wait();
  content.clear();
  r_close >> content;
  BOOST_CHECK_EQUAL(content, "dead");
}

class RedirectHTTPServer
  : public HTTPServer
{
  virtual
  void
  _response(reactor::network::Socket& socket,
            reactor::http::StatusCode,
            elle::ConstWeakBuffer,
            HTTPServer::Cookies const&) override
  {
    std::string answer(
      "HTTP/1.1 303 See Other\r\n"
      "Location: http://example.org/other\r\n");
    socket.write(elle::ConstWeakBuffer(answer));
  }
};

ELLE_TEST_SCHEDULED(redirection)
{
  RedirectHTTPServer server;
  auto url = server.url("redirect");
  reactor::http::Request r(url);
  BOOST_CHECK_EQUAL(r.status(), reactor::http::StatusCode::See_Other);
  BOOST_CHECK_EQUAL(r.headers().at("Location"), "http://example.org/other");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(simple), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(complex), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(not_found), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(bad_request), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(no_answer), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(partial_answer), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(connection_reset), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(concurrent), 0, valgrind(10));
  suite.add(BOOST_TEST_CASE(timeout), 0, valgrind(30));
  suite.add(BOOST_TEST_CASE(post_no_body), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(post_10), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(post_11), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(post_11_chunked), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(put_no_body), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(put_10), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(put_11), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(put_11_chunked), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(cookies), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(request_move), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(interrupted), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(escaped_string), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(no_header_answer), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(download_progress), 0, valgrind(10));
  suite.add(BOOST_TEST_CASE(download_stall), 0, valgrind(40));
  suite.add(BOOST_TEST_CASE(query_string), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(keep_alive), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(redirection), 0, valgrind(1));
}
