#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#endif
#include <boost/filesystem.hpp>

#include <elle/os/getenv.hh>
#include <elle/test.hh>

#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/network/ssl-server.hh>
#include <reactor/network/ssl-socket.hh>
#include <reactor/network/fingerprinted-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.network.SSL.test");

using reactor::network::FingerprintedSocket;
using reactor::network::SSLCertificate;
using reactor::network::SSLSocket;
using reactor::network::SSLServer;

// Local fingerprint in sha1.

static const std::vector<unsigned char> fingerprint =
{
  0x66, 0x84, 0x68, 0xEB, 0xBE, 0x83, 0xA0, 0x5C, 0x6A, 0x32,
  0xAD, 0xD2, 0x58, 0x62, 0x01, 0x31, 0x79, 0x96, 0x78, 0xB8
};

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
            auto root = boost::filesystem::path(elle::os::getenv("DIR_SOURCE"));
            root /= boost::filesystem::path(elle::os::getenv("DIR_BUILD"));
            root /= "tests/reactor/certifs";
            std::cerr << (root / "server-cert.pem").string() << std::endl;
            SSLCertificate certificate(
              (root / "server-cert.pem").string(),
              (root / "private/server-key.pem").string(),
              (root / "dh1024.pem").string());
            ELLE_DEBUG("./read certificate from disk");
            SSLServer server(certificate);
            server.listen(0);
            port = server.port();
            listening.open();
            std::unique_ptr<SSLSocket> socket(server.accept());
            static char servdata[5] = { 0 };
            socket->std::iostream::read(servdata, 4);
            BOOST_CHECK(std::string(servdata) == std::string("lulz"));
            socket->write(std::string("lol"));
            socket->write(std::string("lulz"));
          });
        scope.run_background(
          "client",
          [&]
          {
            SSLCertificate certificate;
            reactor::wait(listening);
            auto endpoint = reactor::network::resolve_tcp(
              *reactor::Scheduler::scheduler(),
              "127.0.0.1",
              boost::lexical_cast<std::string>(port));
            FingerprintedSocket socket(*reactor::Scheduler::scheduler(),
                                       endpoint,
                                       certificate,
                                       fingerprint);
            socket.write(std::string("lulz"));
            static char clientdata[5] = { 0 };
            socket.std::iostream::read(clientdata, 3);
            BOOST_CHECK(std::string(clientdata) == std::string("lol"));
            socket.std::iostream::read(clientdata, 4);
            BOOST_CHECK(std::string(clientdata) == std::string("lulz"));
          });
        reactor::wait(scope);
      };
    });
  sched.run();
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(basics), 0, 10);
}
