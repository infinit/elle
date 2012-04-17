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

#ifndef ELLE_NETWORK_LOCALSERVER_HH
#define ELLE_NETWORK_LOCALSERVER_HH

#include <elle/types.hh>

#include <elle/radix/Entity.hh>

#include <elle/concurrency/Callback.hh>

#include <elle/network/LocalSocket.hh>

#include <elle/idiom/Close.hh>
# include <map>
# include <QObject>
# include <QLocalServer>
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
    /// the LocalSocket class needs to be forward declared to prevent
    /// conflicts.
    ///
    class LocalSocket;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a server waiting for connections.
    ///
    /// \todo XXX note that this class should be put as a nested class of
    ///           LocalServer but QT (as of version 4.5) does not support
    ///           that feature.
    ///
    class LocalServerPorter:
      ::QObject,

      public Entity
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      LocalServerPorter(const Callback<
                          Status,
                          Parameters<LocalSocket*>
                          >&);
      ~LocalServerPorter();

      //
      // methods
      //
      Status            Create(const String&);

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
      String                    name;
      ::QLocalServer*           server;
      Callback<
        Status,
        Parameters<LocalSocket*>
        >                       callback;

      //
      // slots
      //
    private slots:
      void              _accept();
    };

    ///
    /// this class enables the caller to listen for incoming
    /// LocalSocket connections.
    ///
    class LocalServer
    {
    public:
      //
      // types
      //
      typedef std::map<const String,
                       LocalServerPorter*>              Container;
      typedef Container::iterator                       Iterator;
      typedef Container::const_iterator                 Scoutor;

      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();

      static Status     Listen(const String&,
                               const Callback< Status,
                                               Parameters<LocalSocket*> >&);
      static Status     Block(const String&);
      static Status     Retrieve(const String&,
                                 LocalServerPorter*&);
      static Status     Locate(const String&,
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
