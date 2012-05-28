#include <string>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>

#include <elle/network/TCPSocket.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

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
      reactor::network::Buffer buffer
        (reinterpret_cast<const char*>(packet.contents), packet.size);
      this->_socket->write(buffer);
    }

    void
    TCPSocket::ReadData()
    {
      // Grow the buffer if needed.
      if (_buffer_size == _buffer_capacity)
        {
          _buffer_capacity += BUFSIZ;
          _buffer = reinterpret_cast<unsigned char*>(realloc(_buffer, _buffer_capacity));
        }

      // Read data.
      {
        reactor::network::Buffer buffer(_buffer + _buffer_size,
                                        _buffer_capacity - _buffer_size);
        reactor::network::Size size = this->_socket->read_some(buffer);
        _buffer_size += size;
      }
    }

    Parcel*
    TCPSocket::Read()
    {
      if (!this->_buffer_size)
        return 0;

      Packet        packet;
      Region        frame;

      // Create the frame based on the previously extracted raw.
      if (frame.Wrap(this->_buffer, this->_buffer_size) == StatusError)
        throw std::runtime_error("unable to wrap a frame in the raw");

      // Prepare the packet based on the frame.
      if (packet.Wrap(frame) == StatusError)
        throw std::runtime_error("unable to prepare the packet");

      // Allocate the parcel.
      Parcel* parcel = new Parcel;

      // Extract the header.
      if (parcel->header->Extract(packet) == StatusError)
        {
          purge();
          return 0; // No header yet
        }

      // XXX[Check if the size is plausible]

      // Check if there is enough data available.
      if ((packet.size - packet.offset) < parcel->header->size)
          return 0;

      // Extract the data.
      if (packet.Extract(*parcel->data) == StatusError)
        throw std::runtime_error("unable to extract the data");

      ::memmove(_buffer, _buffer + packet.size, _buffer_size - packet.size);
      _buffer_size -= packet.size;

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
              this->ReadData();
              while (Parcel* parcel = this->Read())
                {
                  // Try waking up a slot.
                  if (parcel->header->event.Signal().Emit(parcel))
                    continue;

                  // Otherwise dispatch it.
                  Tag tag = parcel->header->tag;
                  auto it = Network::Procedures.find(tag);

                  if (it == Network::Procedures.end())
                    {
                      // Display error messages
                      if (tag == TagError)
                        {
                          Report  report;
                          // extract the error message.
                          if (report.Extract(*parcel->data) == StatusError)
                            // FIXME
                            //escape("unable to extract the error message");
                            continue;
                          // report the remote error.
                          transpose(report);
                          // log the error.
                          log("an error message has been received "
                              "with no registered procedure");
                          continue;
                        }
                      else
                        {
                          throw std::runtime_error
                            (elle::format("unrecognized RPC tag: %s.", tag));
                        }
                    }

                  Locus l;
                  Host h;
                  // FIXME
                  auto socket =
                    reinterpret_cast<reactor::network::TCPSocket*>(_socket);
                  if (h.Create(socket->Peer().address().to_string()) ==
                      StatusError)
                    continue;
                  if (l.Create(
                        h,
                        static_cast<Natural16>(socket->Peer().port())) ==
                      StatusError)
                    continue;
                  assert(it->second);
                  if (it->second(this, l, *parcel) == StatusError)
                    // FIXME
                    // escape("an error occured while processing the event");
                    continue;
                }
            }
        }
      catch (const reactor::network::ConnectionClosed&)
        {
          // Nothing.
        }
      catch (const std::runtime_error&)
        {
          // Any error with the peer. Consider him alienated and
          // disconnect from him.
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
