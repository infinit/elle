#include <boost/lexical_cast.hpp>

#include <elle/log.hh>
#include <elle/format/hexadecimal.hh>

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

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout)
      : Super(sched,
              resolve_tcp(sched, hostname, port),
              timeout)
      , _write_mutex()
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port,
                         DurationOpt timeout)
      : Super(sched,
              resolve_tcp(sched, hostname,
                          boost::lexical_cast<std::string>(port)),
              timeout)
      , _write_mutex()
    {}

    TCPSocket::~TCPSocket()
    {
      ELLE_TRACE_SCOPE("%s: finalize", *this);
      // XXX: Flush the socket, otherwise the parent ~IOStream will flush the
      // buffer which will in turn write to the (deleted) socket.
      try
      {
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
        try
        {
          stream << "read on " << this->socket()->local_endpoint();
        }
        catch (boost::system::system_error const& e)
        {
          if (e.code() == boost::system::errc::bad_file_descriptor)
            stream << "read on invalid socket";
          else
            throw;
        }
      }

      protected:

        virtual void _start()
        {
          // FIXME: be synchronous if enough bytes are available
          if (_some)
            this->socket()->async_read_some(
              boost::asio::buffer(_buffer.data(), _buffer.size()),
              boost::bind(&Read::_wakeup, this, _1, _2));
          else
            boost::asio::async_read(
              *this->socket(),
              boost::asio::buffer(_buffer.data(), _buffer.size()),
              boost::bind(&Read::_wakeup, this, _1, _2));
        }

      private:
        void _wakeup(const boost::system::error_code& error,
                     std::size_t read)
        {
          if (error == boost::system::errc::operation_canceled)
            {
              ELLE_TRACE("read canceled");
              return;
            }
          if (error)
            ELLE_TRACE("%s: read error: %s", *this, error.message());
          else
            ELLE_TRACE("%s: read completed: %s bytes", *this, read);
          _read = read;
          if (error == boost::asio::error::eof)
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
      ELLE_DUMP("%s: data: 0x%s", *this,
                elle::format::hexadecimal::encode(
                  elle::ConstWeakBuffer(buf.data(), buf.size())));
      return read.read();
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
              Buffer& buffer)
          : Super(scheduler, socket)
          , _buffer(buffer)
          , _written(0)
        {}

      virtual
      void
      print(std::ostream& stream) const override
      {
        try
        {
          stream << "write on " << this->socket()->local_endpoint();
        }
        catch (boost::system::system_error const& e)
        {
          if (e.code() == boost::system::errc::bad_file_descriptor)
            stream << "read on invalid socket";
          else
            throw;
        }
      }

      protected:
        virtual void _start()
        {
          boost::asio::async_write(*this->socket(),
                                   boost::asio::buffer(_buffer.data(),
                                                       _buffer.size()),
                                   boost::bind(&Write::_wakeup,
                                               this, _1, _2));
        }

      private:
        void _wakeup(const boost::system::error_code& error,
                     std::size_t written)
        {
          _written = written;
          if (error == boost::asio::error::eof)
            this->_raise<ConnectionClosed>();
          else if (error)
            this->_raise<Exception>(error.message());
          this->_signal();
        }

        Buffer& _buffer;
        Size _written;
    };

    void
    TCPSocket::write(Buffer buffer)
    {
      scheduler().current()->wait(_write_mutex);
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
