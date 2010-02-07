//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Bridge.hh
//
// created       julien quintard   [thu feb  4 14:39:34 2010]
// updated       julien quintard   [sun feb  7 02:14:13 2010]
//

#ifndef ELLE_NETWORK_BRIDGE_HH
#define ELLE_NETWORK_BRIDGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

#include <elle/network/Door.hh>

#include <QObject>
#include <QLocalServer>

namespace elle
{
  namespace network
  {

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
    class Porter:
      ::QObject,
      public Dumpable
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      Porter();
      ~Porter();

      //
      // methods
      //
      Status		Create(const String&,
			       Callback*);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      ::QLocalServer*	server;
      Callback*		callback;

      //
      // slots
      //
    private slots:
      void		Accept();
    };

    ///
    /// XXX
    ///
    class Bridge
    {
    public:
      //
      // types
      //
      typedef std::list<Porter*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize(); // XXX todo
      static Status	Clean(); // XXX todo: remove Porters

      static Status	Listen(const String&,
			       Callback*);

      static Status	Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static Container	Porters;
    };

  }
}

#endif
