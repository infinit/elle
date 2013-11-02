#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/test/unit_test.hpp>

#include <elle/With.hh>
#include <elle/utility/Move.hh>

#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/signal.hh>
#include <reactor/http/Client.hh>
#include <reactor/http/Request.hh>
#include <reactor/http/exceptions.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.http.test");

// XXX: these tests were integrated from curly, they should all be written using
// the scheduled http server like the latest test.

class HttpServer
{
public:
  HttpServer():
    _io_service(),
    _server(this->_io_service),
    _thread(nullptr)
  {
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 0);
    this->_server.open(endpoint.protocol());
    this->_server.bind(endpoint);
    this->_server.listen();
    ELLE_LOG("%s: listen on port %s", *this, this->port());
    this->_accept();
    auto run = [this] () { this->_io_service.run(); };
    this->_thread.reset(new std::thread(run));
  }

  virtual
  ~HttpServer()
  {
    this->_server.close();
    this->_thread->join();
  }

  void
  _accept()
  {
    assert(!this->_accepted_socket);
    this->_accepted_socket.reset(
      new boost::asio::ip::tcp::socket(this->_io_service));
    auto cb = std::bind(&HttpServer::_accepted, this, std::placeholders::_1);
    this->_server.async_accept(*this->_accepted_socket,
                               this->_accepted_endpoint,
                               cb);
  }

  void
  _accepted(boost::system::error_code const& e)
  {
    if (e)
    {
      if (e == boost::system::errc::operation_canceled)
        return;
      ELLE_ERR("%s: accept error: %s", *this, e.message());
      std::abort();
    }
    ELLE_LOG("%s: accepted connection from %s",
             *this, this->_accepted_endpoint);
    assert(this->_accepted_socket);
    this->_serve(std::move(this->_accepted_socket));
    this->_accept();
  }

  virtual
  void
  _serve(std::unique_ptr<boost::asio::ip::tcp::socket> _socket)
  {
    auto buffer = new boost::asio::streambuf;
    auto& socket = *_socket;
    auto cb = std::bind(&HttpServer::_answer,
                        this,
                        elle::utility::move_on_copy(_socket),
                        buffer,
                        std::placeholders::_1,
                        std::placeholders::_2
      );
    boost::asio::async_read_until(socket, *buffer,
                                  std::string("\r\n\r\n"), cb);
  }

  virtual
  void
  _answer(std::unique_ptr<boost::asio::ip::tcp::socket> _socket,
          boost::asio::streambuf* buffer,
          boost::system::error_code const& e,
          size_t size)
  {
    auto& socket = *_socket;
    auto peer = socket.remote_endpoint();
    if (e)
    {
      ELLE_ERR("%s: read error on %s: %s", *this, peer, e);
      std::abort();
    }
    std::string path;
    {
      std::istream s(buffer);
      char line[1024];
      s.getline(line, sizeof(line));
      ELLE_LOG("%s: read on %s: %s", *this, peer, line);
      std::vector<std::string> words;
      boost::algorithm::split(words, line,
                              boost::algorithm::is_any_of(" "));
      BOOST_CHECK_EQUAL(words[0], "GET");
      BOOST_CHECK_EQUAL(words[2], "HTTP/1.1\r");
      path = words[1];
    }
    delete buffer;
    auto answer = new std::string(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain; charset=UTF-8\r\n"
      "Connection: Close\r\n");
    // *answer += elle::sprintf("Content-length: %s\r\n", path.size());
    *answer += "\r\n";
    *answer += path;
    auto answer_buffer = boost::asio::buffer(answer->c_str(),
                                             answer->size());
    ELLE_LOG("%s: write to %s: %s", *this, peer, *answer);
    auto cb = std::bind(&HttpServer::_answered,
                        this,
                        elle::utility::move_on_copy(_socket),
                        answer,
                        std::placeholders::_1,
                        std::placeholders::_2);
    boost::asio::async_write(socket, answer_buffer, cb);
  }

  virtual
  void
  _answered(std::unique_ptr<boost::asio::ip::tcp::socket> _socket,
            std::string* answer,
            boost::system::error_code const& e,
            size_t size)
  {
    delete answer;
    auto& socket = *_socket;
    auto peer = socket.remote_endpoint();
    if (e)
    {
      ELLE_ERR("%s: write error on %s: %s", *this, peer, e);
      std::abort();
    }
    ELLE_LOG("%s: close connection to %s", *this, peer);
    socket.close();
  }

  int port()
  {
    return this->_server.local_endpoint().port();
  }

  std::string
  root_url()
  {
    // For some reason, resolution sometimes times out under valgrind, hence the
    // IP instead of localhost. This is unrelated to curly since the simplest
    // Curl request yields the same behavior.
    return elle::sprintf("http://127.0.0.1:%s/", this->port());
  }

