#include <boost/bind.hpp>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/socket.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/scheduler.hh>

#include <elle/log.hh>

ELLE_LOG_TRACE_COMPONENT("Reactor.Socket");

namespace reactor
{
  namespace network
  {
    namespace
    {
      class StreamBuffer: public elle::PlainStreamBuffer
      {
      public:
        typedef elle::PlainStreamBuffer Super;
        typedef Super::Size Size;
        StreamBuffer(Socket* socket)
        : _socket(socket)
        {}

      protected:
        virtual Size read(char* buffer, Size size)
        {
          return _socket->read_some(network::Buffer(buffer, size));
        }

        virtual void write(char* buffer, Size size)
        {
          _socket->write(network::Buffer(buffer, size));
        }


      private:
        Socket* _socket;
      };
    }

    /*-------------.
    | Construction |
    `-------------*/

    Socket::Socket(Scheduler& sched)
      : elle::IOStream(new StreamBuffer(this))
      , _sched(sched)
    {}

    Socket::~Socket()
    {}

    /*----------------.
    | Pretty printing |
    `----------------*/

    template <typename AsioSocket>
    void
    PlainSocket<AsioSocket>::print(std::ostream& s) const
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

    template <typename AsioSocket>
    PlainSocket<AsioSocket>::PlainSocket(Scheduler& sched,
                                         const EndPoint& peer)
      : Super(sched)
      , _socket(0)
      , _peer(peer)
    {
      _connect(_peer);
    }

    template <typename AsioSocket>
    PlainSocket<AsioSocket>::PlainSocket(Scheduler& sched,
                                         AsioSocket* socket)
      : Super(sched)
      , _socket(socket)
      , _peer(socket->remote_endpoint())
    {}

    template <typename AsioSocket>
    PlainSocket<AsioSocket>::~PlainSocket()
    {
      _disconnect();
      delete _socket;
    }

    /*-----------.
    | Connection |
    `-----------*/

    template <typename AsioSocket>
    class Connection: public SocketOperation<AsioSocket>
    {
      public:
        typedef typename AsioSocket::endpoint_type EndPoint;
        typedef SocketOperation<AsioSocket> Super;
        Connection(Scheduler& scheduler,
                   PlainSocket<AsioSocket>* socket,
                   const EndPoint& endpoint)
          : Super(scheduler, socket)
          , _endpoint(endpoint)
        {}

        virtual const char* type_name() const
        {
          static const char* name = "socket connect";
          return name;
        }

      protected:
        virtual void _start()
        {
          this->socket()->async_connect(
            _endpoint, boost::bind(&Connection::_wakeup, this, _1));
        }

      private:
        EndPoint _endpoint;
    };

    template <typename AsioSocket>
    void
    PlainSocket<AsioSocket>::_connect(const EndPoint& endpoint)
    {
      ELLE_LOG_TRACE("%s: connecting to %s", *this, endpoint);
      _socket = new AsioSocket(this->scheduler().io_service());
      Connection<AsioSocket> connection(this->scheduler(), this, endpoint);
      try
      {
        connection.run();
      }
      catch (...)
      {
        delete _socket;
        _socket = 0;
        throw;
      }
    }

    template <typename AsioSocket>
    void
    PlainSocket<AsioSocket>::_disconnect()
    {
      if (_socket)
      {
        boost::system::error_code error;
        _socket->shutdown(AsioSocket::shutdown_both, error);
        if (error)
          {
            if (error == boost::asio::error::not_connected)
              ; // It's ok to try to disconnect a non-connected socket.
            else
              throw new Exception(scheduler(), error.message());
          }
        _socket->close();
        delete _socket;
        _socket = 0;
      }
    }

    /*-----------.
    | Scheduling |
    `-----------*/

    Scheduler&
    Socket::scheduler()
    {
      return _sched;
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

    /*------.
    | Write |
    `------*/

    void
    Socket::write(const char* data)
    {
      network::Buffer buffer(data, strlen(data));
      return write(buffer);
    }

    /*-----------.
    | Properties |
    `-----------*/

    template <typename AsioSocket>
    typename PlainSocket<AsioSocket>::EndPoint
    PlainSocket<AsioSocket>::peer() const
    {
      return this->_peer;
    }

    /*------------------------.
    | Explicit instantiations |
    `------------------------*/

    template
    class PlainSocket<boost::asio::ip::tcp::socket>;
    template
    class PlainSocket<boost::asio::ip::udp::socket>;
  }
}
