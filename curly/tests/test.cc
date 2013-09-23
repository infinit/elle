#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <memory>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

#include <elle/log.hh>
#include <elle/printf.hh>
#include <elle/utility/Move.hh>

#include <curly/curly.hh>

ELLE_LOG_COMPONENT("curly.test");

class httpd
{
public:
  httpd():
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

  void
  _accept()
  {
    assert(!this->_accepted_socket);
    this->_accepted_socket.reset(
      new boost::asio::ip::tcp::socket(this->_io_service));
    auto cb = std::bind(&httpd::_accepted, this, std::placeholders::_1);
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
      ELLE_ERR("%s: accept error: %s", *this, e);
      std::abort();
    }
    ELLE_LOG("%s: accepted connection from %s",
             *this, this->_accepted_endpoint);
    assert(this->_accepted_socket);
    this->_serve(std::move(this->_accepted_socket));
    this->_accept();
  }

  void
  _serve(std::unique_ptr<boost::asio::ip::tcp::socket> _socket)
  {
    auto socket = _socket.get();
    auto socket_move = elle::utility::move_on_copy<std::unique_ptr<boost::asio::ip::tcp::socket>>(std::move(_socket));
    auto buffer = new boost::asio::streambuf;
    auto cb = [this, socket_move, buffer]
      (boost::system::error_code const& e, size_t size)
      {
        auto& socket = *socket_move.value;
        auto peer = socket.remote_endpoint();
        if (e)
        {
          ELLE_ERR("%s: read error on %s: %s", *this, peer, e);
          std::abort();
        }
        ELLE_LOG("%s: read on %s: %s", *this, peer, buffer);
        delete buffer;
        auto answer = new std::string(
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/plain; charset=UTF-8\r\n"
          "Connection: Close\r\n"
          "\r\n"
          "lol\n");
        auto answer_buffer = boost::asio::buffer(answer->c_str(),
                                                 answer->size());
        auto cb = [this, socket_move, answer]
          (boost::system::error_code const& e, size_t size)
          {
            delete answer;
            auto& socket = *socket_move.value;
            auto peer = socket.remote_endpoint();
            if (e)
            {
              ELLE_ERR("%s: write error on %s: %s", *this, peer, e);
              std::abort();
            }
          };
        ELLE_LOG("%s: write to %s: %s", *this, peer, *answer);
        boost::asio::async_write(socket, answer_buffer, cb);
      };
    boost::asio::async_read_until(*socket, *buffer,
                                  std::string("\r\n\r\n"), cb);
  }

  ~httpd()
  {
    this->_server.close();
    this->_thread->join();
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

httpd global_server;

static
void
simple()
{
  ELLE_LOG("Get %s", global_server.root_url());
  std::string page = curly::get(global_server.root_url());
  BOOST_CHECK(page.empty() == false);
}

static
void
complex()
{
  auto rc = curly::make_get();

  rc.url(global_server.root_url());
  curly::request r(std::move(rc));
  BOOST_CHECK(r.code() == 200);
}

static
void
timed()
{
  auto rc = curly::make_get();

  rc.url(global_server.root_url());
  curly::request r(std::move(rc));
  BOOST_CHECK(r.code() == 200);
  std::cout << "time is " << r.time().count() << std::endl;
  BOOST_CHECK(r.time() != std::chrono::duration<double>(0));
}

static
void
threaded()
{
  int const number_of_thread = 3;
  std::vector<std::thread> vt;

  std::mutex mutex;
  auto fn = [&] {
    for (int i = 0; i < 10 * number_of_thread; ++i)
    {
      auto rc = curly::make_get();
      rc.url(global_server.root_url());
      rc.option(CURLOPT_CONNECTTIMEOUT, 300000);
      rc.option(CURLOPT_TIMEOUT, 300000);
      rc.header("Connection", "close");

      std::string page;
      {
        std::stringstream ss;
        rc.output(ss);
        curly::request r(std::move(rc));
        page = ss.str();
      }

      {
        std::lock_guard<std::mutex> guard{mutex};
        BOOST_CHECK_EQUAL(page.empty(), false);
      }
    }
  };

  for (int i = 0; i < number_of_thread; ++i)
    vt.emplace_back(fn);

  for (auto& th: vt)
    th.join();
}

static
bool
test_suite()
{
  auto& ts = boost::unit_test::framework::master_test_suite();
  ts.add(BOOST_TEST_CASE(simple), 0, 20);
  ts.add(BOOST_TEST_CASE(complex), 0, 20);
  ts.add(BOOST_TEST_CASE(timed), 0, 20);
  ts.add(BOOST_TEST_CASE(threaded), 0, 20);
  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
