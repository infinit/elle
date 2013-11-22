#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/socket.hh>
#include <reactor/network/tcp-socket.hh>
// #include <reactor/network/udt-socket.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/scheduler.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("reactor.network.Socket");

namespace reactor
{
  namespace network
  {
    size_t const Socket::buffer_size = 1 << 16;

    namespace
    {
      class StreamBuffer: public elle::DynamicStreamBuffer
      {
      public:
        typedef elle::DynamicStreamBuffer Super;
        typedef Super::Size Size;
        StreamBuffer(Socket* socket):
          Super(Socket::buffer_size),
          _socket(socket),
          _pacified(false)
        {}

        virtual Size read(char* buffer, Size size)
        {
          if (!_pacified)
            return _socket->read_some(network::Buffer(buffer, size));
          else
            return 0;
        }

        virtual void write(char* buffer, Size size)
        {
          if (!_pacified)
            _socket->write(elle::ConstWeakBuffer(buffer, size));
        }

        Socket* _socket;
        bool _pacified;
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
    {
    }

    std::unique_ptr<Socket>
    Socket::create(Protocol protocol,
                   Scheduler& sched,
                   const std::string& hostname,
                   int port,
                   DurationOpt timeout)
    {
      switch (protocol)
        {
          case Protocol::tcp:
            return std::unique_ptr<Socket>
              (new TCPSocket(sched, hostname, port, timeout));
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
      static_cast<StreamBuffer*>(this->rdbuf())->_pacified = true;
    }

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
                                         const EndPoint& peer,
                                         DurationOpt timeout)
      : Super(sched)
      , _socket(0)
      , _peer(peer)
    {
      _connect(_peer, timeout);
    }

    template <typename Socket>
    static
    typename Socket::endpoint_type
    _get_peer(Socket& socket)
    {
      try
      {
        return socket.remote_endpoint();
      }
      catch (boost::system::system_error const& e)
      {
        if (e.code() == boost::system::errc::bad_file_descriptor)
        {
          // The socket might not have a remote endpoint if it's a listening
          // socket for instance.
          return typename Socket::endpoint_type();
        }
        else if (e.code() == boost::system::errc::not_connected)
        {
          throw ConnectionClosed();
        }
        else
          throw;
      }
    }

    template <typename AsioSocket>
    PlainSocket<AsioSocket>
    ::PlainSocket(Scheduler& sched,
                  AsioSocket* socket):
      PlainSocket<AsioSocket>(sched, socket, _get_peer(*socket))
    {}

    template <typename AsioSocket>
    PlainSocket<AsioSocket>
    ::PlainSocket(Scheduler& sched,
                  AsioSocket* socket,
                  typename AsioSocket::endpoint_type const& peer):
      Super(sched),
      _socket(socket),
      _peer(peer)
    {}

    template <typename AsioSocket>
    PlainSocket<AsioSocket>::~PlainSocket()
    {
      try
      {
        _disconnect();
      }
      catch (elle::Exception const&)
      {
        // Nothing. If the socket was invalid, no need to disconnect it. We
        // might be able to catch a more precise exception if _disconnect throws
        // differently.
      }
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

      virtual
      void
      print(std::ostream& stream) const override
      {
        stream << "connection to " << this->_endpoint;
      }

      protected:
        virtual void _start()
        {
          this->socket()->async_connect(
            _endpoint, boost::bind(&Connection::_wakeup,
                                   this, this->_canceled, _1));
        }

      private:
        EndPoint _endpoint;
    };

    /*-----------.
    | Connection |
    `-----------*/

    template <typename AsioSocket>
    void
    PlainSocket<AsioSocket>::close()
    {
      ELLE_TRACE_SCOPE("%s: close", *this);
      this->socket()->close();
    }

    template <typename AsioSocket>
    void
    PlainSocket<AsioSocket>::_connect(const EndPoint& endpoint, DurationOpt timeout)
    {
      ELLE_TRACE("%s: connecting to %s", *this, endpoint);
      if (!this->_socket)
        this->_socket = new AsioSocket(this->scheduler().io_service());
      Connection<AsioSocket> connection(this->scheduler(), this, endpoint);
      try
      {
        if (!connection.run(timeout))
          throw TimeOut();
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
            if (error == boost::asio::error::not_connected
#ifdef INFINIT_WINDOWS
                || error == boost::asio::error::bad_descriptor
#endif
                )
              ; // It's ok to try to disconnect a non-connected socket.
            else
              throw Exception(error.message());
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

    template <typename AsioSocket>
    elle::network::Locus
    PlainSocket<AsioSocket>::local_locus() const
    {
      return locus_from_endpoint<AsioSocket>(local_endpoint());
    }

    template <typename AsioSocket>
    elle::network::Locus
    PlainSocket<AsioSocket>::remote_locus() const
    {
      return locus_from_endpoint<AsioSocket>(peer());
    }

    template <typename AsioSocket>
    typename PlainSocket<AsioSocket>::EndPoint
    PlainSocket<AsioSocket>::peer() const
    {
      return this->_peer;
    }

    template <typename AsioSocket>
    typename PlainSocket<AsioSocket>::EndPoint
    PlainSocket<AsioSocket>::local_endpoint() const
    {
        return this->_socket->local_endpoint();
    }

    /*-------------.
    | StreamSocket |
    `-------------*/

    /*------.
    | Write |
    `------*/

    template <typename AsioSocket>
    class Write:
      public SocketOperation<AsioSocket>
    {
    public:
      typedef typename AsioSocket::endpoint_type EndPoint;
      typedef SocketOperation<AsioSocket> Super;
      Write(PlainSocket<AsioSocket>* socket,
            elle::ConstWeakBuffer buffer):
        Super(*Scheduler::scheduler(), socket),
        _socket(*socket),
        _buffer(buffer),
        _written(0)
      {}

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
          ELLE_TRACE("%s: write error: %s", this->_socket, error.message());
        else
          ELLE_TRACE("%s: write completed: %s bytes", this->_socket, written);
        _written = written;
        if (error == boost::asio::error::eof ||
            error == boost::asio::error::operation_aborted ||
            error == boost::asio::error::broken_pipe ||
            error == boost::asio::error::connection_aborted)
          this->template _raise<ConnectionClosed>();
        else if (error)
          this->template _raise<Exception>(error.message());
        this->_signal();
      }

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

      ELLE_ATTRIBUTE(PlainSocket<AsioSocket> const&, socket);
      ELLE_ATTRIBUTE(elle::ConstWeakBuffer, buffer);
      ELLE_ATTRIBUTE_R(Size, written);
    };

    template <typename AsioSocket>
    void
    StreamSocket<AsioSocket>::write(elle::ConstWeakBuffer buffer)
    {
      reactor::wait(this->_write_mutex);
      ELLE_TRACE_SCOPE("%s: write %s bytes", *this, buffer.size());
      try
      {
        Write<AsioSocket> write(this, buffer);
        write.run();
      }
      catch (...)
      {
        _write_mutex.release();
        throw;
      }
      _write_mutex.release();
    }

    /*------------------------.
    | Explicit instantiations |
    `------------------------*/

    template
    class PlainSocket<boost::asio::ip::tcp::socket>;
    template
    class StreamSocket<boost::asio::ip::tcp::socket>;
    template
    class PlainSocket<boost::asio::ip::udp::socket>;
    // template
    // class PlainSocket<boost::asio::ip::udt::socket>;
  }
}