private:
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _server;
  std::unique_ptr<boost::asio::ip::tcp::socket> _accepted_socket;
  boost::asio::ip::tcp::endpoint _accepted_endpoint;
  std::unique_ptr<std::thread> _thread;
};

#define HTTP_TEST(Name)                         \
static                                          \
void                                            \
Name##_impl();                                  \
                                                \
static                                          \
void                                            \
Name()                                          \
{                                               \
  reactor::Scheduler sched;                     \
  reactor::Thread main(                         \
    sched, "main",                              \
    [&]                                         \
    {                                           \
      Name##_impl();                            \
    });                                         \
  sched.run();                                  \
}                                               \
                                                \
static                                          \
void                                            \
Name##_impl()                                   \

HTTP_TEST(simple)
{
  HttpServer server;
  auto url = server.root_url() + "simple";
  ELLE_LOG("Get %s", url);
  auto page = reactor::http::get(url);
  BOOST_CHECK_EQUAL(page, elle::ConstWeakBuffer("/simple"));
}

HTTP_TEST(complex)
{
  HttpServer server;
  auto url = server.root_url() + "complex";
  ELLE_LOG("Get %s", url);
  reactor::http::Request r(url);
  r.wait();
  BOOST_CHECK_EQUAL(r.status(), reactor::http::StatusCode::OK);
  std::string content;
  r >> content;
  BOOST_CHECK_EQUAL(content, "/complex");
  BOOST_CHECK(r.eof());
}

class SilentHttpServer:
  public HttpServer
{
  virtual
  void
  _answer(std::unique_ptr<boost::asio::ip::tcp::socket> _socket,
          boost::asio::streambuf* buffer,
          boost::system::error_code const& e,
          size_t size)
  {
    delete buffer;
    _socket->close();
  }
};

HTTP_TEST(no_answer)
{
  SilentHttpServer server;
  auto url = server.root_url() + "no_answer";
  ELLE_LOG("Get %s", url);
  BOOST_CHECK_THROW(reactor::http::get(url), reactor::http::EmptyResponse);
}

class PartialHttpServer:
  public HttpServer
{
  virtual
  void
  _answer(std::unique_ptr<boost::asio::ip::tcp::socket> socket,
          boost::asio::streambuf* buffer,
          boost::system::error_code const& e,
          size_t size)
  {
    delete buffer;
    auto answer = new std::string(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain; charset=UTF-8\r\n"
      "Content-Length: 42"
      "Connection: Close\r\n"
      "\r\n"
      "crap");
    auto answer_buffer = boost::asio::buffer(answer->c_str(),
                                             answer->size());
    auto peer = socket->remote_endpoint();
    ELLE_LOG("%s: write to %s: %s", *this, peer, *answer);
    auto& s = *socket;
    auto cb = std::bind(&HttpServer::_answered,
                        this,
                        elle::utility::move_on_copy(socket),
                        answer,
                        std::placeholders::_1,
                        std::placeholders::_2);
    boost::asio::async_write(s, answer_buffer, cb);
  }
};

