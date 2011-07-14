//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Channel.hh
//
// created       julien quintard   [thu mar 18 21:05:22 2010]
// updated       julien quintard   [thu jul 14 14:08:10 2011]
//

#ifndef ELLE_NETWORK_CHANNEL_HH
#define ELLE_NETWORK_CHANNEL_HH

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
    class Channel:
      public Socket
    {
    public:
      //
      // constants
      //
      static const Natural64		Capacity;

      //
      // types
      //
      typedef std::list<Parcel*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      Channel(Socket::Type);
      ~Channel();

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
      Status		Reply(const I);

      //
      // attributes
      //
      Region*		buffer;
      Natural64		offset;

      Container		queue;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Channel.hxx>

#endif
