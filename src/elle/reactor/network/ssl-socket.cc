#include <elle/log.hh>
#include <elle/reactor/network/SocketOperation.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/ssl-socket.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/utility/Move.hh>
#include <utility>

ELLE_LOG_COMPONENT("elle.reactor.network.SSLSocket");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      SSLCertificate::SSLCertificate(SSLCertificateMethod meth)
        : _context(meth)
      {
        this->_context.set_options(boost::asio::ssl::verify_none);
      }

      SSLCertificate::SSLCertificate(std::vector<char> const& certificate,
                                     std::vector<char> const& key,
                                     std::vector<char> const& dh,
                                     SSLCertificateMethod meth)
        : _context(meth)
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
                                     SSLCertificateMethod meth)
        : _context(meth)
      {
        this->_context.set_options(boost::asio::ssl::verify_none);
        this->_context.use_certificate_file(certificate,
                                            boost::asio::ssl::context::pem);
        this->_context.use_private_key_file(key,
                                            boost::asio::ssl::context::pem);
        this->_context.use_tmp_dh_file(dhfile);
      }

      SSLCertificateOwner::SSLCertificateOwner(
        std::shared_ptr<SSLCertificate> certificate)
        : _certificate(std::move(certificate))
      {
        if (this->_certificate == nullptr)
          this->_certificate.reset(new SSLCertificate());
        ELLE_ASSERT(this->_certificate != nullptr);
      }

      SSLSocket::SSLSocket(const std::string& hostname,
                           const std::string& port,
                           DurationOpt timeout)
        : SSLSocket(resolve_tcp(hostname, port)[0], timeout)
      {}

      SSLSocket::SSLSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                           DurationOpt timeout)
        : SSLCertificateOwner()
        , Super(std::make_unique<SSLStream>(
                  reactor::Scheduler::scheduler()->io_service(),
                  this->certificate()->context()),
                endpoint, timeout)
        , _shutdown_asynchronous(false)
        , _timeout(timeout)
      {
        this->_client_handshake();
      }

      SSLSocket::SSLSocket(const std::string& hostname,
                           const std::string& port,
                           SSLCertificate& certificate,
                           DurationOpt timeout)
        : SSLSocket(resolve_tcp(hostname, port)[0], certificate, timeout)
      {}

      SSLSocket::SSLSocket(boost::asio::ip::tcp::endpoint const& endpoint,
                           SSLCertificate& certificate,
                           DurationOpt timeout)
        : SSLCertificateOwner()
        , Super(std::make_unique<SSLStream>(
                  reactor::Scheduler::scheduler()->io_service(),
                  certificate.context()),
                endpoint, timeout)
        , _shutdown_asynchronous(false)
        , _timeout(timeout)
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
        catch (elle::Error const&)
        {
          // Ignore shutdown error. This could be configurable.
          ELLE_WARN("SSL shutdow error: %s", elle::exception_string());
        }
        catch (reactor::Terminate const&)
        {
          if (!reactor::scheduler().current()->terminating())
            throw;
          else
            ELLE_WARN("%s: ignore stacked thread termination during SSL "
                      "shutdown", this);
        }
        catch (...)
        {
          ELLE_ABORT("unexpected error in SSL shutdown: %s",
                     elle::exception_string());
        }
      }

      SSLSocket::SSLSocket(std::unique_ptr<SSLStream> socket,
                           SSLEndPoint const& endpoint,
                           std::shared_ptr<SSLCertificate> certificate,
                           DurationOpt handshake_timeout)
        : SSLCertificateOwner(certificate)
        , Super(std::move(socket), endpoint)
        , _shutdown_asynchronous(false)
        , _timeout(std::move(handshake_timeout))
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

        void
        print(std::ostream& stream) const override
        {
          elle::fprintf(stream, "SSL handshake %s", this->_socket);
        }

      protected:
        void
        _start() override
        {
          this->_socket.socket()->async_handshake(
            this->_type,
            [this](const boost::system::error_code& error)
            {
              this->_wakeup(error);
            });
        }

      private:
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
        using Super = DataOperation<boost::asio::ip::tcp::socket>;

        SSLShutdown(SSLSocket& socket)
          : Super(socket.socket()->next_layer())
          , _socket(socket)
        {}

        void
        print(std::ostream& stream) const override
        {
          elle::fprintf(stream, "SSL shutdown %s", this->_socket);
        }

      protected:
        void
        _start() override
        {
          this->_socket.socket()->async_shutdown(
            [this](const boost::system::error_code& error)
            {
              this->_wakeup(error);
            });
        }

        ELLE_ATTRIBUTE(SSLSocket&, socket);
      };

      void
      SSLSocket::_shutdown()
      {
        if (!this->_shutdown_asynchronous)
        {
          ELLE_TRACE_SCOPE("%s: shutdown SSL", *this);
          try
          {
            SSLShutdown shutdown(*this);
            if (!shutdown.run(this->_timeout))
            {
              ELLE_TRACE("%s: SSL shutdown timed out (%s)",
                         *this, *this->_timeout);
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
        else
        {
          ELLE_TRACE_SCOPE("%s: shutdown SSL asynchronously", *this);
          auto const has_timeout = bool(this->_timeout);
          auto socket_raw = this->_socket.get();
          auto socket = elle::utility::move_on_copy(this->_socket);
          auto timer = elle::utility::move_on_copy(
            std::make_unique<boost::asio::deadline_timer>(
              reactor::scheduler().io_service()));
          auto timer_raw = (*timer).get();
          auto over = std::make_shared<bool>(false);
          socket_raw->async_shutdown(
            [over, socket, timer_raw, has_timeout]
            (boost::system::error_code const& error)
            {
              if (!error || error != boost::system::errc::operation_canceled)
              {
                if (has_timeout && !*over)
                  timer_raw->cancel();
                *over = true;
                // EOF simply means the other side shut SSL down properly.
                if (error && error != boost::asio::error::eof)
                  ELLE_WARN("error on asynchronous SSL shutdown: %s",
                            error.message());
              }
              ELLE_TRACE("asynchronous SSL shutdown done");
              boost::system::error_code e;
              socket->next_layer().shutdown(
                boost::asio::ip::tcp::socket::shutdown_both, e);
              if (e)
                ELLE_WARN("error on async shutdown: %s", error.message());
              socket->next_layer().close();
            });
          if (has_timeout)
          {
            auto timeout = *this->_timeout;
            timer->expires_from_now(timeout);
            timer_raw->async_wait(
              [over, timer, socket_raw, timeout]
              (boost::system::error_code const& error)
              {
                if (!error)
                {
                  ELLE_WARN("asynchronous SSL shutdown timeout after %s",
                            timeout);
                  if (!*over)
                    socket_raw->next_layer().cancel();
                  *over = true;
                }
                else if (error !=  boost::asio::error::operation_aborted)
                  ELLE_ABORT(
                    "unexpected asynchronous SSL shutdown timeout error: %s",
                    error.message());
              });
          }
        }
      }
    }
  }
}
