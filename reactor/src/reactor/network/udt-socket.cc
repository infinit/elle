#include <boost/lexical_cast.hpp>

#include <elle/log.hh>

#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/network/udt-socket.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.network.UDTSocket");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    UDTSocket::UDTSocket(Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout)
      : Super(sched,
              resolve_udp(sched, hostname, port),
              timeout)
      , _write_mutex()
    {
    }

    UDTSocket::UDTSocket(Scheduler& sched,
                         UDPSocket& socket):
      Super(sched, new boost::asio::ip::udt::socket{sched.io_service()}),
      _write_mutex()
    {
      ELLE_DEBUG("%s: rebind %s", *this, socket)
        this->socket()->_bind_fd(socket.socket()->native_handle());
    }

    UDTSocket::UDTSocket(Scheduler& sched,
                         UDPSocket& socket,
                         std::string const& hostname,
                         std::string const& port,
                         DurationOpt timeout):
      Super(sched, new boost::asio::ip::udt::socket(sched.io_service())),
      _write_mutex()
    {
      this->_peer = resolve_udp(sched, hostname, port);
      ELLE_DEBUG("%s: rebind %s", *this, socket)
        this->socket()->_bind_fd(socket.socket()->native_handle());
      ELLE_DEBUG("%s: connect to %s:%s", *this, hostname, port)
      _connect(this->_peer, timeout);
    }

    UDTSocket::UDTSocket(Scheduler& sched,
                         int fd,
                         const std::string& hostname,
                         const std::string& port,
                         DurationOpt timeout)
      : Super(sched, new boost::asio::ip::udt::socket(sched.io_service()))
      , _write_mutex()
    {
      this->_peer = resolve_udp(sched, hostname, port);
      ELLE_DEBUG("%s: rebind FD %s", *this, fd)
        this->socket()->_bind_fd(fd);
      ELLE_DEBUG("%s: connect to %s:%s", *this, hostname, port)
      this->socket()->set_option(boost::asio::ip::udt::rendezvous{true});
      _connect(this->_peer, timeout);
    }

    UDTSocket::UDTSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port,
                         DurationOpt timeout)
      : UDTSocket(sched,
                  hostname,
                  boost::lexical_cast<std::string>(port),
                  timeout)
    {}

    UDTSocket::UDTSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port,
                         int local_port,
                         DurationOpt timeout)
      : Super(sched, new boost::asio::ip::udt::socket(sched.io_service()))
      , _write_mutex()
    {
      this->socket()->bind(local_port);
      _connect(resolve_udp(sched, hostname, std::to_string(port)), timeout);
    }

    UDTSocket::~UDTSocket()
    {
      // XXX: Flush the socket, otherwise the parent ~IOStream will flush the
      // buffer which will in turn write to the (deleted) socket.
      this->flush();
    }

    UDTSocket::UDTSocket(Scheduler& sched, AsioSocket* socket)
      : Super(sched, socket)
    {}

    /*--------.
    | Connect |
    `--------*/

    void
    UDTSocket::connect(std::string const& host,
                       int port)
    {
      auto& sched = *reactor::Scheduler::scheduler();
      _connect(resolve_udp(sched, host, std::to_string(port)),
               DurationOpt());
    }

    /*-----.
    | Read |
    `-----*/

    class UDTRead: public SocketOperation<boost::asio::ip::udt::socket>
    {
      public:
        typedef boost::asio::ip::udt::socket AsioSocket;
        typedef boost::asio::ip::udt::socket::endpoint_type EndPoint;
        UDTRead(Scheduler& scheduler,
                PlainSocket<AsioSocket>* socket,
                elle::WeakBuffer& buffer,
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

      protected:

        virtual void _start()
        {
          // FIXME: be synchronous if enough bytes are available
          if (_some)
            this->socket()->async_read_some(
              boost::asio::buffer(_buffer.mutable_contents(), _buffer.size()),
              boost::bind(&UDTRead::_wakeup, this, _1, _2));
          else
            boost::asio::async_read(
              *this->socket(),
              boost::asio::buffer(_buffer.mutable_contents(), _buffer.size()),
              boost::bind(&UDTRead::_wakeup, this, _1, _2));
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

        elle::WeakBuffer& _buffer;
        ELLE_ATTRIBUTE_R(Size, read);
        bool _some;
    };

    void
    UDTSocket::read(elle::WeakBuffer buf, DurationOpt timeout)
    {
      _read(buf, timeout, false);
    }

    Size
    UDTSocket::read_some(elle::WeakBuffer buf, DurationOpt timeout)
    {
      return _read(buf, timeout, true);
    }

    Size
    UDTSocket::_read(elle::WeakBuffer buf, DurationOpt timeout, bool some)
    {
      ELLE_TRACE_SCOPE("%s: read %s%s bytes (timeout: %s)",
                           *this, some ? "up to " : "", buf.size(), timeout);
      UDTRead read(scheduler(), this, buf, some);
      bool finished;
      try
      {
        finished  = read.run(timeout);
      }
      catch (...)
      {
        ELLE_WARN("%s: read threw: %s", *this, elle::exception_string());
        throw;
      }
      if (!finished)
      {
        ELLE_TRACE("%s: read timed out", *this);
        throw TimeOut();
      }
      ELLE_TRACE("%s: read completed: %s bytes", *this, read.read());
      return read.read();
    }

    /*------.
    | Write |
    `------*/

    class UDTWrite: public SocketOperation<boost::asio::ip::udt::socket>
    {
      public:
        typedef boost::asio::ip::udt::socket AsioSocket;
        typedef boost::asio::ip::udt::socket::endpoint_type EndPoint;
        typedef SocketOperation<AsioSocket> Super;
        UDTWrite(Scheduler& scheduler,
                 PlainSocket<AsioSocket>* socket,
                 elle::ConstWeakBuffer& buffer)
          : Super(scheduler, socket)
          , _buffer(buffer)
          , _written(0)
        {}

      protected:
        virtual void _start()
        {
          boost::asio::async_write(*this->socket(),
                                   boost::asio::buffer(_buffer.data(),
                                                       _buffer.size()),
                                   boost::bind(&UDTWrite::_wakeup,
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

        elle::ConstWeakBuffer& _buffer;
        ELLE_ATTRIBUTE_R(Size, written);
    };

    void
    UDTSocket::write(elle::ConstWeakBuffer buffer)
    {
      ELLE_TRACE_SCOPE("%s: write %s bytes", *this, buffer.size());
      scheduler().current()->wait(_write_mutex);
      UDTWrite write(scheduler(), this, buffer);
      try
      {
        write.run();
      }
      catch (...)
      {
        ELLE_WARN("%s: write threw: %s", *this, elle::exception_string());
        _write_mutex.release();
        throw;
      }
      ELLE_TRACE("%s: write completed: %s bytes", *this, write.written());
      ELLE_ASSERT_EQ(buffer.size(), write.written());
      _write_mutex.release();
    }

    /*-----------.
    | Properties |
    `-----------*/


    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    UDTSocket::print(std::ostream& s) const
    {
      s << "UDTSocket(" << peer() << ")";
    }
  }
}
