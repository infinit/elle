#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <elle/Lazy.hh>
#include <elle/format/hexadecimal.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/socket.hh>
#include <reactor/network/tcp-socket.hh>
// #include <reactor/network/udt-socket.hh>
#include <reactor/network/SocketOperation.hh>
#include <reactor/scheduler.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("reactor.network.Socket");

namespace reactor
{
  namespace network
  {
    template <typename Socket_>
    struct SocketSpecialization
    {
      typedef Socket_ Socket;
      typedef Socket_ Stream;

      static
      Socket&
      socket(Stream& s)
      {
        return s;
      }
    };

    template <>
    struct SocketSpecialization<
      boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
    {
      typedef boost::asio::ip::tcp::socket Socket;
      typedef boost::asio::ssl::stream<Socket> Stream;

      static
      Socket&
      socket(Stream& s)
      {
        return s.next_layer();
      }
    };

    size_t const Socket::buffer_size = 1 << 16;

    namespace
    {
      class StreamBuffer:
        public elle::DynamicStreamBuffer
      {
      public:
        typedef elle::DynamicStreamBuffer Super;
        typedef Super::Size Size;
        StreamBuffer(Socket* socket):
          Super(Socket::buffer_size),
          _socket(socket),
          _pacified(false)
        {}

        virtual
        Size
        read(char* buffer, Size size)
        {
          if (!this->_pacified)
            return this->_socket->read_some(network::Buffer(buffer, size));
          else
            return 0;
        }

        virtual
        void
        write(char* buffer, Size size)
        {
          if (!this->_pacified)
            this->_socket->write(elle::ConstWeakBuffer(buffer, size));
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
    {}

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
        // case Protocol::udt:
        //   return std::unique_ptr<Socket>
        //     (new UDTSocket(sched, hostname, port, timeout));
        default:
          elle::unreachable();
      }
    }

    void
    Socket::_pacify_streambuffer()
    {
      static_cast<StreamBuffer*>(this->rdbuf())->pacified(true);
    }

    /*----------------.
    | Pretty printing |
    `----------------*/

    template <typename AsioSocket, typename EndPoint>
    void
    PlainSocket<AsioSocket, EndPoint>::print(std::ostream& s) const
    {
      s << "reactor::network::Socket(" << this->peer() << ")";
    }

    std::ostream& operator << (std::ostream& s, const Socket& socket)
    {
      socket.print(s);
      return s;
    }

    /*-------------.
    | Construction |
    `-------------*/

    template <typename AsioSocket, typename EndPoint>
    PlainSocket<AsioSocket, EndPoint>::PlainSocket(
      std::unique_ptr<AsioSocket> socket,
      const EndPoint& peer,
      DurationOpt timeout):
        Super(),
        _socket(std::move(socket)),
        _peer(peer)
    {
      this->_connect(peer, timeout);
    }

    template <typename AsioSocket, typename EndPoint>
    PlainSocket<AsioSocket, EndPoint>::PlainSocket(
      std::unique_ptr<AsioSocket> socket,
      EndPoint const& peer):
        Super(),
        _socket(std::move(socket)),
        _peer(peer)
    {}

    template <typename AsioSocket, typename EndPoint>
    PlainSocket<AsioSocket, EndPoint>::~PlainSocket()
    {
      try
      {
        this->_disconnect();
      }
      catch (elle::Exception const&)
      {
        // Nothing. If the socket was invalid, no need to disconnect it. We
        // might be able to catch a more precise exception if _disconnect throws
        // differently.
      }
    }

    /*-----------.
    | Connection |
    `-----------*/

    template <typename AsioSocket>
    class Connection:
      public SocketOperation<AsioSocket>
    {
    public:
      typedef typename AsioSocket::endpoint_type EndPoint;
      typedef SocketOperation<AsioSocket> Super;
      Connection(AsioSocket& socket,
                 const EndPoint& endpoint):
        Super(socket),
        _endpoint(endpoint)
      {}


      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "connection to " << this->_endpoint;
      }

    protected:
      virtual
      void
      _start()
      {
        this->socket().async_connect(
          this->_endpoint, boost::bind(&Connection::_wakeup, this, _1));
      }

    private:
      ELLE_ATTRIBUTE(EndPoint, endpoint);
    };

    template <typename AsioSocket, typename EndPoint>
    void
    PlainSocket<AsioSocket, EndPoint>::_connect(const EndPoint& peer,
                                                DurationOpt timeout)
    {
      ELLE_TRACE_SCOPE("%s: connect to %s", *this, peer);
      typedef SocketSpecialization<AsioSocket> Spe;
      try
      {
        Connection<typename Spe::Socket> connection(
          Spe::socket(*this->_socket), peer);
        if (!connection.run(timeout))
          throw TimeOut();
      }
      catch (...)
      {
        ELLE_TRACE("%s: connection failed: %s",
                   *this, elle::exception_string());
        throw;
      }
    }

