#include <boost/optional.hpp>

#include <boost/lexical_cast.hpp>

#include <elle/Lazy.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/log.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.network.TCPSocket");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    TCPSocket::TCPSocket(const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout):
      TCPSocket(*reactor::Scheduler::scheduler(), hostname, port, timeout)
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout):
      TCPSocket(sched, resolve_tcp(sched, hostname, port), timeout)
    {}

    TCPSocket::TCPSocket(const std::string& hostname,
                         int port,
                         DurationOpt timeout):
      TCPSocket(*reactor::Scheduler::scheduler(), hostname, port, timeout)
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port,
                         DurationOpt timeout)
      : TCPSocket(sched, resolve_tcp(sched,
                                     hostname,
                                     boost::lexical_cast<std::string>(port)),
                  timeout)
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         boost::asio::ip::tcp::endpoint const& endpoint,
                         DurationOpt timeout)
      : Super(sched, endpoint, timeout)
      , _write_mutex()
    {}

    TCPSocket::~TCPSocket()
    {
      ELLE_TRACE_SCOPE("%s: finalize", *this);
      // XXX: Flush the socket, otherwise the parent ~IOStream will flush the
      // buffer which will in turn write to the (deleted) socket.
      try
      {
        ELLE_ASSERT(!this->bad() && !this->fail());
        this->flush();
      }
      catch (...)
      {
        _pacify_streambuffer();
        // XXX: std::iostream technically can't throw due to virtual noexcept
        // destructor. Find something better.
        // throw;
        ELLE_ERR("error while flushing socket: %s", elle::exception_string());
      }
    }

    TCPSocket::TCPSocket(Scheduler& sched, AsioSocket* socket)
      : Super(sched, socket)
    {}

    void
    TCPSocket::close()
    {
      ELLE_TRACE_SCOPE("%s: close", *this);
      this->socket()->close();
    }

    /*-----.
    | Read |
    `-----*/

    class Read: public SocketOperation<boost::asio::ip::tcp::socket>
    {
      public:
        typedef boost::asio::ip::tcp::socket AsioSocket;
        typedef boost::asio::ip::tcp::endpoint EndPoint;
        Read(Scheduler& scheduler,
             PlainSocket<AsioSocket>* socket,
             Buffer& buffer,
             bool some)
          : SocketOperation<AsioSocket>(scheduler, socket)
          , _buffer(buffer)
          , _read(0)
          , _some(some)
        {}

        virtual const char* type_name() const
        {
          static const char* name = "socket read";
          return name;
        }

        Size
        read()
        {
          return _read;
        }

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "read on ";
        try
        {
          stream << this->socket()->local_endpoint();
        }
        catch (std::exception const&)
        {
          stream << "an invalid socket (" << elle::exception_string() << ")";
        }
      }

      protected:

        virtual void _start()
        {
          // FIXME: be synchronous if enough bytes are available
          if (_some)
            this->socket()->async_read_some(
              boost::asio::buffer(_buffer.data(), _buffer.size()),
              boost::bind(&Read::_wakeup, this, this->_canceled, _1, _2));
          else
            boost::asio::async_read(
              *this->socket(),
              boost::asio::buffer(_buffer.data(), _buffer.size()),
              boost::bind(&Read::_wakeup, this, this->_canceled, _1, _2));
        }

      private:
        void _wakeup(std::shared_ptr<bool> canceled,
                     const boost::system::error_code& error,
                     std::size_t read)
        {
          if (*canceled)
            return;
          if (error)
            ELLE_TRACE("%s: read error: %s (%s)", *this, error.message(), error);
          else
            ELLE_TRACE("%s: read completed: %s bytes", *this, read);
          _read = read;
          if (error == boost::asio::error::eof
              || error == boost::asio::error::operation_aborted
              || error == boost::asio::error::connection_aborted
#ifdef INFINIT_WINDOWS
              || error == boost::asio::error::bad_descriptor
              || error == boost::asio::error::connection_reset
              || error.value() == ERROR_CONNECTION_ABORTED
#endif
             )
            this->_raise<ConnectionClosed>();
          else if (error)
            this->_raise<Exception>(error.message());
          this->_signal();
        }

        Buffer& _buffer;
        Size _read;
        bool _some;
    };

    void
    TCPSocket::read(Buffer buf, DurationOpt timeout)
    {
      _read(buf, timeout, false);
    }

    Size
    TCPSocket::read_some(Buffer buf, DurationOpt timeout)
    {
      return _read(buf, timeout, true);
    }

    Size
    TCPSocket::_read(Buffer buf, DurationOpt timeout, bool some)
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
          return size;
        buf = Buffer(buf.data() + size, buf.size() - size);
      }
      Read read(scheduler(), this, buf, some);
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
      auto data = buf.data();
      auto size = read.read();
      ELLE_DUMP("%s: data: 0x%s", *this,
                elle::lazy([data, size]
                           {
                             return elle::format::hexadecimal::encode(
                               elle::ConstWeakBuffer(data, size));
                           }));

      return read.read();
    }

    class ReadUntil:
      public SocketOperation<boost::asio::ip::tcp::socket>
    {
    public:
      ReadUntil(TCPSocket* socket,
                boost::asio::streambuf& buffer,
                std::string const& delimiter):
        SocketOperation<boost::asio::ip::tcp::socket>(
          *reactor::Scheduler::scheduler(),
          socket),
        _socket(*socket->socket()),
        _streambuffer(buffer),
        _delimiter(delimiter),
        _buffer()
      {}

    protected:
      virtual void _start()
      {
        boost::asio::async_read_until(
          this->_socket,
          this->_streambuffer,
          this->_delimiter,
          std::bind(&ReadUntil::_wakeup,
                    std::ref(*this),
                    this->_canceled,
                    std::placeholders::_1,
                    std::placeholders::_2));
      }

      void _wakeup(std::shared_ptr<bool> canceled,
                   const boost::system::error_code& error,
                   std::size_t read)
      {
        if (*canceled)
          return;
        if (error)
          ELLE_TRACE("%s: read until error: %s", *this, error.message());
        else
          ELLE_TRACE("%s: read until completed: %s bytes", *this, read);
        if (error == boost::asio::error::eof || \
            error == boost::system::errc::operation_canceled)
          this->_raise<ConnectionClosed>();
        else if (error)
          this->_raise<Exception>(error.message());
        {
          std::istream s(&this->_streambuffer);
          this->_buffer.size(read);
          s.read((char*)this->_buffer.mutable_contents(), read);
          ELLE_ASSERT(!s.fail());
          ELLE_ASSERT_EQ(s.gcount(), static_cast<signed>(read));
        }
        this->_signal();
      }

    private:
      ELLE_ATTRIBUTE(boost::asio::ip::tcp::socket&, socket);
      ELLE_ATTRIBUTE(boost::asio::streambuf&, streambuffer);
      ELLE_ATTRIBUTE(std::string, delimiter);
      ELLE_ATTRIBUTE_RX(elle::Buffer, buffer);
    };

    elle::Buffer
    TCPSocket::read_until(std::string const& delimiter,
                          DurationOpt timeout)
    {
      ELLE_TRACE_SCOPE("%s: read until %s", *this, delimiter);
      ReadUntil read(this, this->_streambuffer, delimiter);
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

    class Write: public SocketOperation<boost::asio::ip::tcp::socket>
    {
      public:
        typedef boost::asio::ip::tcp::socket AsioSocket;
        typedef boost::asio::ip::tcp::endpoint EndPoint;
        typedef SocketOperation<AsioSocket> Super;
        Write(Scheduler& scheduler,
              PlainSocket<AsioSocket>* socket,
              elle::ConstWeakBuffer buffer):
          Super(scheduler, socket),
          _buffer(buffer),
          _written(0)
        {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "write on ";
        try
        {
          stream << this->socket()->local_endpoint();
        }
        catch (std::exception const&)
        {
          stream << "an invalid socket (" << elle::exception_string() << ")";
        }
      }

      protected:
        virtual void _start()
        {
          boost::asio::async_write(
            *this->socket(),
            boost::asio::buffer(this->_buffer.contents(), this->_buffer.size()),
            boost::bind(&Write::_wakeup, this, this->_canceled, _1, _2));
        }

      private:
        void _wakeup(std::shared_ptr<bool> canceled,
                     const boost::system::error_code& error,
                     std::size_t written)
        {
          if (*canceled)
            return;
          if (error)
            ELLE_TRACE("%s: write error: %s", *this, error.message());
          else
            ELLE_TRACE("%s: write completed: %s bytes", *this, written);
          _written = written;
          if (error == boost::asio::error::eof ||
              error == boost::asio::error::operation_aborted ||
              error == boost::asio::error::broken_pipe ||
              error == boost::asio::error::connection_aborted)
            this->_raise<ConnectionClosed>();
          else if (error)
            this->_raise<Exception>(error.message());
          this->_signal();
        }

        elle::ConstWeakBuffer _buffer;
        Size _written;
    };

    void
    TCPSocket::write(elle::ConstWeakBuffer buffer)
    {
      scheduler().current()->wait(_write_mutex);
      ELLE_TRACE_SCOPE("%s: write %s bytes", *this, buffer.size());
      try
        {
          Write write(scheduler(), this, buffer);
          write.run();
        }
      catch (...)
        {
          _write_mutex.release();
          throw;
        }
      _write_mutex.release();
    }

    /*-----------.
    | Properties |
    `-----------*/


    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    TCPSocket::print(std::ostream& s) const
    {
      s << "TCPSocket(" << peer() << ")";
    }
  }
}
