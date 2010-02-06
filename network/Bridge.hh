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
// updated       julien quintard   [sat feb  6 05:50:09 2010]
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
    /// XXX
    ///
    class Bridge
    {
    public:
      //
      // classes
      //

      ///
      /// this class represents a listening entity.
      ///
      class Listener:
	::QObject,
	public Dumpable
      {
	Q_OBJECT;

      public:
	//
	// constructors & destructors
	//
	Listener();
	~Listener();

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
	Callback*	callback;

	//
	// slots
	//
      private slots:
	void		Accept();
      };

      //
      // types
      //
      typedef std::list<Listener*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      Status		Listen(const String&,
			       Callback*);

      Status		Dump(const Natural32 = 0);

      //
      // static attributes
      //
      static Container	Listeners;
    };

  }
}

#endif
