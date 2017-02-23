#pragma once

#include <elle/reactor/Channel.hh>
#include <elle/reactor/network/server.hh>
#include <elle/reactor/network/ssl-socket.hh>
#include <elle/utility/Duration.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      class SSLServer
        : public ProtoServer<boost::asio::ip::tcp::socket,
                             boost::asio::ip::tcp::endpoint,
                             boost::asio::ip::tcp::acceptor>
      {
      /*------.
      | Types |
      `------*/
      public:
        typedef ProtoServer<boost::asio::ip::tcp::socket,
                            boost::asio::ip::tcp::endpoint,
                            boost::asio::ip::tcp::acceptor> Super;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        SSLServer(std::unique_ptr<SSLCertificate> certificate,
                  reactor::Duration const& handshake_timeout = 30_sec);
        virtual
        ~SSLServer();

      /*----------.
      | Accepting |
      `----------*/
      public:
        std::unique_ptr<SSLSocket>
        accept();
        using Super::listen;
        void
        listen(int port);
        int
        port() const;
      protected:
        using Super::_accept;
        virtual
        std::unique_ptr<Socket>
        _accept() override;
        virtual
        EndPoint
        _default_endpoint() const override;
      private:
        void
        _handshake();
        ELLE_ATTRIBUTE(std::shared_ptr<SSLCertificate>, certificate);
        ELLE_ATTRIBUTE(reactor::Duration, handshake_timeout);
        ELLE_ATTRIBUTE(reactor::Channel<std::unique_ptr<SSLSocket>>, sockets);
        ELLE_ATTRIBUTE(reactor::Thread, handshake_thread);
        ELLE_ATTRIBUTE_RW(bool, shutdown_asynchronous);
      };
    }
  }
}
