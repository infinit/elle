#include <elle/reactor/http/Server.hh>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/asio.hpp>

#include <elle/IOStream.hh>
#include <elle/log.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/network/SocketOperation.hh>
#include <elle/reactor/network/TCPSocket.hh>

ELLE_LOG_COMPONENT("elle.reactor.http.Server");

namespace elle
{
  namespace reactor
  {
    namespace http
    {
      using Reader =
        boost::beast::http::request_parser<boost::beast::http::buffer_body>;
      Server::Server(int port)
      {
        this->_server.listen(port);
      }

      int
      Server::port() const
      {
        return this->_server.port();
      }

      template <typename Serializer>
      class HTTPOperation
        : public network::SocketOperation<boost::asio::ip::tcp::socket>
      {
      public:
        using Super = network::SocketOperation<boost::asio::ip::tcp::socket>;
        HTTPOperation(AsioSocket& s, Serializer& serializer)
          : Super(s)
          , _serializer(serializer)
        {}

      protected:
        void
        _wakeup(boost::system::error_code error)
        {
          if (error == boost::beast::http::error::need_buffer)
            error.assign(0, error.category());
          if (error == boost::beast::http::error::end_of_stream)
            this->Super::_wakeup(boost::asio::error::eof);
          else
            this->Super::_wakeup(error);
        }

        ELLE_ATTRIBUTE_RX(Serializer&, serializer);
      };

      class HTTPReadOperation
        : public HTTPOperation<Reader>
      {
      public:
        HTTPReadOperation(AsioSocket& s, Reader& serializer,
                      boost::beast::flat_buffer& buffer)
          : HTTPOperation(s, serializer)
          , _buffer(buffer)
        {}

        ELLE_ATTRIBUTE_RX(boost::beast::flat_buffer&, buffer);
      };

      class HTTPReadHeaders
        : public HTTPReadOperation
      {
      public:
        using Super = HTTPReadOperation;
        using Super::Super;

        void
        _start() override
        {
          boost::beast::http::async_read_header(
            this->socket(), this->buffer(), this->serializer(),
            [this] (boost::system::error_code e, std::size_t)
            {
              this->_wakeup(e);
            });
        }
      };

      void
      Server::run()
      {
        elle::With<Scope>() << [this] (Scope& s)
        {
          while (true)
          {
            auto socket = make_shared(this->_server.accept());
            s.run_background(
              elle::print("{}: {}", this, socket),
              [this, socket]
              {
                try
                {
                  boost::beast::flat_buffer buffer;
                  ELLE_TRACE_SCOPE("{}: handle connection {}", this, socket);
                  while (true)
                  {
                    Reader parser;
                    HTTPReadHeaders read(*socket->socket(), parser, buffer);
                    read.run();
                    ELLE_TRACE_SCOPE("{}: read {}",
                                     this, parser.get());
                    this->_handle(
                      Request(std::make_unique<Request::Impl>(
                                parser, *socket->socket(), buffer)));
                  }
                }
                catch (network::ConnectionClosed const&)
                {
                  ELLE_TRACE("{}: end of connection {}", this, socket);
                }
              });
          }
        };
      }

      class BodyRead
        : public HTTPReadOperation
      {
      public:
        using Super = HTTPReadOperation;
        using Super::Super;

        virtual
        void
        _start() override
        {
          boost::beast::http::async_read_some(
            this->socket(), this->buffer(), this->serializer(),
            [this] (boost::system::error_code e, std::size_t read)
            {
              this->_read = read;
              this->_wakeup(e);
            });
        }

        ELLE_ATTRIBUTE_R(std::size_t, read);
      };

      using Writer =
        boost::beast::http::response_serializer<boost::beast::http::empty_body>;
      using HTTPWriteOperation = HTTPOperation<Writer>;

      class HTTPWriteHeaders
        : public HTTPWriteOperation
      {
      public:
        using Super = HTTPWriteOperation;
        using Super::Super;

        void
        _start() override
        {
          boost::beast::http::async_write_header(
            this->socket(), this->serializer(),
            [this] (boost::system::error_code e, std::size_t)
            {
              this->_wakeup(e);
            });
        }
      };

      class HTTPWrite
        : public HTTPWriteOperation
      {
      public:
        using Super = HTTPWriteOperation;
        using Super::Super;

        void
        _start() override
        {
          boost::beast::http::async_write(
            this->socket(), this->serializer(),
            [this] (boost::system::error_code e, std::size_t written)
            {
              ELLE_LOG("written {}", written);
              this->_wakeup(e);
            });
        }
      };

