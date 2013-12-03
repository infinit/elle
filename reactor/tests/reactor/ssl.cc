#ifndef BOOST_TEST_DYN_LINK
# define BOOST_TEST_DYN_LINK
#endif

#define BOOST_TEST_MODULE SSL_Socket

#include <reactor/scheduler.hh>
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

  auto serv_func = []
  {
    SSLCertif certif(SERVCERT, SERVKEY, DHFILE);

    certif.release();
  };

  auto client_func = [] (reactor::Thread* serv)
  {
    SSLCertif certif(CERTAUTH);
    certif.release();

    serv->terminate_now();
  };

  reactor::Thread* server = new reactor::Thread(sched, "server", serv_func);
  new reactor::Thread(sched, "client", std::bind(client_func, server));

  sched.run();
}
