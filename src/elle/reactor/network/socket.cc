#include <boost/asio/ssl.hpp>
#include <boost/lexical_cast.hpp>

#include <elle/Lazy.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/log.hh>
#include <elle/reactor/lockable.hh>
#include <elle/reactor/network/SocketOperation.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/socket.hh>
#include <elle/reactor/network/tcp-socket.hh>
#include <elle/reactor/scheduler.hh>
#include <utility>

#include <elle/reactor/network/socket.hxx>

ELLE_LOG_COMPONENT("elle.reactor.network.Socket");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      template <>
      struct SocketSpecialization<
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
      {
        using Socket = boost::asio::ip::tcp::socket;
        using Stream = boost::asio::ssl::stream<Socket>;

        static
        Socket&
        socket(Stream& s)
        {
          return s.next_layer();
        }
      };

      constexpr size_t const Socket::buffer_size = 1 << 16;

      namespace
      {
        size_t constexpr buffer_size = 1 << 16;

        class StreamBuffer
          : public std::streambuf
        {
        public:
          StreamBuffer(Socket* socket)
            : _socket(socket)
            , _pacified(false)
          {
            setg(nullptr, nullptr, nullptr);
            setp(nullptr, nullptr);
          }

          char read_buffer[buffer_size];
          int
          underflow() override
          {
            ELLE_TRACE_SCOPE("%s: underflow", *this);
            if (this->_pacified)
            {
              setg(nullptr, nullptr, nullptr);
              return EOF;
            }
            int size = 0;
            try
            {
              this->_socket->read_some(
                elle::WeakBuffer(this->read_buffer),
                {},
                &size);
            }
            catch (...)
            {
              if (size != 0)
              {
                ELLE_TRACE("exception after %s bytes: %s",
                           size, elle::exception_string());
                setg(this->read_buffer, this->read_buffer,
                     this->read_buffer + size);
              }
              throw;
            }
            setg(this->read_buffer, this->read_buffer,
                 this->read_buffer + size);
            return static_cast<unsigned char>(this->read_buffer[0]);
          }

          char write_buffer[buffer_size];
          int
          overflow(int c) override
          {
            ELLE_TRACE_SCOPE("%s: overflow", *this);
            this->sync();
            setp(this->write_buffer,
                 this->write_buffer + sizeof(this->write_buffer));
            this->write_buffer[0] = c;
            pbump(1);
            // Success is indicated by "A value different from EOF".
            return EOF + 1;
          }

          int
          sync() override
          {
            Size size = pptr() - pbase();
            ELLE_TRACE_SCOPE("%s: sync %s bytes", *this, size);
            setp(nullptr, nullptr);
            if (size > 0 && !this->_pacified)
              this->_socket->write(
                elle::ConstWeakBuffer(this->write_buffer, size));
            return 0;
          }

          ELLE_ATTRIBUTE(Socket*, socket);
          ELLE_ATTRIBUTE_RW(bool, pacified);
        };
      }


      /*-------------.
      | Construction |
      `-------------*/

      Socket::Socket():
        elle::IOStream(new StreamBuffer(this))
      {}

      Socket::~Socket()
      = default;

      std::unique_ptr<Socket>
      Socket::create(Protocol protocol,
                     const std::string& hostname,
                     int port,
                     DurationOpt timeout)
      {
        switch (protocol)
        {
          case Protocol::tcp:
            return std::unique_ptr<Socket>(
              new TCPSocket(hostname, port, timeout));
          default:
            elle::unreachable();
        }
      }

      void
      Socket::_pacify_streambuffer()
      {
        static_cast<StreamBuffer*>(this->rdbuf())->pacified(true);
      }

      /*-----.
      | Read |
      `-----*/

      void
      Socket::read(elle::WeakBuffer, DurationOpt, int*)
      {
        std::abort();
        // XXX[unused arguments for now, do something with it]
        // Size s = buffer.size();
        // read_some(buffer);
      }

      elle::Buffer
      Socket::read(Size size,
                   DurationOpt timeout)
      {
        auto res = elle::Buffer(size);
        this->read(elle::WeakBuffer(res), timeout);
        return res;
      }

      elle::Buffer
      Socket::read_some(Size size,
                        DurationOpt timeout)
      {
        auto res = elle::Buffer(size);
        auto r = this->read_some(elle::WeakBuffer(res), timeout);
        res.size(r);
        return res;
      }

      /*------------------------.
      | Explicit instantiations |
      `------------------------*/

      // TCP
      template
      class PlainSocket<boost::asio::ip::tcp::socket>;
      template
      class StreamSocket<boost::asio::ip::tcp::socket>;
      // SSL
      template
      class PlainSocket<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>,
                        boost::asio::ip::tcp::socket::endpoint_type>;
      template
      class StreamSocket<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>,
                         boost::asio::ip::tcp::socket::endpoint_type>;
      // UDP
      template
      class PlainSocket<boost::asio::ip::udp::socket>;

#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
      // Unix Domain
      template
      class PlainSocket<boost::asio::local::stream_protocol::socket>;
      template
      class StreamSocket<boost::asio::local::stream_protocol::socket>;
#endif
    }
  }
}
