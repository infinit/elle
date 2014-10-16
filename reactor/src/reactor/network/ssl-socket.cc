#include <reactor/network/exception.hh>
#include <reactor/network/SocketOperation.hh>
#include <reactor/network/ssl-socket.hh>
#include <reactor/scheduler.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("reactor.network.SSLSocket");

namespace reactor
{
  namespace network
  {
    SSLCertificate::SSLCertificate(SSLCertificateMethod meth):
      _context(meth)
    {
      this->_context.set_options(boost::asio::ssl::verify_none);
    }

    SSLCertificate::SSLCertificate(std::vector<char> const& certificate,
                                   std::vector<char> const& key,
                                   std::vector<char> const& dh,
                                   SSLCertificateMethod meth):
    _context(meth)
    {
      using boost::asio::const_buffer;
      this->_context.set_options(boost::asio::ssl::verify_none);
      this->_context.use_certificate(const_buffer(certificate.data(),
                                                  certificate.size()),
                                     boost::asio::ssl::context::pem);
      this->_context.use_private_key(const_buffer(key.data(), key.size()),
                                     boost::asio::ssl::context::pem);
      this->_context.use_tmp_dh(const_buffer(dh.data(), dh.size()));
    }

    SSLCertificate::SSLCertificate(std::string const& certificate,
                                   std::string const& key,
                                   std::string const& dhfile,
                                   SSLCertificateMethod meth):
      _context(meth)
    {
      this->_context.set_options(boost::asio::ssl::verify_none);
      this->_context.use_certificate_file(certificate,
                                          boost::asio::ssl::context::pem);
      this->_context.use_private_key_file(key, boost::asio::ssl::context::pem);
      this->_context.use_tmp_dh_file(dhfile);
    }

    SSLCertificateOwner::SSLCertificateOwner(
      std::shared_ptr<SSLCertificate> certificate):
        _certificate(certificate)
    {
      if (this->_certificate == nullptr)
        this->_certificate.reset(new SSLCertificate());
      ELLE_ASSERT(this->_certificate != nullptr);
    }

    SSLSocket::SSLSocket(const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout):
      SSLSocket(resolve_tcp(hostname, port), timeout)
    {}

    SSLSocket::SSLSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                         DurationOpt timeout):
      SSLCertificateOwner(),
      Super(elle::make_unique<SSLStream>(
              reactor::Scheduler::scheduler()->io_service(),
              this->certificate()->context()),
            endpoint, timeout),
      _timeout(timeout)
    {
      this->_client_handshake();
    }

    SSLSocket::SSLSocket(const std::string& hostname,
                         const std::string& port,
                         SSLCertificate& certificate,
                         DurationOpt timeout):
      SSLSocket(resolve_tcp(hostname, port), certificate, timeout)
    {}

    SSLSocket::SSLSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                         SSLCertificate& certificate,
                         DurationOpt timeout):
      SSLCertificateOwner(),
      Super(elle::make_unique<SSLStream>(
              reactor::Scheduler::scheduler()->io_service(),
              certificate.context()),
            endpoint, timeout),
      _timeout(timeout)
    {
      this->_server_handshake(this->_timeout);
    }

    SSLSocket::~SSLSocket()
    {
      // Flush before shutting SSL down.
      this->_final_flush();
      try
      {
        this->_shutdown();
      }
      catch (...)
      {
        ELLE_ERR("fatal error in SSL shutdow: %s", elle::exception_string());
        throw;
      }
    }

    SSLSocket::SSLSocket(std::unique_ptr<SSLStream> socket,
                         SSLEndPoint const& endpoint,
                         std::shared_ptr<SSLCertificate> certificate):
      SSLCertificateOwner(certificate),
      Super(std::move(socket), endpoint),
      _timeout(DurationOpt())
    {}

    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    SSLSocket::print(std::ostream& s) const
    {
      s << "SSLSocket(" << peer() << ")";
    }

    /*---------------.
    | SSL connection |
    `---------------*/

    class SSLHandshake:
      public SocketOperation<boost::asio::ip::tcp::socket>
    {
    public:
      SSLHandshake(SSLSocket& socket,
                   SSLStream::handshake_type const& type):
        SocketOperation(socket.socket()->next_layer()),
        _socket(socket),
        _type(type)
      {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        elle::fprintf(stream, "SSL handshake %s", this->_socket);
      }

    protected:
      virtual
      void
      _start()
      {
        this->_socket.socket()->async_handshake(
          this->_type,
          std::bind(&SSLHandshake::_wakeup,
                    this,
                    std::placeholders::_1));
      }

    private:
      virtual
      void
      _handle_error(boost::system::error_code const& error) override
      {
        this->_raise<SSLHandshakeError>(error.message());
      }

      ELLE_ATTRIBUTE(SSLSocket&, socket);
      ELLE_ATTRIBUTE(SSLStream::handshake_type, type);
    };

    void
    SSLSocket::_client_handshake()
    {
      ELLE_TRACE_SCOPE("%s: handshake as client", *this);
      SSLHandshake handshaker(*this, SSLStream::handshake_type::client);
      if (!handshaker.run(this->_timeout))
        throw TimeOut();
    }

    void
    SSLSocket::_server_handshake(reactor::DurationOpt const& timeout)
    {
      ELLE_TRACE_SCOPE("%s: handshake as server", *this);
      SSLHandshake handshaker(*this, SSLStream::handshake_type::server);
      if (!handshaker.run(timeout))
        throw TimeOut();
    }


    class SSLShutdown:
      public DataOperation<boost::asio::ip::tcp::socket>
    {
    public:
      typedef DataOperation<boost::asio::ip::tcp::socket> Super;

      SSLShutdown(SSLSocket& socket)
        : Super(socket.socket()->next_layer())
        , _socket(socket)
      {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        elle::fprintf(stream, "SSL shutdown %s", this->_socket);
      }

    protected:
      virtual
      void
      _start()
      {
        this->_socket.socket()->async_shutdown(
          std::bind(&SSLShutdown::_wakeup,
                    this,
                    std::placeholders::_1));
      }

      ELLE_ATTRIBUTE(SSLSocket&, socket);
    };

    void
    SSLSocket::_shutdown()
    {
      ELLE_TRACE_SCOPE("%s: shutdown SSL", *this);
      try
      {
        SSLShutdown shutdown(*this);
        if (!shutdown.run(this->_timeout))
        {
          ELLE_TRACE("%s: SSL shutdown timed out (%s)", *this, this->_timeout);
          throw TimeOut();
        }
      }
      catch (ConnectionClosed const&)
      {
        ELLE_DEBUG("%s: SSL already shutdown by peer", *this);
      }
      catch (SocketClosed const&)
      {
        ELLE_DEBUG("%s: socket is already closed", *this);
      }
    }
  }
}
