#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
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

ELLE_LOG_COMPONENT("elle.reactor.network.Socket");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      template <typename Socket_>
      struct SocketSpecialization
      {
        using Socket = Socket_;
        using Stream = Socket_;

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

      /*----------------.
      | Pretty printing |
      `----------------*/

      template <typename AsioSocket, typename EndPoint>
      void
      PlainSocket<AsioSocket, EndPoint>::print(std::ostream& s) const
      {
        elle::fprintf(s, "%s(%s)", elle::type_info(*this), this->peer());
      }

      /*-------------.
      | Construction |
      `-------------*/

      template <typename AsioSocket, typename EndPoint>
      PlainSocket<AsioSocket, EndPoint>::PlainSocket(
        std::unique_ptr<AsioSocket> socket)
        : Super()
        , _socket(std::move(socket))
        , _peer()
      {}

      template <typename AsioSocket, typename EndPoint>
      PlainSocket<AsioSocket, EndPoint>::PlainSocket(
        std::unique_ptr<AsioSocket> socket,
        const EndPoint& peer,
        DurationOpt timeout)
        : Super()
        , _socket(std::move(socket))
        , _peer(peer)
      {
        this->_connect(peer, timeout);
      }

      template <typename AsioSocket, typename EndPoint>
      PlainSocket<AsioSocket, EndPoint>::PlainSocket(
        std::unique_ptr<AsioSocket> socket,
        EndPoint const& peer)
        : Super()
        , _socket(std::move(socket))
        , _peer(peer)
      {}

      template <typename AsioSocket, typename EndPoint>
      PlainSocket<AsioSocket, EndPoint>::PlainSocket(Self&& src)
        : Super() // FIXME: this drops the IOStream buffers !
        , _socket(std::move(src._socket))
        , _peer(std::move(src._peer))
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
      class Connection
        : public SocketOperation<AsioSocket>
      {
      public:
        using EndPoint = typename AsioSocket::endpoint_type;
        using Super = SocketOperation<AsioSocket>;
        Connection(AsioSocket& socket,
                   EndPoint  endpoint)
          : Super(socket)
          , _endpoint(std::move(endpoint))
        {}


        void
        print(std::ostream& stream) const override
        {
          stream << "connection to " << this->_endpoint;
        }

      protected:
        void
        _start() override
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
        using Spe = SocketSpecialization<AsioSocket>;
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
        using Spe = SocketSpecialization<AsioSocket>;
        auto& socket = Spe::socket(*this->socket());
        boost::system::error_code e;
        socket.cancel(e);
        if (e && e != boost::asio::error::bad_descriptor)
          throw Error(e.message());
        socket.close();
      }

      template <typename AsioSocket, typename EndPoint>
      void
      PlainSocket<AsioSocket, EndPoint>::_disconnect()
      {
        ELLE_TRACE_SCOPE("%s: disconnect", *this);
        using Spe = SocketSpecialization<AsioSocket>;
        if (this->_socket)
        {
          boost::system::error_code error;
          Spe::socket(*this->_socket).shutdown(Spe::Socket::shutdown_both, error);
          if (error)
          {
            if (error == boost::asio::error::not_connected
                || error == boost::asio::error::bad_descriptor
#ifdef INFINIT_WINDOWS
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
              throw Error(error.message());
            }
          }
          Spe::socket(*this->_socket).close();
        }
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

      /*-----------.
      | Properties |
      `-----------*/

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
        using Spe = SocketSpecialization<AsioSocket>;
        boost::system::error_code erc;
        auto res = Spe::socket(*this->_socket).local_endpoint(erc);
        if (erc)
          throw Error(erc.message());
        return res;
      }

      /*-------------.
      | StreamSocket |
      `-------------*/

      template <typename AsioSocket, typename EndPoint>
      StreamSocket<AsioSocket, EndPoint>::~StreamSocket()
      {
        this->_final_flush();
      }

      template <typename AsioSocket, typename EndPoint>
      void
      StreamSocket<AsioSocket, EndPoint>::_final_flush()
      {
        ELLE_TRACE_SCOPE("%s: final flush", *this);
        // Flush the socket, otherwise the parent ~IOStream will flush the buffer
        // which will in turn write to the (deleted) socket.
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
      class Read
        : public DataOperation<typename SocketSpecialization<AsioSocket>::Socket>
      {
      public:
        using Socket = typename SocketSpecialization<AsioSocket>::Socket;
        using Super = DataOperation<Socket>;
        Read(PlainSocket& plain,
             AsioSocket& socket,
             elle::WeakBuffer& buffer,
             bool some)
          : DataOperation<AsioSocket>(socket)
          , _buffer(buffer)
          , _read(0)
          , _some(some)
          , _socket(plain)
        {}

        void
        print(std::ostream& stream) const override
        {
          stream << "read on " << this->_socket;
        }

      protected:
        void
        _start() override
        {
          // FIXME: be synchronous if enough bytes are available
          if (this->_some)
            this->_socket.socket()->async_read_some(
              boost::asio::buffer(this->_buffer.mutable_contents(),
                                  this->_buffer.size()),
              boost::bind(&Read::_wakeup, this, _1, _2));
          else
            boost::asio::async_read(
              *this->_socket.socket(),
              boost::asio::buffer(this->_buffer.mutable_contents(),
                                  this->_buffer.size()),
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

        ELLE_ATTRIBUTE(elle::WeakBuffer&, buffer);
        ELLE_ATTRIBUTE_R(Size, read);
        ELLE_ATTRIBUTE(bool, some);
        ELLE_ATTRIBUTE(PlainSocket const&, socket);
      };

      template <typename AsioSocket, typename EndPoint>
      void
      StreamSocket<AsioSocket, EndPoint>::read(elle::WeakBuffer buf,
                                               DurationOpt timeout,
                                               int* bytes_read)
      {
        this->_read(buf, timeout, false, bytes_read);
      }

      template <typename AsioSocket, typename EndPoint>
      Size
      StreamSocket<AsioSocket, EndPoint>::read_some(elle::WeakBuffer buf,
                                                    DurationOpt timeout,
                                                    int* bytes_read)
      {
        return this->_read(buf, timeout, true, bytes_read);
      }

      template <typename AsioSocket, typename EndPoint>
      Size
      StreamSocket<AsioSocket, EndPoint>::_read(elle::WeakBuffer buf,
                                                DurationOpt timeout,
                                                bool some,
                                                int* bytes_read)
      {
        ELLE_TRACE_SCOPE("%s: read %s%s bytes%s",
                         *this,
                         some ? "up to " : "",
                         buf.size(),
                         timeout ? elle::sprintf(" in %s", timeout.get()): "");
        if (this->_streambuffer.size())
        {
          std::istream s(&this->_streambuffer);
          s.readsome(reinterpret_cast<char*>(buf.mutable_contents()), buf.size());
          unsigned size = s.gcount();
          ELLE_ASSERT_GT(size, 0u);
          if (size == buf.size() || some)
          {
            ELLE_DEBUG("%s: completed read of %s (cached) bytes: %s",
                       *this, size, buf);
            if (bytes_read)
              *bytes_read = size;
            return size;
          }
          else if (size)
            ELLE_TRACE("%s: read %s cached bytes, carrying on", *this, size);
          buf = buf.range(size);
        }
        using Spe = SocketSpecialization<AsioSocket>;
        auto read = Read<Self, typename Spe::Socket> (*this,
                                                      Spe::socket(*this->socket()),
                                                      buf, some);
        bool finished;
        try
        {
          finished = read.run(timeout);
        }
        catch (...)
        {
          ELLE_TRACE("%s: read threw: %s", *this, elle::exception_string());
          if (bytes_read)
            *bytes_read = read.read();
          throw;
        }
        if (!finished)
        {
          ELLE_TRACE("%s: read timed out", *this);
          if (bytes_read)
            *bytes_read = read.read();
          throw TimeOut();
        }
        ELLE_TRACE("%s: completed read of %s bytes", *this, read.read());
        ELLE_DUMP(": %s", buf);

        auto data = elle::ConstWeakBuffer(buf.contents(), read.read());
        elle::Lazy<std::string> hex(
          [&data]
          {
            return elle::format::hexadecimal::encode(data);
          });
        ELLE_DUMP("%s: data: 0x%s", *this, hex);
        if (bytes_read)
          *bytes_read = read.read();
        return read.read();
      }

      template <typename PlainSocket, typename AsioSocket>
      class ReadUntil:
        public DataOperation<typename SocketSpecialization<AsioSocket>::Socket>
      {
      public:
        using Socket = typename SocketSpecialization<AsioSocket>::Socket;
        using Self = ReadUntil<PlainSocket, AsioSocket>;
        using Spe = SocketSpecialization<AsioSocket>;
        using Super = DataOperation<Socket>;
        ReadUntil(PlainSocket& plain,
                  AsioSocket& socket,
                  boost::asio::streambuf& buffer,
                  std::string  delimiter):
          Super(Spe::socket(socket)),
          _socket(plain),
          _streambuffer(buffer),
          _delimiter(std::move(delimiter)),
          _buffer()
        {}

      protected:
        void
        _start() override
        {
          boost::asio::async_read_until(
            *this->_socket.socket(),
            this->_streambuffer,
            this->_delimiter,
            std::bind(&Self::_wakeup,
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

        void
        print(std::ostream& stream) const override
        {
          stream << "read until on " << this->_socket;
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
        ReadUntil<Self, AsioSocket> read(*this, *this->socket(),
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
        public DataOperation<typename SocketSpecialization<AsioSocket>::Socket>
      {
      public:
        using Socket = typename SocketSpecialization<AsioSocket>::Socket;
        using Super = DataOperation<Socket>;
        using Spe = SocketSpecialization<AsioSocket>;
        Write(PlainSocket& plain,
              AsioSocket& socket,
              elle::ConstWeakBuffer buffer)
          : Super(Spe::socket(socket))
          , _socket(plain)
          , _buffer(std::move(buffer))
          , _written(0)
        {}

      protected:
        void
        _start() override
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
        if (reactor::scheduler().current())
        {
          {
            Lock lock(this->_write_mutex);
            ELLE_TRACE_SCOPE("%s: write %s bytes", this, buffer.size());
            Write<Self, AsioSocket> write(*this, *this->socket(), buffer);
            write.run();
          }
          this->_async_write();
        }
        else
        {
          this->_async_writes.emplace_back(buffer.contents(), buffer.size());
          this->_async_write();
        }
      }

      template <typename AsioSocket, typename EndPoint>
      void
      StreamSocket<AsioSocket, EndPoint>::_async_write()
      {
        if (!this->_async_writes.empty() && !this->_write_mutex.locked())
        {
          this->_write_mutex.acquire();
          auto& buffer = this->_async_writes.front();
          ELLE_TRACE_SCOPE(
            "%s: write %s bytes asynchronously", this, buffer.size());
          auto asio_buffer =
            boost::asio::buffer(buffer.contents(), buffer.size());
          boost::asio::async_write(
            *this->socket(),
            asio_buffer,
            [this]
            (const boost::system::error_code& error, std::size_t written)
            {
              this->_async_writes.pop_front();
              if (error == boost::system::errc::operation_canceled)
                return;
              // FIXME: what do with other errors ?
              ELLE_TRACE_SCOPE(
                "%s: %s bytes written asynchronously", this, written);
              this->_write_mutex.release();
              this->_async_write();
            });
        }
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
