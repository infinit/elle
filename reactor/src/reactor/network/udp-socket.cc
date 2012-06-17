#include <boost/lexical_cast.hpp>

#include <reactor/debug.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/network/udp-server.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    UDPSocket::UDPSocket(Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port)
      : Super(sched, resolve_udp(sched, hostname, port))
    {}

    UDPSocket::UDPSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port)
      : Super(sched, resolve_udp(sched, hostname,
                                 boost::lexical_cast<std::string>(port)))
    {}

    UDPSocket::~UDPSocket()
    {}

    /*-----------.
    | Connection |
    `-----------*/

    void
    UDPSocket::connect(const std::string& hostname, int service)
    {
      connect(hostname, boost::lexical_cast<std::string>(service));
    }

    void
    UDPSocket::connect(const std::string& hostname,
                       const std::string& service)
    {
      _socket = new boost::asio::ip::udp::socket(scheduler().io_service());
      _socket->connect(resolve_udp(scheduler(), hostname, service));
      //Super::connect(resolve_udp(scheduler(), hostname, service));
    }

    /*-----.
    | Read |
    `-----*/

    class UDPRead: public SocketOperation<boost::asio::ip::udp::socket>
    {
      public:
        typedef boost::asio::ip::udp::socket AsioSocket;
        typedef boost::asio::ip::udp::endpoint EndPoint;
        typedef SocketOperation<AsioSocket> Super;
        UDPRead(Scheduler& scheduler,
             PlainSocket<AsioSocket>* socket,
             Buffer& buffer)
          : Super(scheduler, socket)
          , _buffer(buffer)
          , _read(0)
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

      protected:

        virtual void _start()
        {
          // FIXME: be synchronous if enough bytes are available
          EndPoint peer;
          this->socket()->async_receive(
            boost::asio::buffer(_buffer.data(), _buffer.size()),
            boost::bind(&UDPRead::_wakeup, this, _1, _2));
        }

      private:
        void _wakeup(const boost::system::error_code& error,
                     std::size_t read)
        {
          if (error == boost::system::errc::operation_canceled)
            return;
          _read = read;
          if (error == boost::asio::error::eof)
            this->_raise(new ConnectionClosed(scheduler()));
          else if (error)
            this->_raise(new Exception(scheduler(), error.message()));
          this->_signal();
        }

        Buffer& _buffer;
        Size _read;
    };

    Size
    UDPSocket::read_some(Buffer buffer, DurationOpt timeout)
    {
      if (timeout)
        INFINIT_REACTOR_DEBUG(*this << ": read at most "
                              << buffer.size() << " bytes with timeout "
                              << timeout << ".");
      else
        INFINIT_REACTOR_DEBUG(*this << ": read at most "
                              << buffer.size() << " bytes.");
      UDPRead read(scheduler(), this, buffer);
      if (!read.run(timeout))
        throw TimeOut(scheduler());
      return read.read();
    }

    /*------.
    | Write |
    `------*/

    class UDPWrite: public SocketOperation<boost::asio::ip::udp::socket>
    {
      public:
        typedef boost::asio::ip::udp::socket AsioSocket;
        typedef boost::asio::ip::udp::endpoint EndPoint;
        typedef SocketOperation<AsioSocket> Super;
        UDPWrite(Scheduler& scheduler,
              PlainSocket<AsioSocket>* socket,
              Buffer& buffer)
          : Super(scheduler, socket)
          , _buffer(buffer)
          , _written(0)
        {}

      protected:
        virtual void _start()
        {
          this->socket()->async_send(boost::asio::buffer(_buffer.data(),
                                                         _buffer.size()),
                                     boost::bind(&UDPWrite::_wakeup,
                                                 this, _1, _2));
        }

      private:
        void _wakeup(const boost::system::error_code& error,
                     std::size_t written)
        {
          _written = written;
          if (error == boost::asio::error::eof)
            this->_raise(new ConnectionClosed(scheduler()));
          else if (error)
            this->_raise(new Exception(scheduler(), error.message()));
          this->_signal();
        }

        Buffer& _buffer;
        Size _written;
    };

    void
    UDPSocket::write(Buffer buffer)
    {
      INFINIT_REACTOR_DEBUG(*this << ": write " << buffer.size()
                            << " bytes to " << _socket->remote_endpoint()
                            << ".");
      UDPWrite write(scheduler(), this, buffer);
      write.run();
    }

    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    UDPSocket::print(std::ostream& s) const
    {
      s << "UDP Socket " << _socket->local_endpoint();
    }
  }
}
