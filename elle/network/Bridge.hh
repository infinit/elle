//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bridge.hh
//
// created       julien quintard   [wed may 25 15:53:18 2011]
// updated       julien quintard   [sun aug 28 14:01:53 2011]
//

///
/// this very special include is required as Channel needs to know Door/Gate
/// while Door/Gate inherit Channel. including Channel.hh normally makes moc,
/// the QT meta object compiler, unable to detect the QObject classes.
///
/// therefore, Channel.hh is not included when moc processes a header file.
///
#ifndef Q_MOC_RUN
# include <elle/network/Channel.hh>
#endif

#ifndef ELLE_NETWORK_BRIDGE_HH
#define ELLE_NETWORK_BRIDGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>

#include <elle/network/Gate.hh>

#include <elle/idiom/Close.hh>
# include <map>
# include <QObject>
# include <QTcpServer>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace concurrency;

  namespace network
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the Gate class needs to be forward declared to prevent conflicts.
    ///
    class Gate;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a server waiting for connections.
    ///
    /// \todo XXX
    ///   note that this class should be put as a nested class of Bridge
    ///   but QT (as of version 4.5) does not support that feature.
    ///
    class BridgePorter:
      ::QObject,

      public Entity
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      BridgePorter(const Callback< Status,
				   Parameters<Gate*> >&);
      ~BridgePorter();

      //
      // methods
      //
      Status		Create(const Point&);

      //
      // callbacks
      //
      Status		Accept();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Point				point;
      ::QTcpServer*			server;
      Callback< Status,
		Parameters<Gate*> >	callback;

      //
      // slots
      //
    private slots:
      void		_accept();
    };

    ///
    /// XXX
    ///
    /// the Initialize() and Clean() are obviously not thread-safe.
    ///
    class Bridge
    {
    public:
      //
      // types
      //
      typedef std::map<const Point, BridgePorter*>	Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Listen(const Point&,
			       const Callback< Status,
					       Parameters<Gate*> >&);
      static Status	Block(const Point&);
      static Status	Retrieve(const Point&,
				 BridgePorter*&);
      static Status	Locate(const Point&,
			       Iterator* = NULL);

      static Status	Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Container	Porters;
    };

  }
}

#endif
