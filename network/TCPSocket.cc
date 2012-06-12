
#include <elle/network/Header.hh>
#include <elle/utility/Buffer.hh>
#include <elle/standalone/Report.hh>

#include <string>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>

#include <elle/network/TCPSocket.hh>
#include <elle/network/Packet.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

#include <elle/idiom/Close.hh>
#include <elle/printf.hh>
#include <elle/log.hh>
#include <elle/idiom/Open.hh>

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

    void
    TCPSocket::ReadData()
    {
      ELLE_LOG_TRACE("%s: read data.", *this)
        {
          // Grow the buffer if needed.
          if (_buffer_size == _buffer_capacity)
            {
              ELLE_LOG_TRACE("%s: buffer is full (%s bytes), growing.",
                             *this, _buffer_size);
              _buffer_capacity += BUFSIZ;
              _buffer = reinterpret_cast<unsigned char*>(realloc(_buffer, _buffer_capacity));
            }

          // Read data.
          {
            ELLE_LOG_TRACE("%s: reading data ...", *this);
            reactor::network::Buffer buffer(_buffer + _buffer_size,
                                            _buffer_capacity - _buffer_size);
            reactor::network::Size size = this->_socket->read_some(buffer);
            _buffer_size += size;
            ELLE_LOG_TRACE("%s: ... %s bytes read.", *this, size);
          }
        }
    }

    Parcel*
    TCPSocket::Read()
    {
      ELLE_LOG_TRACE("%s: read parcel.", *this)
        {
          if (!this->_buffer_size)
            {
              ELLE_LOG_TRACE("%s: no data, bail out.", *this);
              return 0;
            }
          ELLE_LOG_TRACE("%s: %s bytes available.", *this, _buffer_size);

          elle::utility::WeakBuffer packet(this->_buffer, this->_buffer_size);
          auto reader = packet.Reader();

          // allocate the parcel.
          auto parcel = std::unique_ptr<Parcel>(new Parcel);

          // extract the header.
          reader >> *(parcel->header);
          parcel->header->Dump();

          // XXX[Check if the size is plausible]

          // Check if there is enough data available.
          if (reader.Stream().BytesLeft() < parcel->header->size)
            {
              elle::log::warn(*this, "XXX: Ignore header ?!");
              return 0;
            }


          ELLE_LOG_TRACE("%s: reading parcel data at offset %u", *this, reader.Stream().Offset())
            {
              reader >> *(parcel->data);
              ELLE_LOG_TRACE("%s: new offset is %u, and buf size is %u",
                             *this, reader.Stream().Offset(), parcel->data->Size());
            }

          parcel->data->Dump();

          unsigned int eaten = reader.Stream().Offset();
          ::memmove(_buffer, _buffer + eaten, _buffer_size - eaten);
          _buffer_size -= eaten;

          ELLE_LOG_TRACE("%s: return a parcel (%s bytes eaten).",
                         *this, eaten);

          return parcel.release();
        }
      assert(false && "never reached");
      throw false;
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
              ELLE_LOG_TRACE("%s: handle packets.", *this)
                {
                  this->ReadData();
                  while (Parcel* parcel = this->Read())
                    {
                      ELLE_LOG_TRACE("%s: got a parcel with tag %s.",
                                     *this, parcel->header->tag);
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
                              Report  report;
                              try
                                {
                                  parcel->data->Reader() >> report;
                                }
                              catch (std::exception const& err)
                                {
                                  elle::log::warn("Cannot build the report:", err.what());
                                  continue;
                                }

                              // report the remote error.
                              transpose(report);
                              // log the error.
                              log("an error message has been received "
                                  "with no registered procedure");
                              ELLE_LOG_TRACE("%s: an error message has been "
                                             "received with no registered "
                                             "procedure", *this)
                              continue;
                            }
                          else
                            {
                              ELLE_LOG_TRACE("%s: fatal: unrecognized RPC.", *this);
                              throw std::runtime_error(
                                  elle::sprintf("unrecognized RPC tag: %s.", tag)
                              );
                            }
                        }

                      Locus l;
                      Host h;
                      // FIXME
                      auto socket =
                        reinterpret_cast<reactor::network::TCPSocket*>(_socket);
                      if (h.Create(socket->Peer().address().to_string()) ==
                          Status::Error)
                        continue;
                      if (l.Create(
                            h,
                            static_cast<Natural16>(socket->Peer().port())) ==
                          Status::Error)
                        continue;
                      assert(it->second);
                      ELLE_LOG_TRACE("%s: call procedure.", *this);
                      if (it->second(this, l, *parcel) == Status::Error)
                        {
                          // FIXME
                          // escape("an error occured while processing the event");

                          // XXX
                          printf("ERROR\n");
                          show();
                          continue;
                        }
                    }
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
