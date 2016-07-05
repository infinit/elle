#include <boost/lexical_cast.hpp>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/SocketOperation.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/optional.hh>

ELLE_LOG_COMPONENT("reactor.network.UDPSocket");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    UDPSocket::UDPSocket(Scheduler& sched)
      : Super(
        elle::make_unique<boost::asio::ip::udp::socket>(
          sched.io_service()))
    {}

    UDPSocket::UDPSocket()
      : UDPSocket(reactor::scheduler())
    {}

    UDPSocket::UDPSocket(Scheduler& sched,
                         const std::string& hostname,
                         const std::string& port)
      : Super(
        elle::make_unique<boost::asio::ip::udp::socket>(sched.io_service()),
        resolve_udp(hostname, port),
        DurationOpt())
    {}

    UDPSocket::UDPSocket(const std::string& hostname,
                         const std::string& port)
      : UDPSocket(reactor::scheduler(), hostname, port)
    {}

    UDPSocket::UDPSocket(Scheduler& sched,
                         const std::string& hostname,
                         int port)
      : UDPSocket(sched, hostname, std::to_string(port))
    {}

    UDPSocket::UDPSocket(const std::string& hostname,
                         int port)
      : UDPSocket(reactor::scheduler(), hostname, port)
    {}

    UDPSocket::~UDPSocket()
    {
      // XXX: Flush the socket, otherwise the parent ~IOStream will flush the
      // buffer which will in turn write to the (deleted) socket.
      this->flush();
    }

    /*--------------.
    | Configuration |
    `--------------*/

    void
    UDPSocket::bind(boost::asio::ip::udp::endpoint const& endpoint)
    {
      if (endpoint.address().is_v6())
        socket()->open(boost::asio::ip::udp::v6()); // gives us mapped v4 too
      else
        socket()->open(boost::asio::ip::udp::v4());
      socket()->bind(endpoint);
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
                Buffer& buffer):
          Super(*socket->socket()),
          _buffer(buffer),
          _read(0)
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
          this->socket().async_receive(
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
            this->_raise<ConnectionClosed>();
          else if (error)
            this->_raise<Exception>(error.message());
          this->_signal();
        }

        Buffer& _buffer;
        Size _read;
    };

    Size
    UDPSocket::read_some(Buffer buffer, DurationOpt timeout, int* bytes_read)
    {
      if (timeout)
        ELLE_TRACE("%s: read at most %s bytes with timeout %s",
                       *this, buffer.size(), timeout);
      else
        ELLE_TRACE("%s: read at most %s bytes",
                       *this, buffer.size());
      UDPRead read(scheduler(), this, buffer);
      if (!read.run(timeout))
        throw TimeOut();
      if (bytes_read)
        *bytes_read = read.read();
      return read.read();
    }


    class UDPRecvFrom: public DataOperation<boost::asio::ip::udp::socket>
    {
      public:
        typedef boost::asio::ip::udp::socket AsioSocket;
        typedef boost::asio::ip::udp::endpoint EndPoint;
        typedef DataOperation<AsioSocket> Super;
        UDPRecvFrom(Scheduler& scheduler,
                    PlainSocket<AsioSocket>* socket,
                    Buffer& buffer,
                    boost::asio::ip::udp::endpoint & endpoint)
          : Super(*socket->socket())
          , _buffer(buffer)
          , _read(0)
          , _endpoint(endpoint)
        {}

        virtual const char* type_name() const
        {
          static const char* name = "socket recv_from";
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
          auto wake = [&] (boost::system::error_code const e, std::size_t w) {
            this->_wakeup(e, w);
          };

          this->socket().async_receive_from(
            boost::asio::buffer(_buffer.data(), _buffer.size()),
            this->_endpoint,
            wake);
        }

      private:
        void _wakeup(const boost::system::error_code& error,
                     std::size_t read)
        {
          if (error == boost::system::errc::operation_canceled)
          {
            Super::_wakeup(error);
            return;
          }
          _read = read;
          if (error == boost::asio::error::eof)
            this->_raise<ConnectionClosed>();
          else if (error)
            this->_raise<Exception>(error.message());
          this->_signal();
          Super::_wakeup(error);
        }

        Buffer& _buffer;
        Size _read;
        boost::asio::ip::udp::endpoint &_endpoint;
    };

    Size
    UDPSocket::receive_from(Buffer buffer,
                            boost::asio::ip::udp::endpoint &endpoint,
                            DurationOpt timeout)
    {
      if (timeout)
        ELLE_TRACE("%s: read at most %s bytes with timeout %s",
                       *this, buffer.size(), timeout);
      else
        ELLE_TRACE("%s: read at most %s bytes",
                       *this, buffer.size());
      UDPRecvFrom recvfrom(scheduler(), this, buffer, endpoint);
      if (!recvfrom.run(timeout))
        throw TimeOut();
      return recvfrom.read();
    }


    /*------.
    | Write |
    `------*/

    class UDPWrite: public DataOperation<boost::asio::ip::udp::socket>
    {
      public:
        typedef boost::asio::ip::udp::socket AsioSocket;
        typedef boost::asio::ip::udp::endpoint EndPoint;
        typedef DataOperation<AsioSocket> Super;
        UDPWrite(Scheduler& scheduler,
                 PlainSocket<AsioSocket>* socket,
                 elle::ConstWeakBuffer& buffer):
          Super(*socket->socket()),
          _buffer(buffer),
          _written(0)
        {}

      protected:
        virtual void _start()
        {
          this->socket().async_send(
            boost::asio::buffer(this->_buffer.contents(), this->_buffer.size()),
            boost::bind(&UDPWrite::_wakeup, this, _1, _2));
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
          Super::_wakeup(error);
        }

        elle::ConstWeakBuffer _buffer;
        Size _written;
    };

    class UDPSendTo: public DataOperation<boost::asio::ip::udp::socket>
    {
      public:
        typedef boost::asio::ip::udp::socket AsioSocket;
        typedef boost::asio::ip::udp::endpoint EndPoint;
        typedef DataOperation<AsioSocket> Super;
        UDPSendTo(Scheduler& scheduler,
                  PlainSocket<AsioSocket>* socket,
                  Buffer& buffer,
                  EndPoint const & endpoint):
          Super(*socket->socket()),
          _buffer(buffer),
          _written(0),
          _endpoint(endpoint)
        {}

      protected:
        virtual void _start()
        {
          auto wake = [&] (boost::system::error_code const e, std::size_t w) {
            this->_wakeup(e, w);
          };
          auto buffer = boost::asio::buffer(_buffer.data(), _buffer.size());
          this->socket().async_send_to(buffer, this->_endpoint, wake);
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
          Super::_wakeup(error);
        }

        Buffer& _buffer;
        Size _written;
        EndPoint _endpoint;
    };

    void
    UDPSocket::write(elle::ConstWeakBuffer buffer)
    {
      ELLE_TRACE("%s: write %s bytes to %s",
                     *this, buffer.size(), socket()->remote_endpoint());
      UDPWrite write(scheduler(), this, buffer);
      write.run();
    }

    void
    UDPSocket::send_to(Buffer buffer,
                       EndPoint endpoint)
    {
      ELLE_TRACE("%s: send_to %s bytes to %s",
                     *this, buffer.size(), endpoint);
      UDPSendTo sendto(scheduler(), this, buffer, endpoint);
      sendto.run();
    }

    /*----------------.
    | Pretty Printing |
    `----------------*/

    void
    UDPSocket::print(std::ostream& s) const
    {
      // XXX: Accessing local_endpoint on an unbound socket will result on a
      // boost::system::system_error: Bad File Descriptor.
      // Until 'socket()->bound()', just try catch that error.
      try
      {
        s << "UDP Socket " << socket()->local_endpoint();
      }
      catch (boost::system::system_error const&)
      {
        s << "UDP Socket (" << (void*) this << ")";
      }
    }

    elle::Buffer
    UDPSocket::read_until(std::string const& delimiter,
               DurationOpt opt)
    {
      throw std::runtime_error("Not implemented.");
    }
  }
}
