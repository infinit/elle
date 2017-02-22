#include <elle/assert.h
#include <elle/log.hh>
#include <elle/reactor/network/udp-server-socket.hh>
#include <elle/reactor/network/udp-server.hh>
#include <elle/reactor/operation.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/thread.hh>h>

ELLE_LOG_COMPONENT("elle.reactor.network.UDPServer");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*-------------.
      | Construction |
      `-------------*/

      UDPServer::UDPServer(Scheduler& sched)
        : Super(sched)
        , _acceptor(0)
      {}

      UDPServer::~UDPServer()
      {
        delete _acceptor;
      }

      /*----------.
      | Listening |
      `----------*/

      void
      UDPServer::listen(const EndPoint& end_point)
      {
        _acceptor =
          new boost::asio::ip::udp::socket(scheduler().io_service(), end_point);
        _receive();
      }

      void
      UDPServer::listen(int port, bool enable_ipv6)
      {
        return listen(EndPoint(
          enable_ipv6 ? boost::asio::ip::udp::v6() : boost::asio::ip::udp::v4(),
          port));
      }

      /*----------.
      | Accepting |
      `----------*/

      UDPServerSocket*
      UDPServer::accept()
      {
        ELLE_TRACE("%s: accept", *this);
        if (_accepted.empty())
        {
          ELLE_TRACE("%s: wait for incoming connection", *this);
          scheduler().current()->wait(_accept);
        }
        ELLE_ASSERT(!_accepted.empty());
        UDPServerSocket* res = _accepted.back();
        _accepted.pop_back();
        // ELLE_TRACE("%s: got client: %s", *this, *res);
        return res;
      }

      /*---------------.
      | Demultiplexing |
      `---------------*/

      void
      UDPServer::_receive()
      {
        _acceptor->async_receive_from
          (boost::asio::buffer(_buffer, sizeof(_buffer)), _peer,
           boost::bind(&UDPServer::_receive_handle, this, _1, _2));
      }

      std::size_t
      HashEndpoint::operator()(const boost::asio::ip::udp::endpoint& e) const
      {
        // FIXME: this kinda sucks.
        std::size_t res = boost::hash_value(e.address().to_string());
        boost::hash_combine(res, boost::hash_value(e.port()));
        return res;
      }

      std::size_t
      HashEndpoint::operator()(const boost::asio::ip::tcp::endpoint& e) const
      {
        // FIXME: this kinda sucks.
        std::size_t res = boost::hash_value(e.address().to_string());
        boost::hash_combine(res, boost::hash_value(e.port()));
        return res;
      }

      void
      UDPServer::_receive_handle(const boost::system::error_code& error,
                                 std::size_t bytes)
      {
        if (error == boost::system::errc::operation_canceled)
          return;
        else if (error)
        {
          // FIXME
          std::cerr << error << std::endl;
          std::abort();
        }
        ELLE_TRACE("%s: %s bytes available from %s",
                       *this, bytes, this->_peer);
        Clients::iterator elt = _clients.find(_peer);
        UDPServerSocket* socket = 0;
        if (elt == _clients.end())
        {
          socket = new UDPServerSocket(scheduler(), this, _peer);
          static const Size buffer_size = 512;
          socket->_read_buffer = new Byte[buffer_size];
          socket->_read_buffer_capacity = buffer_size;
          ELLE_TRACE("%s: new client: %s", *this, *socket);
          _clients[_peer] = socket;
          _accepted.push_back(socket);
          _accept.signal();
        }
        else
        {
          socket = elt->second;
          ELLE_TRACE("%s: client: %s", *this, socket);
        }
        // Grow buffer if needed.
        Size free = socket->_read_buffer_capacity - socket->_read_buffer_size;
        while (bytes > free)
        {
          ELLE_TRACE("%s: grow client buffer (free: %s)",
                         *this, free);
          socket->_read_buffer = reinterpret_cast<Byte*>
            (realloc(socket->_read_buffer, socket->_read_buffer_capacity * 2));
          socket->_read_buffer_capacity *= 2;
          free = socket->_read_buffer_capacity - socket->_read_buffer_size;
        }
        // Copy data
        ELLE_TRACE("%s: post client data", *this);
        memmove(socket->_read_buffer + socket->_read_buffer_size,
                _buffer, bytes);
        socket->_read_buffer_size += bytes;
        socket->_read_ready.signal();
        _receive();
      }

      /*---------.
      | Printing |
      `---------*/

      void
      UDPServer::print(std::ostream& s) const
      {
        s << "UDP Server " << _acceptor->local_endpoint();
      }

      std::ostream& operator << (std::ostream& s, const UDPServer& server)
      {
        server.print(s);
        return s;
      }
    }
  }
}
