#include <elle/network/TCPSocket.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Network.hh>

#include <elle/utility/Buffer.hh>

#include <elle/standalone/Report.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>

#include <elle/log.hh>
#include <elle/print.hh>

#include <protocol/PacketStream.hh>

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

    TCPSocket::TCPSocket(reactor::network::Socket*      socket)
      : _buffer(0)
      , _buffer_size(0)
      , _buffer_capacity(0)
      , _socket(socket)
      , _dispatcher(0)
    {
      _dispatcher = new reactor::Thread(concurrency::scheduler(),
                                        "TCPSocket read",
                                        boost::bind(&TCPSocket::Dispatch,
                                                    this));
    }

    TCPSocket::~TCPSocket()
    {
      _dispatcher->terminate_now();
      delete _dispatcher;
      _dispatcher = 0;
      delete this->_socket;
    }

    void
    TCPSocket::Write(const Packet&      packet)
    {
      ELLE_LOG_TRACE("%s: write packet of %s bytes.", *this, packet.Size())
        {
          reactor::network::Buffer buffer(
              packet.Contents(), packet.Size()
          );
          this->_socket->write(buffer);
        }
    }

    Parcel*
    TCPSocket::Read()
    {
      ELLE_LOG_TRACE_SCOPE("%s: read parcel.", *this);

      infinit::protocol::PacketStream ps(*_socket);
      infinit::protocol::Packet packet(ps.read());
      unsigned char* copy = (unsigned char*)malloc(packet._data_size);
      memcpy(copy, packet._data, packet._data_size);
      // Region frame(copy, packet._data_size);
      elle::utility::WeakBuffer buffer(copy, packet._data_size);
      auto reader = buffer.Reader();

      // Allocate the parcel.
      Parcel* parcel = new Parcel;

      // Extract the header.
      ELLE_LOG_TRACE("%s: read tag and event.", *this)
        reader >> parcel->header->tag >> parcel->header->event;

      // Extract the data.
      ELLE_LOG_TRACE("%s: read data.", *this)
        reader >> *parcel->data;

      return parcel;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback fetches parcels and dispatches them.
    ///
    void
    TCPSocket::Dispatch()
    {
      try
        {
          while (true)
            {
              Parcel* parcel = this->Read();
              ELLE_LOG_TRACE_SCOPE("%s: got a parcel with tag %s, event %s.",
                                   *this, parcel->header->tag, parcel->header->event.Identifier());
              // Try waking up a slot.
              if (parcel->header->event.waited)
                {
                  ELLE_LOG_TRACE("%s: wake up event %s.",
                                 *this,
                                 parcel->header->event.Identifier());
                  parcel->header->event.Parcel(parcel);
                  parcel->header->event.Signal().release();
                  continue;
                }

              // Otherwise dispatch it.
              Tag tag = parcel->header->tag;
              ELLE_LOG_TRACE("%s: received RPC with tag %s.", *this, tag);
              auto it = Network::Procedures.find(tag);

              if (it == Network::Procedures.end())
                {
                  // Display error messages
                  if (tag == TagError)
                    {
                      ELLE_LOG_TRACE("%s: RPC is an error.", *this);
                      standalone::Report  report;
                      // extract the error message.
                      parcel->data->Reader() >> report;
                      // report the remote error.
                      transpose(report);
                      // log the error.
                      log("an error message has been received "
                          "with no registered procedure");
                      ELLE_LOG_TRACE("%s: an error message has been "
                                     "received with no registered "
                                     "procedure", *this);
                      continue;
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
                continue;
              if (l.Create(
                    h,
                    static_cast<Natural16>(socket->peer().port())) ==
                  Status::Error)
                continue;
              assert(it->second);
              ELLE_LOG_TRACE("%s: call procedure.", *this);
              if (it->second(this, l, *parcel) == Status::Error)
                // FIXME
                // escape("an error occured while processing the event");
                continue;
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

    void
    TCPSocket::Disconnect()
    {
      delete _socket;
      _socket = 0;
    }
  }
}