      class Server::Response::Impl
        : public elle::StreamBuffer
      {
      public:
        Impl(boost::asio::ip::tcp::socket& socket,
             boost::beast::http::response<Body> r)
          : _response(std::move(r))
          , _socket(socket)
          , _serializer(this->_response)
          , _headers_sent(false)
          , _complete(false)
        {}

        Impl(Impl&&) = default;

        ConstWeakBuffer
        read_buffer() override
        {
          ELLE_ABORT("trying to read from {}", this);
        }

        WeakBuffer
        write_buffer() override
        {
          return {
            this->_body,
            sizeof(this->_body),
          };
        }
        elle::Buffer::Byte _body[4096];

        void
        flush(Size size) override
        {
          // If the body fits in a single buffer, send in one round with
          // content-length.
          if (!this->_headers_sent && this->_complete)
          {
            this->_response.content_length(size);
            this->_send_headers();
            HTTPWrite write(this->_socket, this->_serializer);
            write.run();
            ELLE_ERR("goby");
          }
          else
          {
            // Send the headers before any data.
            if (!this->_headers_sent)
            {
              this->_send_headers();
              this->_headers_sent = true;
            }
            // boost::beast::http::make_chunk(this->_body);
          }
        }

      private:
        void
        _send_headers()
        {
          HTTPWriteHeaders write(this->_socket, this->_serializer);
          write.run();
        }

        ELLE_ATTRIBUTE(
          boost::beast::http::response<Body>,
          response);
        ELLE_ATTRIBUTE(boost::asio::ip::tcp::socket&, socket);
        ELLE_ATTRIBUTE(Writer, serializer);
        ELLE_ATTRIBUTE_R(bool, headers_sent);
        ELLE_ATTRIBUTE_RW(bool, complete);
      };

      Server::Response::Response(StatusCode status, std::unique_ptr<Impl> impl)
        : Pimpl<Response>(std::move(impl))
        , IOStream(this->_impl.get())
        , _status(status)
      {}

      Server::Response::~Response()
      {
        this->_impl->complete(true);
        this->flush();
        // Prevent deletion by IOStream.
        this->_buffer = nullptr;
      }

      template <>
      class Pimpl<Server::Request>::Impl
        : public elle::StreamBuffer
      {
      public:
        Impl(Reader& p,
             boost::asio::ip::tcp::socket& socket,
             boost::beast::flat_buffer& buffer)
          : _parser(p)
          , _socket(socket)
          , _buffer(buffer)
        {}

        Impl(Impl&&) = default;

        ConstWeakBuffer
        read_buffer() override
        {
          if (this->_parser.is_done())
            return {};
          this->_parser.get().body().data = this->_body;
          this->_parser.get().body().size = sizeof(this->_body);
          BodyRead r(this->_socket, this->_parser, this->_buffer);
          r.run();
          return {this->_body, r.read()};
        }

        WeakBuffer
        write_buffer() override
        {
          ELLE_ABORT("trying to write to {}", this);
        }

        ELLE_ATTRIBUTE_RX(Reader&, parser);
        ELLE_ATTRIBUTE_RX(boost::asio::ip::tcp::socket&, socket);
        ELLE_ATTRIBUTE_RX(boost::beast::flat_buffer&, buffer);
      private:
        char _body[4096];
      };

      static
      Method
      convert_method(boost::beast::http::verb v)
      {
        switch (v)
        {
          case boost::beast::http::verb::delete_:
            return Method::DELETE;
          case boost::beast::http::verb::get:
            return Method::GET;
          case boost::beast::http::verb::head:
            return Method::HEAD;
          case boost::beast::http::verb::options:
            return Method::OPTIONS;
          case boost::beast::http::verb::post:
            return Method::POST;
          case boost::beast::http::verb::put:
            return Method::PUT;
          default:
            ELLE_ERR("unhandled HTTP method: {}", v);
            // FIXME: Better than an abort ?
            return Method::GET;
        }
      }

      Server::Request::Request(std::unique_ptr<Impl> impl)
        : Pimpl<Request>(std::move(impl))
        , IOStream(this->_impl.get())
        , Headers()
        , _method(convert_method(this->_impl->parser().get().method()))
        , _uri(this->_impl->parser().get().target().data())
      {}

      Server::Response
      Server::Request::respond(StatusCode status)
      {
        boost::beast::http::response<Body> r;
        r.result(boost::beast::http::int_to_status(unsigned(status)));
        return Response(
          status,
          std::make_unique<Response::Impl>(
            this->_impl->socket(), std::move(r)));
      }

      Server::Request::~Request()
      {
        // Prevent deletion by IOStream.
        this->_buffer = nullptr;
      }
    }
  }
}