    template <typename AsioSocket, typename EndPoint>
    void
    PlainSocket<AsioSocket, EndPoint>::close()
    {
      ELLE_TRACE_SCOPE("%s: close", *this);
      typedef SocketSpecialization<AsioSocket> Spe;
      auto& socket = Spe::socket(*this->socket());
      try
      {
        socket.cancel();
      }
      catch (boost::system::system_error const& e)
      {
        // If the socket was already closed, ignore.
        if (e.code() != boost::asio::error::bad_descriptor)
          throw;
      }
      socket.close();
    }

    template <typename AsioSocket, typename EndPoint>
    void
    PlainSocket<AsioSocket, EndPoint>::_disconnect()
    {
      ELLE_TRACE_SCOPE("%s: disconnect", *this);
      typedef SocketSpecialization<AsioSocket> Spe;
      if (this->_socket)
      {
        boost::system::error_code error;
        Spe::socket(*this->_socket).shutdown(Spe::Socket::shutdown_both, error);
        if (error)
        {
          if (error == boost::asio::error::not_connected
#ifdef INFINIT_WINDOWS
              || error == boost::asio::error::bad_descriptor
              // A request to send or receive data was disallowed because the
              // socket had already been shut down in that direction with a
              // previous shutdown call.
              || error.value() == WSAESHUTDOWN
#endif
            )
            ; // It's ok to try to disconnect a non-connected socket.
          else
          {
            ELLE_TRACE("%s: disconnection error: %s (%s)",
                       *this, error.message(), error);
            throw Exception(error.message());
          }
        }
        Spe::socket(*this->_socket).close();
      }
    }

    /*-----.
    | Read |
    `-----*/

    void
    Socket::read(network::Buffer, DurationOpt)
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
      elle::Buffer res(size);
      this->read(network::Buffer(res.mutable_contents(), size), timeout);
      return res;
    }

    elle::Buffer
    Socket::read_some(Size size,
                      DurationOpt timeout)
    {
      elle::Buffer res(size);
      auto r = this->read_some(network::Buffer(res.mutable_contents(), size));
      res.size(r);
      return res;
    }

    /*-----------.
    | Properties |
    `-----------*/


    template <typename AsioSocket>
    static elle::network::Locus
    locus_from_endpoint(typename AsioSocket::endpoint_type const& endpoint)
    {
      auto host = boost::lexical_cast<std::string>(endpoint.address());
      auto port = endpoint.port();
      return elle::network::Locus(host, port);
    }

    template <typename AsioSocket, typename EndPoint>
    EndPoint
    PlainSocket<AsioSocket, EndPoint>::peer() const
    {
      return this->_peer;
    }

    template <typename AsioSocket, typename EndPoint>
    EndPoint
    PlainSocket<AsioSocket, EndPoint>::local_endpoint() const
    {
      typedef SocketSpecialization<AsioSocket> Spe;
      return Spe::socket(*this->_socket).local_endpoint();
    }

    /*-------------.
    | StreamSocket |
    `-------------*/

    template <typename AsioSocket, typename EndPoint>
    StreamSocket<AsioSocket, EndPoint>::~StreamSocket()
    {
      ELLE_TRACE_SCOPE("%s: finalize", *this);
      // XXX: Flush the socket, otherwise the parent ~IOStream will flush the
      // buffer which will in turn write to the (deleted) socket.
      if (!this->fail() && !this->bad())
        try
        {
          this->flush();
        }
        catch (...)
        {
          this->_pacify_streambuffer();
          // XXX: std::iostream technically can't throw due to virtual noexcept
          // destructor. Find something better.
          // throw;
          ELLE_ERR("error while flushing socket: %s", elle::exception_string());
        }
    }

    /*-----.
    | Read |
    `-----*/

