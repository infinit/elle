//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu mar 18 21:05:22 2010]
//

///
/// this very special include is required as Socket needs to know
/// AbstractSocket while AbstractSocket inherits Socket.
///
/// including Socket.hh normally makes QT's MOC - Meta Object Compiler unable
/// to detect the QObject classes.
///
/// therefore, Socket.hh is not included when MOC processes a header file.
///
#ifndef Q_MOC_RUN
# include <elle/network/Socket.hh>
#endif

#ifndef ELLE_NETWORK_ABSTRACTSOCKET_HH
#define ELLE_NETWORK_ABSTRACTSOCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/standalone/Region.hh>

#include <elle/radix/Status.hh>

#include <elle/concurrency/Event.hh>

#include <elle/network/Socket.hh>
#include <elle/network/Parcel.hh>

#include <elle/idiom/Close.hh>
# include <list>
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
    /// this class abstracts connection-based sockets such as doors and
    /// gates.
    ///
    class AbstractSocket:
      public Socket
    {
    public:
      //
      // enumerations
      //
      enum State
	{
	  StateUnconnected,
	  StateConnecting,
	  StateConnected,
	  StateDisconnected
	};

      //
      // constants
      //
      static const Natural32			Capacity;

      //
      // types
      //
      typedef std::list<Parcel*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      AbstractSocket(Socket::Type);
      ~AbstractSocket();

      //
      // methods
      //
      template <typename I>
      Status		Send(const I,
			     const Event& = Event::Null);
      template <typename O>
      Status		Receive(const Event&,
				O);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O);
      template <typename I>
      Status		Reply(const I,
			      Session* = NULL);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // signals
      //
      struct
      {
	Signal<
	  Parameters<>
	  >		connected;
	Signal<
	  Parameters<>
	  >		disconnected;
	Signal<
	  Parameters<>
	  >		ready;
	Signal<
	  Parameters<
	    const String&
	    >
	  >		error;
      }			signal;

      //
      // attributes
      //
      State		state;

      Region*		buffer;
      Natural32		offset;

      Container		queue;

      Timer*		timer;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/AbstractSocket.hxx>

#endif
