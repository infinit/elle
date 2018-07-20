#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/numeric.hpp>

#include <network/uri.hpp>

#include <elle/log.hh>
#include <elle/reactor/http/WebSocket.hh>
#include <elle/reactor/network/SSLHandshake.hh>
#include <elle/reactor/network/resolve.hh>
#include <elle/reactor/network/socket.hxx>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.http.WebSocket")

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      static
      auto&
      ssl_context()
      {
        static auto ctx =
          boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
        return ctx;
      }

      class WebSocket::WebSocket::Impl
      {
      public:
        using Socket = boost::asio::ip::tcp::socket;
        using SSL = boost::asio::ssl::stream<Socket&>;
        Impl(std::string const& uri);

        ELLE_ATTRIBUTE_R(::network::uri, uri);
        ELLE_ATTRIBUTE_RX(Socket, socket);
        ELLE_ATTRIBUTE_R(SSL, ssl);
        ELLE_ATTRIBUTE_RX(
          boost::beast::websocket::stream<Socket&>, websocket_plain)
        ELLE_ATTRIBUTE_RX(
          boost::beast::websocket::stream<SSL&>, websocket_ssl)
        ELLE_ATTRIBUTE_R(bool, is_ssl)
      };

      WebSocket::WebSocket(std::string const& uri)
        : _impl(std::make_unique<Impl>(uri))
      {}

      WebSocket::WebSocket(WebSocket&& source)
        : _impl(std::move(source._impl))
      {}

      WebSocket::~WebSocket()
      {}

      class WebSocketHandshake
        : public network::SocketOperation<WebSocket::Impl::Socket>
      {
      public:
        using Self = WebSocketHandshake;
        using Super = network::SocketOperation<WebSocket::Impl::Socket>;

        WebSocketHandshake(WebSocket::Impl& socket,
                           std::string host,
                           std::string path)
          : Super(socket.socket())
          , _socket(socket)
          , _host(std::move(host))
          , _path(std::move(path))
        {}

        void
        _start() override
        {
          auto connect = [this] (auto& sock) {
            sock.async_handshake(
              this->_host,
              this->_path,
              [&] (boost::system::error_code const& e)
              {
                this->_wakeup(e);
              });
          };
          if (this->_socket.is_ssl())
            connect(this->_socket.websocket_ssl());
          else
            connect(this->_socket.websocket_plain());
        }

        ELLE_ATTRIBUTE(WebSocket::Impl&, socket);
        ELLE_ATTRIBUTE(std::string, host);
        ELLE_ATTRIBUTE(std::string, path);
      };

      WebSocket::Impl::Impl(std::string const& uri)
        : _uri(uri)
        , _socket(reactor::scheduler().io_service())
        , _ssl(this->_socket, ssl_context())
        , _websocket_plain(this->_socket)
        , _websocket_ssl(this->_ssl)
      {
        auto const host = this->_uri.host().to_string();
        auto const path = this->_uri.path().to_string();
        auto const port = [&] () -> std::string
          {
            auto const scheme = this->_uri.scheme().to_string();
            // Resolve wss and ws, as /etc/services probably don't know them.
            if (scheme == "wss")
              return "443";
            else if (scheme == "ws")
              return "80";
            else
              return scheme;
          }();
        auto const ep = network::resolve_tcp(host, port)[0];
        this->_is_ssl = ep.port() == 443;
        auto c = network::Connection<Socket>(this->_socket, ep);
        c.run();
        if (this->_is_ssl)
        {
          ELLE_DEBUG_SCOPE("{}: SSL handshake", this);
          auto handshake = network::SSLHandshake<SSL>(
            this->_ssl, network::SSLStream::handshake_type::client);
          handshake.run();
        }
        {
          ELLE_DEBUG_SCOPE("{}: websocket handshake", this);
          auto handshake = WebSocketHandshake(*this, host, path);
          handshake.run();
        }
      }

      class WebSocketRead
        : public network::SocketOperation<WebSocket::Impl::Socket>
      {
      public:
        using Self = WebSocketRead;
        using Super = network::SocketOperation<WebSocket::Impl::Socket>;

        WebSocketRead(WebSocket::Impl& socket)
          : Super(socket.socket())
          , _socket(socket)
        {}

        void
        _start() override
        {
          auto read = [this] (auto& sock) {
            sock.async_read(
              this->_buffer,
              [this] (boost::system::error_code const& e, std::size_t)
              {
                this->_wakeup(e);
              });
          };
          if (this->_socket.is_ssl())
            read(this->_socket.websocket_ssl());
          else
            read(this->_socket.websocket_plain());
        }

        ELLE_ATTRIBUTE(WebSocket::Impl&, socket);
        ELLE_ATTRIBUTE_R(boost::beast::flat_buffer, buffer);
      };

      Buffer
      WebSocket::receive()
      {
        using boost::adaptors::transformed;
        auto read = WebSocketRead(*this->_impl);
        read.run();
        Buffer res(read.buffer().size());
        // FIXME: steal the memory instead
        memcpy(
          res.mutable_contents(),
          boost::asio::buffer_cast<Buffer::Byte const*>(
            read.buffer().data()),
          res.size());
        return res;
      }

      class WebSocketWrite
        : public network::SocketOperation<WebSocket::Impl::Socket>
      {
      public:
        using Self = WebSocketWrite;
        using Super = network::SocketOperation<WebSocket::Impl::Socket>;

        WebSocketWrite(WebSocket::Impl& socket, ConstWeakBuffer data)
          : Super(socket.socket())
          , _socket(socket)
          , _buffer(boost::asio::buffer(data.contents(), data.size()))
        {}

        void
        _start() override
        {
          auto write = [this] (auto& sock) {
            sock.async_write(
              this->_buffer,
              [&] (boost::system::error_code const& e, std::size_t)
              {
                this->_wakeup(e);
              });
          };
          if (this->_socket.is_ssl())
            write(this->_socket.websocket_ssl());
          else
            write(this->_socket.websocket_plain());
        }

        ELLE_ATTRIBUTE(WebSocket::Impl&, socket);
        ELLE_ATTRIBUTE_R(boost::asio::const_buffers_1, buffer);
      };

      void
      WebSocket::send(ConstWeakBuffer data)
      {
        using boost::adaptors::transformed;
        auto write = WebSocketWrite(*this->_impl, data);
        write.run();
      }

    }
  }
}
