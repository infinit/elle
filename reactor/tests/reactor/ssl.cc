#ifndef BOOST_TEST_DYN_LINK
# define BOOST_TEST_DYN_LINK
#endif

#define BOOST_TEST_MODULE SSL_Socket

#include <reactor/scheduler.hh>
#include <reactor/Scope.hh>
#include <reactor/Barrier.hh>
#include <reactor/thread.hh>
#include <reactor/network/ssl-socket.hh>
#include <reactor/network/ssl-server.hh>
#include <boost/test/unit_test.hpp>

#define OPENSSLFOLDER   "/home/manny/infinit-backup/elle/reactor/tests/reactor/certifs/"
#define SERVCERT        OPENSSLFOLDER "server-cert.pem"
#define SERVKEY         OPENSSLFOLDER "private/server-key.pem"
#define DHFILE          OPENSSLFOLDER "dh1024.pem"
#define CERTAUTH        OPENSSLFOLDER "cacert.pem"

using reactor::network::SSLCertif;
using reactor::network::SSLSocket;
using reactor::network::SSLServer;

BOOST_AUTO_TEST_CASE(basic)
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
            SSLCertif certif(SERVCERT, SERVKEY, DHFILE);
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
              BOOST_CHECK(not socket.handshake());
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
