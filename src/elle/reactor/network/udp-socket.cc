#include <boost/lexical_cast.hpp>

#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/optional.hh>
#include <elle/reactor/network/SocketOperation.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/resolve.hh>
#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>
#include <utility>

ELLE_LOG_COMPONENT("elle.reactor.network.UDPSocket");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*-------------.
      | Construction |
      `-------------*/

      UDPSocket::UDPSocket(Scheduler& sched)
        : Super(
          std::make_unique<boost::asio::ip::udp::socket>(
            sched.io_service()))
      {}

      UDPSocket::UDPSocket()
        : UDPSocket(reactor::scheduler())
      {}

      UDPSocket::UDPSocket(Scheduler& sched,
                           const std::string& hostname,
                           const std::string& port)
        : Super(
          std::make_unique<boost::asio::ip::udp::socket>(sched.io_service()),
          resolve_udp(hostname, port)[0],
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
      UDPSocket::bind(EndPoint const& endpoint)
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

      class UDPRead
        : public SocketOperation<boost::asio::ip::udp::socket>
      {
      public:
        using AsioSocket = boost::asio::ip::udp::socket;
        using EndPoint = boost::asio::ip::udp::endpoint;
        using Super = SocketOperation<AsioSocket>;
        UDPRead(Scheduler& scheduler,
                PlainSocket<AsioSocket>* socket,
                elle::WeakBuffer& buffer):
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
        void
        _start() override
        {
          // FIXME: be synchronous if enough bytes are available
          EndPoint peer;
          this->socket().async_receive(
            boost::asio::buffer(_buffer.mutable_contents(), _buffer.size()),
            boost::bind(&UDPRead::_wakeup, this, _1, _2));
        }

      private:
        void
        _wakeup(const boost::system::error_code& error,
                std::size_t read)
        {
          this->_read = read;
          Super::_wakeup(error);
        }

        elle::WeakBuffer& _buffer;
        Size _read;
      };

      Size
      UDPSocket::read_some(elle::WeakBuffer buffer, DurationOpt timeout,
                           int* bytes_read)
      {
        if (timeout)
          ELLE_TRACE("%s: read at most %s bytes with timeout %s",
                         *this, buffer.size(), timeout);
        else
          ELLE_TRACE("%s: read at most %s bytes",
                         *this, buffer.size());
        auto read = UDPRead(scheduler(), this, buffer);
        if (!read.run(timeout))
          throw TimeOut();
        if (bytes_read)
          *bytes_read = read.read();
        return read.read();
      }

      class UDPRecvFrom
        : public DataOperation<boost::asio::ip::udp::socket>
      {
      public:
        using AsioSocket = boost::asio::ip::udp::socket;
        using EndPoint = boost::asio::ip::udp::endpoint;
        using Super = DataOperation<AsioSocket>;
        UDPRecvFrom(Scheduler& scheduler,
                    PlainSocket<AsioSocket>* socket,
                    elle::WeakBuffer& buffer,
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

        void
        _start() override
        {
          auto wake = [&] (boost::system::error_code const e, std::size_t w) {
            this->_wakeup(e, w);
          };

          this->socket().async_receive_from(
            boost::asio::buffer(_buffer.mutable_contents(), _buffer.size()),
            this->_endpoint,
            wake);
        }

      private:
        void
        _wakeup(const boost::system::error_code& error,
                std::size_t read)
        {
          this->_read = read;
          Super::_wakeup(error);
          return;
        }

        elle::WeakBuffer& _buffer;
        Size _read;
        boost::asio::ip::udp::endpoint& _endpoint;
      };

      Size
      UDPSocket::receive_from(elle::WeakBuffer buffer,
                              boost::asio::ip::udp::endpoint &endpoint,
                              DurationOpt timeout)
      {
        if (timeout)
          ELLE_TRACE("%s: read at most %s bytes with timeout %s",
                         *this, buffer.size(), timeout);
        else
          ELLE_TRACE("%s: read at most %s bytes",
                         *this, buffer.size());
        auto recvfrom = UDPRecvFrom(scheduler(), this, buffer, endpoint);
        if (!recvfrom.run(timeout))
          throw TimeOut();
        return recvfrom.read();
      }

      /*------.
      | Write |
      `------*/

      class UDPWrite
        : public DataOperation<boost::asio::ip::udp::socket>
      {
      public:
        using AsioSocket = boost::asio::ip::udp::socket;
        using EndPoint = boost::asio::ip::udp::endpoint;
        using Super = DataOperation<AsioSocket>;
        UDPWrite(Scheduler& scheduler,
                 PlainSocket<AsioSocket>* socket,
                 elle::ConstWeakBuffer& buffer)
          : Super(*socket->socket())
          , _buffer(buffer)
          , _written(0)
        {}

      protected:
        void
        _start() override
        {
          this->socket().async_send(
            boost::asio::buffer(this->_buffer.contents(),
                                this->_buffer.size()),
            boost::bind(&UDPWrite::_wakeup, this, _1, _2));
        }

      private:
        void
        _wakeup(const boost::system::error_code& error,
                std::size_t written)
        {
          this->_written = written;
          Super::_wakeup(error);
        }

        elle::ConstWeakBuffer _buffer;
        Size _written;
      };

      class UDPSendTo
        : public DataOperation<boost::asio::ip::udp::socket>
      {
      public:
        using AsioSocket = boost::asio::ip::udp::socket;
        using EndPoint = boost::asio::ip::udp::endpoint;
        using Super = DataOperation<AsioSocket>;
        UDPSendTo(Scheduler& scheduler,
                  PlainSocket<AsioSocket>* socket,
                  elle::ConstWeakBuffer& buffer,
                  EndPoint  endpoint)
          : Super(*socket->socket())
          , _buffer(buffer)
          , _written(0)
          , _endpoint(std::move(endpoint))
        {}

      protected:
        void
        _start() override
        {
          auto wake = [&] (boost::system::error_code const e, std::size_t w)
            {
              this->_wakeup(e, w);
            };
          auto buffer = boost::asio::buffer(_buffer.contents(),
                                            _buffer.size());
          this->socket().async_send_to(buffer, this->_endpoint, wake);
        }

      private:
        void
        _wakeup(const boost::system::error_code& error,
                std::size_t written)
        {
          this->_written = written;
          Super::_wakeup(error);
        }

        elle::ConstWeakBuffer& _buffer;
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
      UDPSocket::send_to(elle::ConstWeakBuffer buffer,
                         EndPoint endpoint)
      {
        ELLE_TRACE("%s: send_to %s bytes to %s",
                       *this, buffer.size(), endpoint);
        // At least on windows and macos, passing a v4 address to send_to() on
        // a  v6 socket is an error;
        if (endpoint.address().is_v4() &&
            this->local_endpoint().address().is_v6())
          endpoint = EndPoint(
            boost::asio::ip::address_v6::v4_mapped(endpoint.address().to_v4()),
            endpoint.port());

        auto sendto = UDPSendTo(scheduler(), this, buffer, endpoint);
        sendto.run();
      }

      /*----------------.
      | Pretty Printing |
      `----------------*/

      void
      UDPSocket::print(std::ostream& s) const
      {
        // Accessing local_endpoint on an unbound socket will result in a bad
        // file descriptor error.
        boost::system::error_code e;
        auto ep = this->socket()->local_endpoint(e);
        if (e)
          elle::fprintf(s, "UDPSocket(%s)", (void*)(this));
        else
          elle::fprintf(s, "UDPSocket(%s)", ep);
      }

      elle::Buffer
      UDPSocket::read_until(std::string const& delimiter,
                 DurationOpt opt)
      {
        throw std::runtime_error("Not implemented.");
      }
    }
  }
}
