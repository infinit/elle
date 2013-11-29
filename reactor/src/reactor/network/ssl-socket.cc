#include <reactor/network/ssl-socket.hh>

namespace reactor
{
  namespace network
  {
    SSLCertif::SSLCertif():
      _ctx(nullptr)
    {}

    SSLCertif::SSLCertif(std::string const& pem,
                         std::string const& dhfile,
                         boost::asio::ssl::context::method meth):
      _ctx(new boost::asio::ssl::context(meth))
    {
      _ctx->use_certificate_chain_file(pem);
      _ctx->use_private_key_file(pem, boost::asio::ssl::context::pem);
      _ctx->use_tmp_dh_file(dhfile);
    }

    SSLCertif::SSLCertif(const SSLCertif& other):
      _ctx(other._ctx)
    {}

    void
    SSLCertif::release()
    {
      if (_ctx != nullptr)
        delete _ctx;
      _ctx = nullptr;
    }

    SSLSocket::SSLSocket(const std::string& hostname,
                         const std::string& port,
                         SSLCertif& cert,
                         DurationOpt timeout):
      SSLSocket(*reactor::Scheduler::scheduler(),
                hostname, port, cert, timeout)
    {}

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         SSLCertif& cert,
                         DurationOpt timeout):
      SSLSocket(sched, resolve_tcp(sched, hostname, port), cert, timeout)
    {}

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         boost::asio::ip::tcp::endpoint const& endpoint,
                         SSLCertif& cert,
                         DurationOpt timeout):
      Super(sched,
            elle::make_unique<boost::asio::ssl::stream<
              boost::asio::ip::tcp::socket>>(sched.io_service(), *cert.ctx()),
            endpoint, timeout),
      _cert(cert)
    {}

    SSLSocket::~SSLSocket()
    {}

    void
    SSLSocket::handshake(boost::asio::ssl::stream<
      boost::asio::ip::tcp::socket>::handshake_type type)
    {
      _socket->handshake(type);
    }

    boost::asio::ip::tcp::socket&
    SSLSocket::asio_socket()
    {
      typedef SocketSpecialization<
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> Spe;

      return Spe::socket(*_socket);
    }

    SSLSocket::SSLSocket(Scheduler& sched,
                         std::unique_ptr<boost::asio::ssl::stream<
                          boost::asio::ip::tcp::socket>> socket,
                         boost::asio::ip::tcp::socket::endpoint_type const& peer):
      Super(sched, std::move(socket), peer)
    {}
  }
}