HTTP_TEST(partial_answer)
{
  PartialHttpServer server;
  auto url = server.root_url() + "partial";
  BOOST_CHECK_THROW(reactor::http::get(url), reactor::http::RequestError);
}

class FuckOffHttpServer:
  public HttpServer
{
  virtual
  void
  _serve(std::unique_ptr<boost::asio::ip::tcp::socket> socket)
  {
    socket->close();
  }
};

HTTP_TEST(connection_reset)
{
  FuckOffHttpServer server;
  auto url = server.root_url() + "connection_reset";
  ELLE_LOG("Get %s", url);
  BOOST_CHECK_THROW(reactor::http::get(url), reactor::http::RequestError);
}

static
void
concurrent()
{
  static int const concurrent = 8;
  static size_t const message_size = 2000;
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
      ELLE_LOG("listen on port %s", port);
      served.open();

      int clients = 0;
      reactor::Barrier everybody_is_there;
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        while (true)
        {
          std::shared_ptr<reactor::network::TCPSocket> socket(server.accept());
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
      auto res = reactor::http::get(url);
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
    reactor::network::TCPServer serv(sched);
    auto* current = sched.current();
    current->wait(go);
    serv.listen(0);
    port = serv.port();
    sig.signal();
    std::unique_ptr<reactor::network::TCPSocket> socket(serv.accept());
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

class ScheduledHttpServer
{
public:
  ScheduledHttpServer():
    _server(),
    _port(0),
    _accepter(),
    _check_version([] (std::string const&) {}),
    _check_method([] (std::string const&) {}),
    _check_expect_continue([] (bool) {}),
    _check_chunked([] (bool) {})
  {
    this->_server.listen(0);
    this->_port = this->_server.port();
    ELLE_LOG("%s: listen on port %s", *this, this->_port);
    this->_accepter.reset(
      new reactor::Thread(*reactor::Scheduler::scheduler(),
                          "accepter",
                          std::bind(&ScheduledHttpServer::_accept,
                                    std::ref(*this))));
  }

  ~ScheduledHttpServer()
  {
    this->_accepter->terminate_now();
  }

  std::string
  url(std::string const& path)
  {
    return elle::sprintf("http://127.0.0.1:%s/%s", this->port(), path);
  }

  ELLE_ATTRIBUTE(reactor::network::TCPServer, server);
  ELLE_ATTRIBUTE_R(int, port);
  ELLE_ATTRIBUTE(std::unique_ptr<reactor::Thread>, accepter);

protected:
  ELLE_ATTRIBUTE_RW(std::function<void (std::string const&)>, check_version);
  ELLE_ATTRIBUTE_RW(std::function<void (std::string const&)>, check_method);
  ELLE_ATTRIBUTE_RW(std::function<void (bool)>, check_expect_continue);
  ELLE_ATTRIBUTE_RW(std::function<void (bool)>, check_chunked);

private:
  void
  _accept()
  {
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      while (true)
      {
        std::unique_ptr<reactor::network::TCPSocket> socket(
          this->_server.accept());
        ELLE_LOG("accept connection from %s", socket->peer());
        auto name = elle::sprintf("request %s", socket->peer());
        scope.run_background(
          name,
          std::bind(&ScheduledHttpServer::_serve, std::ref(*this),
                    elle::utility::move_on_copy(socket)));
      }
    };
  }

  void
  _serve(std::unique_ptr<reactor::network::TCPSocket> socket)
  {
    auto peer = socket->peer();
    std::string method;
    std::string path;
    bool expect = false;
    bool v1_1 = false;
    bool chunked = false;
    int length = -1;
    {
      auto request = socket->read_until("\r\n");
      ELLE_LOG("%s: got request from %s: %s", *this, peer, request.string());
      std::vector<std::string> words;
      boost::algorithm::split(words, request,
                              boost::algorithm::is_any_of(" "));
      method = words[0];
      path = words[1];
      auto version = words[2].substr(0, words[2].size() - 2);
      this->_check_version(version);
      if (version == "HTTP/1.1")
        v1_1 = true;
      else if (version == "HTTP/1.0")
        v1_1 = false;
      else
        BOOST_ERROR(elle::sprintf("unrecognized HTTP version: %s", version));
    }
    std::unordered_map<std::string, std::string> cookies;
    while (true)
    {
      auto buffer = socket->read_until("\r\n");
      if (buffer == "\r\n")
        break;
      buffer.size(buffer.size() - 2);
      ELLE_LOG("%s: got header from %s: %s", *this, peer, buffer.string());
      std::vector<std::string> words;
      boost::algorithm::split(words, buffer,
                              boost::algorithm::is_any_of(" "));
      if (words[0] == "Expect:")
      {
        BOOST_CHECK_EQUAL(words[1], "100-continue");
        BOOST_CHECK_EQUAL(words.size(), 2);
        expect = true;
      }
      else if (words[0] == "Content-Length:")
      {
        BOOST_CHECK_EQUAL(words.size(), 2);
        length = boost::lexical_cast<int>(words[1]);
      }
      else if (words[0] == "Transfer-Encoding:")
      {
        BOOST_CHECK_EQUAL(words[1], "chunked");
        BOOST_CHECK_EQUAL(words.size(), 2);
        chunked = true;
      }
      else if (words[0] == "Cookie:")
      {
        std::vector<std::string> chunks;
        boost::algorithm::split(chunks, words[1],
                                boost::algorithm::is_any_of("="));
        BOOST_CHECK_EQUAL(chunks.size(), 2);
        cookies[chunks[0]] = chunks[1];
      }
    }
    this->_check_method(method);
    this->_check_expect_continue(expect);
    this->_check_chunked(chunked);
    if (method == "GET")
    {
      this->response(*socket, path, cookies);
    }
    else if (method == "POST" || method == "PUT")
    {
      if (v1_1)
      {
        if (expect)
        {
          std::string answer(
            "HTTP/1.1 100 Continue\r\n"
            "\r\n");
          ELLE_LOG("send response header: %s", answer);
          socket->write(elle::ConstWeakBuffer(answer));
        }
        elle::Buffer content;
        if (chunked)
          while (true)
          {
            socket->read_until("\r\n"); // Ignore the chunked header.
            auto buffer = socket->read_until("\r\n");
            if (buffer == "\r\n")
              break;
            ELLE_LOG("%s: got content chunk from %s: %s",
                     *this, peer, buffer.string());
            content.append(buffer.contents(), buffer.size() - 2);
          }
        else
        {
          content = this->read_sized_content(*socket, length);
        }
        ELLE_LOG("%s: got content from %s: %s", *this, peer, content.string());
        this->response(*socket, content, cookies);
      }
      else
      {
        auto content = this->read_sized_content(*socket, length);
        this->response(*socket, content, cookies);
      }
    }
    else
      BOOST_ERROR(elle::sprintf("unrecognized HTTP method: %s", method));
    ELLE_LOG("%s: close connection with %s", *this, socket->peer());
  }

  void
  response(reactor::network::TCPSocket& socket,
           elle::ConstWeakBuffer content,
           std::unordered_map<std::string, std::string> const& cookies)
  {
    std::string response;
    for (auto const& cookie: cookies)
    {
      auto line = elle::sprintf("%s: %s\n", cookie.first, cookie.second);
      response += line;
    }
    response += content.string();
    std::string answer(
      "HTTP/1.1 200 OK\r\n"
      "Server: Custom HTTP of doom\r\n"
      "Content-Length: " + std::to_string(response.size()) + "\r\n");
    for (auto const& value: this->_headers)
      answer += elle::sprintf("%s: %s\r\n", value.first, value.second);
    answer += "\r\n" + response;
    ELLE_LOG("%s: send response to %s: %s", *this, socket.peer(), answer);
    socket.write(elle::ConstWeakBuffer(answer));
  }

  elle::Buffer
  read_sized_content(reactor::network::TCPSocket& socket, int length)
  {
    elle::Buffer content(length);
    BOOST_CHECK_GE(length, 0);
    content.size(length);
    socket.read(
      reactor::network::Buffer(content.mutable_contents(), length));
    char c;
    BOOST_CHECK_THROW(
      socket.read_some(reactor::network::Buffer(&c, 1), 100_ms),
      reactor::network::TimeOut);
    return content;
  }

  typedef std::unordered_map<std::string, std::string> Headers;
  ELLE_ATTRIBUTE_RX(Headers, headers);
};

static
std::ostream&
operator <<(std::ostream& output, ScheduledHttpServer const& server)
{
  output << "HttpServer(" << server.port() << ")";
  return output;
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
  ScheduledHttpServer server;
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

HTTP_TEST(post_no_body)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::POST, false, false, false);
}

