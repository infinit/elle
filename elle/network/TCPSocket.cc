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
      , _offset(0)
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
      // Get data from the network.
      char data[BUFSIZ];
      reactor::network::Buffer buffer(data, sizeof(data));
      reactor::network::Size size = this->_socket->read_some(buffer);

      // Adjust the buffer if needed.
      if (this->_buffer == NULL)
        {
          this->_buffer = new Region;
          if (this->_buffer->Prepare(size) == StatusError)
            throw std::runtime_error("unable to prepare the buffer");
        }
      else
        {
          if (this->_buffer->Adjust(this->_buffer->size + size) == StatusError)
            throw std::runtime_error("unable to adjust the buffer");
        }

      // Copy data into the buffer
      memcpy(reinterpret_cast<char*>(this->_buffer->contents +
                                     this->_buffer->size), data, size);
      this->_buffer->size = this->_buffer->size + size;
    }

    Parcel*
    TCPSocket::Read()
    {
      if (!this->_buffer || this->_buffer->size - this->_offset == 0)
        return 0;

      Packet        packet;
      Region        frame;

      // Create the frame based on the previously extracted raw.
      if (frame.Wrap(this->_buffer->contents + this->_offset,
                     this->_buffer->size - this->_offset) == StatusError)
        throw std::runtime_error("unable to wrap a frame in the raw");

      // Prepare the packet based on the frame.
      if (packet.Wrap(frame) == StatusError)
        throw std::runtime_error("unable to prepare the packet");

      // Allocate the parcel.
      Parcel* parcel = new Parcel;

      // Extract the header.
      if (parcel->header->Extract(packet) == StatusError)
        {
          printf("XXX ERROR HEADER\n");
        return 0; // No header yet
        }

      // XXX[Check if the size is plausible]

      // Check if there is enough data available.
      if ((packet.size - packet.offset) < parcel->header->size)
        {
          printf("XXX IGNORE THIS\n");
        return 0;
        }

      // Extract the data.
      if (packet.Extract(*parcel->data) == StatusError)
        throw std::runtime_error("unable to extract the data");

      this->_offset = this->_offset + packet.offset;

      // perform some operations on the buffer.
      //
      {
        // if there is no more data in the buffer, delete it in order
        // to avoid copying data whenever a new packet is
        // received. indeed, if there is no buffer, the packet becomes
        // the buffer, hence simplifying the process.
        if (this->_offset == this->_buffer->size)
          {
            // delete the buffer.
            delete this->_buffer;

            // reinitialize the buffer to NULL.
            this->_buffer = NULL;
            this->_offset = 0;
          }

        // if the offset is too far, move the existing data to the
        // beginning of the buffer.
        static const Natural32 capacity = 209715200;

        if (this->_offset >= capacity)
          {
            // move the data.
            ::memmove(this->_buffer->contents,
                      this->_buffer->contents + this->_offset,
                      this->_buffer->size - this->_offset);

            // reinitialize the buffer size.
            this->_buffer->size = this->_buffer->size - this->_offset;

            // reinitialize the offset.
            this->_offset = 0;
          }
      }

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
                  auto it = Network::Procedures.find(parcel->header->tag);

                  if (it == Network::Procedures.end())
                    {
                      // Display error messages
                      if (parcel->header->tag == TagError)
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
    }

    void
    TCPSocket::Disconnect()
    {
      delete _socket;
      _socket = 0;
    }
  }
}
