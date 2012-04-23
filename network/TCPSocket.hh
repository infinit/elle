///
/// this very special include is required as AbstractSocket needs to know
/// LocalSocket/TCPSocket while LocalSocket/TCPSocket inherit AbstractSocket.
///
/// including AbstractSocket.hh normally makes QT's MOC - Meta Object
/// Compiler unable to detect the QObject classes.
///
/// therefore, AbstractSocket.hh is not included when MOC processes a
/// header file.
///
#ifndef Q_MOC_RUN
# include <elle/network/AbstractSocket.hh>
#endif

#ifndef ELLE_NETWORK_TCPSOCKET_HH
#define ELLE_NETWORK_TCPSOCKET_HH

#include <elle/types.hh>

#include <elle/standalone/Region.hh>

#include <elle/concurrency/Event.hh>
#include <elle/concurrency/Signal.hh>

#include <elle/network/Parcel.hh>
#include <elle/network/Port.hh>
#include <elle/network/Session.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QTcpSocket>
#include <elle/idiom/Open.hh>

namespace elle
{

  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

  namespace network
  {

    ///
    /// this class represents a TCP socket which provides a reliable
    /// communication channel, contrary to UDPSocket.
    ///
    class TCPSocket:
      public ::QObject,

      public AbstractSocket
    {
      Q_OBJECT;

    public:
      //
      // constants
      //
      static const Natural32            Timeout;

      //
      // constructors & destructors
      //
      TCPSocket();
      ~TCPSocket();

      //
      // methods
      //
      Status            Create();
      Status            Create(::QTcpSocket*);

      Status            Connect(const Locus&,
                                Socket::Mode =
                                  Socket::ModeAsynchronous);
      Status            Disconnect();

      Status            Write(const Packet&);

      Status            Read();
      Status            Fetch();

      Status            Target(Locus&) const;

      template <typename I>
      Status            Send(const I,
                             const Event& = Event::Null);
      template <typename O>
      Status            Receive(Event&, O);
      template <typename I,
                typename O>
      Status            Call(const I,
                             O);
      template <typename I>
      Status            Reply(const I,
                              Session* = NULL);

      //
      // callbacks
      //
      Status            Dispatch();
      Status            Abort();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QTcpSocket*     socket;

    private Q_SLOTS:
      //
      // slots
      //
      void              _connected();
      void              _disconnected();
      void              _ready();
      void              _error(const QAbstractSocket::SocketError);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/TCPSocket.hxx>

#endif
