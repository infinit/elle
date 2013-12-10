#ifndef BOOST_TEST_DYN_LINK
# define BOOST_TEST_DYN_LINK
#endif

#include <boost/filesystem.hpp>

#include <elle/os/getenv.hh>
#include <elle/test.hh>

#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/network/ssl-server.hh>
#include <reactor/network/ssl-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

using reactor::network::SSLCertif;
using reactor::network::SSLSocket;
using reactor::network::SSLServer;

static
void
basics()
{
  reactor::Scheduler sched;
  reactor::Barrier listening;
  int port = 0;
  reactor::Thread t(
    sched, "main",
    [&]
    {
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        scope.run_background(
          "server",
          [&]
          {

            auto root = boost::filesystem::path(elle::os::getenv("DIR_SOURCE"))
              / boost::filesystem::path(test_binary).parent_path() / "certifs";
            SSLCertif certif((root / "server-cert.pem").native(),
                             (root / "private/server-key.pem").native(),
                             (root / "dh1024.pem").native());
              reactor::network::SSLServer server(certif);
              server.listen(0);
              port = server.port();
              listening.open();
              std::unique_ptr<reactor::network::SSLSocket> socket(
                server.accept());
              static char servdata[5] = { 0 };
              socket->std::iostream::read(servdata, 4);
              BOOST_CHECK(std::string(servdata) == std::string("lulz"));
              socket->write(std::string("lol"));
              socket->write(std::string("lulz"));
            certif.release();
          });
        scope.run_background(
          "client",
          [&]
          {
            SSLCertif certif;
              reactor::wait(listening);
              reactor::network::SSLSocket socket("127.0.0.1", std::to_string(port), certif);
              socket.write(std::string("lulz"));
              static char clientdata[5] = { 0 };
              socket.std::iostream::read(clientdata, 3);
              BOOST_CHECK(std::string(clientdata) == std::string("lol"));
              socket.std::iostream::read(clientdata, 4);
              BOOST_CHECK(std::string(clientdata) == std::string("lulz"));
            certif.release();
          });
        reactor::wait(scope);
      };
    });
  sched.run();
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(basics), 0, 3);
}
