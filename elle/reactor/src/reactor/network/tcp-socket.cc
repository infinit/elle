#include <boost/lexical_cast.hpp>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname, const std::string& port)
      : Super(sched, resolve_tcp(sched, hostname, port))
    {}

    TCPSocket::TCPSocket(Scheduler& sched,
                         const std::string& hostname, int port)
      : Super(sched, resolve_tcp(sched, hostname,
                                 boost::lexical_cast<std::string>(port)))
    {}

    TCPSocket::~TCPSocket()
    {}

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
            return;
          _read = read;
          if (error == boost::asio::error::eof)
            this->_raise(new ConnectionClosed());
          else if (error)
            this->_raise(new Exception(error.message()));
          this->_signal();
        }

        Buffer& _buffer;
        Size _read;
        bool _some;
    };

    void
    TCPSocket::read(Buffer buf, DurationOpt timeout)
    {
      Read read(scheduler(), this, buf, false);
      if (!read.run(timeout))
        throw TimeOut();
    }

    Size
    TCPSocket::read_some(Buffer buf, DurationOpt timeout)
    {
      Read read(scheduler(), this, buf, true);
      if (!read.run(timeout))
        throw TimeOut();
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
            this->_raise(new ConnectionClosed());
          else if (error)
            this->_raise(new Exception(error.message()));
          this->_signal();
        }

        Buffer& _buffer;
        Size _written;
    };

    void
    TCPSocket::write(Buffer buffer)
    {
      Write write(scheduler(), this, buffer);
      write.run();
    }

    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    TCPSocket::print(std::ostream& s) const
    {
      s << "TCP Socket " << _socket->local_endpoint();
    }
  }
}
