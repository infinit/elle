#include <reactor/network/ssl-socket.hh>

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

namespace reactor
{
  namespace network
  {
    SSLCertif::SSLCertif(boost::asio::ssl::context::method meth):
      _ctx(new boost::asio::ssl::context(meth))
    {
      _ctx->set_verify_mode(boost::asio::ssl::verify_none);
    }

    SSLCertif::SSLCertif(std::string const& cert,
                         std::string const& key,
                         std::string const& dhfile,
                         boost::asio::ssl::context::method meth):
      _ctx(new boost::asio::ssl::context(meth))
    {
      _ctx->use_certificate_file(cert, boost::asio::ssl::context::pem);
      _ctx->use_private_key_file(key, boost::asio::ssl::context::pem);
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
                         SSLCertif& cert):
      Super(sched,
            elle::make_unique<boost::asio::ssl::stream<
              boost::asio::ip::tcp::socket>>(sched.io_service(), *cert.ctx()),
            endpoint),
      _cert(cert)
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

    bool
    SSLSocket::handshake()
    {
      _socket->handshake(boost::asio::ssl::stream<
        boost::asio::ip::tcp::socket>::handshake_type::client);

      char const* loc_sha1 = "hello";
      unsigned int loc_sha1_size = 5;

      SSL *ssl = _socket->native_handle();
      X509 *cert = SSL_get_peer_certificate(ssl);

      // Get fingerprint of peer certificate in sha1.
      unsigned int peer_sha1_size;
      unsigned char peer_sha1[EVP_MAX_MD_SIZE];
      X509_digest(cert, EVP_sha1(), peer_sha1, &peer_sha1_size);
      X509_free(cert);

      if (peer_sha1_size == loc_sha1_size)
        return !!memcmp(peer_sha1, loc_sha1, peer_sha1_size);
      return !!(peer_sha1_size - loc_sha1_size);
    }

    void
    SSLSocket::server_handshake()
    {
      _socket->handshake(boost::asio::ssl::stream<
        boost::asio::ip::tcp::socket>::handshake_type::server);
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
