#pragma once

#include <boost/asio.hpp>

#include <elle/reactor/network/SocketOperation.hh>
#include <elle/reactor/network/ssl-socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      template <typename Socket>
      class SSLHandshake:
        public SocketOperation<boost::asio::ip::tcp::socket>
      {
      public:
        SSLHandshake(Socket& stream, typename Socket::handshake_type type);

        void
        print(std::ostream& stream) const override;

      protected:
        void
        _start() override;
        void
        _handle_error(boost::system::error_code const& error) override;

        ELLE_ATTRIBUTE(Socket&, stream);
        ELLE_ATTRIBUTE(typename Socket::handshake_type, type);
      };
    }
  }
}

#include <elle/reactor/network/SSLHandshake.hxx>
