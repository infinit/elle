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

    SSLCertificate::SSLCertificate(std::string const& certificate,
                                   std::string const& key,
                                   std::string const& dhfile,
                                   SSLCertificateMethod meth):
      _context(meth)
    {
      this->_context.set_options(boost::asio::ssl::verify_none);
      this->_context.use_certificate_chain_file(certificate);
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
      SSLSocket(resolve_tcp(*reactor::Scheduler::scheduler(), hostname, port),
                timeout)
    {}

    SSLSocket::SSLSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                         DurationOpt timeout):
      SSLCertificateOwner(),
      Super(*reactor::Scheduler::scheduler(),
            elle::make_unique<SSLStream>(
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
      SSLSocket(resolve_tcp(*reactor::Scheduler::scheduler(), hostname, port),
                certificate,
                timeout)
    {}

    SSLSocket::SSLSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                         SSLCertificate& certificate,
                         DurationOpt timeout):
      SSLCertificateOwner(),
      Super(*reactor::Scheduler::scheduler(),
            elle::make_unique<SSLStream>(
              reactor::Scheduler::scheduler()->io_service(),
              certificate.context()),
            endpoint, timeout),
      _timeout(timeout)
    {
      this->_server_handshake();
    }

    SSLSocket::~SSLSocket()
    {}

    SSLSocket::SSLSocket(std::unique_ptr<SSLStream> socket,
                         SSLEndPoint const& endpoint,
                         std::shared_ptr<SSLCertificate> certificate):
      SSLCertificateOwner(certificate),
      Super(*reactor::Scheduler::scheduler(), std::move(socket), endpoint),
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

    class SSLHandshake:
      public SocketOperation<boost::asio::ip::tcp::socket>
    {
    public:
      SSLHandshake(Scheduler& scheduler,
                   SSLStream& socket,
                   SSLStream::handshake_type const& type):
        SocketOperation(scheduler, socket.next_layer()),
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
      virtual
      void
      _start()
      {
        ELLE_DUMP("%s: start async handshake", *this);
        this->_socket.async_handshake(
          this->_type,
          boost::bind(&SSLHandshake::_wakeup,
                      this,
                      boost::asio::placeholders::error));
      }

      virtual
      void
      _abort() override
      {
        boost::system::error_code ec;
        this->_socket.next_layer().cancel(ec);
        // Cancel may fail if for instance the socket was closed manually. If
        // cancel fails, assume the operation is de facto cancelled and we can
        // carry on. I know of no case were we "were not actually able to
        // cancel the operation".
        (void) ec;
        reactor::wait(*this);
      }

    private:
      void
      _wakeup(const boost::system::error_code& error)
      {
        if (error == boost::system::errc::operation_canceled)
        {
          this->_signal();
          return;
        }
        if (error)
        {
          ELLE_ERR("error (%s): %s", *this, error.message());
          this->_raise<SSLHandshakeError>(error.message());
        }
        this->_signal();
      }

      ELLE_ATTRIBUTE(SSLStream&, socket);
      ELLE_ATTRIBUTE(SSLStream::handshake_type, type);
    };

    void
    SSLSocket::_client_handshake()
    {
      ELLE_DEBUG("start client handshake");
      SSLHandshake handshaker(scheduler(),
                              *this->_socket,
                              SSLStream::handshake_type::client);
      if (!handshaker.run(this->_timeout))
        throw TimeOut();
      ELLE_DEBUG("client handshake done");
    }

    void
    SSLSocket::_server_handshake()
    {
      ELLE_DEBUG("start server handshake");
      SSLHandshake handshaker(scheduler(),
                              *this->_socket,
                              SSLStream::handshake_type::server);
      if (!handshaker.run(this->_timeout))
        throw TimeOut();
      ELLE_DEBUG("server handshake done");
    }
  }
}
