#include <string>

#include <elle/network/Header.hh>
#include <elle/network/Network.hh>
#include <elle/network/TCPSocket.hh>
#include <elle/utility/Buffer.hh>
#include <elle/standalone/Report.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>

#include <elle/log.hh>
#include <elle/print.hh>

#include <protocol/Channel.hh>
#include <protocol/ChanneledStream.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Network");

namespace elle
{
  namespace network
  {
    static reactor::LocalStorage<Context>& _current_context()
    {
      static reactor::LocalStorage<Context> res
        (concurrency::scheduler());
      return res;
    }

    std::ostream&
    operator << (std::ostream& s, const TCPSocket& socket)
    {
      s << "TCPSocket " << &socket;
      return s;
    }

    Context current_context()
    {
      return _current_context().Get();
    }

    void current_context(Context socket)
    {
      _current_context().Get() = socket;
    }


    /** The time to wait for a TCP socket to connect to a TCP server
     *  after which the connection is assumed to have failed.
     */
    const Natural32             TCPSocket::Timeout = 5000;

    /*-------------.
    | Construction |
    `-------------*/

    TCPSocket::TCPSocket(reactor::network::Socket* socket, bool master)
      : _socket(socket)
      , _stream(new infinit::protocol::Serializer(socket->scheduler(), *socket))
      , _channels(new infinit::protocol::ChanneledStream(socket->scheduler(), *_stream, master))
      , _stream_write_lock()
      , _dispatcher_channels(0)
      , _dispatcher_default(0)
    {
      using reactor::Thread;
      reactor::Scheduler& sched = concurrency::scheduler();
      _dispatcher_channels =
        new Thread(sched, "TCPSocket channels dispatcher",
                   boost::bind(&TCPSocket::_dispatch_channels, this));
      _dispatcher_default =
        new Thread(sched, "TCPSocket default channel dispatcher",
                   boost::bind(&TCPSocket::_dispatch_default_channel, this));
    }

    TCPSocket::~TCPSocket()
    {
      _dispatcher_channels->terminate_now();
      delete _dispatcher_channels;
      _dispatcher_channels = 0;
      _dispatcher_default->terminate_now();
      delete _dispatcher_default;
      _dispatcher_default = 0;
      delete this->_channels;
      delete this->_stream;
      delete this->_socket;
    }

    void
    TCPSocket::_dispatch_channels()
    {
      try
        {
          while (true)
            {
              ELLE_LOG_TRACE("%s: wait for channel.", *this);
              infinit::protocol::Channel channel = this->_channels->accept();
              ELLE_LOG_TRACE("%s: got new channel %s, read packet.", *this, channel.id());
              _dispatch_packet(channel.read(), channel);
            }
        }
      catch (const reactor::network::ConnectionClosed& err)
        {
          ELLE_LOG_TRACE("%s: connection closed: %s", *this, err.what());
          // Nothing.
        }
      catch (const std::runtime_error& e)
        {
          // Any error with the peer. Consider him alienated and
          // disconnect from him.
          ELLE_LOG_TRACE("%s: fatal protocol error: %s", *this, e.what());
        }
    }

    // FIXME: error handling duplicated from _dispatch_channels
    void
    TCPSocket::_dispatch_default_channel()
    {
      try
        {
          while (true)
            _dispatch_packet(_channels->read(), *_channels);
        }
      catch (const reactor::network::ConnectionClosed& err)
        {
          ELLE_LOG_TRACE("%s: connection closed: %s", *this, err.what());
          // Nothing.
        }
      catch (const std::runtime_error& e)
        {
          // Any error with the peer. Consider him alienated and
          // disconnect from him.
          ELLE_LOG_TRACE("%s: fatal protocol error: %s", *this, e.what());
        }
    }

    Parcel*
    TCPSocket::_read_parcel(infinit::protocol::Packet& packet)
    {
      Parcel* parcel = new Parcel;
      ELLE_LOG_TRACE("%s: got packet, read parcel.", *this)
        {
          // unsigned char* copy = (unsigned char*)malloc(packet.size());
          // packet.read((char*)copy, packet.size());

          // elle::utility::Buffer buffer(copy, packet.size());
          // auto reader = buffer.Reader();

          serialize::InputBinaryArchive input(packet);

          // Extract the header.
          ELLE_LOG_TRACE("%s: read tag.", *this)
            input >> parcel->header->tag;

          // Extract the data.
          ELLE_LOG_TRACE("%s: read data.", *this)
            input >> *parcel->data;
        }
      return parcel;
    }

    void
    TCPSocket::_dispatch_packet(infinit::protocol::Packet packet,
                                infinit::protocol::Stream& channel)
    {
      Parcel* parcel = _read_parcel(packet);
      Tag tag = parcel->header->tag;
      ELLE_LOG_TRACE("%s: received RPC with tag %s.", *this, tag);
      auto it = Network::Procedures.find(tag);

      if (it == Network::Procedures.end())
        {
          // Display error messages
          if (tag == TagError)
            {
              ELLE_LOG_TRACE("%s: RPC is an error.", *this);
              // FIXME: restore error messages extraction
              // standalone::Report  report;
              // extract the error message.
              // if (report.Extract(*parcel->data) == Status::Error)
              //   // FIXME
              //   //escape("unable to extract the error message");
              //   continue;
              // // report the remote error.
              // transpose(report);
              // // log the error.
              // log("an error message has been received "
              //     "with no registered procedure");
              // ELLE_LOG_TRACE("%s: an error message has been "
              //                "received with no registered "
              //                "procedure", *this)
              return;
            }
          else
            {
              ELLE_LOG_TRACE("%s: fatal: unrecognized RPC.", *this);
              throw std::runtime_error
                (elle::sprintf("unrecognized RPC tag: %s.", tag));
            }
        }

      Locus l;
      Host h;
      // FIXME
      auto socket =
        reinterpret_cast<reactor::network::TCPSocket*>(_socket);
      if (h.Create(socket->peer().address().to_string()) ==
          Status::Error)
        return;
      if (l.Create(
            h,
            static_cast<Natural16>(socket->peer().port())) ==
          Status::Error)
        return;
      assert(it->second);
      ELLE_LOG_TRACE("%s: call procedure.", *this);
      if (it->second(this, l, channel, *parcel) == Status::Error)
        // FIXME
        // escape("an error occured while processing the event");
        ;
    }

    void
    TCPSocket::Disconnect()
    {
      delete _socket;
      _socket = 0;
    }
  }
}
