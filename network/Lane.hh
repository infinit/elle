//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Lane.hh
//
// created       julien quintard   [thu feb  4 14:39:34 2010]
// updated       julien quintard   [thu mar 25 03:06:55 2010]
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

#ifndef ELLE_NETWORK_LANE_HH
#define ELLE_NETWORK_LANE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/network/Door.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QLocalServer>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace concurrency;

  namespace network
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a server waiting for connections.
    ///
    /// \todo XXX
    ///   note that this class should be put as a nested class of Lane
    ///   but QT (as of version 4.5) does not support that feature.
    ///
    class LanePorter:
      ::QObject,
      public Dumpable
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      LanePorter(Callback<Door*>&);
      ~LanePorter();

      //
      // methods
      //
      Status		Listen(const String&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QLocalServer*	server;
      Callback<Door*>	callback;

      //
      // slots
      //
    private slots:
      void		Accept();
    };

    ///
    /// XXX
    ///
    /// the Initialize() and Clean() are obviously not thread-safe.
    ///
    class Lane
    {
    public:
      //
      // types
      //
      typedef std::list<LanePorter*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Listen(const String&,
			       Callback<Door*>&);

      static Status	Show(const Natural32 = 0);

      //
      // static attributes
      //
      static Accord	Control;

      static Container	Porters;
    };

  }
}

#endif
