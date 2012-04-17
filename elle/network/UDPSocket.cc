
#include <elle/utility/BufferSerializer.hxx>
#include <elle/network/HeaderSerializer.hxx>

#include <elle/network/UDPSocket.hh>
#include <elle/network/Raw.hh>
#include <elle/network/Inputs.hh>
#include <elle/network/Network.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    UDPSocket::UDPSocket():
      Socket::Socket(Socket::TypeUDP),

      port(0),
      socket(NULL)
    {
    }

    ///
    /// the destructor releases the associated resources.
    ///
    UDPSocket::~UDPSocket()
    {
      // release the socket.
      if (this->socket != NULL)
        this->socket->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a socket and connects the default signals.
    ///
    Status              UDPSocket::Create()
    {
      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket;

      // bind the socket.
      if (this->socket->bind() == false)
        escape(this->socket->errorString().toStdString().c_str());

      // retrieve the port.
      this->port = this->socket->localPort();

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
            Callback<>::Infer(&UDPSocket::Dispatch, this)) == Status::Error)
        escape("unable to subscribe to the signal");

      // connect the QT signals, depending on the mode.
      if (this->connect(this->socket, SIGNAL(readyRead()),
                        this, SLOT(_ready())) == false)
        escape("unable to connect the signal");

      if (this->connect(
            this->socket,
            SIGNAL(error(const QAbstractSocket::SocketError)),
            this,
            SLOT(_error(const QAbstractSocket::SocketError))) == false)
        escape("unable to connect to signal");

      return Status::Ok;
    }

    ///
    /// this method creates a socket with a specific port.
    ///
    Status              UDPSocket::Create(const Port            port)
    {
      // set the port.
      this->port = port;

      // allocate a new UDP socket.
      this->socket = new ::QUdpSocket;

      // bind the socket to the port.
      if (this->socket->bind(this->port) == false)
        escape(this->socket->errorString().toStdString().c_str());

      // subscribe to the signal.
      if (this->signal.ready.Subscribe(
            Callback<>::Infer(&UDPSocket::Dispatch, this)) == Status::Error)
        escape("unable to subscribe to the signal");

      // connect the QT signals.
      if (this->connect(this->socket, SIGNAL(readyRead()),
                        this, SLOT(Fetch())) == false)
        escape("unable to connect the signal");

      if (this->connect(
            this->socket,
            SIGNAL(error(const QAbstractSocket::SocketError)),
            this,
            SLOT(_error(const QAbstractSocket::SocketError))) == false)
        escape("unable to connect to signal");

      return Status::Ok;
    }

    ///
    /// this method writes a packet on the socket so that it gets sent
    /// to the given locus.
    ///
    Status              UDPSocket::Write(const Locus&           locus,
                                         const Packet&          packet)
    {
      // push the datagram into the socket.
      if (this->socket->writeDatagram(
            reinterpret_cast<const char*>(packet.Contents()),
            packet.Size(),
            locus.host.location,
            locus.port) != static_cast<qint64>(packet.Size()))
        escape(this->socket->errorString().toStdString().c_str());

      return Status::Ok;
    }

    ///
    /// this method reads datagram(s) from the socket.
    ///
    /// note that since UDP datagrams are constrained by a maxium size,
    /// there is no need to buffer the fetched datagram hoping for the next
    /// one to complete it.
    ///
    /// therefore, whenever a datagram is fetched, the method tries to makes
    /// sense out of it. if unable, the fetched datagrams are simply dropped.
    ///
    /// indeed there is no way to extract a following datagram if
    /// the first one is invalid since datagrams are variable in size.
    ///
    /// the method returns a raw with the read data.
    ///
    Status              UDPSocket::Read(Locus&                  locus,
                                        Raw&                    raw)
    {
      Natural32         size;

      // retrieve the size of the pending packet.
      size = this->socket->pendingDatagramSize();

      // check if there is data to be read.
      if (size == 0)
        return Status::Ok;

      // set the locus as being an IP locus.
      if (locus.host.Create(Host::TypeIP) == Status::Error)
        escape("unable to create an IP locus");

      // prepare the raw
      if (raw.Prepare(size) == Status::Error)
        escape("unable to prepare the raw");

      // read the datagram from the socket.
      if (this->socket->readDatagram(
            reinterpret_cast<char*>(raw.contents),
            size,
            &locus.host.location,
            &locus.port) != size)
        escape(this->socket->errorString().toStdString().c_str());

      // set the raw's size.
      raw.size = size;

      return Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the socket's state.
    ///
    Status              UDPSocket::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[UDPSocket]" << std::endl;

      // dump the socket.
      if (Socket::Dump(margin + 2) == Status::Error)
        escape("unable to dump the socket");

      return Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback fetches parcels and dispatches them.
    ///
    Status              UDPSocket::Dispatch()
    {
      Locus             locus;
      Natural32         offset;
      Raw               raw;

      // read from the socket.
      if (this->Read(locus, raw) == Status::Error)
        escape("unable to read from the socket");

      // initialize the offset.
      offset = 0;

      while (true)
        {

          // check the remaining data.
          if ((raw.size - offset) == 0)
            break;

          elle::utility::WeakBuffer packet(
              raw.contents + offset,
              raw.size - offset
          );
          auto reader = packet.Reader();

          // allocate the parcel.
          auto parcel = std::shared_ptr<Parcel>(new Parcel);

          reader >> *(parcel->header);

          // if there is not enough data in the raw to complete the parcel...
          if (reader.Stream().BytesLeft() < parcel->header->size)
            {
              log("this should not haappen in normal conditions");

              // exit the loop since there is not enough data anyway.
              break;
            }

          reader >> *(parcel->data);

          // move to the next frame by setting the offset at the end of
          // the extracted frame.
          offset += reader.Stream().Offset();

          // create the session.
          if (parcel->session->Create(this,
                                      locus,
                                      parcel->header->event) == Status::Error)
            escape("unable to create the session");

          // trigger the network shipment mechanism.
          if (Socket::Ship(parcel) == Status::Error)
            log("an error occured while shipping the parcel");
        }

      return Status::Ok;
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered when data is ready on the socket.
    ///
    void                UDPSocket::_ready()
    {
      Closure<
        Status,
        Parameters<>
        >               closure(Callback<>::Infer(&Signal<
                                                    Parameters<>
                                                    >::Emit,
                                                  &this->signal.ready));

      // spawn a fiber.
      if (Fiber::Spawn(closure) == Status::Error)
        yield(_(), "unable to spawn a fiber");
    }

    ///
    /// this slot is triggered whenever an error occurs.
    ///
    /// note here that the type QLocalSocket::LocalSocketError cannot be
    /// written completely ::QLocalSocket::LocalSocketError because the
    /// QT parser is incapable of recognising the type.
    ///
    void                UDPSocket::_error(
                          const QAbstractSocket::SocketError)
    {
      String            cause(this->socket->errorString().toStdString());
      Closure<
        Status,
        Parameters<
          const String&
          >
        >               closure(Callback<>::Infer(&Signal<
                                                    Parameters<
                                                      const String&
                                                      >
                                                    >::Emit,
                                                  &this->signal.error),
                                cause);

      // spawn a fiber.
      if (Fiber::Spawn(closure) == Status::Error)
        yield(_(), "unable to spawn a fiber");
    }

  }
}