    template <typename PlainSocket, typename AsioSocket>
    class Read:
      public SocketOperation<typename SocketSpecialization<AsioSocket>::Socket>
    {
    public:
      typedef typename SocketSpecialization<AsioSocket>::Socket Socket;
      typedef SocketOperation<Socket> Super;
      Read(PlainSocket& plain,
           AsioSocket& socket,
           Buffer& buffer,
           bool some):
        SocketOperation<AsioSocket>(socket),
        _buffer(buffer),
        _read(0),
        _some(some),
        _socket(plain)
      {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "read on " << this->_socket;
      }

    protected:
      virtual
      void
      _start()
      {
        // FIXME: be synchronous if enough bytes are available
        if (this->_some)
          this->_socket.socket()->async_read_some(
            boost::asio::buffer(this->_buffer.data(), this->_buffer.size()),
            boost::bind(&Read::_wakeup, this, _1, _2));
        else
          boost::asio::async_read(
            *this->_socket.socket(),
            boost::asio::buffer(this->_buffer.data(), this->_buffer.size()),
            boost::bind(&Read::_wakeup, this, _1, _2));
      }

    private:
      void
      _wakeup(const boost::system::error_code& error,
              std::size_t read)
      {
        this->_read = read;
        Super::_wakeup(error);
      }

      void
      _handle_error(boost::system::error_code const& error) override
      {
        if (error == boost::asio::error::eof
            || error == boost::asio::error::operation_aborted
            || error == boost::asio::error::connection_aborted
#ifdef INFINIT_WINDOWS
            || error == boost::asio::error::bad_descriptor
            || error == boost::asio::error::connection_reset
            || error.value() == ERROR_CONNECTION_ABORTED
#endif
          )
          this->template _raise<reactor::network::ConnectionClosed>();
        // SSL yields short read if the underlying TCP connection is closed.
        else if (error.category() == boost::asio::error::get_ssl_category() &&
                 error.value() == ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHORT_READ))
        {
          this->template _raise<reactor::network::ConnectionClosed>();
        }
        else
          Super::_handle_error(error);
      }