HTTP_TEST(post_10)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::POST, false, false);
}

HTTP_TEST(post_11)
{
  reactor::http::Request::Configuration conf;
  post(conf, "HTTP/1.1", reactor::http::Method::POST, true, false);
}

HTTP_TEST(post_11_chunked)
{
  reactor::http::Request::Configuration conf;
  conf.chunked_transfers(true);
  post(conf, "HTTP/1.1", reactor::http::Method::POST, true, true);
}

HTTP_TEST(put_no_body)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::PUT, false, false, false);
}

HTTP_TEST(put_10)
{
  reactor::http::Request::Configuration conf;
  conf.version(reactor::http::Version::v10);
  post(conf, "HTTP/1.0", reactor::http::Method::PUT, false, false);
}

HTTP_TEST(put_11)
{
  reactor::http::Request::Configuration conf;
  post(conf, "HTTP/1.1", reactor::http::Method::PUT, true, false);
}

HTTP_TEST(put_11_chunked)
{
  reactor::http::Request::Configuration conf;
  conf.chunked_transfers(true);
  post(conf, "HTTP/1.1", reactor::http::Method::PUT, true, true);
}

HTTP_TEST(cookies)
{
  ScheduledHttpServer server;
  server.headers()["Set-Cookie"] = "we=got";

  {
    BOOST_CHECK_EQUAL(reactor::http::get(server.url("cookies")).string(),
                      "/cookies");
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

HTTP_TEST(request_move)
{
  ScheduledHttpServer server;

  reactor::http::Request a(
    elle::sprintf("http://127.0.0.1:%s/move", server.port()),
    reactor::http::Method::POST,
    "application/json");
  reactor::http::Request b(std::move(a));
  b << "{}";
  reactor::http::Request c(std::move(b));
  c.finalize();
  reactor::http::Request d(std::move(c));
  BOOST_CHECK_EQUAL(d.response(), "{}");
}

static
bool
test_suite()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(simple));
  suite.add(BOOST_TEST_CASE(complex));
  suite.add(BOOST_TEST_CASE(no_answer));
  suite.add(BOOST_TEST_CASE(partial_answer));
  suite.add(BOOST_TEST_CASE(connection_reset));
  suite.add(BOOST_TEST_CASE(concurrent));
  suite.add(BOOST_TEST_CASE(timeout));
  suite.add(BOOST_TEST_CASE(post_no_body));
  suite.add(BOOST_TEST_CASE(post_10));
  suite.add(BOOST_TEST_CASE(post_11));
  suite.add(BOOST_TEST_CASE(post_11_chunked));
  suite.add(BOOST_TEST_CASE(put_no_body));
  suite.add(BOOST_TEST_CASE(put_10));
  suite.add(BOOST_TEST_CASE(put_11));
  suite.add(BOOST_TEST_CASE(put_11_chunked));
  suite.add(BOOST_TEST_CASE(cookies));
  suite.add(BOOST_TEST_CASE(request_move));
  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
