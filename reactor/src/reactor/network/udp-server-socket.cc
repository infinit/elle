#include <reactor/network/buffer.hh>
#include <reactor/network/socket-operation.hh>
#include <reactor/network/udp-server-socket.hh>
#include <reactor/network/udp-server.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <elle/log.hh>
#include <elle/assert.hh>

ELLE_LOG_COMPONENT("reactor.network.UDPServerSocket");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/

    UDPServerSocket::UDPServerSocket(Scheduler& sched,
                                     UDPServer* server,
                                     const EndPoint& peer)
      : Super(sched)
      , _server(server)
      , _peer(peer)
      , _read_buffer(0)
      , _read_buffer_capacity(0)
      , _read_buffer_size(0)
      , _read_ready()
    {}

    UDPServerSocket::~UDPServerSocket()
    {
      delete [] _read_buffer;
    }

    /*-----.
    | Read |
    `-----*/

    void
    UDPServerSocket::read(Buffer buffer,
                          DurationOpt timeout,
                          int* bytes_read)
    {
      while (_read_buffer_size < buffer.size())
        scheduler().current()->wait(_read_ready);
      ELLE_ASSERT_GTE(_read_buffer_size, buffer.size());
      Size size = this->read_some(buffer, timeout, bytes_read);
      ELLE_ASSERT_EQ(size, buffer.size());
    }

    Size
    UDPServerSocket::read_some(Buffer buffer,
                               DurationOpt timeout,
                               int* bytes_read)
    {
      ELLE_TRACE("%s: read at most %s bytes", *this, buffer.size());
      if (_read_buffer_size == 0)
      {
        ELLE_TRACE("%s: wait for data from server.", *this);
        if (!scheduler().current()->wait(_read_ready, timeout))
          throw TimeOut(scheduler());
      }
      ELLE_ASSERT_GT(_read_buffer_size, 0);
      Size size = std::min(buffer.size(), _read_buffer_size);
      ELLE_TRACE("%s: read %s bytes", *this, size);
      memmove(buffer.data(), _read_buffer, size);
      if (size != _read_buffer_size)
        memmove(_read_buffer, _read_buffer + size, _read_buffer_size - size);
      _read_buffer_size -= size;
      if (bytes_read)
        *bytes_read = size;
      return size;
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
            this->_raise<ConnectionClosed>(scheduler());
          else if (error)
            this->_raise<Exception>(scheduler(), error.message());
          this->_signal();
        }

        Buffer& _buffer;
        Size _written;
    };

    void
    UDPServerSocket::write(Buffer buffer)
    {
      _server->_acceptor->send_to(boost::asio::buffer(buffer.data(),
                                                      buffer.size()),
                                  _peer);
    }

    /*----------------.
    | Pretty printing |
    `----------------*/

    void
    UDPServerSocket::print(std::ostream& s) const
    {
      s << "UDP Socket " << _peer;
    }
  }
}
