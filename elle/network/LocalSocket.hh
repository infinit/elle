//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu feb  4 14:42:14 2010]
//

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

#ifndef ELLE_NETWORK_LOCALSOCKET_HH
#define ELLE_NETWORK_LOCALSOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

#include <elle/concurrency/Event.hh>
#include <elle/concurrency/Signal.hh>

#include <elle/network/Packet.hh>
#include <elle/network/Parcel.hh>
#include <elle/network/Session.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QLocalSocket>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace standalone;
  using namespace radix;
  using namespace concurrency;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a local socket.
    ///
    /// such sockets are often implemented on the system through pipes
    /// or local domain sockets.
    ///
    class LocalSocket:
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
      LocalSocket();
      ~LocalSocket();

      //
      // methods
      //
      Status            Create();
      Status            Create(::QLocalSocket*);

      Status            Connect(const String&,
                                Socket::Mode =
                                  Socket::ModeAsynchronous);
      Status            Disconnect();

      Status            Write(const Packet&);

      Status            Read();
      Status            Fetch();

      Status            Target(String&) const;

      template <typename I>
      Status            Send(const I,
                             const Event& = Event::Null);
      template <typename O>
      Status            Receive(const Event&,
                                O);
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
      String            name;

      ::QLocalSocket*   socket;

    private slots:
      //
      // slots
      //
      void              _connected();
      void              _disconnected();
      void              _ready();
      void              _error(const QLocalSocket::LocalSocketError);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/LocalSocket.hxx>

#endif
