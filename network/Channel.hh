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
// updated       julien quintard   [mon may  3 21:20:29 2010]
//

#ifndef ELLE_NETWORK_CHANNEL_HH
#define ELLE_NETWORK_CHANNEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Status.hh>

#include <elle/concurrency/Event.hh>

#include <elle/network/Socket.hh>

namespace elle
{
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
      // constructors & destructors
      //
      Channel(Socket::Type,
	      Socket::Mode);

      //
      // methods
      //
      template <typename I>
      Status	Send(const I,
		     const Event& = Event::Null);
      template <typename I>
      Status		Transmit(const I,
				 const Event& = Event::Null);
      template <typename I,
		typename O>
      Status		Call(const I,
			     O);
      template <typename O>
      Status		Receive(const Event&,
				O);
      template <typename I>
      Status		Reply(const I);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/network/Channel.hxx>

#endif
