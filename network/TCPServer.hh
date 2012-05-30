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

#ifndef ELLE_NETWORK_TCPSERVER_HH
#define ELLE_NETWORK_TCPSERVER_HH

#include <elle/types.hh>

#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>

#include <elle/network/TCPSocket.hh>

#include <elle/idiom/Close.hh>
# include <map>
# include <QObject>
# include <QTcpServer>
#include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;
  using namespace concurrency;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the TCPSocket class needs to be forward declared to prevent conflicts.
    ///
    class TCPSocket;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a server waiting for TCP connections.
    ///
    /// \todo XXX note that this class should be put as a nested class of
    ///           TCPServer but QT (as of version 4.5) does not support that
    ///           feature.
    ///
    class TCPServerPorter:
      ::QObject,

      public Entity
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      TCPServerPorter(const Callback<
                        Status,
                        Parameters<TCPSocket*>
                        >&);
      ~TCPServerPorter();

      //
      // methods
      //
      Status            Create(const Locus&);

      //
      // callbacks
      //
      Status            Accept();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Locus                     locus;
      ::QTcpServer*             server;
      Callback<
        Status,
        Parameters<TCPSocket*>
        >                       callback;

      //
      // slots
      //
    private Q_SLOTS:
      void              _accept();
    };

    ///
    /// this class enables the caller to listen for incoming TCP connections.
    ///
    class TCPServer
    {
    public:
      //
      // types
      //
      typedef std::map<const Locus, TCPServerPorter*>   Container;
      typedef Container::iterator                       Iterator;
      typedef Container::const_iterator                 Scoutor;

      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();

      static Status     Listen(const Locus&,
                               const Callback< Status,
                                               Parameters<TCPSocket*> >&);
      static Status     Block(const Locus&);
      static Status     Retrieve(const Locus&,
                                 TCPServerPorter*&);
      static Status     Locate(const Locus&,
                               Iterator* = NULL);

      static Status     Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Container  Porters;
    };

  }
}

#endif
