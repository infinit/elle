#include <reactor/network/exception.hh>
#include <reactor/network/SocketOperation.hh>
#include <reactor/network/ssl-socket.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("reactor.network.SSLSocket");

namespace reactor
{
  namespace network
  {
    SSLCertificate::SSLCertificate(SSLCertificateMethod meth):
      _context(new boost::asio::ssl::context(meth))
    {
      this->_context->set_options(
        boost::asio::ssl::verify_none |
        boost::asio::ssl::context::default_workarounds);
    }

    SSLCertificate::SSLCertificate(std::string const& certificate,
                                   std::string const& key,
                                   std::string const& dhfile,
                                   SSLCertificateMethod meth):
      _context(new boost::asio::ssl::context(meth))
    {
      this->_context->set_options(
        boost::asio::ssl::verify_none |
        boost::asio::ssl::context::default_workarounds);
      this->_context->use_certificate_file(certificate,
                                           boost::asio::ssl::context::pem);
      this->_context->use_private_key_file(key, boost::asio::ssl::context::pem);
      this->_context->use_tmp_dh_file(dhfile);
    }

    SSLCertificate::SSLCertificate(const SSLCertificate& other):
      _context(other._context)
    {}

    SSLSocket::SSLSocket(const std::string& hostname,
                         const std::string& port,
                         SSLCertificate const& certificate,
                         Handshake_type type,
                         DurationOpt timeout):
      SSLSocket(*reactor::Scheduler::scheduler(),
                hostname, port, certificate, type, timeout)
    {}

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         SSLCertificate const& certificate,
                         Handshake_type type,
                         DurationOpt timeout):
      SSLSocket(sched, resolve_tcp(sched, hostname, port),
                certificate, type, timeout)
    {}

    SSLSocket::SSLSocket(reactor::Scheduler& sched,
                         boost::asio::ip::tcp::endpoint const& endpoint,
                         SSLCertificate const& certificate,
                         Handshake_type type,
                         DurationOpt timeout):
      Super(sched,
            elle::make_unique<SSLStream>(sched.io_service(),
                                         *certificate.context()),
            endpoint, timeout),
      _certificate(certificate),
      _timeout(timeout)
    {
      if (type == Handshake_type::client)
        this->_handshake();
    }

    SSLSocket::~SSLSocket()
    {}

    SSLSocket::SSLSocket(Scheduler& scheduler,
                         std::unique_ptr<SSLStream> socket,
                         SSLEndPoint const& endpoint,
                         SSLCertificate const& certificate):
      Super(scheduler, std::move(socket), endpoint),
      _certificate(certificate),
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
      // virtual
      // void
      // _abort()
      // {
      //   this->_socket.shutdown();
      //   this->_signal();
      // }

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

    private:
      void
      _wakeup(const boost::system::error_code& error)
      {
        if (error == boost::system::errc::operation_canceled)
          return;
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
    SSLSocket::_handshake()
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