      ELLE_ATTRIBUTE(Buffer&, buffer);
      ELLE_ATTRIBUTE_R(Size, read);
      ELLE_ATTRIBUTE(bool, some);
      ELLE_ATTRIBUTE(PlainSocket const&, socket);
    };

    template <typename AsioSocket, typename EndPoint>
    void
    StreamSocket<AsioSocket, EndPoint>::read(Buffer buf,
                                             DurationOpt timeout)
    {
      this->_read(buf, timeout, false);
    }

    template <typename AsioSocket, typename EndPoint>
    Size
    StreamSocket<AsioSocket, EndPoint>::read_some(Buffer buf,
                                                  DurationOpt timeout)
    {
      return this->_read(buf, timeout, true);
    }

    template <typename AsioSocket, typename EndPoint>
    Size
    StreamSocket<AsioSocket, EndPoint>::_read(Buffer buf,
                                              DurationOpt timeout,
                                              bool some)
    {
      ELLE_TRACE_SCOPE("%s: read %s%s bytes (%s)",
                       *this, some ? "up to " : "", buf.size(), timeout);
      if (this->_streambuffer.size())
      {
        std::istream s(&this->_streambuffer);
        s.readsome(reinterpret_cast<char*>(buf.data()), buf.size());
        unsigned size = s.gcount();
        ELLE_ASSERT_GT(size, 0u);
        if (size == buf.size() || some)
        {
          ELLE_DEBUG("%s: completed read of %s (cached) bytes: %s",
                     *this, size, elle::ConstWeakBuffer(buf.data(),
                                                        buf.size()).string());
          return size;
        }
        else if (size)
          ELLE_TRACE("%s: read %s cached bytes, carrying on", *this, size);
        buf = Buffer(buf.data() + size, buf.size() - size);
      }
      typedef SocketSpecialization<AsioSocket> Spe;
      Read<Self, typename Spe::Socket> read(*this,
                                            Spe::socket(*this->socket()),
                                            buf, some);
      bool finished;
      try
      {
        finished  = read.run(timeout);
      }
      catch (...)
      {
        ELLE_TRACE("%s: read threw: %s", *this, elle::exception_string());
        throw;
      }
      if (!finished)
      {
        ELLE_TRACE("%s: read timed out", *this);
        throw TimeOut();
      }
      ELLE_TRACE("%s: completed read of %s bytes: %s",
                 *this, read.read(), buf);

      elle::ConstWeakBuffer data(buf.data(), read.read());
      elle::Lazy<std::string> hex(
        [&data]
        {
          return elle::format::hexadecimal::encode(data);
        });
      ELLE_DUMP("%s: data: 0x%s", *this, hex);

      return read.read();
    }

    template <typename PlainSocket, typename AsioSocket>
    class ReadUntil:
      public SocketOperation<boost::asio::ip::tcp::socket>
    {
    public:
      typedef SocketOperation<boost::asio::ip::tcp::socket> Super;
      ReadUntil(PlainSocket& plain,
                AsioSocket& socket,
                boost::asio::streambuf& buffer,
                std::string const& delimiter):
        SocketOperation<AsioSocket>(socket),
        _socket(plain),
        _streambuffer(buffer),
        _delimiter(delimiter),
        _buffer()
      {}

    protected:
      virtual
      void
      _start()
      {
        boost::asio::async_read_until(
          this->socket(),
          this->_streambuffer,
          this->_delimiter,
          std::bind(&ReadUntil::_wakeup,
                    std::ref(*this),
                    std::placeholders::_1,
                    std::placeholders::_2));
      }

      void
      _wakeup(const boost::system::error_code& error, std::size_t read)
      {
        if (!this->canceled())
        {
          std::istream s(&this->_streambuffer);
          this->_buffer.size(read);
          s.read((char*)this->_buffer.mutable_contents(), read);
          ELLE_ASSERT(!s.fail());
          ELLE_ASSERT_EQ(s.gcount(), static_cast<signed>(read));
        }
        Super::_wakeup(error);
      }

    private:
      ELLE_ATTRIBUTE(PlainSocket&, socket);
      ELLE_ATTRIBUTE(boost::asio::streambuf&, streambuffer);
      ELLE_ATTRIBUTE(std::string, delimiter);
      ELLE_ATTRIBUTE_RX(elle::Buffer, buffer);
    };

    template <typename AsioSocket, typename EndPoint>
    elle::Buffer
    StreamSocket<AsioSocket, EndPoint>::read_until(std::string const& delimiter,
                                                   DurationOpt timeout)
    {
      ELLE_TRACE_SCOPE("%s: read until %s", *this, delimiter);
      typedef SocketSpecialization<AsioSocket> Spe;
      ReadUntil<Self, typename Spe::Socket> read(
        *this, Spe::socket(*this->socket()),
        this->_streambuffer, delimiter);
      bool finished;
      try
      {
        finished  = read.run(timeout);
      }
      catch (...)
      {
        ELLE_TRACE("%s: read until threw: %s", *this, elle::exception_string());
        throw;
      }
      if (!finished)
      {
        ELLE_TRACE("%s: read until timed out", *this);
        throw TimeOut();
      }
      return std::move(read.buffer());
    }

    /*------.
    | Write |
    `------*/

    template <typename PlainSocket, typename AsioSocket>
    class Write:
      public SocketOperation<typename SocketSpecialization<AsioSocket>::Socket>
    {
    public:
      typedef typename SocketSpecialization<AsioSocket>::Socket Socket;
      typedef SocketOperation<Socket> Super;
      typedef SocketSpecialization<AsioSocket> Spe;
      Write(PlainSocket& plain,
            AsioSocket& socket,
            elle::ConstWeakBuffer buffer):
        Super(Spe::socket(socket)),
        _socket(plain),
        _buffer(buffer),
        _written(0)
      {}

    protected:
      virtual
      void
      _start()
      {
        boost::asio::async_write(
          *this->_socket.socket(),
          boost::asio::buffer(this->_buffer.contents(), this->_buffer.size()),
          boost::bind(&Write::_wakeup, this, _1, _2));
      }

    private:
      void
      _wakeup(const boost::system::error_code& error, std::size_t written)
      {
        this->_written = written;
        Super::_wakeup(error);
      }

      void
      _handle_error(boost::system::error_code const& error) override
      {
        if (error == boost::asio::error::eof ||
            error == boost::asio::error::operation_aborted ||
            error == boost::asio::error::broken_pipe ||
            error == boost::asio::error::connection_aborted ||
            error == boost::asio::error::connection_reset)
          this->template _raise<ConnectionClosed>();
        else
          Super::_handle_error(error);
      }

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "write on " << this->_socket;
      }

      ELLE_ATTRIBUTE(PlainSocket const&, socket);
      ELLE_ATTRIBUTE(elle::ConstWeakBuffer, buffer);
      ELLE_ATTRIBUTE_R(Size, written);
    };

    template <typename AsioSocket, typename EndPoint>
    void
    StreamSocket<AsioSocket, EndPoint>::write(elle::ConstWeakBuffer buffer)
    {
      reactor::wait(this->_write_mutex);
      ELLE_TRACE_SCOPE("%s: write %s bytes", *this, buffer.size());
      try
      {
        Write<Self, AsioSocket> write(*this, *this->socket(), buffer);
        write.run();
      }
      catch (...)
      {
        this->_write_mutex.release();
        throw;
      }
      this->_write_mutex.release();
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
    // template
    // class PlainSocket<boost::asio::ip::udp::socket>;
    // UDT
    // template
    // class PlainSocket<boost::asio::ip::udt::socket>;
  }
}
