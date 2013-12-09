#include <reactor/network/ssl-socket.hh>
#include <reactor/network/exception.hh>
#include <reactor/operation.hh>

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("reactor.network.SSLSocket");

namespace reactor
{
  namespace network
  {

    // Local fingerprint in sha1.
    static unsigned int loc_sha1_size = 20;
    static unsigned char loc_sha1[20] =
    {
      0x66, 0x84, 0x68, 0xEB, 0xBE, 0x83, 0xA0, 0x5C, 0x6A, 0x32,
      0xAD, 0xD2, 0x58, 0x62, 0x01, 0x31, 0x79, 0x96, 0x78, 0xB8
    };

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
      _ctx->set_verify_mode(boost::asio::ssl::verify_none);
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
                         Handshake_type type,
                         DurationOpt timeout):
      SSLSocket(*reactor::Scheduler::scheduler(),
                hostname, port, cert, type, timeout)
    {}

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         SSLCertif& cert,
                         Handshake_type type,
                         DurationOpt timeout):
      SSLSocket(sched, resolve_tcp(sched, hostname, port), cert, type, timeout)
    {}

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         boost::asio::ip::tcp::endpoint const& endpoint,
                         SSLCertif& cert,
                         Handshake_type type,
                         DurationOpt timeout):
      Super(sched,
            elle::make_unique<boost::asio::ssl::stream<
              boost::asio::ip::tcp::socket>>(sched.io_service(), *cert.ctx()),
            endpoint, timeout),
      _cert(cert)
    {
      if (type == Handshake_type::client)
        _handshake();
    }

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         boost::asio::ip::tcp::endpoint const& endpoint,
                         SSLCertif& cert,
                         Handshake_type type):
      Super(sched,
            elle::make_unique<boost::asio::ssl::stream<
              boost::asio::ip::tcp::socket>>(sched.io_service(), *cert.ctx()),
            endpoint),
      _cert(cert)
    {
      if (type == Handshake_type::client)
        _handshake();
    }

    SSLSocket::~SSLSocket()
    {}

    class SSLHandshake: public Operation
    {
    public:
      SSLHandshake(Scheduler& scheduler,
          boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
          boost::asio::ssl::stream<
            boost::asio::ip::tcp::socket>::handshake_type&& type):
        Operation(scheduler),
        _socket(socket),
        _type(type)
      {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "socket handshake";
      }

    protected:
      virtual void _abort()
      {
        //TODO
        _signal();
      }

      virtual void _start()
      {
      _socket.async_handshake(this->_type,
        boost::bind(&SSLHandshake::_wakeup, this,
        boost::asio::placeholders::error));
      }

    private:
      void _wakeup(const boost::system::error_code& error)
      {
        if (error == boost::system::errc::operation_canceled)
          return;
        if (error)
          _raise<Exception>(error.message());
        _signal();
      }

      ELLE_ATTRIBUTE(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>&,
        socket);
      ELLE_ATTRIBUTE(boost::asio::ssl::stream<
        boost::asio::ip::tcp::socket>::handshake_type, type);
    };

    bool
    SSLSocket::_handshake()
    {
      ELLE_DEBUG("start client handshake");
      SSLHandshake handshaker(scheduler(), *_socket,
        boost::asio::ssl::stream<
          boost::asio::ip::tcp::socket>::handshake_type::client);
      handshaker.run();
      ELLE_DEBUG("client handshake done");

      ELLE_DEBUG("certificate verification");
      SSL *ssl = _socket->native_handle();
      X509 *cert = SSL_get_peer_certificate(ssl);

      ELLE_DEBUG("ssl obj is %s", ssl);
      ELLE_DEBUG("cert is %s", cert);

      // Get fingerprint of peer certificate in sha1.
      unsigned int peer_sha1_size;
      unsigned char peer_sha1[EVP_MAX_MD_SIZE];
      X509_digest(cert, EVP_sha1(), peer_sha1, &peer_sha1_size);
      X509_free(cert);

      // Comparison of both fingerprints.
      if (peer_sha1_size != loc_sha1_size)
        return 1;
      return !!memcmp(peer_sha1, loc_sha1, peer_sha1_size);
    }

    void
    SSLSocket::_server_handshake()
    {
      ELLE_DEBUG("start server handshake");
      SSLHandshake handshaker(scheduler(), *_socket,
        boost::asio::ssl::stream<
          boost::asio::ip::tcp::socket>::handshake_type::server);
      handshaker.run();
      ELLE_DEBUG("server handshake done");
    }
  }
}
